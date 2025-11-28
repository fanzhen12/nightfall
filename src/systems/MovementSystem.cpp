#include "MovementSystem.h"
#include "../core/Logger.h"
#include <cmath>

namespace Nightfall {

void MovementSystem::init() {
    NF_INFO("移动系统初始化");
}

void MovementSystem::update(float deltaTime, Registry& registry) {
    // 遍历所有有 Transform 和 Velocity 组件的实体
    registry.each<Transform, Velocity>([&](entt::entity entity, Transform& transform, Velocity& velocity) {
        // 限制速度
        clampVelocity(velocity);

        // 更新位置
        transform.position += velocity.velocity * deltaTime;
    });
}

void MovementSystem::clampVelocity(Velocity& velocity) const {
    float speed = std::sqrt(velocity.velocity.x * velocity.velocity.x + 
                           velocity.velocity.y * velocity.velocity.y);
    
    if (speed > velocity.maxSpeed) {
        float scale = velocity.maxSpeed / speed;
        velocity.velocity.x *= scale;
        velocity.velocity.y *= scale;
    }
}

} // namespace Nightfall
