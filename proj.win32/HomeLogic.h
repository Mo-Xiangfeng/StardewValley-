// HouseLogic.h
#ifndef __HOUSE_LOGIC_H__
#define __HOUSE_LOGIC_H__

#include "MapLogic.h"

class HomeLogic : public MapLogic {
public:
    void onEnter(GameWorld*, Player*) override;
    void onInteract(Player*) override;
};

#endif
#pragma once
