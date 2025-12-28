#ifndef __TIME_DISPLAY_SCENE_H__
#define __TIME_DISPLAY_SCENE_H__

#include "cocos2d.h"
#include "TimeManager.h"
#include "WeatherManager.h"

USING_NS_CC;

class TimeDisplayScene : public Scene{
public:
    static Scene * createScene();
    virtual bool init();

    void update(float dt) override;
    void updateDisplay();
    void updateMoneyDisplay();

    CREATE_FUNC(TimeDisplayScene);

private:
    Sprite* timeBg;
    Label* timeLabel;
    Label* dateLabel;
    Label* moneyLabel;
    LayerColor* weatherOverlay;

    void setupUI();
    void onDayChanged(int year, int season, int day);
    Node* weatherEffectNode; // 用于存放当前的粒子或绘图节点
    void applyWeatherEffects(WeatherType type);
    void createLightning();
};

#endif
