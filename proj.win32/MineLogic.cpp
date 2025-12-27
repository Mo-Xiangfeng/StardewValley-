#include "MineLogic.h"
#include "GameWorld.h"
#include "Player.h"
#include "Monster.h"
#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "InventoryManager.h"
#include "TimeDisplayScene.h"
#include "CropData.h"
#include "MoneyManager.h"
MineLogic::MineLogic() : _cachedPlayer(nullptr) {}
MineLogic::~MineLogic() {}

void MineLogic::onEnter(GameWorld* world, Player* player) {
    _cachedPlayer = player;

    // 每次进入矿洞生成 4-7 只史莱姆
    int monsterCount = cocos2d::random(4, 7);
    for (int i = 0; i < monsterCount; i++) {
        spawnMonster(world);
    }
    cocos2d::log("MineLogic: Monsters spawned on walkable tiles.");
}

void MineLogic::spawnMonster(GameWorld* world) {
    // 1. 创建怪物实例

    // 因为 plist 已加载，直接使用帧名创建
    auto monster = Monster::createWithSpriteFrameName("slime-0.png");
    if (!monster) return;

    // 2. 寻找合法的生成位置
    cocos2d::Vec2 spawnPos;
    bool found = false;
    int attempts = 0;

    // 获取当前地图的尺寸（以瓦片为单位）
    // 假设你的 GameWorld 有获取地图范围的方法，或者直接根据逻辑设定范围
    while (!found && attempts < 50) {
        // 随机一个瓦片坐标 (假设矿洞地图大小为 40x40，避开边缘)
        int tx = cocos2d::random(5, 35);
        int ty = cocos2d::random(5, 35);

        // 这里的 16.0f 和 4.0f(缩放) 需匹配你的 GameWorld 缩放逻辑
        float tileSize = 16.0f * 4.0f;
        cocos2d::Vec2 testPos(tx * tileSize, ty * tileSize);

        // 利用 GameWorld 的 isWalkable 校验该点是否可以站立
        if (world->isWalkable(testPos)) {
            spawnPos = testPos;
            found = true;
        }
        attempts++;
    }

    // 如果没找到合法点，默认放在玩家附近但不重合的地方
    if (!found) spawnPos = world->getContentSize() / 2;

    // 3. 设置属性并添加到场景
    monster->setPosition(spawnPos);
    monster->_detectRange = 100.0f; // 侦测范围
    monster->_moveSpeed = 40.0f;   // 移动速度
    monster->currentHP = 30;       // 初始血量

    world->addChild(monster, 5);
    _monsters.push_back(monster);
}

void MineLogic::handleCombat(Player* player) {
    // 迁移：基于朝向的攻击判定
    cocos2d::Rect attackRect = player->getBoundingBox();
    float offset = 20.0f; // 攻击延伸范围

    // 根据玩家朝向 (0:DOWN, 1:LEFT, 2:UP, 3:RIGHT) 偏移判定矩形
    switch (player->direction) {
    case 0: attackRect.origin.y -= offset; break;
    case 1: attackRect.origin.x -= offset; break;
    case 2: attackRect.origin.y += offset; break;
    case 3: attackRect.origin.x += offset; break;
    }

    // 稍微扩大矩形提高打击感
    attackRect.size.width += 15;
    attackRect.size.height += 15;

    for (auto monster : _monsters) {
        if (monster->currentHP > 0 && attackRect.intersectsRect(monster->getBoundingBox())) {
            monster->takeDamage(player->farmPower);

            // 击退判定
            Vec2 diff = monster->getPosition() - player->getPosition();
            diff.normalize();
            Vec2 targetPos = monster->getPosition() + diff * 20.0f;

            // 获取父节点并判定
            auto world = dynamic_cast<GameWorld*>(monster->getParent());
            if (world && world->isWalkable(targetPos)) {
                monster->setPosition(targetPos);
            }
        }
    }
}

void MineLogic::update(float dt) {
    if (!_cachedPlayer) return;

    // 1. 战斗检测
    if (_cachedPlayer->_isAction) {
        handleCombat(_cachedPlayer);
    }

    // 2. AI 与 清理逻辑
    for (auto it = _monsters.begin(); it != _monsters.end(); ) {
        Monster* m = *it;

        // 死亡并动画播放完毕后清理
        if (m->currentHP <= 0 && m->getScale() <= 0.05f) {
            m->removeFromParent();
            it = _monsters.erase(it);
            continue;
        }

        if (m->currentHP > 0) {
            float dist = _cachedPlayer->getPosition().distance(m->getPosition());

            // 追击逻辑
            if (dist < m->_detectRange) {
                auto dir = _cachedPlayer->getPosition() - m->getPosition();
                dir.normalize();
                m->_wanderDirection = dir;
            }

            // 伤害玩家逻辑
            if (dist < 15.0f && m->_attackTimer <= 0) {
                _cachedPlayer->takeDamage(m->attackPower);
                m->_attackTimer = 1.0f;
            }
        }
        ++it;
    }
}

void MineLogic::onExit(GameWorld* world, Player* player) {
    // 切换地图时强制清理所有怪物，防止内存泄漏或显示异常
    for (auto m : _monsters) {
        m->removeFromParent();
    }
    _monsters.clear();
}

void MineLogic::onInteract(Player* player,const Vec2& posInMap) {
    cocos2d::log("Mining action triggered in logic.");
}