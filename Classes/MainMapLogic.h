#ifndef __MAIN_MAP_LOGIC_H__
#define __MAIN_MAP_LOGIC_H__

#include "MapLogic.h"

class MainMapLogic : public MapLogic {
public:
    void onEnter(GameWorld* world, Player* player) override;
    void onInteract(Player* player) override;
};

#endif
#pragma once
