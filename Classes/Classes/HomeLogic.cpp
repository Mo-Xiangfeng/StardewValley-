#include "HomeLogic.h"
#include "ReviveSystem.h"
#include "GameWorld.h"
#include "Player.h"
#include "cocos2d.h"

USING_NS_CC;

void HomeLogic::onEnter(GameWorld* world, Player* player) {
    cocos2d::log("进入主角家");
}

void HomeLogic::onInteract(Player* player, const Vec2& posInMap) {
    // 1. 获取当前 GameWorld
    auto world = dynamic_cast<GameWorld*>(player->getParent());
    if (!world) return;

    // 2. 获取玩家正前方格子的坐标
    // 逻辑参考 GameWorld::startFishingMinigame
    float scale = world->getMapScale();
    float ts = 16.0f * scale;
    Vec2 pos = player->getPosition();

    int tx = std::floor(pos.x / ts);
    int ty = std::floor(pos.y / ts);

    // 根据玩家朝向计算前方格子
    // direction: 0:下, 1:左, 2:右, 3:上 (基于常见逻辑推断)
    if (player->direction == 0) ty--;
    else if (player->direction == 2) ty++;
    else if (player->direction == 1) tx--;
    else if (player->direction == 3) tx++;
    // 3. 检查前方图块是否为“床” (ID: 10)
    if (world->debugGetTile(tx, ty) == 10) {
        CCLOG("[HomeLogic] Bed detected! Starting sleep sequence...");

        // 4. 触发复活/睡眠流程
        auto currentScene = Director::getInstance()->getRunningScene();
        player->currentStamina = player->maxStamina;
        ReviveSystem::getInstance()->triggerRevive(currentScene, "Sleep", [world]() {
            // 睡眠完成后（第二天），你可以手动调用一次农场更新逻辑
            world->nextDay();
            });
    }
}