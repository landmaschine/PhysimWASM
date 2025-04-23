#pragma once
#include "engine/Engine.hpp"
#include "engine/Renderer/Sprite.hpp"

class Game {
public:
  Game();
  ~Game();

  void init();
  void handleInput(const SDL_Event& event);
  void update(double deltaTime);
  void render(IRenderer* renderer);

  bool isRunning() const { return m_running; }

private:
  bool m_running = true;
  
  Texture m_backgroundTexture;
  Texture m_spriteTexture;
  
  Sprite m_backgroundSprite;
  Sprite* m_playerSprite = nullptr;
  
  glm::vec2 m_playerPosition = {100.0f, 100.0f};
  glm::vec2 m_playerVelocity = {0.0f, 0.0f};
  float m_playerSpeed = 200.0f;
  
  bool m_keyUp = false;
  bool m_keyDown = false;
  bool m_keyLeft = false;
  bool m_keyRight = false;
};