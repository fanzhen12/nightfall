#include "Application.h"
#include "Logger.h"
#include "Time.h"
#include "ResourceManager.h"
#include "../utils/Config.h"
#include <optional>

namespace Nightfall {

Application::Application(const std::string& title, unsigned int width, unsigned int height)
    : m_window(sf::VideoMode({width, height}), title)
    , m_isRunning(true)
{
    // 设置帧率限制
    int fpsLimit = Config::getInt("window.fps_limit", 60);
    m_window.setFramerateLimit(fpsLimit);
    
    // 启用垂直同步（如果配置启用）
    bool vsync = Config::getBool("window.vsync", false);
    if (vsync) {
        // m_window.setVerticalSyncEnabled(true);
    }
    
    NF_CORE_INFO("应用程序初始化成功");
    NF_CORE_INFO("窗口大小: {}x{}", width, height);
    NF_CORE_INFO("目标帧率: {} FPS", fpsLimit);
    
    // 初始化时间系统 (从第1天早上6:00开始)
    float realSecondsPerGameHour = Config::getFloat("gameplay.real_seconds_per_game_hour", 120.0f);
    Time::init(6, 0, 1);
    Time::setGameHourDuration(realSecondsPerGameHour);
    
    // 初始化资源管理器
    ResourceManager::getInstance().preloadEssentials();
    
    // 初始化 ECS 系统
    m_renderingSystem.init();
    m_movementSystem.init();
    m_combatSystem.init();
    m_visualEffectsSystem.init();
    m_resourceSystem.init();
    m_aiSystem.init();
    m_aiSystem.setCombatSystem(&m_combatSystem);
    m_buildingSystem.init();
    m_buildingSystem.setResourceSystem(&m_resourceSystem);
    m_turretSystem.init();
    m_turretSystem.setCombatSystem(&m_combatSystem);
    m_turretSystem.setVisualEffectsSystem(&m_visualEffectsSystem);
    m_combatSystem.setVisualEffectsSystem(&m_visualEffectsSystem);
    m_combatSystem.setResourceSystem(&m_resourceSystem);
    
    // 设置物理系统世界边界
    m_physicsSystem.setWorldBounds(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(static_cast<float>(width), static_cast<float>(height))));
    
    // 初始化波次系统
    m_waveSystem.init(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(static_cast<float>(width), static_cast<float>(height))));
    
    // 初始化 UI 系统
    auto* font = ResourceManager::getInstance().getFont("default");
    m_hud.init(font);
    
    // 创建游戏实体
    initEntities();
}

void Application::run() {
    NF_CORE_INFO("========== 游戏循环启动 ==========");
    
    while (m_isRunning && m_window.isOpen()) {
        // 计算帧间隔时间
        float deltaTime = m_clock.restart().asSeconds();
        
        // 限制 deltaTime 防止卡顿时出现大跳跃
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }
        
        processEvents();
        update(deltaTime);
        render();
    }
    
    NF_CORE_INFO("========== 游戏循环结束 ==========");
}

void Application::processEvents() {
    while (std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_isRunning = false;
            m_window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_isRunning = false;
            }
            // 空格键暂停/恢复时间
            else if (keyPressed->code == sf::Keyboard::Key::Space) {
                Time::togglePause();
                NF_INFO("时间 {}", Time::isPaused() ? "暂停" : "恢复");
            }
            // T键加速时间
            else if (keyPressed->code == sf::Keyboard::Key::T) {
                float newScale = Time::getTimeScale() * 2.0f;
                if (newScale > 8.0f) newScale = 1.0f; // 循环: 1x -> 2x -> 4x -> 8x -> 1x
                Time::setTimeScale(newScale);
                NF_INFO("时间倍率: {}x", newScale);
            }
            // N键开始下一波
            else if (keyPressed->code == sf::Keyboard::Key::N) {
                if (!m_waveSystem.isWaveActive()) {
                    m_waveSystem.startNextWave(m_registry);
                }
            }
            // B键放置墙壁
            else if (keyPressed->code == sf::Keyboard::Key::B) {
                if (!m_buildingSystem.isPlacing()) {
                    m_buildingSystem.startPlacement(Building::Type::Wall);
                    NF_INFO("Building mode: Wall");
                } else {
                    m_buildingSystem.cancelPlacement();
                    NF_INFO("Building cancelled");
                }
            }
            // V键放置炮塔
            else if (keyPressed->code == sf::Keyboard::Key::V) {
                if (!m_buildingSystem.isPlacing()) {
                    m_buildingSystem.startPlacement(Building::Type::Turret);
                    NF_INFO("Building mode: Turret");
                } else {
                    m_buildingSystem.cancelPlacement();
                    NF_INFO("Building cancelled");
                }
            }
            // E键采集资源
            else if (keyPressed->code == sf::Keyboard::Key::E) {
                harvestNearbyResources();
            }
        }
        else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                // 左键确认放置建筑
                if (m_buildingSystem.isPlacing()) {
                    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(m_window);
                    sf::Vector2f mouseWorldPos = m_window.mapPixelToCoords(mousePixelPos);
                    m_buildingSystem.tryPlaceBuilding(mouseWorldPos, m_registry);
                }
            }
            else if (mousePressed->button == sf::Mouse::Button::Right) {
                // 右键取消放置
                if (m_buildingSystem.isPlacing()) {
                    m_buildingSystem.cancelPlacement();
                }
            }
        }
    }
}

void Application::update(float deltaTime) {
    // 更新游戏时间
    Time::update(deltaTime);
    
    // 更新采集进度
    updateHarvesting(deltaTime);
    
    // 临时：玩家输入控制
    if (m_registry.isValid(m_player)) {
        auto* velocity = m_registry.tryGetComponent<Velocity>(m_player);
        if (velocity) {
            const float moveSpeed = 200.f; // 像素/秒
            velocity->velocity = sf::Vector2f(0.f, 0.f);
            
            bool isMoving = false;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
                velocity->velocity.y = -moveSpeed;
                isMoving = true;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                velocity->velocity.y = moveSpeed;
                isMoving = true;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                velocity->velocity.x = -moveSpeed;
                isMoving = true;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                velocity->velocity.x = moveSpeed;
                isMoving = true;
            }
            
            // 移动会中断采集
            if (isMoving && m_registry.hasComponent<Harvesting>(m_player)) {
                cancelHarvesting();
                NF_INFO("移动中断采集");
            }
        }
    }
    
    // 更新移动系统
    m_movementSystem.update(deltaTime, m_registry);
    
    // 更新AI系统
    m_aiSystem.update(deltaTime, m_registry, m_player);
    
    // 更新炮塔系统
    m_turretSystem.update(deltaTime, m_registry);
    
    // 更新战斗系统（处理死亡）
    m_combatSystem.update(deltaTime, m_registry);
    
    // 更新波次系统
    m_waveSystem.update(deltaTime, m_registry);
    
    // 更新建筑系统
    m_buildingSystem.update(deltaTime, m_registry);
    
    // 更新资源系统（处理资源生产）
    m_resourceSystem.update(deltaTime, m_registry);
    
    // 更新视觉效果系统
    m_visualEffectsSystem.update(deltaTime, m_registry);
    
    // 更新鼠标位置用于建筑预览
    if (m_buildingSystem.isPlacing()) {
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(m_window);
        sf::Vector2f mouseWorldPos = m_window.mapPixelToCoords(mousePixelPos);
        m_buildingSystem.updatePreview(mouseWorldPos, m_registry);
    }
    
    // 更新物理/碰撞系统
    m_physicsSystem.update(deltaTime, m_registry);
    
    // 更新 HUD
    m_hud.update(deltaTime, m_registry, m_player);
    m_hud.updateWaveInfo(m_waveSystem.getCurrentWave(), m_waveSystem.getEnemiesRemaining());
    m_hud.updateResources(&m_resourceSystem);
    m_hud.updateBuildingCost(&m_buildingSystem);
}

void Application::render() {
    // 根据时间段设置背景色
    sf::Color bgColor;
    switch (Time::getTimeOfDay()) {
        case TimeOfDay::Day:
            bgColor = sf::Color(135, 206, 235); // 天蓝色
            break;
        case TimeOfDay::Dusk:
            bgColor = sf::Color(255, 140, 0);   // 橙色
            break;
        case TimeOfDay::Night:
            bgColor = sf::Color(20, 20, 40);    // 深蓝色
            break;
    }
    
    m_window.clear(bgColor);
    
    // 使用 ECS 渲染系统渲染所有实体
    m_renderingSystem.render(m_window, m_registry);
    
    // 渲染视觉效果(子弹、伤害数字、粒子)
    m_visualEffectsSystem.render(m_window, m_registry);
    
    // 渲染建筑预览
    m_buildingSystem.renderPreview(m_window);
    
    // 渲染 HUD
    m_hud.render(m_window);
    
    // 显示画面
    m_window.display();
}

void Application::initEntities() {
    NF_INFO("初始化游戏实体...");
    
    // 创建玩家
    sf::Vector2u windowSize = m_window.getSize();
    m_player = m_registry.createPlayer(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f));
    
    NF_INFO("玩家实体创建完成: {}", static_cast<uint32_t>(m_player));
    
    // 创建测试墙壁 - 中心的十字障碍物
    float centerX = windowSize.x / 2.f;
    float centerY = windowSize.y / 2.f;
    
    // 横向墙
    auto wall1 = m_registry.createEntity();
    m_registry.addComponent<Transform>(wall1, sf::Vector2f(centerX, centerY - 100.f));
    auto& sprite1 = m_registry.addComponent<Sprite>(wall1, "placeholder", 2);
    sprite1.color = sf::Color(100, 100, 100);
    m_registry.addComponent<Collider>(wall1, 200.f, 30.f);
    m_registry.addComponent<Static>(wall1);
    
    // 纵向墙
    auto wall2 = m_registry.createEntity();
    m_registry.addComponent<Transform>(wall2, sf::Vector2f(centerX - 150.f, centerY));
    auto& sprite2 = m_registry.addComponent<Sprite>(wall2, "placeholder", 2);
    sprite2.color = sf::Color(100, 100, 100);
    m_registry.addComponent<Collider>(wall2, 30.f, 200.f);
    m_registry.addComponent<Static>(wall2);
    
    // 右侧墙
    auto wall3 = m_registry.createEntity();
    m_registry.addComponent<Transform>(wall3, sf::Vector2f(centerX + 150.f, centerY));
    auto& sprite3 = m_registry.addComponent<Sprite>(wall3, "placeholder", 2);
    sprite3.color = sf::Color(100, 100, 100);
    m_registry.addComponent<Collider>(wall3, 30.f, 200.f);
    m_registry.addComponent<Static>(wall3);
    
    NF_INFO("创建了3个测试墙壁实体");
    
    // 创建资源节点 - 树木和矿石
    std::vector<sf::Vector2f> treePositions = {
        {200.f, 150.f}, {300.f, 200.f}, {150.f, 400.f},
        {1000.f, 150.f}, {1100.f, 300.f}, {950.f, 500.f},
        {400.f, 600.f}, {800.f, 550.f}
    };
    
    std::vector<sf::Vector2f> orePositions = {
        {250.f, 500.f}, {350.f, 350.f},
        {1050.f, 500.f}, {900.f, 200.f},
        {600.f, 150.f}, {650.f, 600.f}
    };
    
    for (const auto& pos : treePositions) {
        m_registry.createResourceNode(pos, "wood", 50);  // 50单位木头
    }
    
    for (const auto& pos : orePositions) {
        m_registry.createResourceNode(pos, "metal", 30);  // 30单位金属
    }
    
    NF_INFO("创建了{}个树木资源节点和{}个矿石资源节点", 
            treePositions.size(), orePositions.size());
    
    // TODO: 后续添加更多实体（敌人、建筑等）
}

void Application::harvestNearbyResources() {
    if (!m_registry.isValid(m_player)) return;
    
    auto* playerTransform = m_registry.tryGetComponent<Transform>(m_player);
    if (!playerTransform) return;
    
    // 检查是否已经在采集中
    if (m_registry.hasComponent<Harvesting>(m_player)) {
        NF_INFO("已经在采集中");
        return;
    }
    
    const float harvestRange = 80.f;  // 采集范围
    entt::entity closestNode = entt::null;
    float closestDistSq = harvestRange * harvestRange;
    
    // 查找最近的可采集资源节点
    auto view = m_registry.view<Transform, ResourceNode>();
    for (auto entity : view) {
        const auto& nodeTransform = view.get<Transform>(entity);
        const auto& node = view.get<ResourceNode>(entity);
        
        // 跳过耗尽的资源
        if (node.isDepleted || node.resourceAmount <= 0) continue;
        
        // 计算距离
        float dx = nodeTransform.position.x - playerTransform->position.x;
        float dy = nodeTransform.position.y - playerTransform->position.y;
        float distSq = dx * dx + dy * dy;
        
        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            closestNode = entity;
        }
    }
    
    if (closestNode != entt::null) {
        startHarvesting(closestNode);
    } else {
        NF_INFO("附近没有可采集的资源");
    }
}

void Application::startHarvesting(entt::entity node) {
    if (!m_registry.isValid(m_player) || !m_registry.isValid(node)) return;
    
    auto* nodeTransform = m_registry.tryGetComponent<Transform>(node);
    auto* nodeResource = m_registry.tryGetComponent<ResourceNode>(node);
    
    if (!nodeTransform || !nodeResource) return;
    
    // 添加采集进度组件
    auto& harvesting = m_registry.addComponent<Harvesting>(m_player);
    harvesting.targetNode = node;
    harvesting.progress = 0.f;
    harvesting.harvestTime = nodeResource->harvestTime;
    harvesting.nodePosition = nodeTransform->position;
    
    // 停止玩家移动
    if (auto* velocity = m_registry.tryGetComponent<Velocity>(m_player)) {
        velocity->velocity = sf::Vector2f(0.f, 0.f);
    }
    
    NF_INFO("开始采集 {} (需要 {:.1f} 秒)", nodeResource->resourceType, nodeResource->harvestTime);
}

void Application::updateHarvesting(float deltaTime) {
    if (!m_registry.isValid(m_player)) return;
    
    auto* harvesting = m_registry.tryGetComponent<Harvesting>(m_player);
    if (!harvesting) return;
    
    // 检查目标节点是否还有效
    if (!m_registry.isValid(harvesting->targetNode)) {
        cancelHarvesting();
        return;
    }
    
    auto* nodeResource = m_registry.tryGetComponent<ResourceNode>(harvesting->targetNode);
    if (!nodeResource || nodeResource->isDepleted || nodeResource->resourceAmount <= 0) {
        cancelHarvesting();
        NF_INFO("资源节点已耗尽");
        return;
    }
    
    // 检查玩家是否移动（移动会中断采集）
    auto* playerTransform = m_registry.tryGetComponent<Transform>(m_player);
    auto* nodeTransform = m_registry.tryGetComponent<Transform>(harvesting->targetNode);
    if (playerTransform && nodeTransform) {
        float dx = nodeTransform->position.x - playerTransform->position.x;
        float dy = nodeTransform->position.y - playerTransform->position.y;
        float distSq = dx * dx + dy * dy;
        const float maxDistSq = 100.f * 100.f;  // 允许的最大距离
        
        if (distSq > maxDistSq) {
            cancelHarvesting();
            NF_INFO("离开采集范围，采集中断");
            return;
        }
    }
    
    // 更新进度
    harvesting->progress += deltaTime / harvesting->harvestTime;
    
    // 采集完成
    if (harvesting->progress >= 1.f) {
        // 采集资源
        int amountToHarvest = std::min(nodeResource->harvestAmount, nodeResource->resourceAmount);
        nodeResource->resourceAmount -= amountToHarvest;
        
        // 添加到玩家资源
        m_resourceSystem.addResource(nodeResource->resourceType, amountToHarvest);
        
        NF_INFO("成功采集了 {} 个 {}", amountToHarvest, nodeResource->resourceType);
        
        // 检查是否耗尽
        if (nodeResource->resourceAmount <= 0) {
            nodeResource->isDepleted = true;
            nodeResource->regenTimer = 0.f;
            
            // 改变耗尽节点的颜色
            if (auto* sprite = m_registry.tryGetComponent<Sprite>(harvesting->targetNode)) {
                sprite->color = sf::Color(100, 100, 100, 180);  // 变暗
            }
            
            NF_INFO("资源节点耗尽，{:.0f}秒后再生", nodeResource->regenTime);
        }
        
        // 移除采集组件
        m_registry.removeComponent<Harvesting>(m_player);
    }
}

void Application::cancelHarvesting() {
    if (m_registry.isValid(m_player) && m_registry.hasComponent<Harvesting>(m_player)) {
        m_registry.removeComponent<Harvesting>(m_player);
    }
}

} // namespace Nightfall

