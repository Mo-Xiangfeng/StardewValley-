// NPC2HouseLogic.cpp
#include "NPC2HouseLogic.h"
#include "cocos2d.h"
USING_NS_CC;
void NPC2HouseLogic::onEnter(GameWorld*, Player*) {
    cocos2d::log("进入 NPC2 家");
}

void NPC2HouseLogic::onInteract(Player*) {
    cocos2d::log("与 NPC2 对话");
}
