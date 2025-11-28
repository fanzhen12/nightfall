#include "VisualEffectsSystem.h"
#include "../ecs/Components.h"
#include "../core/Logger.h"
#include "../core/ResourceManager.h"
#include <cmath>
#include <sstream>
#include <iomanip>

namespace Nightfall {

VisualEffectsSystem::VisualEffectsSystem() {
}

VisualEffectsSystem::~VisualEffectsSystem() {
    NF_INFO("Visual effects system shutdown");
}

void VisualEffectsSystem::init() {
    m_font = ResourceManager::getInstance().getFont("default");
    NF_INFO("Visual effects system initialized");
}

void VisualEffectsSystem::update(float deltaTime, Registry& registry) {
    // 更新子弹实体(使用ECS的Temporary组件自动销毁)
    auto bulletView = registry.view<Transform, Velocity, Bullet>();
    for (auto entity : bulletView) {
        // 子弹由MovementSystem移动,这里只需检查碰撞
        // TODO: 可以添加子弹碰撞检测
    }
    
    // 更新伤害数字
    for (auto it = m_damageTexts.begin(); it != m_damageTexts.end();) {
        it->elapsed += deltaTime;
        it->position.y += it->velocity * deltaTime;
        
        if (it->elapsed >= it->lifetime) {
            it = m_damageTexts.erase(it);
        } else {
            ++it;
        }
    }
    
    // 更新攻击线条
    for (auto it = m_attackLines.begin(); it != m_attackLines.end();) {
        it->elapsed += deltaTime;
        
        if (it->elapsed >= it->lifetime) {
            it = m_attackLines.erase(it);
        } else {
            ++it;
        }
    }
    
    // 更新死亡粒子
    for (auto it = m_deathParticles.begin(); it != m_deathParticles.end();) {
        it->elapsed += deltaTime;
        it->position += it->velocity * deltaTime;
        it->velocity.y += 200.f * deltaTime; // 重力
        
        if (it->elapsed >= it->lifetime) {
            it = m_deathParticles.erase(it);
        } else {
            ++it;
        }
    }
}

void VisualEffectsSystem::render(sf::RenderWindow& window, Registry& registry) {
    // 渲染攻击线条
    for (const auto& line : m_attackLines) {
        float alpha = 1.f - (line.elapsed / line.lifetime);
        sf::Color color = line.color;
        color.a = static_cast<std::uint8_t>(alpha * 255);
        
        sf::Vertex vertices[2];
        vertices[0].position = line.from;
        vertices[0].color = color;
        vertices[1].position = line.to;
        vertices[1].color = color;
        
        window.draw(vertices, 2, sf::PrimitiveType::Lines);
    }
    
    // 渲染死亡粒子
    for (const auto& particle : m_deathParticles) {
        float alpha = 1.f - (particle.elapsed / particle.lifetime);
        sf::Color color = particle.color;
        color.a = static_cast<std::uint8_t>(alpha * 200);
        
        sf::CircleShape shape(particle.size);
        shape.setPosition(particle.position);
        shape.setOrigin(sf::Vector2f(particle.size, particle.size));
        shape.setFillColor(color);
        
        window.draw(shape);
    }
    
    // 渲染伤害数字
    if (m_font) {
        for (const auto& dmgText : m_damageTexts) {
            float alpha = 1.f - (dmgText.elapsed / dmgText.lifetime);
            sf::Color color = dmgText.color;
            color.a = static_cast<std::uint8_t>(alpha * 255);
            
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(0) << dmgText.damage;
            
            sf::Text text(*m_font, oss.str(), 16);
            text.setPosition(dmgText.position);
            text.setFillColor(color);
            text.setOutlineColor(sf::Color::Black);
            text.setOutlineThickness(1.f);
            
            // 居中对齐
            sf::FloatRect bounds = text.getLocalBounds();
            text.setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f));
            
            window.draw(text);
        }
    }
}

void VisualEffectsSystem::createBullet(const sf::Vector2f& from, const sf::Vector2f& to, Registry& registry) {
    // 计算方向
    sf::Vector2f direction = to - from;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.f) {
        direction /= length;
    }
    
    // 创建子弹实体
    auto bullet = registry.createEntity();
    registry.addComponent<Transform>(bullet, from);
    registry.addComponent<Velocity>(bullet, direction.x * 500.f, direction.y * 500.f, 500.f);
    
    // 添加Sprite组件(黄色小圆点)
    auto& sprite = registry.addComponent<Sprite>(bullet, "", 10);
    sprite.color = sf::Color::Yellow;
    sprite.scale = sf::Vector2f(0.3f, 0.3f);
    
    // 添加Bullet标记
    registry.addComponent<Bullet>(bullet);
    
    // 添加Temporary组件(0.5秒后自动销毁)
    registry.addComponent<Temporary>(bullet).lifetime = 0.5f;
    
    NF_DEBUG("Created bullet from ({}, {}) to ({}, {})", from.x, from.y, to.x, to.y);
}

void VisualEffectsSystem::createDamageNumber(const sf::Vector2f& position, float damage, bool isCritical) {
    DamageText dmgText;
    dmgText.position = position + sf::Vector2f(0.f, -20.f); // 稍微偏上
    dmgText.damage = damage;
    dmgText.color = isCritical ? sf::Color::Red : sf::Color::Yellow;
    dmgText.lifetime = 1.5f;
    dmgText.velocity = -50.f; // 向上飘
    
    m_damageTexts.push_back(dmgText);
}

void VisualEffectsSystem::createAttackLine(const sf::Vector2f& from, const sf::Vector2f& to) {
    AttackLine line;
    line.from = from;
    line.to = to;
    line.color = sf::Color(255, 255, 0, 200); // 黄色半透明
    line.lifetime = 0.15f;
    
    m_attackLines.push_back(line);
}

void VisualEffectsSystem::createDeathEffect(const sf::Vector2f& position, const sf::Color& color) {
    // 创建多个粒子
    for (int i = 0; i < 10; ++i) {
        DeathParticle particle;
        particle.position = position;
        
        // 随机方向
        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.f;
        float speed = 100.f + static_cast<float>(rand() % 100);
        particle.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed - 100.f);
        
        particle.color = color;
        particle.lifetime = 0.8f + static_cast<float>(rand() % 400) / 1000.f;
        particle.size = 2.f + static_cast<float>(rand() % 4);
        
        m_deathParticles.push_back(particle);
    }
}

} // namespace Nightfall
