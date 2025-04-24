#pragma once
#include "Core/common.hpp"
#include "glad/glad.h"
#include "loadShader.hpp"
#include "Sprite.hpp"

enum class RenderType {
  OPENGL,
  OPENGL_ES_3,
  SOFTWARE,
};

typedef struct WindowData_t {
  int width;
  int height;
  std::string name;
} WindowData;

typedef struct Context_t {
  WindowData window_data;
  RenderType api;
  Shader shader;
} Context; 

class IRenderer {
public:
  virtual ~IRenderer() = default;
  virtual void beginFrame() = 0;
  virtual void render() = 0;
  virtual void endFrame() = 0;
  virtual void resize() = 0;
  virtual void setShader(const Shader&) = 0;

  virtual void initSpriteRenderer() = 0;
  virtual void drawSprite(const Texture* texture, const glm::vec2& position, 
    const glm::vec2& size = glm::vec2(1.0f),
    float rotation = 0.0f, 
    const glm::vec4& color = glm::vec4(1.0f)) = 0;
};
