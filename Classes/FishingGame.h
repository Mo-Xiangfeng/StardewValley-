#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"

class FishingGame : public cocos2d::Layer
{
public:
    CREATE_FUNC(FishingGame);
    virtual bool init() override;
    virtual void update(float dt) override;

    std::function<void(bool)> onGameOver;

private:
    cocos2d::Node* _container = nullptr;
    cocos2d::Sprite* _fishBar = nullptr;
    cocos2d::Sprite* _fish = nullptr;
    cocos2d::ui::LoadingBar* _prog = nullptr;

    bool _isPressing = false;

    float _barY = 0;
    float _barVel = 0;
    float _fishY = 0;
    float _fishTargetY = 0;
    float _fishTimer = 0;
    float _progress = 30.0f;
    bool _ending = false;      // 是否已进入结束流程
    bool _winResult = false;  // 成功 or 失败
    // 常量
    static constexpr float BAR_H = 150.0f;
    static constexpr float AREA_H = 260.0f;
    static constexpr float GRAVITY = -900.0f;
    static constexpr float BUOYANCY = 1400.0f;
};
