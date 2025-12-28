// RockManager.h
#ifndef __ROCK_MANAGER_H__
#define __ROCK_MANAGER_H__

#include <vector>
#include "Rock.h"

class GameWorld;

class RockManager {
public:
    static RockManager* getInstance();
    void init(GameWorld* world);

    // 玩家交互：尝试采矿
    bool removeRockAt(int tx, int ty, int damage);
    bool hasRock(int tx, int ty);

    // 每次进入矿洞时重新生成
    void refreshRocks();

private:
    RockManager() = default;
    void generateRocks();

    GameWorld* _world = nullptr;
    std::vector<Rock> _Rocks;
};

#endif#pragma once
