#include "ResourceSystem.h"
#include "../core/Logger.h"

namespace Nightfall {

ResourceSystem::ResourceSystem() {
}

ResourceSystem::~ResourceSystem() {
    NF_INFO("Resource system shutdown");
}

void ResourceSystem::init() {
    initStartingResources();
    NF_INFO("Resource system initialized");
}

void ResourceSystem::update(float deltaTime, Registry& registry) {
    // 更新资源生产建筑
    auto producerView = registry.view<Building, Producer>();
    for (auto entity : producerView) {
        const auto& building = producerView.get<Building>(entity);
        auto& producer = producerView.get<Producer>(entity);
        
        // 只有建造完成的建筑才能生产
        if (!building.isComplete || !producer.isActive) continue;
        
        producer.productionTimer += deltaTime;
        
        if (producer.productionTimer >= producer.productionInterval) {
            producer.productionTimer = 0.f;
            addResource(producer.resourceType, producer.productionAmount);
            NF_DEBUG("Building {} produced {} {}", 
                     static_cast<uint32_t>(entity),
                     producer.productionAmount,
                     producer.resourceType);
        }
    }
    
    // 更新资源节点再生
    auto nodeView = registry.view<ResourceNode>();
    for (auto entity : nodeView) {
        auto& node = nodeView.get<ResourceNode>(entity);
        
        // 如果资源耗尽且可以再生
        if (node.isDepleted && node.regenTime > 0.f) {
            node.regenTimer += deltaTime;
            
            if (node.regenTimer >= node.regenTime) {
                node.regenTimer = 0.f;
                node.resourceAmount = node.maxResourceAmount;
                node.isDepleted = false;
                
                // 恢复节点颜色
                if (auto* sprite = registry.tryGetComponent<Sprite>(entity)) {
                    sprite->color = sf::Color(255, 255, 255, 255);  // 恢复正常颜色
                }
                
                NF_INFO("Resource node {} regenerated", static_cast<uint32_t>(entity));
            }
        }
    }
}

void ResourceSystem::addResource(const std::string& type, int amount) {
    if (amount <= 0) return;
    
    m_resources[type] += amount;
    NF_DEBUG("Added {} {} (total: {})", amount, type, m_resources[type]);
}

bool ResourceSystem::removeResource(const std::string& type, int amount) {
    if (amount <= 0) return true;
    
    if (!hasResource(type, amount)) {
        NF_WARN("Insufficient {}: need {}, have {}", type, amount, getResourceAmount(type));
        return false;
    }
    
    m_resources[type] -= amount;
    NF_DEBUG("Removed {} {} (remaining: {})", amount, type, m_resources[type]);
    return true;
}

bool ResourceSystem::hasResource(const std::string& type, int amount) const {
    auto it = m_resources.find(type);
    if (it == m_resources.end()) return false;
    return it->second >= amount;
}

int ResourceSystem::getResourceAmount(const std::string& type) const {
    auto it = m_resources.find(type);
    if (it == m_resources.end()) return 0;
    return it->second;
}

void ResourceSystem::initStartingResources() {
    // 初始资源
    m_resources["wood"] = 100;      // 木材
    m_resources["metal"] = 50;      // 金属
    m_resources["food"] = 20;       // 食物
    m_resources["scrap"] = 0;       // 废料（从僵尸获得）
    
    NF_INFO("Starting resources initialized: Wood={}, Metal={}, Food={}, Scrap={}", 
            m_resources["wood"], m_resources["metal"], 
            m_resources["food"], m_resources["scrap"]);
}

} // namespace Nightfall
