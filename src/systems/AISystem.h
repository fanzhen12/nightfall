#pragma once

#include "../ecs/Registry.h"
#include <SFML/System/Vector2.hpp>

namespace Nightfall {

class CombatSystem;

/**
 * @brief AI系统 - 处理敌人的AI行为
 * 
 * 功能：
 * - 僵尸寻路（追踪玩家）
 * - 巡逻行为
 * - 攻击判定
 * - 状态机更新
 */
class AISystem {
public:
    AISystem();
    ~AISystem();

    void init();
    void update(float deltaTime, Registry& registry, entt::entity player);
    
    void setCombatSystem(CombatSystem* combatSystem) { m_combatSystem = combatSystem; }

private:
    void updateZombieAI(float deltaTime, Registry& registry, entt::entity player);
    void updateNPCAI(float deltaTime, Registry& registry);
    
    // AI行为
    void chaseTarget(entt::entity entity, const sf::Vector2f& targetPos, Registry& registry);
    void patrol(entt::entity entity, Registry& registry);
    void attackTarget(entt::entity entity, entt::entity target, Registry& registry);
    
    // 辅助函数
    float getDistanceSquared(const sf::Vector2f& a, const sf::Vector2f& b);
    sf::Vector2f getNormalizedDirection(const sf::Vector2f& from, const sf::Vector2f& to);
    entt::entity findNearestBuilding(const sf::Vector2f& position, Registry& registry, float maxRange);
    
    CombatSystem* m_combatSystem{nullptr};
};

} // namespace Nightfall
