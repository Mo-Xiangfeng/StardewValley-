#ifndef __TIME_MANAGER_H__
#define __TIME_MANAGER_H__

#include "cocos2d.h"
#include <functional>

USING_NS_CC;

class TimeManager {
public:
    static TimeManager* getInstance();

    // 初始化和更新
    void init();
    void update(float dt);

    // 暂停和恢复时间
    void pauseTime();
    void resumeTime();
    bool isTimePaused() const { return timePaused; }

    // 时间控制
    void setTimeScale(float scale); // 设置时间流速倍率
    float getTimeScale() const { return timeScale; }

    // 获取当前时间信息
    int getYear() const { return year; }
    int getSeason() const { return season; } // 0=春, 1=夏, 2=秋, 3=冬
    int getDay() const { return day; }
    int getHour() const { return hour; }
    int getMinute() const { return minute; }

    std::string getSeasonName() const;
    std::string getFormattedTime() const;
    std::string getFormattedDate() const;

    // 设置时间（用于复活系统）
    void setDateTime(int y, int s, int d, int h, int m) {
        year = y;
        season = s;
        day = d;
        hour = h;
        hour_scene = h;
        minute = m;
        second = 0.0f;
    }

    // 时间回调
    void registerHourCallback(std::function<void(int)> callback);
    void registerDayCallback(std::function<void(int, int, int)> callback);
    void registerExhaustionCallback(std::function<void()> callback);
    void resetExhaustionFlag() {
        _hasTriggeredExhaustion = false;
    }
private:
    TimeManager();
    static TimeManager* instance;

    // 时间数据
    int year;
    int season;
    int day;
    int hour;
    int hour_scene;
    int minute;
    float second;

    float timeScale; // 时间流速倍率（1.0 = 正常速度）
    float minutesPerRealSecond; // 每真实秒流逝的游戏分钟数
    void onExhaustionTime();

    bool timePaused;
    bool _hasTriggeredExhaustion;

    // 回调
    std::vector<std::function<void(int)>> hourCallbacks;
    std::vector<std::function<void(int, int, int)>> dayCallbacks;
    std::vector<std::function<void()>> exhaustionCallbacks;

    void advanceTime(float minutes);
    void onHourChange();
    void onDayChange();
};

#endif
