#pragma once

#include "../ecs/Registry.h"
#include <map>
#include <string>

namespace Nightfall {

/// 资源管理系统 - 处理游戏内资源（木材、金属、食物等）
class ResourceSystem {
public:
    ResourceSystem();
    ~ResourceSystem();

    void init();
    void update(float deltaTime, Registry& registry);

    /// 添加资源
    void addResource(const std::string& type, int amount);
    
    /// 移除资源（用于建造等）
    bool removeResource(const std::string& type, int amount);
    
    /// 检查是否有足够资源
    bool hasResource(const std::string& type, int amount) const;
    
    /// 获取资源数量
    int getResourceAmount(const std::string& type) const;
    
    /// 获取所有资源
    const std::map<std::string, int>& getAllResources() const { return m_resources; }
    
    /// 初始化起始资源
    void initStartingResources();

private:
    std::map<std::string, int> m_resources;
};

} // namespace Nightfall
