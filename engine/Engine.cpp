#include "Engine.hpp"
#include "core/core.hpp"
#include "Application/Game.hpp"

Engine& Engine::getInstance() {
  static Engine instance;
  return instance;
}

bool Engine::init(const WindowData& windowData, RenderType renderType) {
  auto ctx = std::make_shared<Context>();
  ctx->window_data = windowData;
  ctx->api = renderType;

  m_coreEngine = std::make_unique<CoreEngine>(ctx);
  return true;
}

void Engine::shutdown() {

}

void Engine::run(std::unique_ptr<Game> game) {
  if(m_coreEngine) {
    m_coreEngine->setGame(std::move(game));
    m_coreEngine->run();
  }
}

IRenderer* Engine::getRenderer() {
  if(m_coreEngine) {
    return m_coreEngine->getRenderer();
  }
  return nullptr;
}

WindowData Engine::getWindowSize() const {
  if(m_coreEngine) {
    return m_coreEngine->getWindowSize();
  }
  return {0, 0, ""};
}

Engine::~Engine() {
  shutdown();
}
