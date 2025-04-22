#pragma once
#include <memory>

#include "Inter_Renderer.hpp"
#include "OpenGLRenderer/OpenGLRenderer.hpp"
#include "OpenGLES3Renderer/OpenGLES3Renderer.hpp"

inline std::unique_ptr<IRenderer> createRenderer(const std::shared_ptr<Context> ctx) {
  switch (ctx->api) {
    case RenderType::OPENGL:
      return std::make_unique<OpenGLRenderer>(ctx);
    case RenderType::OPENGL_ES_3:
      return std::make_unique<OpenGLES3Renderer>(ctx);
//    case RenderType::SOFTWARE:
//      return std::unique_ptr<SoftwareRenderer>(ctx);
      default: 
        WARLOG("Unknown Renderer, defaulting to Software Rendering.");
        //return std::unique_ptr<SoftwareRenderer>(ctx);
        return nullptr;
  }
}
