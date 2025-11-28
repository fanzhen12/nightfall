#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

namespace Nightfall {

/// UI 元素基类
class UIElement {
public:
    UIElement() = default;
    virtual ~UIElement() = default;

    virtual void update(float deltaTime) {}
    virtual void render(sf::RenderWindow& window) = 0;

    void setPosition(const sf::Vector2f& pos) { m_position = pos; }
    void setVisible(bool visible) { m_visible = visible; }
    
    sf::Vector2f getPosition() const { return m_position; }
    bool isVisible() const { return m_visible; }

protected:
    sf::Vector2f m_position{0.f, 0.f};
    bool m_visible{true};
};

/// 文本标签
class UIText : public UIElement {
public:
    UIText() = default;
    UIText(const std::string& text, const sf::Font& font, unsigned int size = 20);

    void setText(const std::string& text) { m_text = text; }
    void setFont(const sf::Font& font) { m_font = &font; }
    void setCharacterSize(unsigned int size) { m_characterSize = size; }
    void setColor(const sf::Color& color) { m_color = color; }

    void render(sf::RenderWindow& window) override;

private:
    std::string m_text;
    const sf::Font* m_font{nullptr};
    unsigned int m_characterSize{20};
    sf::Color m_color{sf::Color::White};
};

/// 进度条
class UIProgressBar : public UIElement {
public:
    UIProgressBar(const sf::Vector2f& size, const sf::Color& fillColor, const sf::Color& bgColor);

    void setValue(float value) { m_value = std::clamp(value, 0.f, 1.f); }
    void setMaxValue(float max) { m_maxValue = max; }
    void setSize(const sf::Vector2f& size) { m_size = size; }
    void setFillColor(const sf::Color& color) { m_fillColor = color; }
    void setBackgroundColor(const sf::Color& color) { m_backgroundColor = color; }

    float getValue() const { return m_value; }

    void render(sf::RenderWindow& window) override;

private:
    sf::Vector2f m_size{100.f, 10.f};
    float m_value{1.f};
    float m_maxValue{1.f};
    sf::Color m_fillColor{sf::Color::Green};
    sf::Color m_backgroundColor{sf::Color(50, 50, 50)};
};

/// 面板/容器
class UIPanel : public UIElement {
public:
    UIPanel(const sf::Vector2f& size, const sf::Color& bgColor);

    void setSize(const sf::Vector2f& size) { m_size = size; }
    void setBackgroundColor(const sf::Color& color) { m_backgroundColor = color; }
    void setBorderColor(const sf::Color& color, float thickness = 1.f) { 
        m_borderColor = color; 
        m_borderThickness = thickness;
    }

    void render(sf::RenderWindow& window) override;

private:
    sf::Vector2f m_size{100.f, 100.f};
    sf::Color m_backgroundColor{sf::Color(0, 0, 0, 180)};
    sf::Color m_borderColor{sf::Color(100, 100, 100)};
    float m_borderThickness{1.f};
};

/// 按钮
class UIButton : public UIElement {
public:
    UIButton(const std::string& text, const sf::Font& font, const sf::Vector2f& size);

    void setText(const std::string& text) { m_text = text; }
    void setCallback(std::function<void()> callback) { m_callback = callback; }
    void setHoverColor(const sf::Color& color) { m_hoverColor = color; }
    void setNormalColor(const sf::Color& color) { m_normalColor = color; }

    bool contains(const sf::Vector2f& point) const;
    void onClick();
    void onHover(bool hovering);

    void render(sf::RenderWindow& window) override;

private:
    std::string m_text;
    const sf::Font* m_font{nullptr};
    sf::Vector2f m_size{100.f, 40.f};
    sf::Color m_normalColor{sf::Color(70, 130, 180)};
    sf::Color m_hoverColor{sf::Color(100, 150, 210)};
    sf::Color m_currentColor{sf::Color(70, 130, 180)};
    std::function<void()> m_callback;
    bool m_isHovered{false};
};

} // namespace Nightfall
