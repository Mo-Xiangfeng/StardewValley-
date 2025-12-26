// NPC1HouseLogic.h
#ifndef __NPC1_HOUSE_LOGIC_H__
#define __NPC1_HOUSE_LOGIC_H__

#include "MapLogic.h"

class NPC1HouseLogic : public MapLogic {
public:
    void onEnter(GameWorld*, Player*) override;
    void onInteract(Player*) override;
};

#endif
#pragma once
