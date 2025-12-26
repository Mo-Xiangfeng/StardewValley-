// ShopLogic.cpp
#include "ShopLogic.h"
#include "cocos2d.h"
USING_NS_CC;
void ShopLogic::onEnter(GameWorld*, Player*) {
    cocos2d::log("进入商店");
}

void ShopLogic::onInteract(Player*) {
    cocos2d::log("打开商店 UI");
}
