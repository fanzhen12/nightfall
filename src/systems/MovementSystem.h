#pragma once

#include "../ecs/Registry.h"

namespace Nightfall {

/// 移动系统
/// 处理所有实体的速度和位置更新
class MovementSystem {
public:
    MovementSystem() = default;
    ~MovementSystem() = default;

    /// 初始化移动系统
    void init();

    /// 更新所有实体的位置
    /// @param deltaTime 帧时间间隔（秒）
    /// @param registry ECS 注册表
    void update(float deltaTime, Registry& registry);

private:
    /// 限制速度到最大值
    void clampVelocity(Velocity& velocity) const;
};

} // namespace Nightfall
