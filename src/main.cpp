#include "core/Application.h"
#include "core/Logger.h"
#include "utils/Config.h"
#include <iostream>
#include <exception>

int main() {
    try {
        // 初始化日志系统
        Nightfall::Logger::init("logs/nightfall.log");
        
        NF_INFO("==================================================");
        NF_INFO("  Project Nightfall - v0.1.0 Alpha");
        NF_INFO("  末日生存建造塔防游戏");
        NF_INFO("==================================================");
        
        // 初始化配置系统
        Nightfall::Config::init("config.json");
        
        // 从配置读取窗口参数
        std::string title = Nightfall::Config::getString("window.title", "Project Nightfall");
        int width = Nightfall::Config::getInt("window.width", 1280);
        int height = Nightfall::Config::getInt("window.height", 720);
        
        Nightfall::Application app(title, width, height);
        app.run();
        
        // 保存配置
        Nightfall::Config::save();
        
        NF_INFO("游戏正常退出");
        return 0;
        
    } catch (const std::exception& e) {
        NF_CRITICAL("发生致命异常: {}", e.what());
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return 1;
    }
}
