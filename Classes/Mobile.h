#pragma once
#include "cocos2d.h"

class Joystick : public cocos2d::Node {
public:
    CREATE_FUNC(Joystick);
    virtual bool init() override;
   
    bool isActive() const { return _active; }

    cocos2d::Vec2 getDirection() const { return _direction; }

private:
    bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
    void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
    void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);

    cocos2d::DrawNode* _base = nullptr;
    cocos2d::DrawNode* _thumb = nullptr;

    float _radius = 80.0f;
    bool _active = false;
    cocos2d::Vec2 _direction = cocos2d::Vec2::ZERO;
};
