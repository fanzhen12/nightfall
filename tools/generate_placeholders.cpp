// 临时工具：生成占位符纹理
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>

void createPlaceholderTexture(const std::string& path, unsigned int size, sf::Color color) {
    sf::RenderTexture renderTexture;
    if (!renderTexture.create({size, size})) {
        std::cerr << "无法创建渲染纹理: " << path << std::endl;
        return;
    }

    // 绘制纯色方块
    sf::RectangleShape rect({static_cast<float>(size), static_cast<float>(size)});
    rect.setFillColor(color);
    renderTexture.draw(rect);

    // 绘制边框
    sf::RectangleShape border({static_cast<float>(size - 4), static_cast<float>(size - 4)});
    border.setPosition(2, 2);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(2);
    border.setOutlineColor(sf::Color(255, 255, 255, 200));
    renderTexture.draw(border);

    renderTexture.display();

    // 保存到文件
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    if (renderTexture.getTexture().copyToImage().saveToFile(path)) {
        std::cout << "创建占位符: " << path << std::endl;
    } else {
        std::cerr << "保存失败: " << path << std::endl;
    }
}

int main() {
    std::cout << "生成占位符纹理..." << std::endl;

    // 创建基础占位符
    createPlaceholderTexture("../assets/textures/placeholder.png", 64, sf::Color(128, 128, 128));
    
    // 玩家纹理（绿色）
    createPlaceholderTexture("../assets/textures/characters/player.png", 64, sf::Color(50, 200, 50));
    
    // 僵尸纹理
    createPlaceholderTexture("../assets/textures/enemies/zombie_normal.png", 64, sf::Color(100, 50, 50));
    createPlaceholderTexture("../assets/textures/enemies/zombie_fast.png", 64, sf::Color(200, 100, 50));
    createPlaceholderTexture("../assets/textures/enemies/zombie_tank.png", 64, sf::Color(80, 40, 40));
    
    // UI 纹理
    createPlaceholderTexture("../assets/textures/ui/button.png", 128, sf::Color(70, 130, 180));
    createPlaceholderTexture("../assets/textures/ui/panel.png", 256, sf::Color(40, 40, 40, 200));

    // 资源节点纹理
    createPlaceholderTexture("../assets/textures/items/tree.png", 48, sf::Color(34, 139, 34));  // 森林绿
    createPlaceholderTexture("../assets/textures/items/ore.png", 40, sf::Color(128, 128, 128)); // 灰色

    std::cout << "完成！" << std::endl;
    return 0;
}
