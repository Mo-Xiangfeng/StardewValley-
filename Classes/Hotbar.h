#ifndef __HOTBAR_H__
#define __HOTBAR_H__

#include "cocos2d.h"
#include "2d/CCDrawNode.h"
#include <vector>

class Player;   // 前向声明（避免 include Player.h）

class Hotbar : public cocos2d::Node {
public:
    CREATE_FUNC(Hotbar);

    bool init() override;
    void onEnter() override;

    // ===== 对外接口 =====
    void refresh();                 // 刷新快捷栏显示
    void setPlayer(Player* player); // ★ Android / PC 通用：绑定玩家
    void setSelectedIndex(int index);

private:
    // ===== 布局参数 =====
    const int   _slotCount = 10;
    const float _slotSize = 80.0f;
    const float _padding = 8.0f;

    // ===== 节点 =====
    cocos2d::Node* _itemsContainer = nullptr;

    // ===== 玩家 =====
    Player* _player = nullptr;

    // ===== 内部逻辑 =====
    void setupLayout();
    int  getSlotIndexByPos(const cocos2d::Vec2& localPos);
    void onSlotTouched(int index);

    int _selectedIndex = -1;

    // ★ 关键修正：加命名空间
    std::vector<cocos2d::DrawNode*> _slotBgs;
};

#endif // __HOTBAR_H__
