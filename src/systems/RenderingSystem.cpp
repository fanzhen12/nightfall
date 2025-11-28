#include "RenderingSystem.h"
#include "../core/ResourceManager.h"
#include "../core/Logger.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <algorithm>

namespace Nightfall {

void RenderingSystem::init() {
    NF_INFO("渲染系统初始化");
}

void RenderingSystem::render(sf::RenderWindow& window, Registry& registry) {
    // 收集所有需要渲染的实体
    struct RenderData {
        entt::entity entity;
        const Transform* transform;
        const Sprite* sprite;
        const Collider* collider;
    };

    std::vector<RenderData> renderQueue;

    // 遍历所有有 Transform 和 Sprite 组件的实体
    registry.each<Transform, Sprite>([&](entt::entity entity, const Transform& transform, const Sprite& sprite) {
        if (sprite.visible) {
            const Collider* collider = registry.tryGetComponent<Collider>(entity);
            renderQueue.push_back({entity, &transform, &sprite, collider});
        }
    });

    // 按 zOrder 排序（从小到大，后面的会覆盖前面的）
    std::sort(renderQueue.begin(), renderQueue.end(), 
        [](const RenderData& a, const RenderData& b) {
            return a.sprite->zOrder < b.sprite->zOrder;
        });

    // 渲染
    auto& resourceMgr = ResourceManager::getInstance();
    
    for (const auto& data : renderQueue) {
        // 获取纹理
        sf::Texture* texture = resourceMgr.getTexture(data.sprite->textureId);
        if (!texture) {
            // 使用占位符纹理
            texture = resourceMgr.getTexture("placeholder");
            if (!texture) continue;
        }

        // 创建临时精灵对象
        sf::Sprite drawableSprite(*texture);
        
        // 设置变换
        drawableSprite.setPosition(data.transform->position - m_cameraPosition);
        drawableSprite.setRotation(sf::degrees(data.transform->rotation));
        
        // 如果有碰撞体，根据碰撞体大小缩放精灵
        auto texSize = texture->getSize();
        if (data.collider) {
            float scaleX = data.collider->size.x / static_cast<float>(texSize.x);
            float scaleY = data.collider->size.y / static_cast<float>(texSize.y);
            drawableSprite.setScale(sf::Vector2f(
                scaleX * data.transform->scale.x * data.sprite->scale.x, 
                scaleY * data.transform->scale.y * data.sprite->scale.y));
        } else {
            drawableSprite.setScale(sf::Vector2f(
                data.transform->scale.x * data.sprite->scale.x, 
                data.transform->scale.y * data.sprite->scale.y));
        }
        
        drawableSprite.setColor(data.sprite->color);
        
        // 设置原点为中心
        drawableSprite.setOrigin(sf::Vector2f(texSize.x / 2.f, texSize.y / 2.f));

        window.draw(drawableSprite);
    }
    
    // 渲染采集进度条
    renderHarvestingProgress(window, registry);
}

void RenderingSystem::renderHarvestingProgress(sf::RenderWindow& window, Registry& registry) {
    auto view = registry.view<Harvesting>();
    for (auto entity : view) {
        const auto& harvesting = view.get<Harvesting>(entity);
        
        // 进度条位置在节点上方
        sf::Vector2f barPosition = harvesting.nodePosition - m_cameraPosition;
        barPosition.y -= 40.f;  // 在节点上方40像素
        
        const float barWidth = 60.f;
        const float barHeight = 8.f;
        
        // 背景（黑色半透明）
        sf::RectangleShape background({barWidth, barHeight});
        background.setPosition(barPosition);
        background.setFillColor(sf::Color(0, 0, 0, 180));
        background.setOutlineColor(sf::Color(255, 255, 255, 200));
        background.setOutlineThickness(1.f);
        window.draw(background);
        
        // 进度（绿色）
        float progressWidth = barWidth * std::clamp(harvesting.progress, 0.f, 1.f);
        if (progressWidth > 0.f) {
            sf::RectangleShape progressBar({progressWidth, barHeight});
            progressBar.setPosition(barPosition);
            progressBar.setFillColor(sf::Color(50, 200, 50, 220));
            window.draw(progressBar);
        }
    }
}

void RenderingSystem::setCameraPosition(const sf::Vector2f& position) {
    m_cameraPosition = position;
}

} // namespace Nightfall
