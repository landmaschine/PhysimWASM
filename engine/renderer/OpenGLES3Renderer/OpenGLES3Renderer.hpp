#pragma once
#include "engine/Core/common.hpp"
#include "Renderer/Inter_Renderer.hpp"
#include "Renderer/SpriteRenderer.hpp"
#include "Platform/SDLCompatability.hpp"

class OpenGLES3Renderer : public IRenderer {
public:
  OpenGLES3Renderer(const std::shared_ptr<Context> ctx);
  ~OpenGLES3Renderer() {
    if(m_glContext) {
      SDLCompat::DestroyGLContext(m_glContext);
    }

    if(m_window) {
      SDL_DestroyWindow(m_window);
    }
  };

  void beginFrame() override;
  void render() override;
  void endFrame() override;
  void resize() override;
  void setShader(const Shader&) override;

  void initSpriteRenderer() override;
  void drawSprite(const Texture* texture, const glm::vec2& position, 
                 const glm::vec2& size, float rotation, 
                 const glm::vec4& color) override;

private:
  SDL_Window* m_window;
  SDL_GLContext m_glContext;
  Shader* m_shader;
  std::shared_ptr<Context> m_ctx;

  std::unique_ptr<SpriteRenderer> m_spriteRenderer;
};
