// MineLogic.cpp
#include "MineLogic.h"
#include "cocos2d.h"
USING_NS_CC;
void MineLogic::onEnter(GameWorld*, Player*) {
    cocos2d::log("进入矿洞");
}

void MineLogic::update(float dt) {
    // 刷怪 / 掉落 / 倒计时
}

void MineLogic::onInteract(Player*) {
    cocos2d::log("挖矿");

}
