#ifndef __HOTBAR_H__
#define __HOTBAR_H__

#include "cocos2d.h"
#include "InventoryManager.h"

class Hotbar : public cocos2d::Node {
public:
    CREATE_FUNC(Hotbar);
    virtual bool init() override;
    void refresh(); // 刷新快捷栏显示
    virtual void onEnter() override;
private:
    const int _slotCount = 10;
    const float _slotSize = 20.0f; // 快捷栏格子稍微大一点
    const float _padding = 8.0f;

    cocos2d::Node* _itemsContainer = nullptr;
    void setupLayout();
};

#endif