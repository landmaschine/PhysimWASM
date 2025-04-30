#pragma once

#include "core/common.hpp"

struct RigidBody_t {
  glm::vec2 position;
  float rotation;
  glm::vec2 velocity;
  float angularVelocity;
  float mass;
  float invMass;
  float inertia;
  float invInertia;
  glm::vec2 forceAccumulator;
  float torqueAccumulator;
} Rigidbody;


