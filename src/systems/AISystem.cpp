#include "AISystem.h"
#include "CombatSystem.h"
#include "../ecs/Components.h"
#include "../core/Logger.h"
#include <cmath>

namespace Nightfall {

AISystem::AISystem() {
}

AISystem::~AISystem() {
    NF_INFO("AI system shutdown");
}

void AISystem::init() {
    NF_INFO("AI system initialized");
}

void AISystem::update(float deltaTime, Registry& registry, entt::entity player) {
    updateZombieAI(deltaTime, registry, player);
    updateNPCAI(deltaTime, registry);
}

void AISystem::updateZombieAI(float deltaTime, Registry& registry, entt::entity player) {
    if (!registry.isValid(player)) return;
    
    auto* playerTransform = registry.tryGetComponent<Transform>(player);
    if (!playerTransform) return;
    
    // 更新所有僵尸AI
    auto view = registry.view<Transform, AI, Zombie, Hostile>();
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& ai = view.get<AI>(entity);
        auto& zombie = view.get<Zombie>(entity);
        
        // 更新AI计时器
        ai.stateTimer += deltaTime;
        
        // 计算与玩家的距离
        float distSq = getDistanceSquared(transform.position, playerTransform->position);
        float detectionRange = ai.detectionRange * ai.detectionRange;
        float attackRange = ai.attackRange * ai.attackRange;
        
        // 检查附近是否有建筑物阻挡
        entt::entity nearestBuilding = findNearestBuilding(transform.position, registry, 100.f);
        bool hasBlockingBuilding = (nearestBuilding != entt::null);
        
        // 状态机
        switch (ai.state) {
            case AIState::Idle:
                // 如果玩家进入检测范围，切换到追击
                if (distSq < detectionRange) {
                    ai.state = AIState::Chase;
                    ai.stateTimer = 0.f;
                } else if (ai.stateTimer > 3.f) {
                    // 闲置太久，开始巡逻
                    ai.state = AIState::Patrol;
                    ai.stateTimer = 0.f;
                }
                break;
                
            case AIState::Patrol:
                patrol(entity, registry);
                // 如果玩家进入检测范围，切换到追击
                if (distSq < detectionRange) {
                    ai.state = AIState::Chase;
                    ai.stateTimer = 0.f;
                }
                break;
                
            case AIState::Chase:
                // 追击玩家
                if (distSq < attackRange) {
                    // 进入攻击范围
                    ai.state = AIState::Attack;
                    ai.target = player;
                    ai.stateTimer = 0.f;
                } else if (hasBlockingBuilding && distSq > 150.f * 150.f) {
                    // 有建筑物阻挡且玩家较远，转为攻击建筑
                    ai.state = AIState::Attack;
                    ai.target = nearestBuilding;
                    ai.stateTimer = 0.f;
                } else if (distSq > detectionRange * 1.5f) {
                    // 玩家逃出范围，回到闲置
                    ai.state = AIState::Idle;
                    ai.target = entt::null;
                    ai.stateTimer = 0.f;
                    // 停止移动
                    if (auto* velocity = registry.tryGetComponent<Velocity>(entity)) {
                        velocity->velocity = sf::Vector2f(0.f, 0.f);
                    }
                } else {
                    // 继续追击
                    chaseTarget(entity, playerTransform->position, registry);
                }
                break;
                
            case AIState::Attack: {
                // 攻击状态
                if (!registry.isValid(ai.target)) {
                    // 目标消失，返回追击状态
                    ai.state = AIState::Chase;
                    ai.target = entt::null;
                    ai.stateTimer = 0.f;
                    break;
                }
                
                // 计算与目标的距离
                auto* targetTransform = registry.tryGetComponent<Transform>(ai.target);
                if (!targetTransform) {
                    ai.state = AIState::Chase;
                    ai.target = entt::null;
                    break;
                }
                
                float targetDistSq = getDistanceSquared(transform.position, targetTransform->position);
                
                if (targetDistSq > attackRange * 1.5f) {
                    // 目标离开攻击范围，继续追击
                    if (ai.target == player) {
                        ai.state = AIState::Chase;
                        ai.target = entt::null;
                    } else {
                        // 建筑被摧毁或离开，返回追击玩家
                        ai.state = AIState::Chase;
                        ai.target = entt::null;
                    }
                    ai.stateTimer = 0.f;
                } else {
                    // 执行攻击
                    if (ai.stateTimer >= ai.attackCooldown) {
                        attackTarget(entity, ai.target, registry);
                        ai.stateTimer = 0.f;
                    }
                    // 攻击时停止移动
                    if (auto* velocity = registry.tryGetComponent<Velocity>(entity)) {
                        velocity->velocity = sf::Vector2f(0.f, 0.f);
                    }
                }
                break;
            }
                
            case AIState::Flee:
                // 逃跑（未实现）
                break;
        }
    }
}

void AISystem::updateNPCAI(float deltaTime, Registry& registry) {
    // TODO: 实现友好NPC的AI
    // 例如：
    // - 自动采集资源
    // - 自动建造
    // - 跟随玩家等
}

void AISystem::chaseTarget(entt::entity entity, const sf::Vector2f& targetPos, Registry& registry) {
    auto* transform = registry.tryGetComponent<Transform>(entity);
    auto* velocity = registry.tryGetComponent<Velocity>(entity);
    auto* ai = registry.tryGetComponent<AI>(entity);
    
    if (!transform || !velocity || !ai) return;
    
    // 计算朝向目标的方向
    sf::Vector2f direction = getNormalizedDirection(transform->position, targetPos);
    
    // 设置速度
    velocity->velocity = direction * ai->moveSpeed;
}

void AISystem::patrol(entt::entity entity, Registry& registry) {
    auto* transform = registry.tryGetComponent<Transform>(entity);
    auto* velocity = registry.tryGetComponent<Velocity>(entity);
    auto* ai = registry.tryGetComponent<AI>(entity);
    auto* patrol = registry.tryGetComponent<Patrol>(entity);
    
    if (!transform || !velocity || !ai || !patrol) return;
    
    if (patrol->waypoints.empty()) {
        // 没有巡逻点，随机游荡
        if (ai->stateTimer > 2.f) {
            // 每2秒改变方向
            float angle = static_cast<float>(rand()) / RAND_MAX * 2.f * 3.14159f;
            velocity->velocity = sf::Vector2f(std::cos(angle), std::sin(angle)) * (ai->moveSpeed * 0.5f);
            ai->stateTimer = 0.f;
        }
        return;
    }
    
    // 朝当前巡逻点移动
    sf::Vector2f targetPos = patrol->waypoints[patrol->currentWaypoint];
    float distSq = getDistanceSquared(transform->position, targetPos);
    
    if (distSq < 100.f) { // 到达巡逻点（10像素范围内）
        // 切换到下一个巡逻点
        patrol->currentWaypoint = (patrol->currentWaypoint + 1) % patrol->waypoints.size();
        targetPos = patrol->waypoints[patrol->currentWaypoint];
    }
    
    // 朝目标移动
    sf::Vector2f direction = getNormalizedDirection(transform->position, targetPos);
    velocity->velocity = direction * (ai->moveSpeed * 0.5f); // 巡逻时速度减半
}

void AISystem::attackTarget(entt::entity entity, entt::entity target, Registry& registry) {
    auto* combat = registry.tryGetComponent<Combat>(entity);
    if (!combat || !m_combatSystem) return;
    
    // 使用 CombatSystem 应用伤害
    m_combatSystem->applyDamage(entity, target, combat->attackDamage, registry);
}

float AISystem::getDistanceSquared(const sf::Vector2f& a, const sf::Vector2f& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return dx * dx + dy * dy;
}

sf::Vector2f AISystem::getNormalizedDirection(const sf::Vector2f& from, const sf::Vector2f& to) {
    sf::Vector2f direction = to - from;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (length > 0.f) {
        direction /= length;
    }
    
    return direction;
}

entt::entity AISystem::findNearestBuilding(const sf::Vector2f& position, Registry& registry, float maxRange) {
    entt::entity nearest = entt::null;
    float nearestDistSq = maxRange * maxRange;
    
    auto view = registry.view<Transform, Building, Collider>();
    for (auto entity : view) {
        const auto& transform = view.get<Transform>(entity);
        const auto& building = view.get<Building>(entity);
        
        // 跳过未完成的建筑
        if (!building.isComplete) continue;
        
        float distSq = getDistanceSquared(position, transform.position);
        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearest = entity;
        }
    }
    
    return nearest;
}

} // namespace Nightfall
