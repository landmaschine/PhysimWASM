#include "engine/Core/core.hpp"
#include "Game.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

int main(int, char**){
  auto& engine = Engine::getInstance();

#ifdef __EMSCRIPTEN__
  int width, height;
  emscripten_get_canvas_element_size("#canvas", &width, &height);
  
  WindowData windowConfig = {
    static_cast<uint32_t>(width),
    static_cast<uint32_t>(height),
    "Physim WASM"
  };
  
  LOG("Canvas size:", windowConfig.width, "x", windowConfig.height);
  
  if(!engine.init(windowConfig, RenderType::OPENGL_ES_3)) {
    CRITLOG("Failed to init engine");
    return 1;
  }
#else
  WindowData windowConfig = {800, 600, "Physim"};

  if(!engine.init(windowConfig, RenderType::OPENGL)) {
    CRITLOG("Failed to init engine");
    return 1;
  }
#endif

  auto game = std::make_unique<Game>();
  game->init();

  engine.run(std::move(game));

  return 0;
}