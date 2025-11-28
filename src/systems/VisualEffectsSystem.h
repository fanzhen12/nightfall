#pragma once

#include "../ecs/Registry.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

namespace Nightfall {

/// 视觉效果系统 - 处理子弹、伤害数字、粒子效果
class VisualEffectsSystem {
public:
    VisualEffectsSystem();
    ~VisualEffectsSystem();

    void init();
    void update(float deltaTime, Registry& registry);
    void render(sf::RenderWindow& window, Registry& registry);

    /// 创建子弹实体(从炮塔到目标)
    void createBullet(const sf::Vector2f& from, const sf::Vector2f& to, Registry& registry);
    
    /// 创建伤害数字
    void createDamageNumber(const sf::Vector2f& position, float damage, bool isCritical = false);
    
    /// 创建攻击线条(即时攻击的视觉反馈)
    void createAttackLine(const sf::Vector2f& from, const sf::Vector2f& to);
    
    /// 创建死亡粒子效果
    void createDeathEffect(const sf::Vector2f& position, const sf::Color& color);

private:
    /// 伤害数字结构
    struct DamageText {
        sf::Vector2f position;
        float damage;
        float lifetime{1.5f};
        float elapsed{0.f};
        sf::Color color;
        float velocity{-50.f}; // 向上飘
    };
    
    /// 攻击线条结构
    struct AttackLine {
        sf::Vector2f from;
        sf::Vector2f to;
        float lifetime{0.15f};
        float elapsed{0.f};
        sf::Color color;
    };
    
    /// 死亡粒子
    struct DeathParticle {
        sf::Vector2f position;
        sf::Vector2f velocity;
        float lifetime{1.0f};
        float elapsed{0.f};
        sf::Color color;
        float size{4.f};
    };

    std::vector<DamageText> m_damageTexts;
    std::vector<AttackLine> m_attackLines;
    std::vector<DeathParticle> m_deathParticles;
    
    sf::Font* m_font{nullptr};
};

} // namespace Nightfall
