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
                return;
            }
        }
    }
    
    if (game) {
        const double targetFrameTime = 1.0 / 60.0;
        game->update(targetFrameTime);
    }
    
    if (engine->getRenderer()) {
        IRenderer* renderer = engine->getRenderer();
        renderer->beginFrame();
        
        if (game) {
            game->render(renderer);
        }
        
        renderer->endFrame();
    }
}

void WasmAdapter::init(Engine* engine, std::unique_ptr<Game> game) {
    m_engine = engine;
    m_game = std::move(game);
    
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

extern "C" {
    EMSCRIPTEN_KEEPALIVE void updateCanvasSize() {
        WasmAdapter::getInstance().handleCanvasResize();
    }
}

#endif 