#include "WeatherManager.h"
#include <cstdlib>
#include <ctime>

WeatherManager* WeatherManager::instance = nullptr;

WeatherManager::WeatherManager() : currentWeather(WeatherType::SUNNY) {
    srand(static_cast<unsigned int>(time(nullptr)));
}

WeatherManager* WeatherManager::getInstance() {
    if (!instance) {
        instance = new WeatherManager();
    }
    return instance;
}

void WeatherManager::init() {
    currentWeather = WeatherType::SUNNY;
}

void WeatherManager::updateWeather(int season) {
    currentWeather = generateWeatherBySeason(season);
}

WeatherType WeatherManager::generateWeatherBySeason(int season) {
    int random = rand() % 100;

    switch (season) {
        case 0: // ¥∫ºæ - ∂‡”Í
            if (random < 40) return WeatherType::SUNNY;
            else if (random < 75) return WeatherType::RAINY;
            else if (random < 90) return WeatherType::CLOUDY;
            else return WeatherType::STORMY;

        case 1: // œƒºæ - «Á¿ Œ™÷˜
            if (random < 70) return WeatherType::SUNNY;
            else if (random < 85) return WeatherType::CLOUDY;
            else if (random < 95) return WeatherType::RAINY;
            else return WeatherType::STORMY;

        case 2: // «Ôºæ - ∂‡‘∆
            if (random < 50) return WeatherType::CLOUDY;
            else if (random < 80) return WeatherType::SUNNY;
            else return WeatherType::RAINY;

        case 3: // ∂¨ºæ - œ¬—©
            if (random < 50) return WeatherType::SNOWY;
            else if (random < 75) return WeatherType::CLOUDY;
            else return WeatherType::SUNNY;
    }

    return WeatherType::SUNNY;
}

std::string WeatherManager::getWeatherName() const {
    switch (currentWeather) {
        case WeatherType::SUNNY: return "Sunny";
        case WeatherType::CLOUDY: return "Cloudy";
        case WeatherType::RAINY: return "Rainy";
        case WeatherType::SNOWY: return "Snowy";
        case WeatherType::STORMY: return "Stormy";
        default: return "Unknown";
    }
}

Color3B WeatherManager::getWeatherTint() const {
    switch (currentWeather) {
        case WeatherType::SUNNY: return Color3B(255, 255, 255);
        case WeatherType::CLOUDY: return Color3B(200, 200, 220);
        case WeatherType::RAINY: return Color3B(150, 150, 180);
        case WeatherType::SNOWY: return Color3B(240, 240, 255);
        case WeatherType::STORMY: return Color3B(120, 120, 140);
        default: return Color3B::WHITE;
    }
}
