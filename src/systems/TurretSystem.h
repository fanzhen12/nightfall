#pragma once

#include "../ecs/Registry.h"
#include <SFML/System/Vector2.hpp>

namespace Nightfall {

class CombatSystem;
class VisualEffectsSystem;

/// 炮塔系统 - 处理炮塔自动瞄准和攻击
class TurretSystem {
public:
    TurretSystem();
    ~TurretSystem();

    void init();
    void update(float deltaTime, Registry& registry);
    
    void setCombatSystem(CombatSystem* combatSystem) { m_combatSystem = combatSystem; }
    void setVisualEffectsSystem(VisualEffectsSystem* vfx) { m_visualEffects = vfx; }

private:
    /// 更新单个炮塔
    void updateTurret(entt::entity turret, float deltaTime, Registry& registry);
    
    /// 寻找最近的敌人
    entt::entity findNearestEnemy(const sf::Vector2f& position, float range, Registry& registry);
    
    /// 攻击目标
    void attackTarget(entt::entity turret, entt::entity target, Registry& registry);
    
    float getDistanceSquared(const sf::Vector2f& a, const sf::Vector2f& b);

private:
    CombatSystem* m_combatSystem{nullptr};
    VisualEffectsSystem* m_visualEffects{nullptr};
};

} // namespace Nightfall
