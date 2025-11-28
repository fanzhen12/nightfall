#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <memory>
#include <unordered_map>
#include <string>

namespace Nightfall {

/// 资源管理器
/// 单例模式，管理所有游戏资源的加载、缓存和释放
class ResourceManager {
public:
    // 获取单例实例
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    // 禁止拷贝和赋值
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // ==================== 纹理管理 ====================

    /// 加载纹理
    /// @param id 纹理唯一标识符
    /// @param filepath 文件路径（相对于 assets/textures/）
    /// @return 是否加载成功
    bool loadTexture(const std::string& id, const std::string& filepath);

    /// 获取纹理
    /// @param id 纹理标识符
    /// @return 纹理指针，如果不存在返回 nullptr
    sf::Texture* getTexture(const std::string& id);

    /// 卸载纹理
    void unloadTexture(const std::string& id);

    /// 检查纹理是否已加载
    bool hasTexture(const std::string& id) const;

    // ==================== 音频管理 ====================

    /// 加载音效
    bool loadSound(const std::string& id, const std::string& filepath);

    /// 获取音效缓冲
    sf::SoundBuffer* getSound(const std::string& id);

    /// 卸载音效
    void unloadSound(const std::string& id);

    /// 加载音乐路径（音乐不预加载，只记录路径）
    void registerMusic(const std::string& id, const std::string& filepath);

    /// 获取音乐文件路径
    std::string getMusicPath(const std::string& id) const;

    // ==================== 字体管理 ====================

    /// 加载字体
    bool loadFont(const std::string& id, const std::string& filepath);

    /// 获取字体
    sf::Font* getFont(const std::string& id);

    /// 卸载字体
    void unloadFont(const std::string& id);

    // ==================== 批量加载 ====================

    /// 从配置文件批量加载资源
    bool loadFromManifest(const std::string& manifestPath);

    /// 预加载所有基础资源
    void preloadEssentials();
    
    /// 生成占位符纹理
    void generatePlaceholderTexture();
    
    /// 生成默认字体
    void generateDefaultFont();

    // ==================== 资源管理 ====================

    /// 清除所有资源
    void clearAll();

    /// 清除未使用的资源（简单的垃圾回收）
    void clearUnused();

    /// 获取已加载资源统计
    struct Stats {
        size_t textureCount{0};
        size_t soundCount{0};
        size_t fontCount{0};
        size_t totalMemoryMB{0};
    };
    Stats getStats() const;

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    // 资源容器
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> m_sounds;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> m_fonts;
    std::unordered_map<std::string, std::string> m_musicPaths;

    // 基础路径
    const std::string m_assetsPath = "assets/";
    const std::string m_texturePath = m_assetsPath + "textures/";
    const std::string m_soundPath = m_assetsPath + "audio/sfx/";
    const std::string m_musicPath = m_assetsPath + "audio/music/";
    const std::string m_fontPath = m_assetsPath + "fonts/";
};

} // namespace Nightfall
