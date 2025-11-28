#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace Nightfall {

/**
 * @brief 游戏配置管理系统
 * 
 * 功能：
 * - 从JSON文件加载配置
 * - 保存配置到文件
 * - 运行时访问配置项
 */
class Config {
public:
    /**
     * @brief 初始化配置系统
     * @param configFile 配置文件路径（默认: "config.json"）
     */
    static void init(const std::string& configFile = "config.json");

    /**
     * @brief 保存当前配置到文件
     */
    static void save();

    /**
     * @brief 获取整数配置项
     */
    static int getInt(const std::string& key, int defaultValue = 0);

    /**
     * @brief 获取浮点数配置项
     */
    static float getFloat(const std::string& key, float defaultValue = 0.0f);

    /**
     * @brief 获取字符串配置项
     */
    static std::string getString(const std::string& key, const std::string& defaultValue = "");

    /**
     * @brief 获取布尔配置项
     */
    static bool getBool(const std::string& key, bool defaultValue = false);

    /**
     * @brief 设置配置项
     */
    template<typename T>
    static void set(const std::string& key, const T& value) {
        s_config[key] = value;
    }

    /**
     * @brief 获取整个配置JSON对象
     */
    static const nlohmann::json& getConfig() { return s_config; }

private:
    static void loadDefaults();
    static void createDefaultConfig();

private:
    static nlohmann::json s_config;
    static std::string s_configFile;
};

} // namespace Nightfall
