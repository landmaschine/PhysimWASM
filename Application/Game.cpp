#include "Game.hpp"

Game::Game() {

}

Game::~Game() {

}

void Game::init() {
  LOG("Initializing game...");
  
  auto& engine = Engine::getInstance();
  IRenderer* renderer = engine.getRenderer();
  
  if (!renderer) {
    ERRLOG("No renderer available!");
    return;
  }
  
  renderer->initSpriteRenderer();
  
  bool loadSuccess = true;
  
  if (!renderer->loadTexture("assets/textures/particle.png", m_spriteTexture)) {
    WARLOG("Failed to load player texture, using fallback color");
    loadSuccess = false;
  }
  
  const WindowData& winSize = engine.getWindowSize();

  m_playerPosition.x = winSize.width / 2.0f - 32.0f;
  m_playerPosition.y = winSize.height / 2.0f - 32.0f;

  m_playerSprite.init(64.0f, 64.0f);
  m_playerSprite.setPosition(m_playerPosition);
  m_playerSprite.setTexture(&m_spriteTexture);
  
  LOG("Game initialized");
}

void Game::handleInput(const SDL_Event& event) {
  SDL_GetMouseState(&m_playerPosition.x, &m_playerPosition.y);
}

void Game::update(double deltaTime) {  
  m_playerSprite.setPosition(m_playerPosition);
}

void Game::render(IRenderer* renderer) {
  if (!renderer) return;
  
  renderer->drawSprite(&m_spriteTexture, m_playerPosition, {64.0f, 64.0f});
}