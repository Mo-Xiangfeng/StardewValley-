#ifndef __MAIN_MAP_LOGIC_H__
#define __MAIN_MAP_LOGIC_H__

#include "MapLogic.h"
#include"cocos2d.h"
class MainMapLogic : public MapLogic {
public:
    void onEnter(GameWorld* world, Player* player) override;
    void onInteract(Player* player, const cocos2d::Vec2& posInMap) override;
};

#endif
#pragma once
