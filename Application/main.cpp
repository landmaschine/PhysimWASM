#include "engine/core/core.hpp"
#include "Game.hpp"

int main(int, char**){
  auto& engine = Engine::getInstance();

  WindowData windowConfig = {800, 600, "Physim"};

  if(!engine.init(windowConfig, RenderType::OPENGL)) {
    CRITLOG("Failed to init engine");
    return 1;
  }

  auto game = std::make_unique<Game>();
  game->init();

  engine.run(std::move(game));

  return 0;
}
