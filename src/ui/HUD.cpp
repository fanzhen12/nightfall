#include "HUD.h"
#include "../systems/ResourceSystem.h"
#include "../systems/BuildingSystem.h"
#include "../core/Time.h"
#include "../core/Logger.h"
#include <sstream>
#include <iomanip>

namespace Nightfall {

HUD::HUD() {
    NF_INFO("HUD 系统初始化");
}

void HUD::init(const sf::Font* font) {
    m_font = font;
    
    if (!m_font) {
        NF_WARN("HUD 初始化：未提供字体，文本将不显示");
        return;
    }

    // 创建状态面板（左上角）
    m_statsPanel = std::make_unique<UIPanel>(sf::Vector2f(250.f, 180.f), sf::Color(0, 0, 0, 150));
    m_statsPanel->setPosition(sf::Vector2f(10.f, 10.f));
    m_statsPanel->setBorderColor(sf::Color(100, 100, 100), 2.f);

    float yOffset = 20.f;
    float barWidth = 200.f;
    float barHeight = 15.f;
    float labelX = 20.f;
    float barX = 30.f;
    float spacing = 35.f;

    // Health
    m_healthLabel = std::make_unique<UIText>("Health", *m_font, 16);
    m_healthLabel->setPosition(sf::Vector2f(labelX, yOffset));
    m_healthLabel->setColor(sf::Color::White);
    
    m_healthBar = std::make_unique<UIProgressBar>(
        sf::Vector2f(barWidth, barHeight), 
        sf::Color(200, 50, 50), 
        sf::Color(50, 20, 20)
    );
    m_healthBar->setPosition(sf::Vector2f(barX, yOffset + 20.f));
    yOffset += spacing;

    // Hunger
    m_hungerLabel = std::make_unique<UIText>("Hunger", *m_font, 16);
    m_hungerLabel->setPosition(sf::Vector2f(labelX, yOffset + 20.f));
    m_hungerLabel->setColor(sf::Color::White);
    
    m_hungerBar = std::make_unique<UIProgressBar>(
        sf::Vector2f(barWidth, barHeight), 
        sf::Color(200, 150, 50), 
        sf::Color(80, 60, 20)
    );
    m_hungerBar->setPosition(sf::Vector2f(barX, yOffset + 40.f));
    yOffset += spacing;

    // Temperature
    m_temperatureLabel = std::make_unique<UIText>("Temperature", *m_font, 16);
    m_temperatureLabel->setPosition(sf::Vector2f(labelX, yOffset + 40.f));
    m_temperatureLabel->setColor(sf::Color::White);
    
    m_temperatureBar = std::make_unique<UIProgressBar>(
        sf::Vector2f(barWidth, barHeight), 
        sf::Color(50, 150, 200), 
        sf::Color(20, 60, 80)
    );
    m_temperatureBar->setPosition(sf::Vector2f(barX, yOffset + 60.f));
    yOffset += spacing;

    // Stamina
    m_staminaLabel = std::make_unique<UIText>("Stamina", *m_font, 16);
    m_staminaLabel->setPosition(sf::Vector2f(labelX, yOffset + 60.f));
    m_staminaLabel->setColor(sf::Color::White);
    
    m_staminaBar = std::make_unique<UIProgressBar>(
        sf::Vector2f(barWidth, barHeight), 
        sf::Color(100, 200, 100), 
        sf::Color(40, 80, 40)
    );
    m_staminaBar->setPosition(sf::Vector2f(barX, yOffset + 80.f));

    // 时间面板（右上角）
    m_timePanel = std::make_unique<UIPanel>(sf::Vector2f(220.f, 80.f), sf::Color(0, 0, 0, 150));
    m_timePanel->setPosition(sf::Vector2f(1050.f, 10.f));
    m_timePanel->setBorderColor(sf::Color(100, 100, 100), 2.f);

    m_timeText = std::make_unique<UIText>("Time: 06:00", *m_font, 18);
    m_timeText->setPosition(sf::Vector2f(1065.f, 25.f));
    m_timeText->setColor(sf::Color(255, 255, 150));

    m_dayText = std::make_unique<UIText>("Day 1 - Daytime", *m_font, 16);
    m_dayText->setPosition(sf::Vector2f(1065.f, 55.f));
    m_dayText->setColor(sf::Color(150, 200, 255));
    
    // Wave info panel (上中)
    m_wavePanel = std::make_unique<UIPanel>(sf::Vector2f(280.f, 80.f), sf::Color(0, 0, 0, 150));
    m_wavePanel->setPosition(sf::Vector2f(500.f, 10.f));
    m_wavePanel->setBorderColor(sf::Color(200, 50, 50), 2.f);
    
    m_waveText = std::make_unique<UIText>("Wave 0", *m_font, 20);
    m_waveText->setPosition(sf::Vector2f(515.f, 25.f));
    m_waveText->setColor(sf::Color(255, 100, 100));
    
    m_enemiesText = std::make_unique<UIText>("Enemies: 0", *m_font, 16);
    m_enemiesText->setPosition(sf::Vector2f(515.f, 55.f));
    m_enemiesText->setColor(sf::Color(255, 200, 200));

    // FPS 显示（右下角）
    m_fpsText = std::make_unique<UIText>("FPS: 60", *m_font, 14);
    m_fpsText->setPosition(sf::Vector2f(1200.f, 690.f));
    m_fpsText->setColor(sf::Color(200, 200, 200));

    // 创建资源面板（左下角）
    m_resourcePanel = std::make_unique<UIPanel>(sf::Vector2f(280.f, 120.f), sf::Color(0, 0, 0, 150));
    m_resourcePanel->setPosition(sf::Vector2f(10.f, 580.f));
    m_resourcePanel->setBorderColor(sf::Color(100, 150, 100), 2.f);

    // 资源文本
    float resourceY = 590.f;
    float resourceSpacing = 25.f;
    
    m_woodText = std::make_unique<UIText>("Wood: 0", *m_font, 18);
    m_woodText->setPosition(sf::Vector2f(20.f, resourceY));
    m_woodText->setColor(sf::Color(139, 69, 19)); // 棕色
    
    m_metalText = std::make_unique<UIText>("Metal: 0", *m_font, 18);
    m_metalText->setPosition(sf::Vector2f(160.f, resourceY));
    m_metalText->setColor(sf::Color(192, 192, 192)); // 银色
    
    m_foodText = std::make_unique<UIText>("Food: 0", *m_font, 18);
    m_foodText->setPosition(sf::Vector2f(20.f, resourceY + resourceSpacing));
    m_foodText->setColor(sf::Color(255, 215, 0)); // 金色
    
    m_scrapText = std::make_unique<UIText>("Scrap: 0", *m_font, 18);
    m_scrapText->setPosition(sf::Vector2f(160.f, resourceY + resourceSpacing));
    m_scrapText->setColor(sf::Color(100, 100, 120)); // 灰色

    // 创建建筑成本面板（中下方，默认隐藏）
    m_buildingCostPanel = std::make_unique<UIPanel>(sf::Vector2f(220.f, 100.f), sf::Color(0, 0, 0, 180));
    m_buildingCostPanel->setPosition(sf::Vector2f(530.f, 580.f));
    m_buildingCostPanel->setBorderColor(sf::Color(255, 200, 100), 2.f);
    
    m_buildingCostTitle = std::make_unique<UIText>("Building Cost:", *m_font, 18);
    m_buildingCostTitle->setPosition(sf::Vector2f(545.f, 590.f));
    m_buildingCostTitle->setColor(sf::Color(255, 200, 100));
    
    m_buildingCostWood = std::make_unique<UIText>("Wood: 0", *m_font, 16);
    m_buildingCostWood->setPosition(sf::Vector2f(545.f, 620.f));
    m_buildingCostWood->setColor(sf::Color(139, 69, 19));
    
    m_buildingCostMetal = std::make_unique<UIText>("Metal: 0", *m_font, 16);
    m_buildingCostMetal->setPosition(sf::Vector2f(545.f, 645.f));
    m_buildingCostMetal->setColor(sf::Color(192, 192, 192));

    NF_INFO("HUD UI 元素创建完成");
}

void HUD::update(float deltaTime, Registry& registry, entt::entity player) {
    if (!m_visible) return;

    // 更新玩家状态
    updatePlayerStats(registry, player);

    // 更新时间显示
    updateTimeDisplay();

    // 更新 FPS
    m_frameCount++;
    m_fpsUpdateTimer += deltaTime;
    if (m_fpsUpdateTimer >= 0.5f) {
        m_currentFps = m_frameCount / m_fpsUpdateTimer;
        m_frameCount = 0;
        m_fpsUpdateTimer = 0.f;

        if (m_fpsText) {
            std::ostringstream oss;
            oss << "FPS: " << std::fixed << std::setprecision(0) << m_currentFps;
            m_fpsText->setText(oss.str());
        }
    }
}

void HUD::updatePlayerStats(Registry& registry, entt::entity player) {
    if (!registry.isValid(player)) return;

    // 更新生命值
    if (const auto* health = registry.tryGetComponent<Health>(player)) {
        m_healthBar->setValue(health->getPercentage());
    }

    // 更新饥饿度
    if (const auto* hunger = registry.tryGetComponent<Hunger>(player)) {
        m_hungerBar->setValue(hunger->getPercentage());
    }

    // 更新体温
    if (const auto* temp = registry.tryGetComponent<Temperature>(player)) {
        float normalized = (temp->current - temp->minimum) / (temp->maximum - temp->minimum);
        m_temperatureBar->setValue(normalized);
    }

    // 更新精力
    if (const auto* stamina = registry.tryGetComponent<Stamina>(player)) {
        m_staminaBar->setValue(stamina->getPercentage());
    }
}

void HUD::updateTimeDisplay() {
    if (!m_timeText || !m_dayText) return;

    // Update time text
    int hour = Time::getHour();
    int minute = Time::getMinute();
    std::ostringstream timeOss;
    timeOss << "Time: " << std::setfill('0') << std::setw(2) << hour 
            << ":" << std::setw(2) << minute;
    m_timeText->setText(timeOss.str());

    // Update day and period
    int day = Time::getDay();
    std::string periodName;
    sf::Color periodColor;

    switch (Time::getTimeOfDay()) {
        case TimeOfDay::Day:
            periodName = "Daytime";
            periodColor = sf::Color(150, 200, 255);
            break;
        case TimeOfDay::Dusk:
            periodName = "Dusk";
            periodColor = sf::Color(255, 180, 100);
            break;
        case TimeOfDay::Night:
            periodName = "Night";
            periodColor = sf::Color(100, 100, 150);
            break;
    }

    std::ostringstream dayOss;
    dayOss << "Day " << day << " - " << periodName;
    m_dayText->setText(dayOss.str());
    m_dayText->setColor(periodColor);
}

void HUD::updateResources(ResourceSystem* resourceSystem) {
    if (!resourceSystem) return;

    if (m_woodText) {
        std::ostringstream oss;
        oss << "Wood: " << resourceSystem->getResourceAmount("wood");
        m_woodText->setText(oss.str());
    }

    if (m_metalText) {
        std::ostringstream oss;
        oss << "Metal: " << resourceSystem->getResourceAmount("metal");
        m_metalText->setText(oss.str());
    }

    if (m_foodText) {
        std::ostringstream oss;
        oss << "Food: " << resourceSystem->getResourceAmount("food");
        m_foodText->setText(oss.str());
    }

    if (m_scrapText) {
        std::ostringstream oss;
        oss << "Scrap: " << resourceSystem->getResourceAmount("scrap");
        m_scrapText->setText(oss.str());
    }
}

void HUD::updateBuildingCost(BuildingSystem* buildingSystem) {
    if (!buildingSystem) {
        m_showBuildingCost = false;
        return;
    }
    
    m_showBuildingCost = buildingSystem->isPlacing();
    
    if (m_showBuildingCost) {
        auto cost = buildingSystem->getCurrentBuildingCost();
        
        if (m_buildingCostWood) {
            std::ostringstream oss;
            oss << "Wood: " << cost.wood;
            m_buildingCostWood->setText(oss.str());
        }
        
        if (m_buildingCostMetal) {
            std::ostringstream oss;
            oss << "Metal: " << cost.metal;
            m_buildingCostMetal->setText(oss.str());
        }
    }
}

void HUD::updateWaveInfo(int waveNumber, int enemiesRemaining) {
    if (m_waveText) {
        std::ostringstream oss;
        oss << "Wave " << waveNumber;
        m_waveText->setText(oss.str());
    }
    
    if (m_enemiesText) {
        std::ostringstream oss;
        oss << "Enemies: " << enemiesRemaining;
        m_enemiesText->setText(oss.str());
    }
}

void HUD::render(sf::RenderWindow& window) {
    if (!m_visible || !m_font) return;

    // 渲染状态面板
    if (m_statsPanel) m_statsPanel->render(window);
    if (m_healthLabel) m_healthLabel->render(window);
    if (m_healthBar) m_healthBar->render(window);
    if (m_hungerLabel) m_hungerLabel->render(window);
    if (m_hungerBar) m_hungerBar->render(window);
    if (m_temperatureLabel) m_temperatureLabel->render(window);
    if (m_temperatureBar) m_temperatureBar->render(window);
    if (m_staminaLabel) m_staminaLabel->render(window);
    if (m_staminaBar) m_staminaBar->render(window);

    // 渲染时间面板
    if (m_timePanel) m_timePanel->render(window);
    if (m_timeText) m_timeText->render(window);
    if (m_dayText) m_dayText->render(window);
    
    // 渲染波次面板
    if (m_wavePanel) m_wavePanel->render(window);
    if (m_waveText) m_waveText->render(window);
    if (m_enemiesText) m_enemiesText->render(window);

    // 渲染 FPS
    if (m_fpsText) m_fpsText->render(window);

    // 渲染资源面板
    if (m_resourcePanel) m_resourcePanel->render(window);
    if (m_woodText) m_woodText->render(window);
    if (m_metalText) m_metalText->render(window);
    if (m_foodText) m_foodText->render(window);
    if (m_scrapText) m_scrapText->render(window);
    
    // 渲染建筑成本面板（仅在建造模式下）
    if (m_showBuildingCost) {
        if (m_buildingCostPanel) m_buildingCostPanel->render(window);
        if (m_buildingCostTitle) m_buildingCostTitle->render(window);
        if (m_buildingCostWood) m_buildingCostWood->render(window);
        if (m_buildingCostMetal) m_buildingCostMetal->render(window);
    }
}

} // namespace Nightfall
