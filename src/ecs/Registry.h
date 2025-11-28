#pragma once

#include <entt/entt.hpp>
#include "Components.h"

namespace Nightfall {

/// ECS 注册表封装
/// 封装 EnTT 的核心功能，提供更简洁的接口
class Registry {
public:
    Registry() = default;
    ~Registry() = default;

    // 禁止拷贝
    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;

    /// 创建新实体
    entt::entity createEntity() {
        return m_registry.create();
    }

    /// 销毁实体
    void destroyEntity(entt::entity entity) {
        if (m_registry.valid(entity)) {
            m_registry.destroy(entity);
        }
    }

    /// 检查实体是否有效
    bool isValid(entt::entity entity) const {
        return m_registry.valid(entity);
    }

    /// 添加组件
    template<typename Component, typename... Args>
    decltype(auto) addComponent(entt::entity entity, Args&&... args) {
        return m_registry.emplace<Component>(entity, std::forward<Args>(args)...);
    }

    /// 获取组件
    template<typename Component>
    Component& getComponent(entt::entity entity) {
        return m_registry.get<Component>(entity);
    }

    template<typename Component>
    const Component& getComponent(entt::entity entity) const {
        return m_registry.get<Component>(entity);
    }

    /// 尝试获取组件（可能返回 nullptr）
    template<typename Component>
    Component* tryGetComponent(entt::entity entity) {
        return m_registry.try_get<Component>(entity);
    }

    template<typename Component>
    const Component* tryGetComponent(entt::entity entity) const {
        return m_registry.try_get<Component>(entity);
    }

    /// 检查实体是否有某个组件
    template<typename Component>
    bool hasComponent(entt::entity entity) const {
        return m_registry.all_of<Component>(entity);
    }

    /// 检查实体是否有所有指定组件
    template<typename... Components>
    bool hasAllComponents(entt::entity entity) const {
        return m_registry.all_of<Components...>(entity);
    }

    /// 检查实体是否有任一指定组件
    template<typename... Components>
    bool hasAnyComponent(entt::entity entity) const {
        return m_registry.any_of<Components...>(entity);
    }

    /// 移除组件
    template<typename Component>
    void removeComponent(entt::entity entity) {
        m_registry.remove<Component>(entity);
    }

    /// 获取或添加组件
    template<typename Component, typename... Args>
    Component& getOrAddComponent(entt::entity entity, Args&&... args) {
        return m_registry.get_or_emplace<Component>(entity, std::forward<Args>(args)...);
    }

    /// 遍历所有拥有指定组件的实体
    template<typename... Components, typename Func>
    void each(Func&& func) {
        auto view = m_registry.view<Components...>();
        for (auto entity : view) {
            func(entity, view.get<Components>(entity)...);
        }
    }

    /// 遍历所有拥有指定组件的实体（const 版本）
    template<typename... Components, typename Func>
    void each(Func&& func) const {
        auto view = m_registry.view<Components...>();
        for (auto entity : view) {
            func(entity, view.get<Components>(entity)...);
        }
    }

    /// 获取视图（用于更复杂的查询）
    template<typename... Components>
    auto view() {
        return m_registry.view<Components...>();
    }

    template<typename... Components>
    auto view() const {
        return m_registry.view<Components...>();
    }

    /// 清除所有实体
    void clear() {
        m_registry.clear();
    }

    /// 获取实体数量
    size_t getEntityCount() const {
        // 使用 storage 获取所有实体
        const auto* storage = m_registry.storage<entt::entity>();
        return storage ? storage->size() : 0;
    }

    /// 获取底层 EnTT 注册表（高级用法）
    entt::registry& raw() { return m_registry; }
    const entt::registry& raw() const { return m_registry; }

    // ==================== 便捷创建函数 ====================

    /// 创建玩家实体
    entt::entity createPlayer(const sf::Vector2f& position);

    /// 创建僵尸实体
    entt::entity createZombie(const sf::Vector2f& position, ZombieType type);

    /// 创建 NPC 实体
    entt::entity createNPC(const sf::Vector2f& position, const std::string& name, NPC::Profession profession);

    /// 创建建筑实体
    entt::entity createBuilding(const sf::Vector2f& position, Building::Type type);

    /// 创建掉落物品
    entt::entity createDroppedItem(const sf::Vector2f& position, const std::string& itemId, int quantity);

    /// 创建炮塔
    entt::entity createTurret(const sf::Vector2f& position);

    /// 创建粒子
    entt::entity createParticle(const sf::Vector2f& position, const sf::Vector2f& velocity, float lifetime);

    /// 创建资源节点（树木、矿石等）
    entt::entity createResourceNode(const sf::Vector2f& position, const std::string& resourceType, int amount = 10);

private:
    entt::registry m_registry;
};

} // namespace Nightfall
