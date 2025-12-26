#include "MainMapLogic.h"
#include "cocos2d.h"
USING_NS_CC;
void MainMapLogic::onEnter(GameWorld*, Player*) {
    cocos2d::log("进入主地图");
}

void MainMapLogic::onInteract(Player*) {
    cocos2d::log("主地图交互：如采集、对话");
}
