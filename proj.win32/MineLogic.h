// MineLogic.h
#ifndef __MINE_LOGIC_H__
#define __MINE_LOGIC_H__

#include "MapLogic.h"
#include "Monster.h"
#include <vector>

class MineLogic : public MapLogic {
public:
    MineLogic();
    virtual ~MineLogic();

    void onEnter(GameWorld* world, Player* player) override;
    void update(float dt) override;
    void onInteract(Player* player) override;
    void onExit(GameWorld* world, Player* player) override;

private:
    void spawnMonster(GameWorld* world);
    void handleCombat(Player* player);

    Player* _cachedPlayer = nullptr;
    std::vector<Monster*> _monsters;
};

#endif