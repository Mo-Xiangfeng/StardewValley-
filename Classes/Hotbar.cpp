#include "Hotbar.h"

USING_NS_CC;

bool Hotbar::init() {
    if (!Node::init()) return false;

    _itemsContainer = Node::create();
    this->addChild(_itemsContainer);

    setupLayout();
    refresh();
    // --- 注册监听器 ---
    auto listener = cocos2d::EventListenerCustom::create("inventory_changed", [this](cocos2d::EventCustom* event) {
        this->refresh(); // 收到信号就刷新显示
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void Hotbar::setupLayout() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 计算快捷栏整体居中位置
    float totalWidth = _slotCount * _slotSize + (_slotCount - 1) * _padding;
    Vec2 startPos = Vec2(-totalWidth / 2, 0); // 以 Hotbar 节点中心为参考

    for (int i = 0; i < _slotCount; ++i) {
        Vec2 pos = startPos + Vec2(i * (_slotSize + _padding) + _slotSize / 2, _slotSize / 2);

        // 绘制格子底框
        auto bg = DrawNode::create();
        // 1. 绘制实心背景
        bg->drawSolidRect(Vec2(-_slotSize / 2, -_slotSize / 2), Vec2(_slotSize / 2, _slotSize / 2), Color4F(0, 0, 0, 0.5f));

        // 2. 绘制边框 (解决报错的部分)
        // 先定义一个明确的数组
        Vec2 vertices[] = {
            Vec2(-_slotSize / 2, -_slotSize / 2),
            Vec2(_slotSize / 2, -_slotSize / 2),
            Vec2(_slotSize / 2, _slotSize / 2),
            Vec2(-_slotSize / 2, _slotSize / 2)
        };

        // 传递数组名（自动转为指针）和顶点数量（4）
        bg->drawPolygon(vertices, 4, Color4F(0, 0, 0, 0), 1, Color4F::WHITE);
        bg->setPosition(pos);
        this->addChild(bg, -1);
    }
}// drawNode->drawRect(Vec2(-_slotSize / 2, -_slotSize / 2), Vec2(_slotSize / 2, _slotSize / 2), Color4F::WHITE);

void Hotbar::refresh() {
    _itemsContainer->removeAllChildren();

    // 获取背包前 10 个数据
    auto items = InventoryManager::getInstance()->getItems();
    float totalWidth = _slotCount * _slotSize + (_slotCount - 1) * _padding;
    Vec2 startPos = Vec2(-totalWidth / 2, 0);

    for (int i = 0; i < _slotCount && i < items.size(); ++i) {
        if (!items[i] || items[i]->id == 0) continue;

        Vec2 pos = startPos + Vec2(i * (_slotSize + _padding) + _slotSize / 2, _slotSize / 2);

        // 创建物品图标
        auto sprite = Sprite::create(items[i]->iconPath);
        if (sprite) {
            sprite->setPosition(pos);
            float targetSize = _slotSize * 0.8f;
            sprite->setScale(std::min(targetSize / sprite->getContentSize().width, targetSize / sprite->getContentSize().height));
            _itemsContainer->addChild(sprite);

            // 数量显示
            if (items[i]->count > 1) {
                auto countLabel = Label::createWithSystemFont(std::to_string(items[i]->count), "Arial", 12);
                countLabel->setAnchorPoint(Vec2(1, 0));
                countLabel->setPosition(pos + Vec2(_slotSize / 2 - 2, -_slotSize / 2 + 2));
                _itemsContainer->addChild(countLabel);
            }
        }
    }
}

void Hotbar::onEnter() {
    Node::onEnter();
    this->refresh(); // 每次进入主场景（或从背包返回）时强制刷新
}