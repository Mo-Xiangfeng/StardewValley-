// Tree.h
#pragma once
#include "cocos2d.h"

struct Tree {
    cocos2d::Vec2 tilePos; // 树所在 tile 坐标
    int hp = 2;
    int age = 0;             // 存活天数
    int life = 5 + rand() % 6; // 5~10 天自然死亡
};
#pragma once
