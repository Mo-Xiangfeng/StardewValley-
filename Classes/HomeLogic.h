// HouseLogic.h
#ifndef __HOUSE_LOGIC_H__
#define __HOUSE_LOGIC_H__

#include "MapLogic.h"

class HomeLogic : public MapLogic {
public:
    void onEnter(GameWorld*, Player*) override;
    void onInteract(Player*, const cocos2d::Vec2& posInMap) override;
};

#endif
#pragma once
