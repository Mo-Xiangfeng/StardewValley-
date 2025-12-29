#ifndef __SHOP_LAYER_H__
#define __SHOP_LAYER_H__

#include "cocos2d.h"
#include "CropData.h"  //  确保有这行

USING_NS_CC;

class ShopLayer : public Layer {
public:
    static ShopLayer* create();
    virtual bool init() override;
    virtual ~ShopLayer();  //  确保有这行

    void refreshShop();

private:
    void createShopItem(int cropType, Vec2 position);
    void onBuyButtonClicked(int cropType);
    void updateMoneyDisplay();

    Layer* scrollContainer;
    Label* moneyLabel;

    float scrollOffset;
    float minScrollOffset;
    float maxScrollOffset;
};

#endif
