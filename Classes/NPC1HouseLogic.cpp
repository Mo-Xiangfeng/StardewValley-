// NPC1HouseLogic.cpp
#include "NPC1HouseLogic.h"
#include "cocos2d.h"
USING_NS_CC;
void NPC1HouseLogic::onEnter(GameWorld*, Player*) {
    cocos2d::log("进入 NPC1 家");
}

void NPC1HouseLogic::onInteract(Player*, const Vec2& posInMap) {
    cocos2d::log("与 NPC1 对话");
}
