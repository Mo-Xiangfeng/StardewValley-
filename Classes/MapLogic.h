#ifndef __MAP_LOGIC_H__
#define __MAP_LOGIC_H__
#include"cocos2d.h"
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

    
    virtual void onInteract(Player* player, const cocos2d::Vec2& posInMap = cocos2d::Vec2::ZERO) = 0;
};

#endif
