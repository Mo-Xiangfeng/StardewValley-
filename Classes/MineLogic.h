// MineLogic.h
#ifndef __MINE_LOGIC_H__
#define __MINE_LOGIC_H__

#include "MapLogic.h"

class MineLogic : public MapLogic {
public:
    void onEnter(GameWorld*, Player*) override;
    void update(float dt) override;
    void onInteract(Player*) override;
};

#endif
