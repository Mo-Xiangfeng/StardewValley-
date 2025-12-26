#include "HomeLogic.h"
#include "cocos2d.h"
USING_NS_CC;
void HomeLogic::onEnter(GameWorld*, Player*) {
    cocos2d::log("进入主角家");
}

void HomeLogic::onInteract(Player*) {
    cocos2d::log("睡觉 / 存档");
}