#include "PhysicsSystem.h"
#include "../ecs/Components.h"
#include "../core/Logger.h"
#include <cmath>

namespace Nightfall {

PhysicsSystem::PhysicsSystem() 
    : m_worldBounds(sf::Vector2f(0.f, 0.f), sf::Vector2f(1280.f, 720.f)) {
    NF_INFO("Physics system initialized");
}

PhysicsSystem::~PhysicsSystem() {
    NF_INFO("Physics system shutdown");
}

void PhysicsSystem::update(float deltaTime, Registry& registry) {
    updatePhysics(registry);
    resolveCollisions(registry);
    enforceWorldBounds(registry);
}

void PhysicsSystem::updatePhysics(Registry& registry) {
    // Apply gravity and physics forces if needed
    // For now, basic velocity damping
    auto view = registry.view<Transform, Velocity>();
    for (auto entity : view) {
        auto& velocity = view.get<Velocity>(entity);
        
        // Apply friction/damping
        const float damping = 0.95f;
        velocity.velocity.x *= damping;
        velocity.velocity.y *= damping;
        
        // Stop very slow movement
        if (std::abs(velocity.velocity.x) < 0.1f) velocity.velocity.x = 0.f;
        if (std::abs(velocity.velocity.y) < 0.1f) velocity.velocity.y = 0.f;
    }
}

void PhysicsSystem::resolveCollisions(Registry& registry) {
    // Get all entities with collision boxes
    auto movingView = registry.view<Transform, Collider, Velocity>();
    auto staticView = registry.view<Transform, Collider, Static>();
    
    // Check moving entities against static entities
    for (auto movingEntity : movingView) {
        auto& movingTransform = movingView.get<Transform>(movingEntity);
        auto& movingCollider = movingView.get<Collider>(movingEntity);
        
        sf::FloatRect movingRect(
            movingTransform.position - movingCollider.size / 2.f,
            movingCollider.size
        );
        
        for (auto staticEntity : staticView) {
            if (movingEntity == staticEntity) continue;
            
            auto& staticTransform = staticView.get<Transform>(staticEntity);
            auto& staticCollider = staticView.get<Collider>(staticEntity);
            
            sf::FloatRect staticRect(
                staticTransform.position - staticCollider.size / 2.f,
                staticCollider.size
            );
            
            if (checkAABBCollision(movingRect, staticRect)) {
                // Resolve collision by pushing moving entity out
                sf::Vector2f correction = resolveCollision(movingRect, staticRect);
                movingTransform.position += correction;
                
                // Stop velocity in collision direction
                auto& velocity = movingView.get<Velocity>(movingEntity);
                if (std::abs(correction.x) > std::abs(correction.y)) {
                    velocity.velocity.x = 0.f;
                } else {
                    velocity.velocity.y = 0.f;
                }
            }
        }
    }
    
    // Check moving entities against other moving entities
    std::vector<entt::entity> movingEntities(movingView.begin(), movingView.end());
    for (size_t i = 0; i < movingEntities.size(); ++i) {
        for (size_t j = i + 1; j < movingEntities.size(); ++j) {
            auto entityA = movingEntities[i];
            auto entityB = movingEntities[j];
            
            auto& transformA = movingView.get<Transform>(entityA);
            auto& colliderA = movingView.get<Collider>(entityA);
            auto& velocityA = movingView.get<Velocity>(entityA);
            
            auto& transformB = movingView.get<Transform>(entityB);
            auto& colliderB = movingView.get<Collider>(entityB);
            auto& velocityB = movingView.get<Velocity>(entityB);
            
            sf::FloatRect rectA(
                transformA.position - colliderA.size / 2.f,
                colliderA.size
            );
            
            sf::FloatRect rectB(
                transformB.position - colliderB.size / 2.f,
                colliderB.size
            );
            
            if (checkAABBCollision(rectA, rectB)) {
                // Simple elastic collision - push entities apart
                sf::Vector2f delta = transformB.position - transformA.position;
                float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                
                if (distance > 0.f) {
                    sf::Vector2f normal = delta / distance;
                    float overlap = (colliderA.size.x + colliderB.size.x) / 2.f - distance;
                    
                    transformA.position -= normal * overlap * 0.5f;
                    transformB.position += normal * overlap * 0.5f;
                    
                    // Exchange some velocity
                    float relativeVelocity = (velocityB.velocity.x - velocityA.velocity.x) * normal.x + 
                                            (velocityB.velocity.y - velocityA.velocity.y) * normal.y;
                    
                    if (relativeVelocity < 0.f) {
                        float impulse = relativeVelocity * 0.5f;
                        velocityA.velocity.x -= impulse * normal.x;
                        velocityA.velocity.y -= impulse * normal.y;
                        velocityB.velocity.x += impulse * normal.x;
                        velocityB.velocity.y += impulse * normal.y;
                    }
                }
            }
        }
    }
}

void PhysicsSystem::enforceWorldBounds(Registry& registry) {
    auto view = registry.view<Transform, Collider>();
    
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& collider = view.get<Collider>(entity);
        
        float halfWidth = collider.size.x / 2.f;
        float halfHeight = collider.size.y / 2.f;
        
        // Clamp position to world bounds
        if (transform.position.x - halfWidth < m_worldBounds.position.x) {
            transform.position.x = m_worldBounds.position.x + halfWidth;
            if (auto* velocity = registry.tryGetComponent<Velocity>(entity)) {
                velocity->velocity.x = 0.f;
            }
        }
        if (transform.position.x + halfWidth > m_worldBounds.position.x + m_worldBounds.size.x) {
            transform.position.x = m_worldBounds.position.x + m_worldBounds.size.x - halfWidth;
            if (auto* velocity = registry.tryGetComponent<Velocity>(entity)) {
                velocity->velocity.x = 0.f;
            }
        }
        if (transform.position.y - halfHeight < m_worldBounds.position.y) {
            transform.position.y = m_worldBounds.position.y + halfHeight;
            if (auto* velocity = registry.tryGetComponent<Velocity>(entity)) {
                velocity->velocity.y = 0.f;
            }
        }
        if (transform.position.y + halfHeight > m_worldBounds.position.y + m_worldBounds.size.y) {
            transform.position.y = m_worldBounds.position.y + m_worldBounds.size.y - halfHeight;
            if (auto* velocity = registry.tryGetComponent<Velocity>(entity)) {
                velocity->velocity.y = 0.f;
            }
        }
    }
}

bool PhysicsSystem::checkAABBCollision(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.position.x < b.position.x + b.size.x &&
           a.position.x + a.size.x > b.position.x &&
           a.position.y < b.position.y + b.size.y &&
           a.position.y + a.size.y > b.position.y;
}

bool PhysicsSystem::checkCircleCollision(const sf::Vector2f& posA, float radiusA, 
                                         const sf::Vector2f& posB, float radiusB) {
    float dx = posB.x - posA.x;
    float dy = posB.y - posA.y;
    float distanceSquared = dx * dx + dy * dy;
    float radiusSum = radiusA + radiusB;
    return distanceSquared < radiusSum * radiusSum;
}

sf::Vector2f PhysicsSystem::resolveCollision(const sf::FloatRect& moving, const sf::FloatRect& stationary) {
    // Calculate centers
    sf::Vector2f movingCenter = moving.position + moving.size / 2.f;
    sf::Vector2f staticCenter = stationary.position + stationary.size / 2.f;
    
    // Calculate delta
    sf::Vector2f delta = movingCenter - staticCenter;
    
    // Calculate overlaps
    float combinedHalfWidths = (moving.size.x + stationary.size.x) / 2.f;
    float combinedHalfHeights = (moving.size.y + stationary.size.y) / 2.f;
    
    float overlapX = combinedHalfWidths - std::abs(delta.x);
    float overlapY = combinedHalfHeights - std::abs(delta.y);
    
    // Push out on the axis with smallest overlap
    if (overlapX < overlapY) {
        return sf::Vector2f((delta.x > 0.f ? overlapX : -overlapX), 0.f);
    } else {
        return sf::Vector2f(0.f, (delta.y > 0.f ? overlapY : -overlapY));
    }
}

} // namespace Nightfall
