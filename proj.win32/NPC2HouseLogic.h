// NPC2HouseLogic.h
#ifndef __NPC2_HOUSE_LOGIC_H__
#define __NPC2_HOUSE_LOGIC_H__

#include "MapLogic.h"

class NPC2HouseLogic : public MapLogic {
public:
    void onEnter(GameWorld*, Player*) override;
    void onInteract(Player*) override;
};

#endif
#pragma once
