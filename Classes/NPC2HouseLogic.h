// NPC2HouseLogic.h
#ifndef __NPC2_HOUSE_LOGIC_H__
#define __NPC2_HOUSE_LOGIC_H__

#include "MapLogic.h"
#include"cocos2d.h"
class NPC2HouseLogic : public MapLogic {
public:
    void onEnter(GameWorld*, Player*) override;
    void onInteract(Player*, const cocos2d::Vec2& posInMap) override;
};

#endif
#pragma once
