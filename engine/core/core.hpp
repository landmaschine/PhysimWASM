#pragma once
#include "common.hpp"
#include "renderer/Inter_Renderer.hpp"
#include "renderer/factory_renderer.hpp"

class Game;

typedef struct GameLoopData_t {
  uint64_t previousTicks;          
  double accumulator;              
  const double maxFrameTime = 0.25; 
  
  const double targetFPS = 165.0;    
  const double targetFrameTime = 1.0 / 165.0;  
  
  uint32_t fpsFrameCount = 0;      
  double fpsTimeElapsed = 0.0;     
  uint64_t lastFPSUpdateTime = 0;  
} GameLoopData;

class CoreEngine {
public:
  CoreEngine(std::shared_ptr<Context> ctx);
  ~CoreEngine();

  void run();

  void setGame(std::unique_ptr<Game> game);

  IRenderer* getRenderer() const { return m_renderer.get(); }

  WindowData getWindowSize() const {
    if(m_context) {
      return m_context->window_data;
    }

    ERRLOG("No Context availabe");
    return {0, 0, ""};
  }

private:
  void input();
  void update();
  void render();

private:
  bool m_quit = false;
  SDL_Event m_event;
  GameLoopData m_gameLoopData;
  
  std::unique_ptr<Game> m_game;
  std::unique_ptr<IRenderer> m_renderer;
  std::shared_ptr<Context> m_context;

};
