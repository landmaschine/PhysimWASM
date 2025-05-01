#pragma once

#include "body.hpp"
#include <unordered_map>
#include <vector>
#include <functional>

class SpatialHash {
public:
  SpatialHash(float cellSize = 100.0f) : m_cellSize(cellSize) {}
  
  void insert(RigidBody* body) {
    if (!body) return;
    
    auto cells = getCellsForAABB(body->aabb);
    for (const auto& cell : cells) {
      m_cells[cell].push_back(body);
    }
  }
  
  void clear() {
    m_cells.clear();
  }
  
  std::vector<RigidBody*> queryPotentialCollisions(RigidBody* body) {
    if (!body) return {};
    
    std::vector<RigidBody*> result;
    std::unordered_map<RigidBody*, bool> visited;
    
    auto cells = getCellsForAABB(body->aabb);
    for (const auto& cell : cells) {
      auto it = m_cells.find(cell);
      if (it != m_cells.end()) {
        for (RigidBody* other : it->second) {
          if (other != body && !visited[other]) {
            visited[other] = true;
            result.push_back(other);
          }
        }
      }
    }
    
    return result;
  }
  
  std::vector<std::pair<RigidBody*, RigidBody*>> queryAllPotentialCollisions() {
      std::vector<std::pair<RigidBody*, RigidBody*>> result;
      std::unordered_map<uint64_t, bool> collisionPairs;
      
      for (const auto& cellEntry : m_cells) {
        const auto& bodies = cellEntry.second;
        
        for (size_t i = 0; i < bodies.size(); ++i) {
          for (size_t j = i + 1; j < bodies.size(); ++j) {
            RigidBody* bodyA = bodies[i];
            RigidBody* bodyB = bodies[j];
            
            uint64_t pairId = getPairId(bodyA, bodyB);
            
            if (collisionPairs.find(pairId) != collisionPairs.end()) {
                continue;
            }
            
            collisionPairs[pairId] = true;
            result.emplace_back(bodyA, bodyB);
        }
      }
    }
    
    return result;
  }
    
private:
  float m_cellSize;
  std::unordered_map<uint64_t, std::vector<RigidBody*>> m_cells;
  
  uint64_t hashCell(int x, int y) const {
    return (static_cast<uint64_t>(x) << 32) | static_cast<uint64_t>(y);
  }
  
  std::vector<uint64_t> getCellsForAABB(const AABB& aabb) {
    std::vector<uint64_t> cells;
    
    int minCellX = static_cast<int>(std::floor(aabb.min.x / m_cellSize));
    int minCellY = static_cast<int>(std::floor(aabb.min.y / m_cellSize));
    int maxCellX = static_cast<int>(std::floor(aabb.max.x / m_cellSize));
    int maxCellY = static_cast<int>(std::floor(aabb.max.y / m_cellSize));
    
    for (int y = minCellY; y <= maxCellY; ++y) {
      for (int x = minCellX; x <= maxCellX; ++x) {
        cells.push_back(hashCell(x, y));
      }
    }
    
    return cells;
  }
    
  uint64_t getPairId(RigidBody* a, RigidBody* b) const {
    uintptr_t addrA = reinterpret_cast<uintptr_t>(a);
    uintptr_t addrB = reinterpret_cast<uintptr_t>(b);
    
    if (addrA > addrB) {
      std::swap(addrA, addrB);
    }
    
    return (static_cast<uint64_t>(addrA) << 32) | static_cast<uint64_t>(addrB);
  }
};
