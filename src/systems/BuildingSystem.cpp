#include "BuildingSystem.h"
#include "ResourceSystem.h"
#include "../ecs/Components.h"
#include "../core/Logger.h"
#include <cmath>

namespace Nightfall {

BuildingSystem::BuildingSystem() {
}

BuildingSystem::~BuildingSystem() {
    NF_INFO("Building system shutdown");
}

void BuildingSystem::init() {
    NF_INFO("Building system initialized");
    
    // 初始化预览形状
    m_previewShape.setFillColor(sf::Color(100, 100, 255, 100));
    m_previewShape.setOutlineThickness(2.f);
}

void BuildingSystem::update(float deltaTime, Registry& registry) {
    // 更新建造进度
    auto view = registry.view<Building, Transform>();
    for (auto entity : view) {
        auto& building = view.get<Building>(entity);
        
        if (!building.isComplete && building.constructionProgress < 1.f) {
            // 自动建造 (每秒增加50%进度)
            building.constructionProgress += deltaTime * 0.5f;
            
            if (building.constructionProgress >= 1.f) {
                building.constructionProgress = 1.f;
                building.isComplete = true;
                NF_INFO("Building completed: {}", static_cast<int>(building.type));
            }
        }
    }
}

void BuildingSystem::startPlacement(Building::Type buildingType) {
    m_isPlacing = true;
    m_currentBuildingType = buildingType;
    m_previewValid = false;
    
    // 设置预览形状大小
    sf::Vector2f size = getBuildingSize(buildingType);
    m_previewShape.setSize(size);
    m_previewShape.setOrigin(size / 2.f);
    
    NF_DEBUG("Started placement for building type: {}", static_cast<int>(buildingType));
}

void BuildingSystem::cancelPlacement() {
    m_isPlacing = false;
    NF_DEBUG("Placement cancelled");
}

bool BuildingSystem::tryPlaceBuilding(const sf::Vector2f& position, Registry& registry) {
    if (!m_isPlacing) return false;
    
    // 检查位置是否有效
    if (!isValidPlacement(position, registry)) {
        NF_WARN("Invalid placement position");
        return false;
    }
    
    // 检查资源是否足够
    if (m_resourceSystem) {
        BuildingCost cost = getBuildingCost(m_currentBuildingType);
        
        if (!m_resourceSystem->hasResource("wood", cost.wood) || 
            !m_resourceSystem->hasResource("metal", cost.metal)) {
            NF_WARN("Insufficient resources: need {}W/{}M, have {}W/{}M",
                   cost.wood, cost.metal,
                   m_resourceSystem->getResourceAmount("wood"),
                   m_resourceSystem->getResourceAmount("metal"));
            return false;
        }
        
        // 扣除资源
        m_resourceSystem->removeResource("wood", cost.wood);
        m_resourceSystem->removeResource("metal", cost.metal);
    }
    
    // 创建建筑实体
    entt::entity building = registry.createBuilding(position, m_currentBuildingType);
    
    NF_INFO("Building placed at ({}, {}), type: {}", 
            position.x, position.y, static_cast<int>(m_currentBuildingType));
    
    m_isPlacing = false;
    return true;
}

void BuildingSystem::updatePreview(const sf::Vector2f& mousePos, Registry& registry) {
    if (!m_isPlacing) return;
    
    m_previewPosition = mousePos;
    // TODO: 可以添加网格对齐
    
    // 检查位置是否有效（碰撞检测）
    bool validPosition = isValidPlacement(mousePos, registry);
    
    // 检查资源是否足够
    bool hasResources = true;
    if (m_resourceSystem) {
        auto cost = getBuildingCost(m_currentBuildingType);
        hasResources = m_resourceSystem->hasResource("wood", cost.wood) && 
                      m_resourceSystem->hasResource("metal", cost.metal);
    }
    
    // 只有位置有效且资源足够才显示绿色
    m_previewValid = validPosition && hasResources;
    
    // 更新预览颜色
    if (m_previewValid) {
        m_previewShape.setOutlineColor(sf::Color::Green);
        m_previewShape.setFillColor(sf::Color(100, 255, 100, 100));
    } else {
        m_previewShape.setOutlineColor(sf::Color::Red);
        m_previewShape.setFillColor(sf::Color(255, 100, 100, 100));
    }
    
    m_previewShape.setPosition(m_previewPosition);
}

void BuildingSystem::renderPreview(sf::RenderWindow& window) {
    if (!m_isPlacing) return;
    
    window.draw(m_previewShape);
}

bool BuildingSystem::isValidPlacement(const sf::Vector2f& position, Registry& registry) const {
    sf::Vector2f buildingSize = getBuildingSize(m_currentBuildingType);
    sf::FloatRect buildingBounds(
        sf::Vector2f(position.x - buildingSize.x / 2.f, position.y - buildingSize.y / 2.f),
        buildingSize
    );
    
    // 检查是否与其他建筑或墙壁重叠
    auto view = registry.view<Transform, Collider>();
    for (auto entity : view) {
        const auto& transform = view.get<Transform>(entity);
        const auto& collider = view.get<Collider>(entity);
        
        // 跳过玩家和僵尸
        if (registry.hasComponent<Player>(entity) || registry.hasComponent<Zombie>(entity)) {
            continue;
        }
        
        sf::FloatRect entityBounds(
            sf::Vector2f(transform.position.x - collider.size.x / 2.f,
                        transform.position.y - collider.size.y / 2.f),
            collider.size
        );
        
        if (buildingBounds.findIntersection(entityBounds).has_value()) {
            return false;
        }
    }
    
    // TODO: 检查是否在地图边界内
    
    return true;
}

BuildingSystem::BuildingCost BuildingSystem::getBuildingCost(Building::Type type) const {
    BuildingCost cost;
    
    switch (type) {
        case Building::Type::Wall:
            cost.wood = 10;
            cost.metal = 0;
            break;
        case Building::Type::Turret:
            cost.wood = 20;
            cost.metal = 30;
            break;
        case Building::Type::Gate:
            cost.wood = 15;
            cost.metal = 10;
            break;
        case Building::Type::Workshop:
            cost.wood = 50;
            cost.metal = 40;
            break;
        case Building::Type::Storage:
            cost.wood = 40;
            cost.metal = 20;
            break;
        case Building::Type::Farm:
            cost.wood = 60;
            cost.metal = 0;
            break;
        case Building::Type::House:
            cost.wood = 80;
            cost.metal = 30;
            break;
        default:
            break;
    }
    
    return cost;
}

sf::Vector2f BuildingSystem::getBuildingSize(Building::Type type) const {
    switch (type) {
        case Building::Type::Wall:
            return sf::Vector2f(64.f, 64.f);
        case Building::Type::Turret:
            return sf::Vector2f(48.f, 48.f);
        case Building::Type::Gate:
            return sf::Vector2f(64.f, 64.f);
        case Building::Type::Workshop:
            return sf::Vector2f(96.f, 96.f);
        case Building::Type::Storage:
            return sf::Vector2f(80.f, 80.f);
        case Building::Type::Farm:
            return sf::Vector2f(128.f, 128.f);
        case Building::Type::House:
            return sf::Vector2f(96.f, 96.f);
        default:
            return sf::Vector2f(64.f, 64.f);
    }
}

} // namespace Nightfall
