// ShopLogic.h
#ifndef __SHOP_LOGIC_H__
#define __SHOP_LOGIC_H__

#include "MapLogic.h"
#include"cocos2d.h"
class ShopLogic : public MapLogic {
public:
    void onEnter(GameWorld*, Player*) override;
    void onInteract(Player*, const cocos2d::Vec2& posInMap) override;
};

#endif
