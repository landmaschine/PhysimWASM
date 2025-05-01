#pragma once

#include "Core/common.hpp"
#include <functional>
#include <string>
#include <vector>
#include <memory>

enum class BodyType {
  STATIC,
  DYNAMIC
};

enum class ShapeType {
  CIRCLE,
  RECTANGLE
};

struct Shape {
  virtual ~Shape() = default;
  virtual ShapeType getType() const = 0;
};

struct CircleShape : public Shape {
  float radius;
  
  CircleShape(float r) : radius(r) {}
  
  ShapeType getType() const override { 
    return ShapeType::CIRCLE; 
  }
};

struct RectangleShape : public Shape {
  glm::vec2 size;
  
  RectangleShape(const glm::vec2& s) : size(s) {}
  
  ShapeType getType() const override { 
    return ShapeType::RECTANGLE; 
  }
};

struct AABB {
  glm::vec2 min;
  glm::vec2 max;
  
  AABB() : min(0.0f), max(0.0f) {}
  AABB(const glm::vec2& min, const glm::vec2& max) : min(min), max(max) {}
  
  bool overlaps(const AABB& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x &&
            min.y <= other.max.y && max.y >= other.min.y);
  }
  
  glm::vec2 getCenter() const {
    return (min + max) * 0.5f;
  }
  
  glm::vec2 getExtent() const {
    return (max - min) * 0.5f;
  }
};

struct RigidBody {
  BodyType type = BodyType::DYNAMIC;
  std::string id;
  bool active = true;
  
  glm::vec2 position = {0.0f, 0.0f};
  glm::vec2 prevPosition = {0.0f, 0.0f}; 
  float rotation = 0.0f;
  
  glm::vec2 velocity = {0.0f, 0.0f};
  glm::vec2 acceleration = {0.0f, 0.0f};
  float angularVelocity = 0.0f;
  
  float mass = 1.0f;
  float invMass = 1.0f;
  float inertia = 1.0f;
  float invInertia = 1.0f;
  
  float restitution = 0.5f;  
  float friction = 0.2f;
  
  glm::vec2 forceAccumulator = {0.0f, 0.0f};
  float torqueAccumulator = 0.0f;
  
  std::unique_ptr<Shape> shape;
  
  AABB aabb;
  
  static RigidBody createCircle(BodyType type, const glm::vec2& pos, float radius, float m = 1.0f) {
    RigidBody body;
    body.type = type;
    body.position = pos;
    body.prevPosition = pos;
    body.shape = std::make_unique<CircleShape>(radius);
    
    if (type == BodyType::STATIC) {
      body.mass = 0.0f;
      body.invMass = 0.0f;
      body.inertia = 0.0f;
      body.invInertia = 0.0f;
    } else {
      body.mass = m;
      body.invMass = 1.0f / m;
      body.inertia = 0.5f * m * radius * radius; 
      body.invInertia = 1.0f / body.inertia;
    }
    
    body.updateAABB();
    
    return body;
  }
  
  static RigidBody createRectangle(BodyType type, const glm::vec2& pos, const glm::vec2& size, float m = 1.0f) {
    RigidBody body;
    body.type = type;
    body.position = pos;
    body.prevPosition = pos;
    body.shape = std::make_unique<RectangleShape>(size);
    
    if (type == BodyType::STATIC) {
      body.mass = 0.0f;
      body.invMass = 0.0f;
      body.inertia = 0.0f;
      body.invInertia = 0.0f;
    } else {
      body.mass = m;
      body.invMass = 1.0f / m;
      body.inertia = (1.0f/12.0f) * m * (size.x*size.x + size.y*size.y); 
      body.invInertia = 1.0f / body.inertia;
    }
    
    body.updateAABB();
    
    return body;
  }
  
  void updateAABB() {
    if (!shape) return;
    
    switch (shape->getType()) {
      case ShapeType::CIRCLE: {
        CircleShape* circle = static_cast<CircleShape*>(shape.get());
        float r = circle->radius;
        aabb.min = position - glm::vec2(r, r);
        aabb.max = position + glm::vec2(r, r);
        break;
      }
      case ShapeType::RECTANGLE: {
        RectangleShape* rect = static_cast<RectangleShape*>(shape.get());
        glm::vec2 halfSize = rect->size * 0.5f;
        
        if (rotation == 0.0f) {
          aabb.min = position - halfSize;
          aabb.max = position + halfSize;
        } else {
          float cosAngle = std::cos(rotation);
          float sinAngle = std::sin(rotation);
          
          glm::vec2 corners[4] = {
            glm::vec2(halfSize.x * cosAngle - halfSize.y * sinAngle, 
                      halfSize.x * sinAngle + halfSize.y * cosAngle),
            glm::vec2(halfSize.x * cosAngle + halfSize.y * sinAngle, 
                      halfSize.x * sinAngle - halfSize.y * cosAngle),
            glm::vec2(-halfSize.x * cosAngle + halfSize.y * sinAngle, 
                      -halfSize.x * sinAngle - halfSize.y * cosAngle),
            glm::vec2(-halfSize.x * cosAngle - halfSize.y * sinAngle, 
                      -halfSize.x * sinAngle + halfSize.y * cosAngle)
          };
          
          aabb.min = aabb.max = position + corners[0];
          
          for (int i = 1; i < 4; ++i) {
            glm::vec2 corner = position + corners[i];
            aabb.min = glm::min(aabb.min, corner);
            aabb.max = glm::max(aabb.max, corner);
          }
        }
        break;
      }
    }
  }
  
  void applyForce(const glm::vec2& force) {
    forceAccumulator += force;
  }
  
  void applyForceAtPoint(const glm::vec2& force, const glm::vec2& point) {
    forceAccumulator += force;
    
    glm::vec2 arm = point - position;
    torqueAccumulator += (arm.x * force.y - arm.y * force.x);
  }
  
  void applyImpulse(const glm::vec2& impulse, const glm::vec2& contactPoint) {
    if (type == BodyType::STATIC) return;
    
    velocity += impulse * invMass;
    
    glm::vec2 arm = contactPoint - position;
    float angularImpulse = arm.x * impulse.y - arm.y * impulse.x;
    angularVelocity += angularImpulse * invInertia;
  }
  
  void clearForces() {
    forceAccumulator = glm::vec2(0.0f);
    torqueAccumulator = 0.0f;
  }
};

struct Collision {
  RigidBody* bodyA;
  RigidBody* bodyB;
  glm::vec2 normal;        
  glm::vec2 contactPoint;  
  float penetration;       
  bool hasCollision;
  
  Collision() : bodyA(nullptr), bodyB(nullptr), normal(0.0f), contactPoint(0.0f), 
                penetration(0.0f), hasCollision(false) {}
};
