#ifndef __BAG_SCENE_H__
#define __BAG_SCENE_H__
#include "cocos2d.h"
class Player;
class BagScene : public cocos2d::Layer {
public:
    virtual bool init() override;
    CREATE_FUNC(BagScene);
    void setPlayer(Player* player) { _player = player; }
private:
    Player* _player = nullptr;
    void initAbilityUI(); // 初始化能力数值显示
    void initUI();
    void updateSlotUI(int index);
    int getSlotIndexByPos(cocos2d::Vec2 pos);
    void onBackToGame(cocos2d::Ref* sender);

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    cocos2d::Sprite* _trashIcon = nullptr; // 垃圾桶精灵
    cocos2d::Rect _trashRect;              // 垃圾桶的感应矩形区域
    cocos2d::Sprite* _storeIcon = nullptr; // 售卖箱精灵
    cocos2d::Rect _storeRect;              // 售卖箱的感应矩形区域

    cocos2d::Node* _itemsContainer = nullptr;
    cocos2d::Sprite* _dragSprite = nullptr;
    int _sourceIdx = -1;
    cocos2d::Size _visibleSize;
    cocos2d::Vec2 _startPos;
    float _slotSize = 85.0f;
    float _padding = 5.0f;
    const int _rows = 2, _cols = 10;
};
#endif