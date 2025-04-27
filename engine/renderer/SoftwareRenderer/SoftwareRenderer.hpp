#pragma once

#include "Renderer/Inter_Renderer.hpp"

class SoftwareRenderer : public IRenderer {
public:
  SoftwareRenderer(const std::shared_ptr<Context> ctx);
  ~SoftwareRenderer() {
    
  }

  void beginFrame() override;
  void render() override;
  void endFrame() override;
  void resize() override;
  void setShader(const Shader& shader) override;

  void initSpriteRenderer() override;
  void drawSprite(const Sprite& sprite) override;
  void drawSprite(const Texture* texture, const glm::vec2& position, const glm::vec2& size = glm::vec2(1.0f), float rotation = 0.0f, const glm::vec4& color = glm::vec4(1.0f)) override;

private:

};
