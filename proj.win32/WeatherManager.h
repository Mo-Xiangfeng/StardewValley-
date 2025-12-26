#ifndef __WEATHER_MANAGER_H__
#define __WEATHER_MANAGER_H__

#include "cocos2d.h"

USING_NS_CC;

enum class WeatherType {
    SUNNY,      // 晴天
    CLOUDY,     // 多云
    RAINY,      // 雨天
    SNOWY,      // 雪天
    STORMY      // 暴风雨
};

class WeatherManager {
public:
    static WeatherManager* getInstance();

    void init();
    void updateWeather(int season);

    WeatherType getCurrentWeather() const { return currentWeather; }
    std::string getWeatherName() const;
    Color3B getWeatherTint() const; // 获取天气色调

private:
    WeatherManager();
    static WeatherManager* instance;

    WeatherType currentWeather;

    WeatherType generateWeatherBySeason(int season);
};

#endif
