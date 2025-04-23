#include "Game.hpp"

Game::Game() {

}

Game::~Game() {

}

void Game::init() {
  LOG("Initializing game...");
  
  auto& engine = Engine::getInstance();
  IRenderer* renderer = engine.getRenderer();
  ResourceManager* resourceManager = engine.getResourceManager();
  
  if (!renderer) {
    ERRLOG("No renderer available!");
    return;
  }

  if(!resourceManager) {
    ERRLOG("No resource manager available!");
    return;
  }
  
  renderer->initSpriteRenderer();
  
  if (!resourceManager->loadTexture("player", "assets/textures/white_particle.png")) {
    WARLOG("Failed to load player texture, using fallback color");
  }
  
  const WindowData& winSize = engine.getWindowSize();

  m_playerPosition.x = winSize.width / 2.0f;
  m_playerPosition.y = winSize.height / 2.0f;

  m_playerSprite = resourceManager->createSpriteWithNativeSize("playerSprite", "player");

  if(m_playerSprite) {
    m_playerSprite->setPosition(m_playerPosition);
    LOG("Player sprite created with size: ", m_playerSprite->getSize().x, "x", m_playerSprite->getSize().y);
  } else {
    ERRLOG("Failed to create player sprite");
  }
  
  LOG("Game initialized");
}

void Game::handleInput(const SDL_Event& event) {
  SDL_GetMouseState(&m_playerPosition.x, &m_playerPosition.y);
}

void Game::update(double deltaTime) {
  if(m_playerSprite) {
    m_playerSprite->setPosition(m_playerPosition);
  }
}

void Game::render(IRenderer* renderer) {
  if (!renderer) return;
  
  renderer->drawSprite(m_playerSprite->getTexture(), m_playerPosition, m_playerSprite->getSize() / 6.0f);
}