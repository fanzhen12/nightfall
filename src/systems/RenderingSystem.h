#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include "../ecs/Registry.h"

namespace Nightfall {

/// 渲染系统
/// 负责渲染所有可见实体
class RenderingSystem {
public:
    RenderingSystem() = default;
    ~RenderingSystem() = default;

    /// 初始化渲染系统
    void init();

    /// 渲染所有实体
    /// @param window 渲染目标窗口
    /// @param registry ECS 注册表
    void render(sf::RenderWindow& window, Registry& registry);

    /// 设置相机位置（用于视角跟随）
    void setCameraPosition(const sf::Vector2f& position);

    /// 获取相机位置
    sf::Vector2f getCameraPosition() const { return m_cameraPosition; }

private:
    /// 渲染采集进度条
    void renderHarvestingProgress(sf::RenderWindow& window, Registry& registry);

    sf::Vector2f m_cameraPosition{0.f, 0.f};
};

} // namespace Nightfall
