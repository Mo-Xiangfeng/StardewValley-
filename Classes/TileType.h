#ifndef __TILE_TYPE_H__
#define __TILE_TYPE_H__

enum class TileType {
    Grass = 0,      // 可走，草地
    Road = 1,       // 可走，路
    Water = 2,      // 不可走，水
    Mountain = 3,   // 不可走，高地
    Building = 4,   // 不可走，建筑
    Fence = 5,      // 不可走，围栏
    Rock = 6,       // 不可走，石头
    Tree = 7,       // 不可走，树
    FarmDirt = 8,   // 可走，田地
    Bridge = 9,     // 可走，桥
	Bed = 10, 	    // 不可走，床
	Businessman = 11, // 不可走，商人
};

inline bool isWalkable(TileType t)
{
    return (t == TileType::Grass ||
        t == TileType::Road ||
        t == TileType::FarmDirt ||
        t == TileType::Bridge);
}

#endif
#pragma once
