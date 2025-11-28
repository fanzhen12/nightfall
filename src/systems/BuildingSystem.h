#pragma once

#include "../ecs/Registry.h"
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

namespace Nightfall {

class ResourceSystem;

/// 建筑系统 - 处理建筑放置、建造、升级
class BuildingSystem {
public:
    BuildingSystem();
    ~BuildingSystem();

    void init();
    void update(float deltaTime, Registry& registry);
    
    void setResourceSystem(ResourceSystem* resources) { m_resourceSystem = resources; }

    /// 开始放置建筑
    void startPlacement(Building::Type buildingType);
    
    /// 取消放置
    void cancelPlacement();
    
    /// 确认放置建筑
    bool tryPlaceBuilding(const sf::Vector2f& position, Registry& registry);
    
    /// 更新放置预览
    void updatePreview(const sf::Vector2f& mousePos, Registry& registry);
    
    /// 渲染建筑预览
    void renderPreview(sf::RenderWindow& window);
    
    /// 检查位置是否可以放置建筑
    bool isValidPlacement(const sf::Vector2f& position, Registry& registry) const;
    
    /// 是否正在放置建筑
    bool isPlacing() const { return m_isPlacing; }
    
    /// 获取当前放置的建筑类型
    Building::Type getCurrentBuildingType() const { return m_currentBuildingType; }
    
    /// 获取当前建筑成本（用于UI显示）
    struct BuildingCost {
        int wood{0};
        int metal{0};
    };
    BuildingCost getCurrentBuildingCost() const { return getBuildingCost(m_currentBuildingType); }

private:
    /// 获取建筑成本
    BuildingCost getBuildingCost(Building::Type type) const;
    
    /// 获取建筑大小
    sf::Vector2f getBuildingSize(Building::Type type) const;

private:
    bool m_isPlacing{false};
    Building::Type m_currentBuildingType{Building::Type::Wall};
    sf::Vector2f m_previewPosition;
    bool m_previewValid{false};
    
    // 预览渲染
    sf::RectangleShape m_previewShape;
    
    ResourceSystem* m_resourceSystem{nullptr};
};

} // namespace Nightfall
