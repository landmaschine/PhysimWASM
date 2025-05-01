#pragma once

#include "Core/common.hpp"
#include "body.hpp"
#include "SpatialHash.hpp"
#include <vector>
#include <memory>
#include <functional>

class PhysicsEngine;

enum class IntegrationMethod {
  VERLET,
  LEAPFROG
};

class Solver {
public:
  virtual ~Solver() = default;
  virtual void integrate(std::vector<RigidBody>& bodies, float dt) = 0;
};

class VerletSolver : public Solver {
public:
  VerletSolver() = default;
  ~VerletSolver() override = default;

  void integrate(std::vector<RigidBody>& bodies, float dt) override;
private:
  glm::vec2 m_gravity = {0.0f, 9.81f};
};

class LeapFrogSolver : public Solver {
public:
  LeapFrogSolver() = default;
  ~LeapFrogSolver() override = default;

  void integrate(std::vector<RigidBody>& bodies, float dt) override;
private:
  glm::vec2 m_gravity = {0.0f, 9.81f};
};

bool detectCollision(const RigidBody& bodyA, const RigidBody& bodyB, Collision& collision);
bool circleVsCircle(const RigidBody& bodyA, const RigidBody& bodyB, Collision& collision);
bool rectangleVsRectangle(const RigidBody& bodyA, const RigidBody& bodyB, Collision& collision);
bool circleVsRectangle(const RigidBody& bodyA, const RigidBody& bodyB, Collision& collision);

class PhysicsEngine {
public:
  PhysicsEngine();
  ~PhysicsEngine();

  size_t addBody(const RigidBody& body);
  void removeBody(size_t index);
  RigidBody* getBody(size_t index);
  void setIntegrationMethod(IntegrationMethod method);
  void setGravity(const glm::vec2& gravity);
  void setSpatialHashCellSize(float cellSize);
  void update(float dt);

  using CollisionCallback = std::function<void(const Collision&)>;
  void setCollisionCallback(CollisionCallback callback);

  void debugDraw();
private:
  struct Config {
    float gravity = 9.81f;
    glm::vec2 gravityVec = {0.0f, 9.81f};
    float spatialHashCellSize = 100.f;
    int velocityIterations = 8;
    int positionIterations = 3;
    float damping = 0.99f;
  } m_config;

  std::vector<RigidBody> m_bodies;
  std::unique_ptr<Solver> m_solver;
  SpatialHash m_spatialHash;
  CollisionCallback m_collisionCallback;
  IntegrationMethod m_integrationMethod = IntegrationMethod::VERLET;

  void broadPhaseCollision();
  void narrowPhaseCollision();
  void resolveCollisions();
  void integrateVelocities(float dt);
  void integratePositions(float dt);
  void updateAABBs();

  void resolveCollision(Collision& collision);
  void updateSpatialHash();
  std::vector<std::pair<RigidBody*, RigidBody*>> m_potentialCollisions;
  std::vector<Collision> m_collisions;
};
