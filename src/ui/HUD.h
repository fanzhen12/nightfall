#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "UIElements.h"
#include "../ecs/Registry.h"

namespace Nightfall {

class ResourceSystem;
class BuildingSystem;

/// HUD（抬头显示）系统
/// 显示玩家状态、时间、小地图等信息
class HUD {
public:
    HUD();
    ~HUD() = default;

    /// 初始化 HUD
    void init(const sf::Font* font);

    /// 更新 HUD（从玩家实体读取数据）
    void update(float deltaTime, Registry& registry, entt::entity player);
    
    /// 更新波次信息
    void updateWaveInfo(int waveNumber, int enemiesRemaining);
    
    /// 更新资源显示
    void updateResources(ResourceSystem* resourceSystem);
    
    /// 更新建筑成本显示
    void updateBuildingCost(BuildingSystem* buildingSystem);

    /// 渲染 HUD
    void render(sf::RenderWindow& window);

    /// 显示/隐藏 HUD
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

private:
    void updatePlayerStats(Registry& registry, entt::entity player);
    void updateTimeDisplay();

private:
    bool m_visible{true};
    const sf::Font* m_font{nullptr};

    // UI 元素
    std::unique_ptr<UIPanel> m_statsPanel;
    std::unique_ptr<UIText> m_healthLabel;
    std::unique_ptr<UIProgressBar> m_healthBar;
    std::unique_ptr<UIText> m_hungerLabel;
    std::unique_ptr<UIProgressBar> m_hungerBar;
    std::unique_ptr<UIText> m_temperatureLabel;
    std::unique_ptr<UIProgressBar> m_temperatureBar;
    std::unique_ptr<UIText> m_staminaLabel;
    std::unique_ptr<UIProgressBar> m_staminaBar;
    
    std::unique_ptr<UIPanel> m_timePanel;
    std::unique_ptr<UIText> m_timeText;
    std::unique_ptr<UIText> m_dayText;
    
    // Wave info panel
    std::unique_ptr<UIPanel> m_wavePanel;
    std::unique_ptr<UIText> m_waveText;
    std::unique_ptr<UIText> m_enemiesText;
    
    // Resource panel
    std::unique_ptr<UIPanel> m_resourcePanel;
    std::unique_ptr<UIText> m_woodText;
    std::unique_ptr<UIText> m_metalText;
    std::unique_ptr<UIText> m_foodText;
    std::unique_ptr<UIText> m_scrapText;
    
    // Building cost panel (shown when placing buildings)
    std::unique_ptr<UIPanel> m_buildingCostPanel;
    std::unique_ptr<UIText> m_buildingCostTitle;
    std::unique_ptr<UIText> m_buildingCostWood;
    std::unique_ptr<UIText> m_buildingCostMetal;
    bool m_showBuildingCost{false};

    // FPS 显示
    std::unique_ptr<UIText> m_fpsText;
    float m_fpsUpdateTimer{0.f};
    int m_frameCount{0};
    float m_currentFps{0.f};
};

} // namespace Nightfall
