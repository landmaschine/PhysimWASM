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

  m_playerSprite.init(64.0f, 64.0f);
  m_playerSprite.setPosition(m_playerPosition);
  m_playerSprite.setTexture(&m_spriteTexture);
  
  m_playerPosition.x = winSize.width / 2.0f - 32.0f;
  m_playerPosition.y = winSize.height / 2.0f - 32.0f;
  
  LOG("Game initialized");
}

void Game::handleInput(const SDL_Event& event) {
  if (event.type == SDL_EVENT_KEY_DOWN) {
    switch(event.key.key) {
      case SDLK_ESCAPE:
        m_running = false;
        break;
      case SDLK_UP:
      case SDLK_W:
        m_keyUp = true;
        break;
      case SDLK_DOWN:
      case SDLK_S:
        m_keyDown = true;
        break;
      case SDLK_LEFT:
      case SDLK_A:
        m_keyLeft = true;
        break;
      case SDLK_RIGHT:
      case SDLK_D:
        m_keyRight = true;
        break;
      default:
        break;
    }
  }
  else if (event.type == SDL_EVENT_KEY_UP) {
    switch(event.key.key) {
      case SDLK_UP:
      case SDLK_W:
        m_keyUp = false;
        break;
      case SDLK_DOWN:
      case SDLK_S:
        m_keyDown = false;
        break;
      case SDLK_LEFT:
      case SDLK_A:
        m_keyLeft = false;
        break;
      case SDLK_RIGHT:
      case SDLK_D:
        m_keyRight = false;
        break;
      default:
        break;
    }
  }
}

void Game::update(double deltaTime) {
  m_playerVelocity = {0.0f, 0.0f};
  
  if (m_keyUp)
    m_playerVelocity.y += 1.0f;
  if (m_keyDown)
    m_playerVelocity.y -= 1.0f;
  if (m_keyLeft)
    m_playerVelocity.x -= 1.0f;
  if (m_keyRight)
    m_playerVelocity.x += 1.0f;
  
  if (m_playerVelocity.x != 0.0f && m_playerVelocity.y != 0.0f) {
    float length = sqrt(m_playerVelocity.x * m_playerVelocity.x + m_playerVelocity.y * m_playerVelocity.y);
    m_playerVelocity.x /= length;
    m_playerVelocity.y /= length;
  }
  
  m_playerPosition.x += m_playerVelocity.x * m_playerSpeed * static_cast<float>(deltaTime);
  m_playerPosition.y += m_playerVelocity.y * m_playerSpeed * static_cast<float>(deltaTime);
  
  const WindowData& winSize = Engine::getInstance().getWindowSize();
  
  if (m_playerPosition.x < 0)
    m_playerPosition.x = 0;
  if (m_playerPosition.x > winSize.width - 64)
    m_playerPosition.x = winSize.width - 64;
  if (m_playerPosition.y < 0)
    m_playerPosition.y = 0;
  if (m_playerPosition.y > winSize.height - 64)
    m_playerPosition.y = winSize.height - 64;
  
  m_playerSprite.setPosition(m_playerPosition);
}

void Game::render(IRenderer* renderer) {
  if (!renderer) return;
  
  //renderer->drawSprite(&m_backgroundTexture, {0.0f, 0.0f}, 
  //                     {static_cast<float>(Engine::getInstance().getWindowSize().width), 
  //                      static_cast<float>(Engine::getInstance().getWindowSize().height)});
  
  renderer->drawSprite(&m_spriteTexture, m_playerPosition, {64.0f, 64.0f});
}