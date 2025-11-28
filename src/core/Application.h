#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "../ecs/Registry.h"
#include "../systems/RenderingSystem.h"
#include "../systems/MovementSystem.h"
#include "../systems/PhysicsSystem.h"
#include "../systems/AISystem.h"
#include "../systems/WaveSystem.h"
#include "../systems/CombatSystem.h"
#include "../systems/BuildingSystem.h"
#include "../systems/TurretSystem.h"
#include "../systems/VisualEffectsSystem.h"
#include "../systems/ResourceSystem.h"
#include "../ui/HUD.h"

namespace Nightfall {

/**
 * @brief 游戏应用程序主类
 * 
 * 负责：
 * - 窗口管理
 * - 主游戏循环
 * - ECS 系统管理
 */
class Application {
public:
    /**
     * @brief 构造函数
     * @param title 窗口标题
     * @param width 窗口宽度
     * @param height 窗口高度
     */
    Application(const std::string& title = "Project Nightfall", 
                unsigned int width = 1280, 
                unsigned int height = 720);
    
    ~Application() = default;

    // 禁止拷贝
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    /**
     * @brief 启动游戏主循环
     */
    void run();

    /**
     * @brief 获取窗口引用
     */
    sf::RenderWindow& getWindow() { return m_window; }

    /**
     * @brief 获取 ECS 注册表
     */
    Registry& getRegistry() { return m_registry; }

private:
    /**
     * @brief 处理输入事件
     */
    void processEvents();

    /**
     * @brief 更新游戏逻辑
     * @param deltaTime 帧间隔时间（秒）
     */
    void update(float deltaTime);

    /**
     * @brief 渲染画面
     */
    void render();

    /**
     * @brief 初始化游戏实体
     */
    void initEntities();
    
    /**
     * @brief 采集附近的资源
     */
    void harvestNearbyResources();
    
    /**
     * @brief 开始采集资源节点
     */
    void startHarvesting(entt::entity node);
    
    /**
     * @brief 更新采集进度
     */
    void updateHarvesting(float deltaTime);
    
    /**
     * @brief 取消采集
     */
    void cancelHarvesting();

private:
    sf::RenderWindow m_window;      // 游戏窗口
    sf::Clock m_clock;              // 用于计算 deltaTime
    bool m_isRunning;               // 游戏是否运行中
    
    // ECS 系统
    Registry m_registry;
    RenderingSystem m_renderingSystem;
    MovementSystem m_movementSystem;
    PhysicsSystem m_physicsSystem;
    CombatSystem m_combatSystem;
    AISystem m_aiSystem;
    WaveSystem m_waveSystem;
    BuildingSystem m_buildingSystem;
    TurretSystem m_turretSystem;
    VisualEffectsSystem m_visualEffectsSystem;
    ResourceSystem m_resourceSystem;
    
    // UI 系统
    HUD m_hud;

    // 玩家实体
    entt::entity m_player{entt::null};
};

} // namespace Nightfall

