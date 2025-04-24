#include "WasmAdapter.hpp"

#ifdef __EMSCRIPTEN__
#include "engine/Engine.hpp"
#include "Application/Game.hpp"

void emscripten_main_loop_callback(void* arg) {
    WasmAdapter* adapter = static_cast<WasmAdapter*>(arg);
    if (!adapter) return;
    
    Engine* engine = adapter->getEngine();
    Game* game = adapter->getGame();
    
    if (!engine || !game) return;
    
    uint64_t currentTime = SDL_GetTicks();
    double deltaTime = (currentTime - adapter->getLastFrameTime()) / 1000.0;
    
    if (deltaTime > 0.25) {
        WARLOG("Frame time exceeded maximum threshold: ", deltaTime, "s. Capping to 0.25s");
        deltaTime = 0.25;
    }
    
    adapter->setLastFrameTime(currentTime);
    
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_EVENT_QUIT) {
            return;
        }

        if(event.type == SDL_EVENT_WINDOW_RESIZED) {
            if (engine->getRenderer()) {
                engine->getRenderer()->resize();
            }
        }

        if(game) {
            game->handleInput(event);
            if(!game->isRunning()) {
                emscripten_cancel_main_loop();
                return;
            }
        }
    }
    
    adapter->addAccumulatedTime(deltaTime);
    
    int updateCount = 0;
    const double fixedTimeStep = 1.0 / 60.0; // 60 Hz physics
    
    while (adapter->getAccumulatedTime() >= fixedTimeStep && updateCount < 5) {
        if (game) {
            game->update(fixedTimeStep);
        }
        adapter->reduceAccumulatedTime(fixedTimeStep);
        updateCount++;
    }
    
    double alpha = adapter->getAccumulatedTime() / fixedTimeStep;
    
    if (engine->getRenderer()) {
        IRenderer* renderer = engine->getRenderer();
        renderer->beginFrame();
        
        if (game) {
            game->render(renderer);
        }
        
        renderer->endFrame();
    }
    
    adapter->incrementFrameCount();
    if (currentTime - adapter->getLastFPSUpdateTime() >= 1000) {
        double fps = adapter->getFrameCount() / ((currentTime - adapter->getLastFPSUpdateTime()) / 1000.0);
        LOOPLOG("WASM FPS: ", std::fixed, std::setprecision(1), fps);
        
        adapter->resetFrameCount();
        adapter->setLastFPSUpdateTime(currentTime);
    }
}

void WasmAdapter::init(Engine* engine, std::unique_ptr<Game> game) {
    m_engine = engine;
    m_game = std::move(game);
    
    m_lastFrameTime = SDL_GetTicks();
    m_lastFPSUpdateTime = m_lastFrameTime;
    m_accumulatedTime = 0.0;
    m_frameCount = 0;
    
    if (m_game) {
        m_game->init();
    }
}

void WasmAdapter::run() {
    if (!m_engine || !m_game) {
        ERRLOG("Cannot run WASM adapter: Engine or Game is null");
        return;
    }
    
    m_running = true;
    
    emscripten_set_main_loop_arg(emscripten_main_loop_callback, this, 0, 1);
}

void WasmAdapter::handleCanvasResize() {
    if (!m_engine) return;
    
    double width, height;
    
    emscripten_get_element_css_size("#canvas", &width, &height);
    
    WindowData newSize = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        m_engine->getWindowSize().name
    };
    
    emscripten_set_canvas_element_size("#canvas", newSize.width, newSize.height);
    
    if (m_engine->getRenderer()) {
        m_engine->getRenderer()->resize();
    }
    
    LOG("Canvas resized to: ", newSize.width, "x", newSize.height);
}

uint64_t WasmAdapter::getLastFrameTime() const {
    return m_lastFrameTime;
}

void WasmAdapter::setLastFrameTime(uint64_t time) {
    m_lastFrameTime = time;
}

double WasmAdapter::getAccumulatedTime() const {
    return m_accumulatedTime;
}

void WasmAdapter::addAccumulatedTime(double time) {
    m_accumulatedTime += time;
}

void WasmAdapter::reduceAccumulatedTime(double time) {
    m_accumulatedTime -= time;
}

uint32_t WasmAdapter::getFrameCount() const {
    return m_frameCount;
}

void WasmAdapter::incrementFrameCount() {
    m_frameCount++;
}

void WasmAdapter::resetFrameCount() {
    m_frameCount = 0;
}

uint64_t WasmAdapter::getLastFPSUpdateTime() const {
    return m_lastFPSUpdateTime;
}

void WasmAdapter::setLastFPSUpdateTime(uint64_t time) {
    m_lastFPSUpdateTime = time;
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE void updateCanvasSize() {
        WasmAdapter::getInstance().handleCanvasResize();
    }
}

#endif