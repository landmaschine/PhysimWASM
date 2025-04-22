#pragma once
#include "engine/Engine.hpp"

class Game {
public:
  Game();
  ~Game();

  void init();
  void handleInput(const SDL_Event& event);
  void update(double update);
  void render(IRenderer* renderer);

  bool isRunning() const { return m_running; }

private:
  bool m_running = true;
};
