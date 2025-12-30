#include "RockManager.h"
#include "GameWorld.h"
#include "InventoryManager.h"
USING_NS_CC;

static RockManager* s_instance = nullptr;

RockManager* RockManager::getInstance() {
    if (!s_instance)
        s_instance = new RockManager();
    return s_instance;
}

void RockManager::init(GameWorld* world) {
    _world = world;
    // 注意：如果是进入新的一层矿洞，通常需要清空旧数据
    
}

// 刷新/生成当前层的所有矿石
void RockManager::refreshRocks() {
  
    if (!_world || _world->getCurrentMapId() != "Mine") return;
   
    // --- 核心修正点 ---
    // 如果 _Rocks 不为空，说明这层矿洞的石头坐标已经固定好了（或者是从存档读入的）
    if (!_Rocks.empty()) {
        // 既然石头位置不变，我们只需要把还没被挖掉（HP>0）的石头的精灵画出来即可
        for (auto& r : _Rocks) { 
            if (r.hp > 0) {
                _world->addRockSprite(r.tilePos.x, r.tilePos.y);
            }
        }
        return; // 直接返回，不再跑下面的随机生成逻辑
    }

    // 只有在第一次进入、且 _Rocks 完全为空时，才执行下面的初始化生成
    int w = _world->getMapWidth();
    int h = _world->getMapHeight();
    int targetCount = 45 + rand() % 20;

    for (int i = 0; i < targetCount; i++) {
        int tx = rand() % w;
        int ty = rand() % h;

        if (_world->debugGetTile(tx, ty) == 1 && !hasRock(tx, ty)) {
            Rock r;
            r.tilePos = Vec2(tx, ty);
            r.hp = 3;
            r.type =  0;

            _Rocks.push_back(r);
            _world->addRockSprite(tx, ty);
			cocos2d::log("RockManager: Generated rock at (%d,%d)", tx, ty);
        }
    }
}

bool RockManager::hasRock(int tx, int ty) {
    for (auto& r : _Rocks) {
        if ((int)r.tilePos.x == tx && (int)r.tilePos.y == ty)
            return true;
    }
    return false;
}

bool RockManager::removeRockAt(int tx, int ty, int damage) {
    for (auto it = _Rocks.begin(); it != _Rocks.end(); ++it) {
        if ((int)it->tilePos.x == tx && (int)it->tilePos.y == ty) {
            it->hp -= damage;
           
            if (it->hp <= 0) {
                
                // 视觉移除：从 rockLayer 中移除精灵
                _world->removeRockSprite(it->tilePos);
                // 数据移除
                InventoryManager::getInstance()->addItemByID(4402, 1);
                _Rocks.erase(it);
                return true; // 矿石破碎
            }
            else {
                
                // 受击反馈：在正确位置播放反馈
                _world->playRockHitEffect(tx, ty);
                return false;
            }
        }
    }
    return false;
}