#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <memory>

namespace Nightfall {

// ==================== 基础组件 ====================

/// 变换组件 - 位置、旋转、缩放
struct Transform {
    sf::Vector2f position{0.f, 0.f};
    float rotation{0.f};  // 角度
    sf::Vector2f scale{1.f, 1.f};

    Transform() = default;
    Transform(float x, float y) : position(x, y) {}
    Transform(const sf::Vector2f& pos) : position(pos) {}
};

/// 精灵组件 - 可视化表示
struct Sprite {
    std::string textureId;
    int zOrder{0};  // 渲染层级，数值越大越靠前
    bool visible{true};
    sf::Vector2f scale{1.f, 1.f};
    sf::Color color{255, 255, 255, 255};

    Sprite() = default;
    Sprite(const std::string& texId, int z = 0) 
        : textureId(texId), zOrder(z) {}
};

/// 速度组件 - 运动
struct Velocity {
    sf::Vector2f velocity{0.f, 0.f};
    float maxSpeed{100.f};  // 像素/秒

    Velocity() = default;
    Velocity(float vx, float vy, float maxSpd = 100.f)
        : velocity(vx, vy), maxSpeed(maxSpd) {}
};

/// 碰撞箱组件
struct Collider {
    sf::Vector2f size{32.f, 32.f};
    sf::Vector2f offset{0.f, 0.f};  // 相对于 Transform 的偏移
    bool isTrigger{false};  // 是否只用于触发事件，不阻挡移动
    
    Collider() = default;
    Collider(float w, float h) : size(w, h) {}
};

// ==================== 生命与资源 ====================

/// 生命值组件
struct Health {
    float current{100.f};
    float maximum{100.f};
    float regeneration{0.f};  // 每秒恢复量
    bool invincible{false};

    Health() = default;
    Health(float max) : current(max), maximum(max) {}

    bool isDead() const { return current <= 0.f; }
    float getPercentage() const { return current / maximum; }
};

/// 饥饿系统
struct Hunger {
    float current{100.f};  // 饱食度
    float maximum{100.f};
    float drainRate{1.f};  // 每秒消耗量
    float damageThreshold{20.f};  // 低于此值开始扣血
    float damageRate{1.f};  // 饥饿扣血速率

    float getPercentage() const { return current / maximum; }
    bool isStarving() const { return current < damageThreshold; }
};

/// 体温系统
struct Temperature {
    float current{37.f};  // 当前体温（摄氏度）
    float comfortable{37.f};  // 舒适体温
    float minimum{30.f};  // 最低安全体温
    float maximum{42.f};  // 最高安全体温
    
    bool isFreezing() const { return current < minimum + 2.f; }
    bool isOverheating() const { return current > maximum - 2.f; }
};

/// 精力/耐力系统
struct Stamina {
    float current{100.f};
    float maximum{100.f};
    float regeneration{10.f};  // 每秒恢复
    float sprintCost{20.f};  // 冲刺消耗/秒

    bool canSprint() const { return current > 10.f; }
    float getPercentage() const { return current / maximum; }
};

// ==================== 战斗相关 ====================

/// 战斗属性
struct Combat {
    float attackDamage{10.f};
    float attackSpeed{1.f};  // 攻击/秒
    float attackRange{50.f};  // 攻击范围（像素）
    float attackCooldown{0.f};  // 当前冷却时间
    
    bool canAttack() const { return attackCooldown <= 0.f; }
};

/// 护甲/防御
struct Armor {
    float physicalDefense{0.f};  // 物理防御
    float elementalDefense{0.f};  // 元素防御（火、冰等）
    float durability{100.f};  // 耐久度
    float maxDurability{100.f};
};

/// 武器组件
struct Weapon {
    enum class Type {
        Melee,      // 近战
        Ranged,     // 远程
        Magic,      // 魔法
        Explosive   // 爆炸物
    };

    Type type{Type::Melee};
    float damage{10.f};
    float range{50.f};
    float fireRate{1.f};  // 攻击速率
    int ammo{-1};  // -1 表示无限弹药
    int maxAmmo{30};
    float reloadTime{2.f};
    bool isReloading{false};
};

// ==================== AI 与行为 ====================

/// AI状态枚举
enum class AIState {
    Idle,
    Patrol,
    Chase,
    Attack,
    Flee,
    Dead
};

/// 僵尸类型枚举
enum class ZombieType {
    Normal,     // 普通僵尸
    Fast,       // 快速僵尸
    Tank,       // 坦克僵尸
    Exploder,   // 爆炸僵尸
    Boss        // Boss
};

/// AI 组件
struct AI {
    AIState state{AIState::Idle};
    float stateTimer{0.f};           // 状态计时器
    float detectionRange{300.f};      // 检测范围
    float attackRange{50.f};          // 攻击范围
    float attackCooldown{1.5f};       // 攻击冷却时间
    float moveSpeed{80.f};            // 移动速度
    float fleeHealthThreshold{0.2f};  // 低于此血量比例时逃跑
    entt::entity target{entt::null};  // 当前目标实体
};

/// 僵尸特性
struct Zombie {
    ZombieType type{ZombieType::Normal};
    float aggressiveness{1.f};  // 攻击性（影响追击距离）
    bool isInfected{true};  // 是否携带感染
    float infectionChance{0.1f};  // 感染几率
};

/// 巡逻路径
struct Patrol {
    std::vector<sf::Vector2f> waypoints;
    size_t currentWaypoint{0};
    float waypointReachedDistance{10.f};
    bool loop{true};
};

// ==================== 玩家与 NPC ====================

/// 玩家组件（标记性组件）
struct Player {
    int playerId{0};
};

/// NPC 组件
struct NPC {
    enum class Profession {
        None,
        Farmer,     // 农民
        Builder,    // 建筑工
        Soldier,    // 士兵
        Doctor,     // 医生
        Scientist,  // 科学家
        Engineer    // 工程师
    };

    std::string name;
    Profession profession{Profession::None};
    float morale{100.f};  // 士气
    int level{1};
    int experience{0};
};

/// 背包/库存
struct Inventory {
    struct Slot {
        std::string itemId;
        int count{0};
        int maxStack{99};
    };

    std::vector<Slot> slots;
    int maxSlots{20};

    Inventory() {
        slots.resize(maxSlots);
    }
};

// ==================== 建筑相关 ====================

/// 建筑组件
struct Building {
    enum class Type {
        Wall,           // 墙壁
        Turret,         // 炮塔
        Gate,           // 大门
        Generator,      // 发电机
        Storage,        // 储物箱
        Workshop,       // 工作台
        Farm,           // 农场
        House           // 房屋
    };

    Type type;
    float constructionProgress{0.f};  // 建造进度 0-1
    bool isComplete{false};
    float durability{100.f};
    float maxDurability{100.f};
};

/// 可建造标记（玩家正在放置的建筑预览）
struct Buildable {
    bool canPlace{true};  // 当前位置是否可以放置
    sf::Color previewColor{255, 255, 255, 128};  // 预览颜色
};

/// 资源生产器
struct Producer {
    std::string resourceType;  // 生产的资源类型
    int productionAmount{1};   // 每次生产数量
    float productionInterval{10.f};  // 生产间隔（秒）
    float productionTimer{0.f};
    bool isActive{true};
};

/// 炮塔组件
struct Turret {
    float range{200.f};           // 射程
    float damage{15.f};           // 伤害
    float attackSpeed{1.f};       // 攻击速度（次/秒）
    float attackCooldown{0.f};    // 攻击冷却
    entt::entity currentTarget{entt::null};  // 当前目标
    float rotationSpeed{180.f};   // 旋转速度（度/秒）
    float targetRotation{0.f};    // 目标旋转角度
    bool autoTarget{true};        // 自动瞄准
};

// ==================== 世界与环境 ====================

/// 光源组件
struct Light {
    float radius{100.f};
    float intensity{1.f};
    sf::Color color{255, 255, 200};  // 暖色光
    bool castsShadows{false};
};

/// 粒子发射器
struct ParticleEmitter {
    sf::Vector2f emissionRate{10.f, 20.f};  // 每秒发射粒子数范围
    float lifetime{2.f};  // 粒子生命周期
    sf::Vector2f velocity{-10.f, 10.f};  // 速度范围
    sf::Color startColor{255, 255, 255, 255};
    sf::Color endColor{255, 255, 255, 0};
    bool active{true};
};

/// 可交互对象
struct Interactable {
    enum class Type {
        Item,       // 物品
        Door,       // 门
        Container,  // 容器
        Workbench,  // 工作台
        NPC,        // NPC
        Vehicle     // 载具
    };

    Type type;
    float interactionRange{50.f};
    std::string promptText;  // 提示文本，如 "按 E 拾取"
    bool isActive{true};
};

/// 掉落物品
struct Dropped {
    std::string itemId;
    int quantity{1};
    float despawnTimer{300.f};  // 5分钟后消失
};

/// 资源采集点（树木、矿石等）
struct ResourceNode {
    std::string resourceType;  // 资源类型 (wood, metal, stone)
    int resourceAmount{10};    // 剩余资源量
    int maxResourceAmount{10}; // 最大资源量
    float harvestTime{2.f};    // 采集所需时间（秒）
    int harvestAmount{1};      // 每次采集数量
    float regenTime{60.f};     // 再生时间（秒，0表示不再生）
    float regenTimer{0.f};     // 再生计时器
    bool isDepleted{false};    // 是否耗尽
};

/// 采集进度组件（玩家正在采集某个资源节点）
struct Harvesting {
    entt::entity targetNode{entt::null};  // 正在采集的节点
    float progress{0.f};                   // 采集进度 0-1
    float harvestTime{2.f};                // 总采集时间
    sf::Vector2f nodePosition;             // 节点位置（用于渲染进度条）
};

// ==================== 标签组件 ====================

/// 敌对标记
struct Hostile {};

/// 友好标记
struct Friendly {};

/// 可摧毁标记
struct Destructible {};

/// 静态对象（不移动）
struct Static {};

/// 子弹标记
struct Bullet {
    float damage{0.f};  // 子弹伤害(可选,通常由发射者决定)
    entt::entity owner{entt::null};  // 发射者
};

/// 临时实体（会自动销毁）
struct Temporary {
    float lifetime{1.f};
    float timer{0.f};
};

// ==================== 调试组件 ====================

/// 调试信息
struct DebugInfo {
    std::string label;
    bool showCollider{false};
    bool showPath{false};
};

} // namespace Nightfall
