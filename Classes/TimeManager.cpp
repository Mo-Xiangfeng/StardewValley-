#include "TimeManager.h"

TimeManager* TimeManager::instance = nullptr;

TimeManager::TimeManager()
    : year(1), season(0), day(1), hour(6), minute(0), second(0.0f),
    timeScale(1.0f), minutesPerRealSecond(1.0f), timePaused(false),_hasTriggeredExhaustion(false) {  //  初始化暂停标志
}

TimeManager* TimeManager::getInstance() {
    if (!instance) {
        instance = new TimeManager();
    }
    return instance;
}

void TimeManager::init() {
    year = 1;
    season = 0;
    day = 1;
    hour = 6;
    hour_scene = 6;
    minute = 0;
    second = 0.0f;
    timePaused = false;  //  重置暂停状态
    _hasTriggeredExhaustion = false;
}

//  新增：暂停时间
void TimeManager::pauseTime() {
    timePaused = true;
}

//  新增：恢复时间
void TimeManager::resumeTime() {
    timePaused = false;

    // 恢复时重置 hour，让时间正常进入新的一天
    if (_hasTriggeredExhaustion) {
        hour = 6;
        hour_scene = 6;

        if (day > 28) {
            day = 1;
            season++;
            if (season > 3) {
                season = 0;
                year++;
            }
        }

        onDayChange();  // 触发日期变更回调
    }

    // 重置疲劳标志，允许下次触发
    _hasTriggeredExhaustion = false;

    CCLOG("[TimeManager] Time RESUMED at %02d:%02d (Day %d)", hour_scene, minute, day);
}

// 修改：update 中检查暂停状态
void TimeManager::update(float dt) {
    if (timePaused) {
        return;  //  如果暂停，直接返回，不更新时间
    }

    // 累积秒数
    second += dt * timeScale;

    // 每秒转换为游戏内的分钟数
    float minutesToAdd = second * minutesPerRealSecond;

    if (minutesToAdd >= 1.0f) {
        int wholeMinutes = static_cast<int>(minutesToAdd);
        second = 0.0f;
        advanceTime(wholeMinutes);
    }
}

void TimeManager::advanceTime(float minutes) {
    int oldHour = hour;
    int oldDay = day;

    minute += minutes;

    while (minute >= 60) {
        minute -= 60;
        hour++;
        hour_scene++;

        if (hour_scene == 24) {
            hour_scene = 0;
        }

        // 凌晨2点触发疲劳回调
        if (hour == 26 && !_hasTriggeredExhaustion) {
            _hasTriggeredExhaustion = true;  // 设置标志
            onExhaustionTime();

            // 触发后立即返回，防止继续执行 day++
            // 注意：hour 保持在 26，不重置
            return;
        }

        // 只有在未触发疲劳 或 已恢复后才执行日期变更
        if (hour >= 26 && !_hasTriggeredExhaustion) {
            hour = 6;
            hour_scene = 6;
            day++;

            if (day > 28) {
                day = 1;
                season++;

                if (season > 3) {
                    season = 0;
                    year++;
                }
            }
        }
    }

    if (hour != oldHour) {
        onHourChange();
    }
    if (day != oldDay) {
        onDayChange();
    }
}

void TimeManager::setTimeScale(float scale) {
    timeScale = scale;
}

std::string TimeManager::getSeasonName() const {
    const char* seasons[] = { "Spring", "Summer", "Autumn", "Winter" };
    return seasons[season];
}

std::string TimeManager::getFormattedTime() const {
    char buffer[20];
    sprintf(buffer, "%02d:%02d", hour_scene, minute);
    return std::string(buffer);
}

std::string TimeManager::getFormattedDate() const {
    char buffer[50];
    sprintf(buffer, "Year %d %s Day %d", year, getSeasonName().c_str(), day);
    return std::string(buffer);
}

void TimeManager::registerHourCallback(std::function<void(int)> callback) {
    hourCallbacks.push_back(callback);
}

void TimeManager::registerDayCallback(std::function<void(int, int, int)> callback) {
    dayCallbacks.push_back(callback);
}

void TimeManager::onHourChange() {
    for (auto& callback : hourCallbacks) {
        callback(hour);
    }
}

void TimeManager::onDayChange() {
    for (auto& callback : dayCallbacks) {
        callback(year, season, day);
    }
}

void TimeManager::registerExhaustionCallback(std::function<void()> callback) {
    exhaustionCallbacks.push_back(callback);
}

void TimeManager::onExhaustionTime() {
    CCLOG("[TimeManager] Exhaustion time (02:00, hour=26) reached!");
    for (auto& callback : exhaustionCallbacks) {
        callback();
    }
}
