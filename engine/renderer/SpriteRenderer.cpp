#include "SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer() {}

SpriteRenderer::~SpriteRenderer() {}

void SpriteRenderer::init(int screenWidth, int screenHeight) {
  m_shader.init("shaders/OpenGL/vertex.vert", "shaders/OpenGL/fragment.frag");

  resize(screenWidth, screenHeight);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SpriteRenderer::resize(int screenWidth, int screenHeight) {
  m_projectionMatrix = glm::ortho(
    0.0f, static_cast<float>(screenWidth),
    static_cast<float>(screenHeight), 0.0f,
    -1.0f, 1.0f
  );
}

void SpriteRenderer::begin() {
  m_shader.use();
  m_shader.setMat4("projection", m_projectionMatrix);
}

void SpriteRenderer::end() {

}

void SpriteRenderer::drawSprite(const Sprite& sprite) {
  if(sprite.getTexture() == nullptr) {
    WARLOG("Attempting to draw sprite with no texture");
    return;
  }

  glm::mat4 model = glm::mat4(1.0f);

  model = glm::translate(model, glm::vec3(
    sprite.getPosition().x - (sprite.getSize().x / 2.0f),
    sprite.getPosition().y - (sprite.getSize().y / 2.0f),
    0.0f));

  if(sprite.getRotation() != 0.0f) {
    model = glm::translate(model, glm::vec3(sprite.getSize().x * 0.5f, sprite.getSize().y * 0.5f, 0.0f));
    model = glm::rotate(model, sprite.getRotation(), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-sprite.getSize().x * 0.5f, -sprite.getSize().y * 0.5f, 0.0f));
  }

  model = glm::scale(model, glm::vec3(sprite.getSize(), 1.0f));

  m_shader.setMat4("model", model);
  m_shader.setVec4("color", sprite.getColor());

  sprite.getTexture()->bind();

  glBindVertexArray(sprite.getVAO());
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void SpriteRenderer::drawSprite(const Texture* texture, const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color) {
    if (texture == nullptr) {
      WARLOG("Attempting to draw sprite with no texture");
      return;
    }
  
    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model, glm::vec3(
      position.x - (size.x / 2.0f), 
      position.y - (size.y / 2.0f), 
      0.0f));
    
    if (rotation != 0.0f) {
      model = glm::translate(model, glm::vec3(size.x * 0.5f, size.y * 0.5f, 0.0f));
      model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
      model = glm::translate(model, glm::vec3(-size.x * 0.5f, -size.y * 0.5f, 0.0f));
    }
    
    model = glm::scale(model, glm::vec3(size, 1.0f));
  
    Sprite tempSprite;
    tempSprite.init(size.x, size.y);
    tempSprite.setTexture(texture);
  
    m_shader.setMat4("model", model);
    m_shader.setVec4("color", color);
  
    texture->bind();
  
    glBindVertexArray(tempSprite.getVAO());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SpriteRenderer::setShader(const Shader& shader) {
  m_shader = shader;
}
