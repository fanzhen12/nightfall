#pragma once

#include "../ecs/Registry.h"
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

namespace Nightfall {

class VisualEffectsSystem;
class ResourceSystem;

/// 战斗系统 - 处理伤害、死亡、战斗逻辑
class CombatSystem {
public:
    CombatSystem();
    ~CombatSystem();

    void init();
    void update(float deltaTime, Registry& registry);
    
    void setVisualEffectsSystem(VisualEffectsSystem* vfx) { m_visualEffects = vfx; }
    void setResourceSystem(ResourceSystem* resources) { m_resourceSystem = resources; }

    /// 处理实体死亡
    void handleDeath(entt::entity entity, Registry& registry);
    
    /// 处理建筑被摧毁
    void handleBuildingDestruction(entt::entity entity, Registry& registry);

    /// 应用伤害到目标
    void applyDamage(entt::entity attacker, entt::entity target, float damage, Registry& registry);

private:
    /// 清理死亡实体
    void cleanupDeadEntities(Registry& registry);
    
    VisualEffectsSystem* m_visualEffects{nullptr};
    ResourceSystem* m_resourceSystem{nullptr};
};

} // namespace Nightfall
