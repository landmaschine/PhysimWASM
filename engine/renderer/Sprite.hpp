#pragma once
#include "core/common.hpp"

class Texture {
public:
  Texture() = default;
  ~Texture();

  bool loadFromFile(const std::string& path);
  void bind(unsigned int slot = 0) const;
  void unbind() const;

  inline unsigned int getID() const { return m_textureID; }
  inline int getWidth() const { return m_width; }
  inline int getHeight() const { return m_height; }

private:
  unsigned int m_textureID = 0;
  int m_width;
  int m_height;
  int m_channels;
};

class Sprite {
public:
  Sprite();
  ~Sprite();

  void init(float width, float height);
  void setTexture(const Texture* texture);
  void setPosition(const glm::vec2& position);
  void setSize(const glm::vec2& size);
  void setRotation(float rotation);
  void setColor(const glm::vec4& color);

  const glm::vec2& getPosition() const { return m_position; }
  const glm::vec2& getSize() const { return m_size; }
  float getRotation() const { return m_rotation; }
  const glm::vec4& getColor() const { return m_color; }
  const Texture* getTexture() const { return m_texture; }

  unsigned int getVAO() const { return m_VAO; }
  unsigned int getVBO() const { return m_VBO; }
  unsigned int getEBO() const { return m_EBO; }

private:
  void createBuffers();

  unsigned int m_VAO = 0;
  unsigned int m_VBO = 0;
  unsigned int m_EBO = 0;

  glm::vec2 m_position = { 0.0f, 0.0f };
  glm::vec2 m_size = { 1.0f, 1.0f };
  float m_rotation = 0.0f;
  glm::vec4 m_color = { 1.0f, 1.0f, 1.0f, 1.0f };

  const Texture* m_texture = nullptr;
};
