#include "core.hpp"
#include "Application/Game.hpp"

CoreEngine::CoreEngine(std::shared_ptr<Context> ctx) : m_context(ctx) {
  if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    CRITLOG("Failed to Initialize SDL3!");
  }

  m_context = std::make_shared<Context>();
  m_context->window_data = {800, 600, "Physim"};
  m_context->api = RenderType::OPENGL;
  
  m_renderer = createRenderer(m_context);
  m_resourceManager = std::make_unique<ResourceManager>();

  LOG("Core engine initialized");

  m_gameLoopData.previousTicks = SDL_GetTicks();
  m_gameLoopData.accumulator = 0.0f;
}

CoreEngine::~CoreEngine() {
  SDL_Quit();
}

void CoreEngine::run() {
  while(!m_quit) {
    
    uint64_t currentTime = SDL_GetTicks();
    double deltaTime = (currentTime - m_gameLoopData.previousTicks) / 1000.0; 
    m_gameLoopData.previousTicks = currentTime;
    
    if (deltaTime > m_gameLoopData.maxFrameTime) {
      deltaTime = m_gameLoopData.maxFrameTime;
    }
    
    input();
    
    m_gameLoopData.accumulator += deltaTime;
    while (m_gameLoopData.accumulator >= m_gameLoopData.targetFrameTime) {
      update();
      m_gameLoopData.accumulator -= m_gameLoopData.targetFrameTime;
    }
    
    render();
    
    m_gameLoopData.fpsFrameCount++;
    
    if (currentTime - m_gameLoopData.lastFPSUpdateTime >= 1000) { 
      double elapsedSeconds = (currentTime - m_gameLoopData.lastFPSUpdateTime) / 1000.0;
      double fps = m_gameLoopData.fpsFrameCount / elapsedSeconds;
      
      LOOPLOG("FPS: ", fps);
      
      m_gameLoopData.fpsFrameCount = 0;
      m_gameLoopData.lastFPSUpdateTime = currentTime;
    }
    
    double frameEndTime = SDL_GetTicks();
    double frameProcessingTime = (frameEndTime - currentTime) / 1000.0;
    double targetFrameTime = m_gameLoopData.targetFrameTime;
    
    if (frameProcessingTime < targetFrameTime) {
      uint32_t sleepTime = static_cast<uint32_t>((targetFrameTime - frameProcessingTime) * 1000);
      if (sleepTime > 0 && sleepTime < 100) { 
        SDL_Delay(sleepTime);
      }
    }
  }
}

void CoreEngine::input() {
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

void CoreEngine::update() {
  if(m_game) {
    m_game->update(m_gameLoopData.targetFrameTime);
  }
}

void CoreEngine::render() {
  m_renderer->beginFrame();

  if(m_game) {
    m_game->render(m_renderer.get());
  }

  m_renderer->endFrame();
}

void CoreEngine::setGame(std::unique_ptr<Game> game) {
  m_game = std::move(game);
}
