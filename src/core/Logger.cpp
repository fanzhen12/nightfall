#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <vector>
#include <filesystem>

namespace Nightfall {

std::shared_ptr<spdlog::logger> Logger::s_coreLogger;
std::shared_ptr<spdlog::logger> Logger::s_appLogger;

void Logger::init(const std::string& logFileName) {
    // 创建日志目录
    std::filesystem::path logPath(logFileName);
    if (logPath.has_parent_path()) {
        std::filesystem::create_directories(logPath.parent_path());
    }

    // 创建日志输出目标
    std::vector<spdlog::sink_ptr> sinks;
    
    // 1. 控制台彩色输出
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("%^[%T] [%n] [%l] %v%$");
    sinks.push_back(consoleSink);
    
    // 2. 文件输出
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFileName, true);
    fileSink->set_pattern("[%T] [%n] [%l] %v");
    sinks.push_back(fileSink);

    // 创建核心日志器
    s_coreLogger = std::make_shared<spdlog::logger>("CORE", sinks.begin(), sinks.end());
    s_coreLogger->set_level(spdlog::level::trace);
    s_coreLogger->flush_on(spdlog::level::trace);
    spdlog::register_logger(s_coreLogger);

    // 创建应用日志器
    s_appLogger = std::make_shared<spdlog::logger>("APP", sinks.begin(), sinks.end());
    s_appLogger->set_level(spdlog::level::trace);
    s_appLogger->flush_on(spdlog::level::trace);
    spdlog::register_logger(s_appLogger);

    NF_CORE_INFO("日志系统初始化成功");
    NF_CORE_INFO("日志文件: {}", logFileName);
}

} // namespace Nightfall
