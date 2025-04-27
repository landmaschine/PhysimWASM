#include "OpenGLES3Renderer.hpp"

OpenGLES3Renderer::OpenGLES3Renderer(const std::shared_ptr<Context> ctx) {
  LOG("Init OpenGL ES3 Renderer");

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles");
  SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1");

  m_window = SDL_CreateWindow(ctx->window_data.name.c_str(), ctx->window_data.width, ctx->window_data.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if(!m_window) {
    CRITLOG("Failed to create SDL3 Window");
    exit(1);
  }

  m_glContext = SDL_GL_CreateContext(m_window);
  if(!m_glContext) {
    CRITLOG("Failed to create SDL3 OpenGL Context");
    exit(1);
  }

  if(!gladLoadGLES2Loader((GLADloadproc)(SDL_GL_GetProcAddress))) {
    CRITLOG("Failed to load OpenGL ES");
    exit(1);
  }

  m_ctx = ctx;

  const uint8_t* version = glGetString(GL_VERSION);  
  LOG("OpenGL Version: ", (version ? (const char*)version : "Unknown"));
  
  initSpriteRenderer();
}

void OpenGLES3Renderer::beginFrame() {
  glClearColor(0.3, 0.3, 0.3, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_ctx->window_data.width, m_ctx->window_data.height);
  
  if (m_spriteRenderer) {
    m_spriteRenderer->begin();
  }
}

void OpenGLES3Renderer::render() {

}

void OpenGLES3Renderer::endFrame() {
  if (m_spriteRenderer) {
    m_spriteRenderer->end();
  }
  
  SDL_GL_SwapWindow(m_window);
}

void OpenGLES3Renderer::resize() {
  SDL_GetWindowSizeInPixels(m_window, (int*)&m_ctx->window_data.width, (int*)&m_ctx->window_data.height);
  
  if (m_spriteRenderer) {
    m_spriteRenderer->resize(m_ctx->window_data.width, m_ctx->window_data.height);
  }
}

void OpenGLES3Renderer::setShader(const Shader& shader) {
  if (m_spriteRenderer) {
    m_spriteRenderer->setShader(shader);
  }
}

void OpenGLES3Renderer::initSpriteRenderer() {
  m_spriteRenderer = std::make_unique<SpriteRenderer>();
  m_spriteRenderer->init(m_ctx->window_data.width, m_ctx->window_data.height);
  LOG("Sprite renderer initialized");
}

void OpenGLES3Renderer::drawSprite(const Texture* texture, const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color) {
  if (m_spriteRenderer) {
    m_spriteRenderer->drawSprite(texture, position, size, rotation, color);
  }
}

void OpenGLES3Renderer::drawSprite(const Sprite& sprite) {
  if(m_spriteRenderer) {
    m_spriteRenderer->drawSprite(sprite);
  }
}
