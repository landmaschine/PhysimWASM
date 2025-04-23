#pragma once

#include <unordered_map>

#include "Core/common.hpp"
#include "Renderer/Sprite.hpp"

class ResourceManager {
public:
  ResourceManager();
  ~ResourceManager();

  bool loadTexture(const std::string& name, const std::string& filepath);
  Texture* getTexture(const std::string& name);
  void unloadTexture(const std::string& name);

  Sprite* createSprite(const std::string& name, float width, float height, const std::string& textureName);
  Sprite* createSpriteWithNativeSize(const std::string& name, const std::string& textureName);
  Sprite* getSprite(const std::string& name);
  void destroySprite(const std::string& name);

  void cleanup();

  struct MemoryStats {
    size_t totalTextureMemory;
    size_t textureCount;
    size_t spriteCount;
  };

  MemoryStats getMemoryStats() const;

private:
  std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
  std::unordered_map<std::string, std::unique_ptr<Sprite>> m_sprites;
};
