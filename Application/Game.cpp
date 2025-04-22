#include "Game.hpp"

Game::Game() {

}

Game::~Game() {

}

void Game::init() {

}

void Game::handleInput(const SDL_Event& event) {
  if(event.key.key == SDLK_ESCAPE) {
    m_running = false;
  }


}

void Game::update(double deltaTime) {

}

void Game::render(IRenderer* renderer) {
  renderer->render();
}
