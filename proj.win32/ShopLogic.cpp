// ShopLogic.cpp
#include "ShopLogic.h"
#include "ShopLayer.h" // 确保包含 UI 头文件
#include "cocos2d.h"

USING_NS_CC;

void ShopLogic::onEnter(GameWorld* world, Player* player) {
    CCLOG("已进入商店地图，找到商人即可交易");
}

// ShopLogic.cpp
void ShopLogic::onInteract(Player* player,const Vec2& posInMap) {
    auto currentScene = Director::getInstance()->getRunningScene();
    if (!currentScene) return;

    // --- 【新增判定】防止重复弹出 ---
    // 检查当前场景是否已经有名为 "ActiveShop" 的节点
    if (currentScene->getChildByName("ActiveShop")) {
        CCLOG("商店已经打开，跳过重复创建");
        return;
    }

    CCLOG("与商人对话中...打开商店界面");

    auto shopLayer = ShopLayer::create();

    // 给商店层设置一个名字，方便下次检查
    shopLayer->setName("ActiveShop");

    currentScene->addChild(shopLayer, 1000);
}