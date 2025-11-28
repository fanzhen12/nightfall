#include "UIElements.h"
#include "../core/Logger.h"

namespace Nightfall {

// ==================== UIText ====================

UIText::UIText(const std::string& text, const sf::Font& font, unsigned int size)
    : m_text(text), m_font(&font), m_characterSize(size) {
}

void UIText::render(sf::RenderWindow& window) {
    if (!m_visible || !m_font) return;

    sf::Text text(*m_font);
    text.setString(m_text);
    text.setCharacterSize(m_characterSize);
    text.setPosition(m_position);
    text.setFillColor(m_color);
    window.draw(text);
}

// ==================== UIProgressBar ====================

UIProgressBar::UIProgressBar(const sf::Vector2f& size, const sf::Color& fillColor, const sf::Color& bgColor)
    : m_size(size), m_fillColor(fillColor), m_backgroundColor(bgColor) {
}

void UIProgressBar::render(sf::RenderWindow& window) {
    if (!m_visible) return;

    // 背景
    sf::RectangleShape background(m_size);
    background.setPosition(m_position);
    background.setFillColor(m_backgroundColor);
    window.draw(background);

    // 填充
    sf::RectangleShape fill(sf::Vector2f(m_size.x * m_value, m_size.y));
    fill.setPosition(m_position);
    fill.setFillColor(m_fillColor);
    window.draw(fill);

    // 边框
    sf::RectangleShape border(m_size);
    border.setPosition(m_position);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(1.f);
    border.setOutlineColor(sf::Color(200, 200, 200, 150));
    window.draw(border);
}

// ==================== UIPanel ====================

UIPanel::UIPanel(const sf::Vector2f& size, const sf::Color& bgColor)
    : m_size(size), m_backgroundColor(bgColor) {
}

void UIPanel::render(sf::RenderWindow& window) {
    if (!m_visible) return;

    sf::RectangleShape panel(m_size);
    panel.setPosition(m_position);
    panel.setFillColor(m_backgroundColor);
    
    if (m_borderThickness > 0) {
        panel.setOutlineThickness(m_borderThickness);
        panel.setOutlineColor(m_borderColor);
    }
    
    window.draw(panel);
}

// ==================== UIButton ====================

UIButton::UIButton(const std::string& text, const sf::Font& font, const sf::Vector2f& size)
    : m_text(text), m_font(&font), m_size(size), m_currentColor(m_normalColor) {
}

bool UIButton::contains(const sf::Vector2f& point) const {
    return point.x >= m_position.x && point.x <= m_position.x + m_size.x &&
           point.y >= m_position.y && point.y <= m_position.y + m_size.y;
}

void UIButton::onClick() {
    if (m_callback) {
        m_callback();
    }
}

void UIButton::onHover(bool hovering) {
    m_isHovered = hovering;
    m_currentColor = hovering ? m_hoverColor : m_normalColor;
}

void UIButton::render(sf::RenderWindow& window) {
    if (!m_visible) return;

    // 按钮背景
    sf::RectangleShape button(m_size);
    button.setPosition(m_position);
    button.setFillColor(m_currentColor);
    button.setOutlineThickness(2.f);
    button.setOutlineColor(sf::Color(255, 255, 255, 100));
    window.draw(button);

    // 按钮文本
    if (m_font) {
        sf::Text text(*m_font);
        text.setString(m_text);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        
        // 居中文本
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
        text.setPosition(m_position + sf::Vector2f(m_size.x / 2.f, m_size.y / 2.f));
        
        window.draw(text);
    }
}

} // namespace Nightfall
