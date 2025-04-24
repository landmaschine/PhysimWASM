#include "Engine.hpp"
#include "Core/core.hpp"
#include "Application/Game.hpp"

#ifdef __EMSCRIPTEN__
#include "Platform/WasmAdapter.hpp"
#endif

Engine& Engine::getInstance() {
  static Engine instance;
  return instance;
}

bool Engine::init(const WindowData& windowData, RenderType renderType) {
  auto ctx = std::make_shared<Context>();
  ctx->window_data = windowData;

#ifdef __EMSCRIPTEN__
  ctx->api = RenderType::OPENGL_ES_3;
  LOG("WASM build detected, forcing OpenGL ES 3 renderer");
#else
  ctx->api = renderType;
#endif

  m_coreEngine = std::make_unique<CoreEngine>(ctx);
  return true;
}

void Engine::shutdown() {

}

void Engine::run(std::unique_ptr<Game> game) {
  #ifdef __EMSCRIPTEN__
  if(m_coreEngine) {
    WasmAdapter::getInstance().init(this, std::move(game));
    WasmAdapter::getInstance().run();
  }
#else
  if(m_coreEngine) {
    m_coreEngine->setGame(std::move(game));
    m_coreEngine->run();
  }
#endif
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
