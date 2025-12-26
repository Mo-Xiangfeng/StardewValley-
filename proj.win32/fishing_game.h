#pragma once
#ifndef __FISHING_GAME_NODE_H__
#define __FISHING_GAME_NODE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class FishingGame : public cocos2d::Node {
public:
    CREATE_FUNC(FishingGame);
    virtual bool init() override;
    void update(float dt) override;

    // 当钓鱼结束时的回调：true 为抓到，false 为逃跑
    std::function<void(bool)> onGameOver;

private:
    cocos2d::Node* _container;       
    cocos2d::Sprite* _fishBar;
    cocos2d::Sprite* _fish;
    cocos2d::ui::LoadingBar* _prog;

    float _barY = 0.0f;
    float _barVel = 0.0f;
    float _fishY = 0.0f;
    float _fishTargetY = 0.0f;
    float _fishTimer = 0.0f;
    float _progress = 30.0f; // 初始进度
    bool _isPressing = false;

    // 物理常量
    const float AREA_H = 133.0f;    // 钓鱼槽高度
    const float BAR_H = 27.0f;      // 绿条高度
    const float GRAVITY = -300.0f; // 重力
    const float BUOYANCY = 400.0f; // 鼠标按住时的浮力
};

#endif