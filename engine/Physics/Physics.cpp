#include "Physics.hpp"

void VerletSolver::integrate(std::vector<RigidBody>& bodies, float dt) {
  for (auto& body : bodies) {
    if (body.type == BodyType::STATIC || !body.active) continue;
    
    body.applyForce(body.mass * m_gravity);
    
    glm::vec2 oldPosition = body.position;
    
    glm::vec2 acceleration = body.forceAccumulator * body.invMass;
    body.velocity += acceleration * dt;
    
    body.position = 2.0f * body.position - body.prevPosition + acceleration * dt * dt;
    
    body.prevPosition = oldPosition;
    
    float angularAcceleration = body.torqueAccumulator * body.invInertia;
    body.angularVelocity += angularAcceleration * dt;
    body.rotation += body.angularVelocity * dt;
    
    body.clearForces();
  }
}

void LeapFrogSolver::integrate(std::vector<RigidBody>& bodies, float dt) {
  for (auto& body : bodies) {
    if (body.type == BodyType::STATIC || !body.active) continue;
    
    body.applyForce(body.mass * m_gravity);
    
    glm::vec2 acceleration = body.forceAccumulator * body.invMass;
    
    body.velocity += acceleration * (dt * 0.5f);
    
    body.position += body.velocity * dt;
    
    body.velocity += acceleration * (dt * 0.5f);
    
    float angularAcceleration = body.torqueAccumulator * body.invInertia;
    body.angularVelocity += angularAcceleration * dt;
    body.rotation += body.angularVelocity * dt;
    
    body.clearForces();
  }
}

bool detectCollision(const RigidBody& bodyA, const RigidBody& bodyB, Collision& collision) {
  if (!bodyA.shape || !bodyB.shape) {
    return false;
  }
  
  if (!bodyA.aabb.overlaps(bodyB.aabb)) {
    return false;
  }
  
  ShapeType typeA = bodyA.shape->getType();
  ShapeType typeB = bodyB.shape->getType();
  
  if (typeA == ShapeType::CIRCLE && typeB == ShapeType::CIRCLE) {
    return circleVsCircle(bodyA, bodyB, collision);
  } else if (typeA == ShapeType::RECTANGLE && typeB == ShapeType::RECTANGLE) {
    return rectangleVsRectangle(bodyA, bodyB, collision);
  } else if (typeA == ShapeType::CIRCLE && typeB == ShapeType::RECTANGLE) {
    return circleVsRectangle(bodyA, bodyB, collision);
  } else if (typeA == ShapeType::RECTANGLE && typeB == ShapeType::CIRCLE) {
    bool result = circleVsRectangle(bodyB, bodyA, collision);
    if (result) {
      std::swap(collision.bodyA, collision.bodyB);
      collision.normal = -collision.normal;
    }
    return result;
  }
  
  return false;
}

bool circleVsCircle(const RigidBody& bodyA, const RigidBody& bodyB, Collision& collision) {
  const CircleShape* circleA = static_cast<const CircleShape*>(bodyA.shape.get());
  const CircleShape* circleB = static_cast<const CircleShape*>(bodyB.shape.get());
  
  glm::vec2 direction = bodyB.position - bodyA.position;
  float distanceSquared = glm::dot(direction, direction);
  
  float radiusSum = circleA->radius + circleB->radius;
  
  if (distanceSquared > radiusSum * radiusSum) {
    return false;
  }
  
  float distance = std::sqrt(distanceSquared);
  
  collision.bodyA = const_cast<RigidBody*>(&bodyA);
  collision.bodyB = const_cast<RigidBody*>(&bodyB);
  
  if (distance > 0.0f) {
    collision.normal = direction / distance; 
  } else {
    collision.normal = glm::vec2(1.0f, 0.0f);
  }
  
  collision.penetration = radiusSum - distance;
  collision.contactPoint = bodyA.position + collision.normal * circleA->radius;
  collision.hasCollision = true;
  
  return true;
}

bool rectangleVsRectangle(const RigidBody& bodyA, const RigidBody& bodyB, Collision& collision) {
  const RectangleShape* rectA = static_cast<const RectangleShape*>(bodyA.shape.get());
  const RectangleShape* rectB = static_cast<const RectangleShape*>(bodyB.shape.get());
  
  glm::vec2 halfSizeA = rectA->size * 0.5f;
  glm::vec2 halfSizeB = rectB->size * 0.5f;
  
  glm::vec2 diff = bodyB.position - bodyA.position;
  
  float overlapX = halfSizeA.x + halfSizeB.x - std::abs(diff.x);
  float overlapY = halfSizeA.y + halfSizeB.y - std::abs(diff.y);
  
  if (overlapX <= 0 || overlapY <= 0) {
    return false;
  }
  
  collision.bodyA = const_cast<RigidBody*>(&bodyA);
  collision.bodyB = const_cast<RigidBody*>(&bodyB);
  
  if (overlapX < overlapY) {
    collision.penetration = overlapX;
    collision.normal = glm::vec2(diff.x < 0 ? -1.0f : 1.0f, 0.0f);
  } else {
    collision.penetration = overlapY;
    collision.normal = glm::vec2(0.0f, diff.y < 0 ? -1.0f : 1.0f);
  }
  
  glm::vec2 contactDir = glm::vec2(
    diff.x < 0 ? -halfSizeA.x : halfSizeA.x,
    diff.y < 0 ? -halfSizeA.y : halfSizeA.y
  );
  collision.contactPoint = bodyA.position + contactDir;
  collision.hasCollision = true;
  
  return true;
}

bool circleVsRectangle(const RigidBody& circle, const RigidBody& rectangle, Collision& collision) {
  const CircleShape* circleShape = static_cast<const CircleShape*>(circle.shape.get());
  const RectangleShape* rectShape = static_cast<const RectangleShape*>(rectangle.shape.get());
  
  glm::vec2 circlePos = circle.position - rectangle.position;
  
  glm::vec2 halfSize = rectShape->size * 0.5f;
  
  glm::vec2 closest = glm::vec2(
    glm::clamp(circlePos.x, -halfSize.x, halfSize.x),
    glm::clamp(circlePos.y, -halfSize.y, halfSize.y)
  );
  
  glm::vec2 toCircle = circlePos - closest;
  float distanceSquared = glm::dot(toCircle, toCircle);
  
  if (distanceSquared > circleShape->radius * circleShape->radius) {
    return false;
  }
  
  float distance = std::sqrt(distanceSquared);
  
  collision.bodyA = const_cast<RigidBody*>(&circle);
  collision.bodyB = const_cast<RigidBody*>(&rectangle);
  
  if (distance > 0.0f) {
    collision.normal = toCircle / distance; 
  } else {
    float overlapX = halfSize.x - std::abs(circlePos.x);
    float overlapY = halfSize.y - std::abs(circlePos.y);
    
    if (overlapX < overlapY) {
      collision.normal = glm::vec2(circlePos.x < 0 ? -1.0f : 1.0f, 0.0f);
    } else {
      collision.normal = glm::vec2(0.0f, circlePos.y < 0 ? -1.0f : 1.0f);
    }
  }
  
  collision.penetration = circleShape->radius - distance;
  collision.contactPoint = circle.position - collision.normal * circleShape->radius;
  collision.hasCollision = true;
  
  return true;
}

PhysicsEngine::PhysicsEngine() 
  : m_spatialHash(m_config.spatialHashCellSize) {
  m_solver = std::make_unique<VerletSolver>();
}

PhysicsEngine::~PhysicsEngine() {

}

size_t PhysicsEngine::addBody(const RigidBody& body) {
  m_bodies.push_back(body);
  return m_bodies.size() - 1;
}

void PhysicsEngine::removeBody(size_t index) {
  if (index < m_bodies.size()) {
    m_bodies.erase(m_bodies.begin() + index);
  }
}

RigidBody* PhysicsEngine::getBody(size_t index) {
  if (index < m_bodies.size()) {
    return &m_bodies[index];
  }
  return nullptr;
}

void PhysicsEngine::setIntegrationMethod(IntegrationMethod method) {
  if (m_integrationMethod == method) {
    return;
  }
  
  m_integrationMethod = method;
  
  switch (method) {
    case IntegrationMethod::VERLET:
      m_solver = std::make_unique<VerletSolver>();
      break;
    case IntegrationMethod::LEAPFROG:
      m_solver = std::make_unique<LeapFrogSolver>();
      break;
  }
}

void PhysicsEngine::setGravity(const glm::vec2& gravity) {
  m_config.gravityVec = gravity;
}

void PhysicsEngine::setSpatialHashCellSize(float cellSize) {
  m_config.spatialHashCellSize = cellSize;
  m_spatialHash = SpatialHash(cellSize);
}

void PhysicsEngine::update(float dt) {
  if (dt <= 0.0f) return;
  
  const float maxDt = 1.0f / 30.0f; 
  if (dt > maxDt) {
    dt = maxDt;
  }
  
  updateAABBs();
  
  updateSpatialHash();
  
  broadPhaseCollision();
  
  narrowPhaseCollision();
  
  m_solver->integrate(m_bodies, dt);
  
  resolveCollisions();
}

void PhysicsEngine::setCollisionCallback(CollisionCallback callback) {
  m_collisionCallback = callback;
}

void PhysicsEngine::debugDraw() {

}

void PhysicsEngine::updateAABBs() {
  for (auto& body : m_bodies) {
    body.updateAABB();
  }
}

void PhysicsEngine::updateSpatialHash() {
  m_spatialHash.clear();
  
  for (auto& body : m_bodies) {
    if (body.active) {
      m_spatialHash.insert(&body);
    }
  }
}

void PhysicsEngine::broadPhaseCollision() {
    m_potentialCollisions = m_spatialHash.queryAllPotentialCollisions();
}

void PhysicsEngine::narrowPhaseCollision() {
  m_collisions.clear();
  
  for (const auto& pair : m_potentialCollisions) {
    RigidBody* bodyA = pair.first;
    RigidBody* bodyB = pair.second;
    
    if (!bodyA->active || !bodyB->active) {
      continue;
    }
    
    if (bodyA->type == BodyType::STATIC && bodyB->type == BodyType::STATIC) {
      continue;
    }
    
    Collision collision;
    if (detectCollision(*bodyA, *bodyB, collision)) {
      m_collisions.push_back(collision);
      
      if (m_collisionCallback) {
        m_collisionCallback(collision);
      }
    }
  }
}

void PhysicsEngine::resolveCollisions() {
  for (auto& collision : m_collisions) {
    resolveCollision(collision);
  }
}

void PhysicsEngine::resolveCollision(Collision& collision) {
  RigidBody* bodyA = collision.bodyA;
  RigidBody* bodyB = collision.bodyB;
  
  if (bodyA->type == BodyType::STATIC && bodyB->type == BodyType::STATIC) {
    return;
  }
  
  glm::vec2 relativeVelocity = bodyB->velocity - bodyA->velocity;
  
  float normalVelocity = glm::dot(relativeVelocity, collision.normal);
  
  if (normalVelocity > 0) {
    return;
  }
  
  float restitution = std::min(bodyA->restitution, bodyB->restitution);
  
  float j = -(1.0f + restitution) * normalVelocity;
  j /= bodyA->invMass + bodyB->invMass;
  
  glm::vec2 impulse = j * collision.normal;
  
  if (bodyA->type == BodyType::DYNAMIC) {
    bodyA->applyImpulse(-impulse, collision.contactPoint);
  }
  
  if (bodyB->type == BodyType::DYNAMIC) {
    bodyB->applyImpulse(impulse, collision.contactPoint);
  }
  
  const float percent = 0.2f; 
  const float slop = 0.01f;   
  
  glm::vec2 correction = std::max(collision.penetration - slop, 0.0f) * percent * 
                        collision.normal / (bodyA->invMass + bodyB->invMass);
  
  if (bodyA->type == BodyType::DYNAMIC) {
    bodyA->position -= correction * bodyA->invMass;
  }
  
  if (bodyB->type == BodyType::DYNAMIC) {
    bodyB->position += correction * bodyB->invMass;
  }
}