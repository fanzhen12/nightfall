#include "Time.h"
#include "Logger.h"
#include <sstream>
#include <iomanip>

namespace Nightfall {

// 静态成员初始化
int Time::s_hour = 6;
int Time::s_minute = 0;
int Time::s_day = 1;
int Time::s_totalMinutes = 0;
TimeOfDay Time::s_timeOfDay = TimeOfDay::Day;
float Time::s_timeScale = 1.0f;
bool Time::s_isPaused = false;
float Time::s_accumulator = 0.0f;
float Time::s_realSecondsPerGameHour = 120.0f; // 1游戏小时 = 2分钟真实时间

void Time::init(int startHour, int startMinute, int startDay) {
    s_hour = startHour;
    s_minute = startMinute;
    s_day = startDay;
    s_totalMinutes = (startDay - 1) * 24 * 60 + startHour * 60 + startMinute;
    s_timeScale = 1.0f;
    s_isPaused = false;
    s_accumulator = 0.0f;

    updateTimeOfDay();

    NF_CORE_INFO("时间系统初始化: {} {}", getFormattedTime(), getTimeOfDayName());
}

void Time::update(float realDeltaTime) {
    if (s_isPaused) {
        return;
    }

    // 应用时间倍率
    float scaledDeltaTime = realDeltaTime * s_timeScale;
    s_accumulator += scaledDeltaTime;

    // 计算1游戏分钟需要多少真实秒数
    float realSecondsPerGameMinute = s_realSecondsPerGameHour / 60.0f;

    // 当累积时间超过1游戏分钟，推进时间
    while (s_accumulator >= realSecondsPerGameMinute) {
        s_accumulator -= realSecondsPerGameMinute;
        advanceTime(1); // 推进1分钟
    }
}

void Time::advanceTime(int minutes) {
    TimeOfDay oldTimeOfDay = s_timeOfDay;

    s_minute += minutes;
    s_totalMinutes += minutes;

    // 处理分钟溢出
    while (s_minute >= 60) {
        s_minute -= 60;
        s_hour++;

        // 处理小时溢出
        if (s_hour >= 24) {
            s_hour -= 24;
            s_day++;
            NF_INFO("========== 第 {} 天开始 ==========", s_day);
        }
    }

    // 更新时间段
    updateTimeOfDay();

    // 时间段变化时记录日志
    if (oldTimeOfDay != s_timeOfDay) {
        NF_INFO("时间段变化: {} -> {} ({})", 
                static_cast<int>(oldTimeOfDay), 
                static_cast<int>(s_timeOfDay),
                getTimeOfDayName());
    }
}

void Time::updateTimeOfDay() {
    if (s_hour >= 6 && s_hour < 18) {
        s_timeOfDay = TimeOfDay::Day;      // 06:00 - 18:00
    } else if (s_hour >= 18 && s_hour < 20) {
        s_timeOfDay = TimeOfDay::Dusk;     // 18:00 - 20:00
    } else {
        s_timeOfDay = TimeOfDay::Night;    // 20:00 - 06:00
    }
}

std::string Time::getFormattedTime() {
    std::ostringstream oss;
    oss << "第" << s_day << "天 "
        << std::setfill('0') << std::setw(2) << s_hour << ":"
        << std::setfill('0') << std::setw(2) << s_minute;
    return oss.str();
}

std::string Time::getTimeOfDayName() {
    switch (s_timeOfDay) {
        case TimeOfDay::Day:   return "白昼 (探索与生产)";
        case TimeOfDay::Dusk:  return "黄昏 (整备与物流)";
        case TimeOfDay::Night: return "夜晚 (生存与防御)";
        default: return "未知";
    }
}

} // namespace Nightfall
