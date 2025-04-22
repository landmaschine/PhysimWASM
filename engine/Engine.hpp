#pragma once

#include "core/core.hpp"
#include "renderer/Inter_Renderer.hpp"

class CoreEngine;
class IRenderer;
class Game;

class Engine {
public:
  static Engine& getInstance();

  bool init(const WindowData& windowData, RenderType renderType);
  void shutdown();

  void run(std::unique_ptr<Game> game);

  IRenderer* getRenderer();
  WindowData getWindowSize() const;

private:
  Engine() = default;
  ~Engine();

  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  std::unique_ptr<CoreEngine> m_coreEngine;
};
