#pragma once
#include "core/common.hpp"
#include "glad/glad.h"
#include "loadShader.hpp"

enum class RenderType {
  OPENGL,
  OPENGL_ES_3,
  SOFTWARE,
};

typedef struct WindowData_t {
  uint32_t width;
  uint32_t height;
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
};

