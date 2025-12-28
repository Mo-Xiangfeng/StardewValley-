// Rock.h
#pragma once
#include "cocos2d.h"

struct Rock {
    cocos2d::Vec2 tilePos; // 瓦片坐标 
    int hp = 3;            // 采矿通常比砍树更费力
    int type = 0;          // 0: 普通石头, 1: 铜矿, 等等
};
