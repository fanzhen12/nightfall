#pragma once

#include "../ecs/Registry.h"
#include <SFML/Graphics.hpp>

namespace Nightfall {

class PhysicsSystem {
public:
    PhysicsSystem();
    ~PhysicsSystem();

    void update(float deltaTime, Registry& registry);

    // Collision detection
    static bool checkAABBCollision(const sf::FloatRect& a, const sf::FloatRect& b);
    static bool checkCircleCollision(const sf::Vector2f& posA, float radiusA, 
                                     const sf::Vector2f& posB, float radiusB);
    
    // Collision resolution
    static sf::Vector2f resolveCollision(const sf::FloatRect& moving, const sf::FloatRect& stationary);
    
    // World boundaries
    void setWorldBounds(const sf::FloatRect& bounds) { m_worldBounds = bounds; }
    const sf::FloatRect& getWorldBounds() const { return m_worldBounds; }

private:
    void updatePhysics(Registry& registry);
    void resolveCollisions(Registry& registry);
    void enforceWorldBounds(Registry& registry);
    
    sf::FloatRect m_worldBounds;
};

} // namespace Nightfall
