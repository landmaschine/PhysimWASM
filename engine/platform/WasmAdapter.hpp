#pragma once
#include "core/common.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

class Engine;
class Game;

void emscripten_main_loop_callback(void* arg);

class WasmAdapter {
public:
    static WasmAdapter& getInstance() {
        static WasmAdapter instance;
        return instance;
    }

    void init(Engine* engine, std::unique_ptr<Game> game);
    
    void run();
    
    void handleCanvasResize();
    
    Engine* getEngine() { return m_engine; }
    
    Game* getGame() { return m_game.get(); }

private:
    WasmAdapter() = default;
    ~WasmAdapter() = default;
    
    WasmAdapter(const WasmAdapter&) = delete;
    WasmAdapter& operator=(const WasmAdapter&) = delete;
    
    Engine* m_engine = nullptr;
    std::unique_ptr<Game> m_game = nullptr;
    bool m_running = false;
};

extern "C" {
    EMSCRIPTEN_KEEPALIVE void updateCanvasSize();
}

#endif 