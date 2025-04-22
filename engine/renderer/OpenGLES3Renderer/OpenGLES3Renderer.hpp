#pragma once
#include "engine/core/common.hpp"
#include "renderer/Inter_Renderer.hpp"

class OpenGLES3Renderer : public IRenderer {
public:
  OpenGLES3Renderer(const std::shared_ptr<Context> ctx);
  ~OpenGLES3Renderer() {
    if(m_glContext) {
      SDL_GL_DestroyContext(m_glContext);
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

private:
  SDL_Window* m_window;
  SDL_GLContext m_glContext;
  Shader* m_shader;
  std::shared_ptr<Context> m_ctx;

};
