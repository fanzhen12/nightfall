#include "TurretSystem.h"
#include "CombatSystem.h"
#include "VisualEffectsSystem.h"
#include "../ecs/Components.h"
#include "../core/Logger.h"
#include <cmath>

namespace Nightfall {

TurretSystem::TurretSystem() {
}

TurretSystem::~TurretSystem() {
    NF_INFO("Turret system shutdown");
}

void TurretSystem::init() {
    NF_INFO("Turret system initialized");
}

void TurretSystem::update(float deltaTime, Registry& registry) {
    // 更新所有炮塔
    auto view = registry.view<Transform, Building, Turret>();
    for (auto entity : view) {
        const auto& building = view.get<Building>(entity);
        
        // 只有完成建造的炮塔才能工作
        if (building.isComplete && building.type == Building::Type::Turret) {
            updateTurret(entity, deltaTime, registry);
        }
    }
}

void TurretSystem::updateTurret(entt::entity turretEntity, float deltaTime, Registry& registry) {
    auto& transform = registry.getComponent<Transform>(turretEntity);
    auto& turret = registry.getComponent<Turret>(turretEntity);
    
    // 更新攻击冷却
    if (turret.attackCooldown > 0.f) {
        turret.attackCooldown -= deltaTime;
    }
    
    // 如果有当前目标，检查是否还在范围内且存活
    if (registry.isValid(turret.currentTarget)) {
        auto* targetHealth = registry.tryGetComponent<Health>(turret.currentTarget);
        auto* targetTransform = registry.tryGetComponent<Transform>(turret.currentTarget);
        
        if (!targetHealth || targetHealth->isDead() || !targetTransform) {
            // 目标死亡或无效，清除目标
            turret.currentTarget = entt::null;
        } else {
            // 检查是否在射程内
            float distSq = getDistanceSquared(transform.position, targetTransform->position);
            if (distSq > turret.range * turret.range) {
                // 目标超出射程
                turret.currentTarget = entt::null;
            }
        }
    }
    
    // 如果没有目标，寻找新目标
    if (turret.currentTarget == entt::null) {
        turret.currentTarget = findNearestEnemy(transform.position, turret.range, registry);
    }
    
    // 如果有目标且冷却完成，攻击
    if (turret.currentTarget != entt::null && turret.attackCooldown <= 0.f) {
        attackTarget(turretEntity, turret.currentTarget, registry);
        turret.attackCooldown = 1.f / turret.attackSpeed; // 重置冷却
        
        // 攻击后检查目标是否仍然有效（可能已被击杀）
        if (!registry.isValid(turret.currentTarget)) {
            turret.currentTarget = entt::null;
        } else {
            auto* targetHealth = registry.tryGetComponent<Health>(turret.currentTarget);
            if (targetHealth && targetHealth->isDead()) {
                turret.currentTarget = entt::null;
            }
        }
    }
}

entt::entity TurretSystem::findNearestEnemy(const sf::Vector2f& position, float range, Registry& registry) {
    entt::entity nearest = entt::null;
    float nearestDistSq = range * range;
    
    // 搜索所有敌对实体
    auto view = registry.view<Transform, Health, Hostile>();
    for (auto entity : view) {
        const auto& transform = view.get<Transform>(entity);
        const auto& health = view.get<Health>(entity);
        
        if (health.isDead()) continue;
        
        float distSq = getDistanceSquared(position, transform.position);
        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearest = entity;
        }
    }
    
    return nearest;
}

void TurretSystem::attackTarget(entt::entity turret, entt::entity target, Registry& registry) {
    if (!m_combatSystem) return;
    
    auto& turretComp = registry.getComponent<Turret>(turret);
    auto& turretTransform = registry.getComponent<Transform>(turret);
    auto* targetTransform = registry.tryGetComponent<Transform>(target);
    
    if (!targetTransform) return;
    
    // 创建视觉效果
    if (m_visualEffects) {
        // 创建攻击线条(即时反馈)
        m_visualEffects->createAttackLine(turretTransform.position, targetTransform->position);
        
        // 创建子弹(视觉效果)
        m_visualEffects->createBullet(turretTransform.position, targetTransform->position, registry);
    }
    
    // 使用 CombatSystem 应用伤害
    m_combatSystem->applyDamage(turret, target, turretComp.damage, registry);
    
    NF_DEBUG("Turret {} attacked enemy {} for {} damage", 
             static_cast<uint32_t>(turret),
             static_cast<uint32_t>(target),
             turretComp.damage);
    
    // TODO: 创建子弹实体
    // TODO: 播放攻击音效
}

float TurretSystem::getDistanceSquared(const sf::Vector2f& a, const sf::Vector2f& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return dx * dx + dy * dy;
}

} // namespace Nightfall
