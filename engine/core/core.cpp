#include "core.hpp"
#include "Application/Game.hpp"

CoreEngine::CoreEngine(std::shared_ptr<Context> ctx) : m_context(ctx) {
  if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    CRITLOG("Failed to Initialize SDL3!");
  }

  m_renderer = createRenderer(m_context);
  m_resourceManager = std::make_unique<ResourceManager>();

  LOG("Core engine initialized");

  m_gameLoopData.lastFrameTime = SDL_GetTicks();
  m_gameLoopData.lastFixedUpdateTime = m_gameLoopData.lastFrameTime;
  m_gameLoopData.frameCount = 0;
  m_gameLoopData.lastFPSUpdateTime = m_gameLoopData.lastFrameTime;
  m_gameLoopData.accumulator = 0.0;

  m_performanceMetrics.avgFrameTime = 0.0;
  m_performanceMetrics.avgUpdateTime = 0.0;
  m_performanceMetrics.avgRenderTime = 0.0;
  m_performanceMetrics.frameTimeHistory.resize(m_performanceMetrics.frameTimeHistorySize, 0.0);
  m_performanceMetrics.updateTimeHistory.resize(m_performanceMetrics.frameTimeHistorySize, 0.0);
  m_performanceMetrics.renderTimeHistory.resize(m_performanceMetrics.frameTimeHistorySize, 0.0);
}

CoreEngine::~CoreEngine() {
  SDL_Quit();
}

void CoreEngine::run() {
  LOG("Starting main game loop");
  
  while(!m_quit) {
    uint64_t currentTime = SDL_GetTicks();
    double deltaTime = (currentTime - m_gameLoopData.lastFrameTime) / 1000.0;
    m_gameLoopData.lastFrameTime = currentTime;
    
    if (deltaTime > m_gameLoopData.maxFrameTime) {
      WARLOG("Frame time exceeded maximum threshold: ", deltaTime, "s. Capping to ", m_gameLoopData.maxFrameTime, "s");
      deltaTime = m_gameLoopData.maxFrameTime;
    }

    uint64_t frameStartTime = SDL_GetTicks();
    
    processInput();
    
    m_gameLoopData.accumulator += deltaTime;
    
    uint64_t updateStartTime = SDL_GetTicks();
    int updateCount = 0;
    
    while (m_gameLoopData.accumulator >= m_gameLoopData.fixedTimeStep) {
      fixedUpdate(m_gameLoopData.fixedTimeStep);
      m_gameLoopData.accumulator -= m_gameLoopData.fixedTimeStep;
      m_gameLoopData.fixedUpdateCount++;
      updateCount++;
      
      if (updateCount >= m_gameLoopData.maxUpdatesPerFrame) {
        WARLOG("Too many updates in one frame: ", updateCount, ". Dropping remainder of time: ", m_gameLoopData.accumulator);
        m_gameLoopData.accumulator = 0;
        break;
      }
    }
    
    uint64_t updateEndTime = SDL_GetTicks();
    double updateTime = (updateEndTime - updateStartTime) / 1000.0;
    
    variableUpdate(deltaTime);
    
    double alpha = m_gameLoopData.accumulator / m_gameLoopData.fixedTimeStep;
    
    uint64_t renderStartTime = SDL_GetTicks();
    render(alpha);
    uint64_t renderEndTime = SDL_GetTicks();
    double renderTime = (renderEndTime - renderStartTime) / 1000.0;
    
    uint64_t frameEndTime = SDL_GetTicks();
    double frameTime = (frameEndTime - frameStartTime) / 1000.0;
    
    updatePerformanceMetrics(frameTime, updateTime, renderTime);
    
    m_gameLoopData.frameCount++;
    if (currentTime - m_gameLoopData.lastFPSUpdateTime >= m_gameLoopData.fpsUpdateInterval) {
      double elapsedSeconds = (currentTime - m_gameLoopData.lastFPSUpdateTime) / 1000.0;
      double fps = m_gameLoopData.frameCount / elapsedSeconds;
      double frameTimeAvg = elapsedSeconds / m_gameLoopData.frameCount;
      
      m_gameLoopData.currentFPS = fps;
      
      LOOPLOG("FPS: ", std::fixed, std::setprecision(1), fps, 
              " | Frame time: ", std::setprecision(2), frameTimeAvg * 1000.0, "ms", 
              " | Updates/s: ", m_gameLoopData.fixedUpdateCount / elapsedSeconds);
      
      m_gameLoopData.frameCount = 0;
      m_gameLoopData.fixedUpdateCount = 0;
      m_gameLoopData.lastFPSUpdateTime = currentTime;
    }
    
    limitFrameRate(currentTime);
  }
  
  LOG("Game loop terminated");
}

void CoreEngine::processInput() {
  while(SDL_PollEvent(&m_event)) {
    if(m_event.type == SDL_EVENT_QUIT) {
      m_quit = true;
    }

    if(m_event.type == SDL_EVENT_WINDOW_RESIZED) {
      m_renderer->resize();
    }

    if(m_game) {
      m_game->handleInput(m_event);
      if(!m_game->isRunning()) {
        m_quit = true;
      }
    }
  }
}

void CoreEngine::fixedUpdate(double fixedTimeStep) {
  if(m_game) {
    m_game->update(fixedTimeStep);
  }
}

void CoreEngine::variableUpdate(double deltaTime) {

}

void CoreEngine::render(double interpolation) {
  m_renderer->beginFrame();

  if(m_game) {
    m_game->render(m_renderer.get());
  }

  m_renderer->endFrame();
}

void CoreEngine::updatePerformanceMetrics(double frameTime, double updateTime, double renderTime) {
  m_performanceMetrics.frameTimeHistory[m_performanceMetrics.frameTimeHistoryIndex] = frameTime;
  m_performanceMetrics.updateTimeHistory[m_performanceMetrics.frameTimeHistoryIndex] = updateTime;
  m_performanceMetrics.renderTimeHistory[m_performanceMetrics.frameTimeHistoryIndex] = renderTime;
  
  m_performanceMetrics.frameTimeHistoryIndex = 
    (m_performanceMetrics.frameTimeHistoryIndex + 1) % m_performanceMetrics.frameTimeHistorySize;
  
  m_performanceMetrics.avgFrameTime = 0.0;
  m_performanceMetrics.avgUpdateTime = 0.0;
  m_performanceMetrics.avgRenderTime = 0.0;
  
  for (size_t i = 0; i < m_performanceMetrics.frameTimeHistorySize; i++) {
    m_performanceMetrics.avgFrameTime += m_performanceMetrics.frameTimeHistory[i];
    m_performanceMetrics.avgUpdateTime += m_performanceMetrics.updateTimeHistory[i];
    m_performanceMetrics.avgRenderTime += m_performanceMetrics.renderTimeHistory[i];
  }
  
  m_performanceMetrics.avgFrameTime /= m_performanceMetrics.frameTimeHistorySize;
  m_performanceMetrics.avgUpdateTime /= m_performanceMetrics.frameTimeHistorySize;
  m_performanceMetrics.avgRenderTime /= m_performanceMetrics.frameTimeHistorySize;
}

void CoreEngine::limitFrameRate(uint64_t frameStartTime) {
  if (!m_gameLoopData.limitFrameRate) {
    return;
  }
  
  double targetFrameTime = 1.0 / m_gameLoopData.targetFPS;
  uint64_t currentTime = SDL_GetTicks();
  double elapsedFrameTime = (currentTime - frameStartTime) / 1000.0;
  
  if (elapsedFrameTime < targetFrameTime) {
    double sleepTime = targetFrameTime - elapsedFrameTime;
    uint32_t sleepTimeMs = static_cast<uint32_t>(sleepTime * 1000);
    
    if (sleepTimeMs > 1 && sleepTimeMs < 100) {
      SDL_Delay(sleepTimeMs);
    }
  }
}

void CoreEngine::setGame(std::unique_ptr<Game> game) {
  m_game = std::move(game);
}

PerformanceMetrics CoreEngine::getPerformanceMetrics() const {
  return m_performanceMetrics;
}