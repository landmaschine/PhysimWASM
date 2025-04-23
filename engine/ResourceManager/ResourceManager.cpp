#include "ResourceManager.hpp"
#include "Engine.hpp"

ResourceManager::ResourceManager() {
  LOG("ResourceManager initialized");
}

ResourceManager::~ResourceManager() {
  cleanup();
}

bool ResourceManager::loadTexture(const std::string& name, const std::string& filepath) {
  if(m_textures.find(name) != m_textures.end()) {
    WARLOG("Texture '", name, "' already exists");
    return true;
  }

  auto texture = std::make_unique<Texture>();
  if(texture->loadFromFile(filepath)) {
    m_textures[name] = std::move(texture);
    LOG("Loaded texture: ", name, " from ", filepath);
    return true;
  }

  ERRLOG("Failed to load texture: ", name, " from ", filepath);
  return false;
}

Texture* ResourceManager::getTexture(const std::string& name) {
  auto it = m_textures.find(name);
  if(it != m_textures.end()) {
    return it->second.get();
  }

  WARLOG("Texture '", name, "' not found");
  return nullptr;
}

void ResourceManager::unloadTexture(const std::string& name) {
  auto it = m_textures.find(name);
  if(it != m_textures.end()) {
    bool isUsed = false;
    for(const auto& spritePair : m_sprites) {
      if(spritePair.second->getTexture() == it->second.get()) {
        isUsed = true;
        WARLOG("Cannot unload texture '", name, "' as it's used by sprite '", spritePair.first, "'");
        break;
      }
    }

    if(!isUsed) {
      m_textures.erase(it);
      LOG("Unloaded texture: ", name);
    }
  }
}

Sprite* ResourceManager::createSprite(const std::string& name, float width, float height, const std::string& textureName) {
  if(m_sprites.find(name) != m_sprites.end()) {
    WARLOG("Sprite '", name, "' already exists");
    return m_sprites[name].get();
  }

  Texture* texture = getTexture(textureName);
  if(!texture) {
    ERRLOG("Cannot create Sprite '", name, "': texture '", textureName, "' not found");
    return nullptr;
  }

  auto sprite = std::make_unique<Sprite>();
  sprite->init(width, height);
  sprite->setTexture(texture);

  m_sprites[name] = std::move(sprite);
  LOG("Created sprite: ", name);
  return m_sprites[name].get();
}

Sprite* ResourceManager::createSpriteWithNativeSize(const std::string& name, const std::string& textureName) {
  if (m_sprites.find(name) != m_sprites.end()) {
    WARLOG("Sprite '", name, "' already exists");
    return m_sprites[name].get();
  }

  Texture* texture = getTexture(textureName);
  if (!texture) {
    ERRLOG("Cannot create sprite '", name, "': texture '", textureName, "' not found");
    return nullptr;
  }

  auto sprite = std::make_unique<Sprite>();
  sprite->initWithNativeSize(texture);

  m_sprites[name] = std::move(sprite);
  glm::vec2 size = m_sprites[name]->getSize();
  LOG("Created sprite: ", name, " with native texture size ", size.x, "x", size.y);
  return m_sprites[name].get();
}

Sprite* ResourceManager::getSprite(const std::string& name) {
  auto it = m_sprites.find(name);
  if (it != m_sprites.end()) {
    return it->second.get();
  }

  WARLOG("Sprite '", name, "' not found");
  return nullptr;
}

void ResourceManager::destroySprite(const std::string& name) {
  auto it = m_sprites.find(name);
  if(it != m_sprites.end()) {
    m_sprites.erase(it);
    LOG("Destroyed sprite: ", name);
  }
}

void ResourceManager::cleanup() {
  m_sprites.clear();
  m_textures.clear();
  LOG("Resource Manager cleanup complete");
}

ResourceManager::MemoryStats ResourceManager::getMemoryStats() const {
  MemoryStats stats = {0, 0, 0};
  
  stats.textureCount = m_textures.size();
  stats.spriteCount = m_sprites.size();

  for(const auto& texturePair : m_textures) {
    const Texture* texture = texturePair.second.get();
    size_t pixelSize = texture->getChannels();
    size_t textureSize = texture->getWidth() * texture->getHeight() * pixelSize;
    stats.totalTextureMemory += textureSize;
  }

  return stats;
}
