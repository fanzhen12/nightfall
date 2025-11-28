#include "Registry.h"
#include "../core/Logger.h"

namespace Nightfall {

entt::entity Registry::createPlayer(const sf::Vector2f& position) {
    auto entity = createEntity();

    // 基础组件
    addComponent<Transform>(entity, position);
    addComponent<Velocity>(entity, 0.f, 0.f, 200.f);  // 最大速度 200 px/s
    addComponent<Sprite>(entity, "player", 10);  // 高渲染层级
    addComponent<Collider>(entity, 32.f, 32.f);

    // 生存组件
    addComponent<Health>(entity, 100.f);
    addComponent<Hunger>(entity);
    addComponent<Temperature>(entity);
    addComponent<Stamina>(entity);

    // 战斗组件
    addComponent<Combat>(entity);
    addComponent<Armor>(entity);

    // 玩家标记
    addComponent<Player>(entity);
    addComponent<Inventory>(entity);

    NF_INFO("创建玩家实体: {}", static_cast<uint32_t>(entity));
    return entity;
}

entt::entity Registry::createZombie(const sf::Vector2f& position, ZombieType type) {
    auto entity = createEntity();

    // 基础组件
    addComponent<Transform>(entity, position);
    addComponent<Sprite>(entity, "zombie_normal", 5);
    addComponent<Collider>(entity, 32.f, 32.f);

    // 根据僵尸类型设置属性
    float maxSpeed = 50.f;
    float health = 50.f;
    float damage = 10.f;
    float detectionRange = 200.f;

    switch (type) {
        case ZombieType::Fast:
            maxSpeed = 150.f;
            health = 30.f;
            damage = 8.f;
            addComponent<Sprite>(entity, "zombie_fast", 5);
            break;

        case ZombieType::Tank:
            maxSpeed = 30.f;
            health = 200.f;
            damage = 20.f;
            addComponent<Sprite>(entity, "zombie_tank", 5);
            break;

        case ZombieType::Exploder:
            maxSpeed = 60.f;
            health = 40.f;
            damage = 50.f;  // 爆炸伤害
            addComponent<Sprite>(entity, "zombie_exploder", 5);
            break;

        case ZombieType::Boss:
            maxSpeed = 40.f;
            health = 500.f;
            damage = 30.f;
            detectionRange = 400.f;
            addComponent<Sprite>(entity, "zombie_boss", 6);
            break;

        case ZombieType::Normal:
        default:
            // 使用默认值
            break;
    }

    addComponent<Velocity>(entity, 0.f, 0.f, maxSpeed);
    addComponent<Health>(entity, health);

    // 战斗组件
    auto& combat = addComponent<Combat>(entity);
    combat.attackDamage = damage;
    combat.attackSpeed = 1.f;
    combat.attackRange = 40.f;

    // AI 组件
    auto& ai = addComponent<AI>(entity);
    ai.detectionRange = detectionRange;
    ai.attackRange = 40.f;
    ai.state = AIState::Patrol;
    ai.stateTimer = 0.f;
    ai.attackCooldown = 0.f;
    ai.moveSpeed = maxSpeed;

    // 僵尸特性
    addComponent<Zombie>(entity).type = type;

    // 标记
    addComponent<Hostile>(entity);
    addComponent<Destructible>(entity);

    NF_DEBUG("创建僵尸实体: {} (类型: {})", static_cast<uint32_t>(entity), static_cast<int>(type));
    return entity;
}

entt::entity Registry::createNPC(const sf::Vector2f& position, const std::string& name, NPC::Profession profession) {
    auto entity = createEntity();

    // 基础组件
    addComponent<Transform>(entity, position);
    addComponent<Velocity>(entity, 0.f, 0.f, 100.f);
    addComponent<Sprite>(entity, "npc_generic", 8);
    addComponent<Collider>(entity, 32.f, 32.f);

    // 生存组件
    addComponent<Health>(entity, 80.f);
    addComponent<Hunger>(entity);
    addComponent<Temperature>(entity);

    // NPC 特性
    auto& npc = addComponent<NPC>(entity);
    npc.name = name;
    npc.profession = profession;
    npc.morale = 100.f;
    npc.level = 1;

    // AI（NPC 也有 AI，但状态不同）
    auto& ai = addComponent<AI>(entity);
    ai.state = AIState::Idle;
    ai.stateTimer = 0.f;
    ai.attackCooldown = 0.f;
    ai.moveSpeed = 100.f;
    ai.detectionRange = 150.f;

    // 根据职业设置特殊能力
    switch (profession) {
        case NPC::Profession::Soldier:
            addComponent<Combat>(entity);
            addComponent<Weapon>(entity);
            break;
        case NPC::Profession::Doctor:
            // 医生有治疗能力（后续实现）
            break;
        case NPC::Profession::Engineer:
            // 工程师建造速度更快（后续实现）
            break;
        default:
            break;
    }

    addComponent<Inventory>(entity);
    addComponent<Friendly>(entity);
    addComponent<Interactable>(entity).type = Interactable::Type::NPC;

    NF_INFO("创建 NPC: {} (名字: {}, 职业: {})", static_cast<uint32_t>(entity), name, static_cast<int>(profession));
    return entity;
}

entt::entity Registry::createBuilding(const sf::Vector2f& position, Building::Type type) {
    auto entity = createEntity();

    addComponent<Transform>(entity, position);
    addComponent<Sprite>(entity, "building_" + std::to_string(static_cast<int>(type)), 3);
    addComponent<Collider>(entity, 64.f, 64.f);

    auto& building = addComponent<Building>(entity);
    building.type = type;
    building.isComplete = false;
    building.constructionProgress = 0.f;

    // 根据建筑类型设置属性
    switch (type) {
        case Building::Type::Wall:
            building.maxDurability = 200.f;
            building.durability = 200.f;
            break;

        case Building::Type::Turret:
            building.maxDurability = 100.f;
            building.durability = 100.f;
            addComponent<Turret>(entity);
            addComponent<Combat>(entity).attackRange = 300.f;
            break;

        case Building::Type::Generator:
            building.maxDurability = 150.f;
            building.durability = 150.f;
            addComponent<Producer>(entity).resourceType = "electricity";
            break;

        case Building::Type::Farm:
            building.maxDurability = 80.f;
            building.durability = 80.f;
            {
                auto& producer = addComponent<Producer>(entity);
                producer.resourceType = "food";
                producer.productionAmount = 2;
                producer.productionInterval = 10.f;  // 每10秒生产2个食物
            }
            break;

        case Building::Type::Storage:
            building.maxDurability = 120.f;
            building.durability = 120.f;
            addComponent<Inventory>(entity).maxSlots = 50;
            addComponent<Interactable>(entity).type = Interactable::Type::Container;
            break;

        case Building::Type::Workshop:
            building.maxDurability = 100.f;
            building.durability = 100.f;
            {
                auto& producer = addComponent<Producer>(entity);
                producer.resourceType = "metal";
                producer.productionAmount = 1;
                producer.productionInterval = 15.f;  // 每15秒生产1个金属
            }
            addComponent<Interactable>(entity).type = Interactable::Type::Workbench;
            break;

        default:
            building.maxDurability = 100.f;
            building.durability = 100.f;
            break;
    }

    addComponent<Static>(entity);
    addComponent<Destructible>(entity);

    NF_INFO("创建建筑: {} (类型: {})", static_cast<uint32_t>(entity), static_cast<int>(type));
    return entity;
}

entt::entity Registry::createDroppedItem(const sf::Vector2f& position, const std::string& itemId, int quantity) {
    auto entity = createEntity();

    addComponent<Transform>(entity, position);
    addComponent<Sprite>(entity, "item_" + itemId, 2);
    addComponent<Collider>(entity, 16.f, 16.f).isTrigger = true;

    auto& dropped = addComponent<Dropped>(entity);
    dropped.itemId = itemId;
    dropped.quantity = quantity;
    dropped.despawnTimer = 300.f;  // 5分钟

    addComponent<Interactable>(entity).type = Interactable::Type::Item;
    addComponent<Temporary>(entity).lifetime = 300.f;

    NF_DEBUG("创建掉落物品: {} (物品: {}, 数量: {})", static_cast<uint32_t>(entity), itemId, quantity);
    return entity;
}

entt::entity Registry::createTurret(const sf::Vector2f& position) {
    auto entity = createBuilding(position, Building::Type::Turret);
    
    // 炮塔已在 createBuilding 中添加，这里可以额外配置
    auto* turret = tryGetComponent<Turret>(entity);
    if (turret) {
        turret->rotationSpeed = 180.f;  // 180度/秒
        turret->attackSpeed = 2.f;  // 每秒2次
        turret->range = 250.f;  // 射程
        turret->damage = 15.f;  // 伤害
    }

    return entity;
}

entt::entity Registry::createParticle(const sf::Vector2f& position, const sf::Vector2f& velocity, float lifetime) {
    auto entity = createEntity();

    addComponent<Transform>(entity, position);
    addComponent<Velocity>(entity, velocity.x, velocity.y, 500.f);
    addComponent<Sprite>(entity, "particle", 15);  // 最高渲染层级

    auto& temp = addComponent<Temporary>(entity);
    temp.lifetime = lifetime;
    temp.timer = 0.f;

    return entity;
}

entt::entity Registry::createResourceNode(const sf::Vector2f& position, const std::string& resourceType, int amount) {
    auto entity = createEntity();

    addComponent<Transform>(entity, position);
    
    // 根据资源类型设置不同的外观
    if (resourceType == "wood") {
        addComponent<Sprite>(entity, "tree", 4);  // 树木，层级4（在地面上方）
        addComponent<Collider>(entity, 48.f, 48.f);
    } else if (resourceType == "metal") {
        addComponent<Sprite>(entity, "ore", 4);  // 矿石
        addComponent<Collider>(entity, 40.f, 40.f);
    } else {
        addComponent<Sprite>(entity, "resource_" + resourceType, 4);
        addComponent<Collider>(entity, 40.f, 40.f);
    }

    auto& node = addComponent<ResourceNode>(entity);
    node.resourceType = resourceType;
    node.resourceAmount = amount;
    node.maxResourceAmount = amount;
    
    if (resourceType == "wood") {
        node.harvestTime = 2.f;      // 采集需要2秒
        node.harvestAmount = 5;      // 每次获得5个木头
        node.regenTime = 120.f;      // 2分钟后重新生长
    } else if (resourceType == "metal") {
        node.harvestTime = 3.f;      // 采集需要3秒
        node.harvestAmount = 3;      // 每次获得3个金属
        node.regenTime = 180.f;      // 3分钟后重新生长
    }

    addComponent<Interactable>(entity).type = Interactable::Type::Item;
    addComponent<Static>(entity);

    NF_INFO("创建资源节点: {} (类型: {}, 数量: {})", 
            static_cast<uint32_t>(entity), resourceType, amount);
    return entity;
}

} // namespace Nightfall
