#include "Sprite.hpp"
#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

Texture::~Texture() {
  if(m_textureID > 0) {
    glDeleteTextures(1, &m_textureID);
  }
}

bool Texture::loadFromFile(const std::string& path) {
  if(m_textureID > 0) {
    glDeleteTextures(1, &m_textureID);
  }

  glGenTextures(1, &m_textureID);
  glBindTexture(GL_TEXTURE_2D, m_textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);

  if(data) {
    GLenum format = GL_RGB;
    if(m_channels == 1) {
      format = GL_RED;
    } else if(m_channels == 3) {
      format = GL_RGB;
    } else if(m_channels == 4) {
      format = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    LOG("Loaded texture: ", path, " (", m_width, "x", m_height, ", ", m_channels, " channels)");

    stbi_image_free(data);
    return true;
  } else {
    ERRLOG("Failed to load texture: ", path);
    stbi_image_free(data);
    return false;
  }
}

void Texture::bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::unbind() const {
  glBindTexture(GL_TEXTURE_2D, 0);
}

Sprite::Sprite() {
  createBuffers();
}

Sprite::~Sprite() {
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO);
  glDeleteBuffers(1, &m_EBO);
}

void Sprite::init(float width, float height) {
  m_size = { width, height };
}

void Sprite::initWithNativeSize(const Texture* texture) {
  if(!texture) {
    WARLOG("Cannot initialize sprite with null texture");
    return;
  }

  m_texture = texture;
  m_size = texture->getNativeSize();

  LOG("Initialized sprite with native texture size: ", m_size.x, "x", m_size.y);
}

void Sprite::setTexture(const Texture* texture) {
  m_texture = texture;
}

void Sprite::setPosition(const glm::vec2& position) {
  m_position = position;
}

void Sprite::setSize(const glm::vec2& size) {
  m_size = size;
}

void Sprite::setRotation(float rotation) {
  m_rotation = rotation;
}

void Sprite::setColor(const glm::vec4& color) {
  m_color = color;
}

void Sprite::createBuffers() {
  float vertices[] = {
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
  };

  unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
  };

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);
  glGenBuffers(1, &m_EBO);

  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
