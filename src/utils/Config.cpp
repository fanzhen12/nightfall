#include "Config.h"
#include "../core/Logger.h"
#include <fstream>
#include <filesystem>

namespace Nightfall {

nlohmann::json Config::s_config;
std::string Config::s_configFile;

void Config::init(const std::string& configFile) {
    s_configFile = configFile;
    
    // 检查配置文件是否存在
    if (std::filesystem::exists(configFile)) {
        try {
            std::ifstream file(configFile);
            file >> s_config;
            NF_CORE_INFO("配置文件加载成功: {}", configFile);
        } catch (const std::exception& e) {
            NF_CORE_ERROR("配置文件加载失败: {}", e.what());
            NF_CORE_WARN("使用默认配置");
            loadDefaults();
        }
    } else {
        NF_CORE_WARN("配置文件不存在: {}", configFile);
        NF_CORE_INFO("创建默认配置文件");
        loadDefaults();
        save();
    }
}

void Config::save() {
    try {
        std::ofstream file(s_configFile);
        file << std::setw(4) << s_config << std::endl;
        NF_CORE_INFO("配置已保存到: {}", s_configFile);
    } catch (const std::exception& e) {
        NF_CORE_ERROR("保存配置失败: {}", e.what());
    }
}

void Config::loadDefaults() {
    s_config = nlohmann::json{
        {"window", {
            {"title", "Project Nightfall - Alpha"},
            {"width", 1280},
            {"height", 720},
            {"fullscreen", false},
            {"vsync", false},
            {"fps_limit", 60}
        }},
        {"audio", {
            {"master_volume", 100},
            {"music_volume", 80},
            {"sfx_volume", 100},
            {"muted", false}
        }},
        {"gameplay", {
            {"difficulty", "normal"},
            {"time_scale", 1.0},
            {"auto_pause", true},
            {"show_tutorial", true},
            {"real_seconds_per_game_hour", 120}
        }},
        {"graphics", {
            {"show_fps", true},
            {"show_time", true},
            {"lighting_enabled", true},
            {"particle_quality", "high"}
        }},
        {"controls", {
            {"move_up", "W"},
            {"move_down", "S"},
            {"move_left", "A"},
            {"move_right", "D"},
            {"pause", "Space"},
            {"time_speed", "T"},
            {"inventory", "I"},
            {"build_menu", "B"},
            {"interact", "E"}
        }}
    };
}

int Config::getInt(const std::string& key, int defaultValue) {
    try {
        // 支持嵌套键，如 "window.width"
        nlohmann::json* current = &s_config;
        std::string remaining = key;
        size_t pos;
        
        while ((pos = remaining.find('.')) != std::string::npos) {
            std::string part = remaining.substr(0, pos);
            if (current->contains(part)) {
                current = &(*current)[part];
            } else {
                return defaultValue;
            }
            remaining = remaining.substr(pos + 1);
        }
        
        if (current->contains(remaining)) {
            return (*current)[remaining].get<int>();
        }
    } catch (...) {}
    return defaultValue;
}

float Config::getFloat(const std::string& key, float defaultValue) {
    try {
        nlohmann::json* current = &s_config;
        std::string remaining = key;
        size_t pos;
        
        while ((pos = remaining.find('.')) != std::string::npos) {
            std::string part = remaining.substr(0, pos);
            if (current->contains(part)) {
                current = &(*current)[part];
            } else {
                return defaultValue;
            }
            remaining = remaining.substr(pos + 1);
        }
        
        if (current->contains(remaining)) {
            return (*current)[remaining].get<float>();
        }
    } catch (...) {}
    return defaultValue;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) {
    try {
        nlohmann::json* current = &s_config;
        std::string remaining = key;
        size_t pos;
        
        while ((pos = remaining.find('.')) != std::string::npos) {
            std::string part = remaining.substr(0, pos);
            if (current->contains(part)) {
                current = &(*current)[part];
            } else {
                return defaultValue;
            }
            remaining = remaining.substr(pos + 1);
        }
        
        if (current->contains(remaining)) {
            return (*current)[remaining].get<std::string>();
        }
    } catch (...) {}
    return defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) {
    try {
        nlohmann::json* current = &s_config;
        std::string remaining = key;
        size_t pos;
        
        while ((pos = remaining.find('.')) != std::string::npos) {
            std::string part = remaining.substr(0, pos);
            if (current->contains(part)) {
                current = &(*current)[part];
            } else {
                return defaultValue;
            }
            remaining = remaining.substr(pos + 1);
        }
        
        if (current->contains(remaining)) {
            return (*current)[remaining].get<bool>();
        }
    } catch (...) {}
    return defaultValue;
}

} // namespace Nightfall
