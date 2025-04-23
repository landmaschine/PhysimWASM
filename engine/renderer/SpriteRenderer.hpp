#pragma once
#include "Core/common.hpp"
#include "Sprite.hpp"
#include "loadShader.hpp"

class SpriteRenderer {
public:
  SpriteRenderer();
  ~SpriteRenderer();

  void init(int screenWidth, int screenHeight);
  void resize(int screenWidth, int screenHeight);

  void begin();
  void end();

  void drawSprite(const Sprite& sprite);
  void drawSprite(const Texture* texture, const glm::vec2& position, 
                  const glm::vec2& size = glm::vec2(1.0f),
                  float rotation = 0.0f,
                  const glm::vec4& color = glm::vec4(1.0f));

  void setShader(const Shader& shader);
  Shader& getShader() { return m_shader; }

private:
  Shader m_shader;
  glm::mat4 m_projectionMatrix;
};
