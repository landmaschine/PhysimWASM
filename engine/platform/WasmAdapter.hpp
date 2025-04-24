#pragma once
#include "Core/common.hpp"
#include <iomanip>

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
    
    uint64_t getLastFrameTime() const;
    void setLastFrameTime(uint64_t time);
    
    double getAccumulatedTime() const;
    void addAccumulatedTime(double time);
    void reduceAccumulatedTime(double time);
    
    uint32_t getFrameCount() const;
    void incrementFrameCount();
    void resetFrameCount();
    
    uint64_t getLastFPSUpdateTime() const;
    void setLastFPSUpdateTime(uint64_t time);

private:
    WasmAdapter() = default;
    ~WasmAdapter() = default;
    
    WasmAdapter(const WasmAdapter&) = delete;
    WasmAdapter& operator=(const WasmAdapter&) = delete;
    
    Engine* m_engine = nullptr;
    std::unique_ptr<Game> m_game = nullptr;
    bool m_running = false;
    
    uint64_t m_lastFrameTime = 0;
    uint64_t m_lastFPSUpdateTime = 0;
    double m_accumulatedTime = 0.0;
    uint32_t m_frameCount = 0;
};

extern "C" {
    EMSCRIPTEN_KEEPALIVE void updateCanvasSize();
}

#endif