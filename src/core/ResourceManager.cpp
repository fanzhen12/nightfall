#include "ResourceManager.h"
#include "Logger.h"
#include <SFML/Graphics/Image.hpp>

namespace Nightfall {

// ==================== 纹理管理 ====================

bool ResourceManager::loadTexture(const std::string& id, const std::string& filepath) {
    // 检查是否已加载
    if (hasTexture(id)) {
        NF_WARN("纹理 '{}' 已经加载，跳过", id);
        return true;
    }

    auto texture = std::make_unique<sf::Texture>();
    std::string fullPath = m_texturePath + filepath;

    if (!texture->loadFromFile(fullPath)) {
        NF_ERROR("无法加载纹理: {} (路径: {})", id, fullPath);
        return false;
    }

    // 设置纹理平滑
    texture->setSmooth(true);

    m_textures[id] = std::move(texture);
    NF_INFO("加载纹理成功: {} ({}x{})", id, 
            m_textures[id]->getSize().x, 
            m_textures[id]->getSize().y);
    return true;
}

sf::Texture* ResourceManager::getTexture(const std::string& id) {
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        return it->second.get();
    }

    NF_WARN("纹理 '{}' 未找到", id);
    return nullptr;
}

void ResourceManager::unloadTexture(const std::string& id) {
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        NF_INFO("卸载纹理: {}", id);
        m_textures.erase(it);
    }
}

bool ResourceManager::hasTexture(const std::string& id) const {
    return m_textures.find(id) != m_textures.end();
}

// ==================== 音频管理 ====================

bool ResourceManager::loadSound(const std::string& id, const std::string& filepath) {
    auto sound = std::make_unique<sf::SoundBuffer>();
    std::string fullPath = m_soundPath + filepath;

    if (!sound->loadFromFile(fullPath)) {
        NF_ERROR("无法加载音效: {} (路径: {})", id, fullPath);
        return false;
    }

    m_sounds[id] = std::move(sound);
    NF_INFO("加载音效成功: {} (时长: {:.2f}秒)", id, m_sounds[id]->getDuration().asSeconds());
    return true;
}

sf::SoundBuffer* ResourceManager::getSound(const std::string& id) {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        return it->second.get();
    }

    NF_WARN("音效 '{}' 未找到", id);
    return nullptr;
}

void ResourceManager::unloadSound(const std::string& id) {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        NF_INFO("卸载音效: {}", id);
        m_sounds.erase(it);
    }
}

void ResourceManager::registerMusic(const std::string& id, const std::string& filepath) {
    m_musicPaths[id] = m_musicPath + filepath;
    NF_INFO("注册音乐: {} -> {}", id, filepath);
}

std::string ResourceManager::getMusicPath(const std::string& id) const {
    auto it = m_musicPaths.find(id);
    if (it != m_musicPaths.end()) {
        return it->second;
    }
    NF_WARN("音乐路径 '{}' 未找到", id);
    return "";
}

// ==================== 字体管理 ====================

bool ResourceManager::loadFont(const std::string& id, const std::string& filepath) {
    auto font = std::make_unique<sf::Font>();
    std::string fullPath = m_fontPath + filepath;

    if (!font->openFromFile(fullPath)) {
        NF_ERROR("无法加载字体: {} (路径: {})", id, fullPath);
        return false;
    }

    m_fonts[id] = std::move(font);
    NF_INFO("加载字体成功: {}", id);
    return true;
}

sf::Font* ResourceManager::getFont(const std::string& id) {
    auto it = m_fonts.find(id);
    if (it != m_fonts.end()) {
        return it->second.get();
    }

    NF_WARN("字体 '{}' 未找到", id);
    return nullptr;
}

void ResourceManager::unloadFont(const std::string& id) {
    auto it = m_fonts.find(id);
    if (it != m_fonts.end()) {
        NF_INFO("卸载字体: {}", id);
        m_fonts.erase(it);
    }
}

// ==================== 批量加载 ====================

bool ResourceManager::loadFromManifest(const std::string& manifestPath) {
    // TODO: 实现从 JSON 配置文件批量加载资源
    NF_WARN("loadFromManifest 尚未实现");
    return false;
}

void ResourceManager::preloadEssentials() {
    NF_INFO("预加载基础资源...");

    // 生成占位符纹理
    generatePlaceholderTexture();
    
    // 尝试生成默认字体（使用系统字体）
    generateDefaultFont();

    // 加载 UI 纹理
    loadTexture("ui_button", "ui/button.png");
    loadTexture("ui_panel", "ui/panel.png");

    // 加载占位符纹理
    loadTexture("placeholder", "placeholder.png");
    
    // 加载玩家纹理
    loadTexture("player", "characters/player.png");

    NF_INFO("基础资源预加载完成");
}

void ResourceManager::generateDefaultFont() {
    // 尝试加载系统字体
    const char* systemFonts[] = {
        "C:/Windows/Fonts/msyh.ttc",     // 微软雅黑
        "C:/Windows/Fonts/simhei.ttf",   // 黑体
        "C:/Windows/Fonts/simsun.ttc",   // 宋体
        "C:/Windows/Fonts/arial.ttf",    // Arial
    };

    auto font = std::make_unique<sf::Font>();
    for (const char* fontPath : systemFonts) {
        if (font->openFromFile(fontPath)) {
            m_fonts["default"] = std::move(font);
            NF_INFO("加载系统字体成功: {}", fontPath);
            return;
        }
    }

    NF_WARN("无法加载系统字体，UI 文本将不显示");
}

void ResourceManager::generatePlaceholderTexture() {
    // 创建一个简单的占位符纹理（64x64 的灰色方块）
    sf::Image placeholderImage({64, 64}, sf::Color(100, 100, 100));
    
    // 添加边框
    for (unsigned int x = 0; x < 64; ++x) {
        placeholderImage.setPixel({x, 0}, sf::Color::White);
        placeholderImage.setPixel({x, 63}, sf::Color::White);
    }
    for (unsigned int y = 0; y < 64; ++y) {
        placeholderImage.setPixel({0, y}, sf::Color::White);
        placeholderImage.setPixel({63, y}, sf::Color::White);
    }

    auto texture = std::make_unique<sf::Texture>();
    if (texture->loadFromImage(placeholderImage)) {
        texture->setSmooth(true);
        m_textures["placeholder"] = std::move(texture);
        NF_INFO("生成占位符纹理");
    }

    // 创建玩家纹理（绿色方块）
    sf::Image playerImage({64, 64}, sf::Color(50, 200, 50));
    
    // 添加边框
    for (unsigned int x = 0; x < 64; ++x) {
        playerImage.setPixel({x, 0}, sf::Color::White);
        playerImage.setPixel({x, 63}, sf::Color::White);
    }
    for (unsigned int y = 0; y < 64; ++y) {
        playerImage.setPixel({0, y}, sf::Color::White);
        playerImage.setPixel({63, y}, sf::Color::White);
    }

    auto playerTexture = std::make_unique<sf::Texture>();
    if (playerTexture->loadFromImage(playerImage)) {
        playerTexture->setSmooth(true);
        m_textures["player"] = std::move(playerTexture);
        NF_INFO("生成玩家占位符纹理");
    }
}

// ==================== 资源管理 ====================

void ResourceManager::clearAll() {
    NF_INFO("清除所有资源...");
    m_textures.clear();
    m_sounds.clear();
    m_fonts.clear();
    m_musicPaths.clear();
    NF_INFO("资源清除完成");
}

void ResourceManager::clearUnused() {
    // 简单实现：目前不跟踪引用计数，后续可以改进
    NF_INFO("清除未使用资源（当前未实现引用计数）");
}

ResourceManager::Stats ResourceManager::getStats() const {
    Stats stats;
    stats.textureCount = m_textures.size();
    stats.soundCount = m_sounds.size();
    stats.fontCount = m_fonts.size();

    // 估算内存使用（粗略）
    for (const auto& [id, texture] : m_textures) {
        auto size = texture->getSize();
        // 假设每像素 4 字节（RGBA）
        stats.totalMemoryMB += (size.x * size.y * 4) / (1024 * 1024);
    }

    for (const auto& [id, sound] : m_sounds) {
        // 音频内存估算（每采样2字节）
        stats.totalMemoryMB += sound->getSampleCount() * 2 / (1024 * 1024);
    }

    return stats;
}

} // namespace Nightfall
