#ifndef __MAP_LOGIC_H__
#define __MAP_LOGIC_H__

class GameWorld;
class Player;

class MapLogic {
public:
    virtual ~MapLogic() = default;

    // 进入该地图
    virtual void onEnter(GameWorld* world, Player* player) {}

    // 离开该地图
    virtual void onExit(GameWorld* world, Player* player) {}

    // 每帧更新（矿洞 / NPC 用）
    virtual void update(float dt) {}

    // 玩家交互（E 键）
    virtual void onInteract(Player* player) {}
};

#endif
