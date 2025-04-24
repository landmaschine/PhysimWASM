#pragma once
#include "common.hpp"
#include "Renderer/Inter_Renderer.hpp"
#include "Renderer/factory_renderer.hpp"
#include "ResourceManager/ResourceManager.hpp"

class Game;

typedef struct GameLoopData_t {
  uint64_t lastFrameTime = 0;
  uint64_t lastFixedUpdateTime = 0;
  double accumulator = 0.0f;


  bool limitFrameRate = true;
  double targetFPS = 165.0;
  double fixedTimeStep = 1.0f / 165.f;
  double maxFrameTime = 0.25f;

  uint32_t frameCount = 0;
  uint32_t fixedUpdateCount = 0;
  uint64_t lastFPSUpdateTime = 0;
  uint32_t fpsUpdateInterval = 1000;

  double currentFPS = 0.0f;

  int maxUpdatesPerFrame = 5;
} GameLoopData;

typedef struct PerformanceMetrics_t {
  double avgFrameTime = 0.0f;
  double avgUpdateTime = 0.0f;
  double avgRenderTime = 0.0f;

  std::vector<double> frameTimeHistory;
  std::vector<double> updateTimeHistory;
  std::vector<double> renderTimeHistory;

  size_t frameTimeHistorySize = 60;
  size_t frameTimeHistoryIndex = 0;
} PerformanceMetrics;

class CoreEngine {
public:
  CoreEngine(std::shared_ptr<Context> ctx);
  ~CoreEngine();

  void run();
  void setGame(std::unique_ptr<Game> game);

  void processInput();
  void fixedUpdate(double fixedTimeStep);
  void variableUpdate(double deltaTime);
  void render(double interpolation);

  void updatePerformanceMetrics(double frameTime, double updateTime, double renderTime);
  void limitFrameRate(uint64_t frameStartTime);
  PerformanceMetrics getPerformanceMetrics() const;

  IRenderer* getRenderer() const { return m_renderer.get(); }
  ResourceManager* getResourceManager() const { return m_resourceManager.get(); }

  WindowData getWindowSize() const {
    if(m_context) {
      return m_context->window_data;
    }

    ERRLOG("No Context availabe");
    return {0, 0, ""};
  }

private:
  bool m_quit = false;
  SDL_Event m_event;
  GameLoopData m_gameLoopData;
  PerformanceMetrics m_performanceMetrics;
  
  std::unique_ptr<Game> m_game;
  std::unique_ptr<IRenderer> m_renderer;
  std::unique_ptr<ResourceManager> m_resourceManager;
  std::shared_ptr<Context> m_context;

};
