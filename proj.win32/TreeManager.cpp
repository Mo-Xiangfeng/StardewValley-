#include "TreeManager.h"
#include "GameWorld.h"
#include "TileType.h"   // 你已有的 TileType::GRASS

USING_NS_CC;

static TreeManager* s_instance = nullptr;

TreeManager* TreeManager::getInstance() {
    if (!s_instance)
        s_instance = new TreeManager();
    return s_instance;
}

void TreeManager::init(GameWorld* world) {
    _world = world;
    _trees.clear();

    // 初始生成，保证进地图就有树
    tryGenerateTrees();
}

int TreeManager::getTreeCount() const {
    return (int)_trees.size();
}

bool TreeManager::hasTree(int tx, int ty) {
    for (auto& t : _trees) {
        if ((int)t.tilePos.x == tx && (int)t.tilePos.y == ty)
            return true;
    }
    return false;
}

void TreeManager::generateOneTree() {
    if (!_world) return;
    if (_world->getCurrentMapId() != "Map") return;
    int w = _world->getMapWidth();
    int h = _world->getMapHeight();
    if (w <= 0 || h <= 0) return;
    int treeTot = rand() % 101;
    for (int i = 0; i < std::max(60,treeTot); i++) { // 防死循环
        int tx = rand() % w;
        int ty = rand() % h;

        // 只允许草地
		cocos2d::log("Debug: TreeManager try generate at (%d,%d) tileId=%d", tx, ty, _world->debugGetTile(tx, ty));
        if (_world->debugGetTile(tx, ty) != 0)
            continue;

        if (hasTree(tx, ty))
            continue;

        Tree t;
        t.tilePos = Vec2(tx, ty);
        t.age = 0;
        t.life = 5 + rand() % 6; // 5~10 天寿命

        _trees.push_back(t);
        _world->addTreeSprite(tx, ty);
        return;
    }
}

void TreeManager::tryGenerateTrees() {
    int count = getTreeCount();

    if (count >= 60)
        return;

    int target = 60 + rand() % 41; // 60~100
    int need = target - count;

    for (int i = 0; i < need; i++)
        generateOneTree();
}

void TreeManager::dailyUpdate() {
    // 树自然死亡
    for (auto it = _trees.begin(); it != _trees.end(); ) {
        it->age++;
        if (it->age >= it->life) {
            _world->removeTreeSprite(it->tilePos);
            it = _trees.erase(it);
        }
        else {
            ++it;
        }
    }

    // 不足则补
    tryGenerateTrees();
}

bool TreeManager::removeTreeAt(int tx, int ty, int damage) {
    for (auto it = _trees.begin(); it != _trees.end(); ++it) {
        if ((int)it->tilePos.x == tx && (int)it->tilePos.y == ty) {
            it->hp -= damage; // 扣血

            if (it->hp <= 0) {
                // 视觉移除
                _world->removeTreeSprite(it->tilePos);
                // 数据移除
                _trees.erase(it);
                return true; // 树倒了
            }
            return false; // 树还在
        }
    }
    return false; // 该位置没树
}
