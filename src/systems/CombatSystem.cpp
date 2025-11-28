#include "CombatSystem.h"
#include "VisualEffectsSystem.h"
#include "ResourceSystem.h"
#include "../ecs/Components.h"
#include "../core/Logger.h"

namespace Nightfall {

CombatSystem::CombatSystem() {
}

CombatSystem::~CombatSystem() {
    NF_INFO("Combat system shutdown");
}

void CombatSystem::init() {
    NF_INFO("Combat system initialized");
}

void CombatSystem::update(float deltaTime, Registry& registry) {
    cleanupDeadEntities(registry);
}

void CombatSystem::applyDamage(entt::entity attacker, entt::entity target, float damage, Registry& registry) {
    if (!registry.isValid(target)) return;
    
    // 检查是否是建筑物
    auto* building = registry.tryGetComponent<Building>(target);
    if (building) {
        // 对建筑造成伤害
        building->durability -= damage;
        
        // 显示伤害数字
        if (m_visualEffects) {
            auto* targetTransform = registry.tryGetComponent<Transform>(target);
            if (targetTransform) {
                m_visualEffects->createDamageNumber(targetTransform->position, damage);
            }
        }
        
        NF_DEBUG("Entity {} dealt {} damage to building {} (Durability: {}/{})", 
                 static_cast<uint32_t>(attacker),
                 damage,
                 static_cast<uint32_t>(target),
                 building->durability,
                 building->maxDurability);
        
        // 建筑被摧毁
        if (building->durability <= 0.f) {
            handleBuildingDestruction(target, registry);
        }
        return;
    }
    
    auto* health = registry.tryGetComponent<Health>(target);
    if (!health || health->invincible) return;
    
    health->current -= damage;
    
    // 确保生命值不低于0
    if (health->current < 0.f) {
        health->current = 0.f;
    }
    
    // 显示伤害数字
    if (m_visualEffects) {
        auto* targetTransform = registry.tryGetComponent<Transform>(target);
        if (targetTransform) {
            m_visualEffects->createDamageNumber(targetTransform->position, damage);
        }
    }
    
    NF_DEBUG("Entity {} dealt {} damage to entity {} (HP: {}/{})", 
             static_cast<uint32_t>(attacker),
             damage,
             static_cast<uint32_t>(target),
             health->current,
             health->maximum);
    
    // 如果目标死亡，处理死亡逻辑
    if (health->isDead()) {
        handleDeath(target, registry);
    }
}

void CombatSystem::handleDeath(entt::entity entity, Registry& registry) {
    if (!registry.isValid(entity)) return;
    
    NF_INFO("Entity {} died", static_cast<uint32_t>(entity));
    
    // 检查是否是玩家
    if (registry.tryGetComponent<Player>(entity)) {
        NF_WARN("Player died! Game Over");
        // TODO: 触发游戏结束逻辑
        return;
    }
    
    // 如果是僵尸，可能掉落物品
    if (registry.tryGetComponent<Zombie>(entity)) {
        // 创建死亡粒子效果
        if (m_visualEffects) {
            auto* transform = registry.tryGetComponent<Transform>(entity);
            if (transform) {
                m_visualEffects->createDeathEffect(transform->position, sf::Color::Red);
            }
        }
        
        // 掉落资源
        if (m_resourceSystem) {
            m_resourceSystem->addResource("scrap", 1 + rand() % 3); // 1-3 scrap
        }
        
        // TODO: 掉落物品逻辑
        // TODO: 播放死亡音效
    }
    
    // 销毁实体
    registry.destroyEntity(entity);
}

void CombatSystem::handleBuildingDestruction(entt::entity entity, Registry& registry) {
    if (!registry.isValid(entity)) return;
    
    NF_INFO("Building {} destroyed", static_cast<uint32_t>(entity));
    
    // 创建爆炸效果
    if (m_visualEffects) {
        auto* transform = registry.tryGetComponent<Transform>(entity);
        if (transform) {
            m_visualEffects->createDeathEffect(transform->position, sf::Color(200, 100, 0));
        }
    }
    
    // 销毁建筑实体
    registry.destroyEntity(entity);
}

void CombatSystem::cleanupDeadEntities(Registry& registry) {
    // 收集所有死亡的实体
    std::vector<entt::entity> deadEntities;
    
    auto view = registry.view<Health>();
    for (auto entity : view) {
        const auto& health = view.get<Health>(entity);
        if (health.isDead() && !registry.tryGetComponent<Player>(entity)) {
            deadEntities.push_back(entity);
        }
    }
    
    // 销毁死亡实体
    for (auto entity : deadEntities) {
        handleDeath(entity, registry);
    }
}

} // namespace Nightfall
