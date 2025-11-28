#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>

namespace Nightfall {

/**
 * @brief 日志系统封装
 * 
 * 使用 spdlog 库实现，支持：
 * - 控制台彩色输出
 * - 文件日志记录
 * - 多级别日志（Trace/Debug/Info/Warning/Error/Critical）
 */
class Logger {
public:
    /**
     * @brief 初始化日志系统
     * @param logFileName 日志文件名（默认: "nightfall.log"）
     */
    static void init(const std::string& logFileName = "logs/nightfall.log");

    /**
     * @brief 获取核心日志器
     */
    static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_coreLogger; }

    /**
     * @brief 获取应用日志器
     */
    static std::shared_ptr<spdlog::logger>& getAppLogger() { return s_appLogger; }

private:
    static std::shared_ptr<spdlog::logger> s_coreLogger;
    static std::shared_ptr<spdlog::logger> s_appLogger;
};

} // namespace Nightfall

// 日志宏定义 - 核心系统使用
#define NF_CORE_TRACE(...)    ::Nightfall::Logger::getCoreLogger()->trace(__VA_ARGS__)
#define NF_CORE_DEBUG(...)    ::Nightfall::Logger::getCoreLogger()->debug(__VA_ARGS__)
#define NF_CORE_INFO(...)     ::Nightfall::Logger::getCoreLogger()->info(__VA_ARGS__)
#define NF_CORE_WARN(...)     ::Nightfall::Logger::getCoreLogger()->warn(__VA_ARGS__)
#define NF_CORE_ERROR(...)    ::Nightfall::Logger::getCoreLogger()->error(__VA_ARGS__)
#define NF_CORE_CRITICAL(...) ::Nightfall::Logger::getCoreLogger()->critical(__VA_ARGS__)

// 日志宏定义 - 应用层使用
#define NF_TRACE(...)    ::Nightfall::Logger::getAppLogger()->trace(__VA_ARGS__)
#define NF_DEBUG(...)    ::Nightfall::Logger::getAppLogger()->debug(__VA_ARGS__)
#define NF_INFO(...)     ::Nightfall::Logger::getAppLogger()->info(__VA_ARGS__)
#define NF_WARN(...)     ::Nightfall::Logger::getAppLogger()->warn(__VA_ARGS__)
#define NF_ERROR(...)    ::Nightfall::Logger::getAppLogger()->error(__VA_ARGS__)
#define NF_CRITICAL(...) ::Nightfall::Logger::getAppLogger()->critical(__VA_ARGS__)
