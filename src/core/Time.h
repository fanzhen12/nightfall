#pragma once
#include <cstdint>
#include <string>

namespace Nightfall {

/**
 * @brief 时间段枚举
 */
enum class TimeOfDay {
    Day,      // 白昼 06:00 - 18:00
    Dusk,     // 黄昏 18:00 - 20:00
    Night     // 夜晚 20:00 - 06:00
};

/**
 * @brief 游戏时间管理系统
 * 
 * 功能：
 * - 游戏内时间追踪（小时:分钟）
 * - 昼夜循环系统
 * - 时间加速/暂停
 * - 天数统计
 */
class Time {
public:
    /**
     * @brief 初始化时间系统
     * @param startHour 开始小时（0-23）
     * @param startMinute 开始分钟（0-59）
     * @param startDay 开始天数（默认1）
     */
    static void init(int startHour = 6, int startMinute = 0, int startDay = 1);

    /**
     * @brief 更新时间系统
     * @param realDeltaTime 真实世界的帧间隔时间（秒）
     */
    static void update(float realDeltaTime);

    /**
     * @brief 获取当前游戏内小时（0-23）
     */
    static int getHour() { return s_hour; }

    /**
     * @brief 获取当前游戏内分钟（0-59）
     */
    static int getMinute() { return s_minute; }

    /**
     * @brief 获取当前天数
     */
    static int getDay() { return s_day; }

    /**
     * @brief 获取当前时间段
     */
    static TimeOfDay getTimeOfDay() { return s_timeOfDay; }

    /**
     * @brief 获取格式化的时间字符串
     * @return 格式："第X天 HH:MM"
     */
    static std::string getFormattedTime();

    /**
     * @brief 获取时间段名称
     */
    static std::string getTimeOfDayName();

    /**
     * @brief 设置时间流速倍率
     * @param scale 倍率（1.0 = 正常，2.0 = 2倍速，0.0 = 暂停）
     */
    static void setTimeScale(float scale) { s_timeScale = scale; }

    /**
     * @brief 获取时间流速倍率
     */
    static float getTimeScale() { return s_timeScale; }

    /**
     * @brief 暂停时间
     */
    static void pause() { s_isPaused = true; }

    /**
     * @brief 恢复时间
     */
    static void resume() { s_isPaused = false; }

    /**
     * @brief 切换暂停状态
     */
    static void togglePause() { s_isPaused = !s_isPaused; }

    /**
     * @brief 是否暂停
     */
    static bool isPaused() { return s_isPaused; }

    /**
     * @brief 获取自游戏开始的总游戏分钟数
     */
    static int getTotalGameMinutes() { return s_totalMinutes; }

    /**
     * @brief 设置游戏内1小时对应真实世界的秒数
     * @param seconds 默认120秒（1游戏小时=2分钟真实时间）
     */
    static void setGameHourDuration(float seconds) { s_realSecondsPerGameHour = seconds; }

private:
    static void updateTimeOfDay();
    static void advanceTime(int minutes);

private:
    // 游戏内时间
    static int s_hour;          // 当前小时 (0-23)
    static int s_minute;        // 当前分钟 (0-59)
    static int s_day;           // 当前天数
    static int s_totalMinutes;  // 总游戏分钟数

    // 时间段
    static TimeOfDay s_timeOfDay;

    // 时间控制
    static float s_timeScale;              // 时间流速倍率
    static bool s_isPaused;                // 是否暂停
    static float s_accumulator;            // 累积的真实时间
    static float s_realSecondsPerGameHour; // 1游戏小时对应真实秒数
};

} // namespace Nightfall
