#include "Hotbar.h"
#include "InventoryManager.h"
#include "Player.h"
#include   "GameScene.h" 
USING_NS_CC;

bool Hotbar::init() {
    if (!Node::init()) return false;

    _itemsContainer = Node::create();
    this->addChild(_itemsContainer);

    setupLayout();
    refresh();

    // ±³°ü±ä»¯¼àÌý
    auto listener = EventListenerCustom::create(
        "inventory_changed",
        [this](EventCustom*) {
            this->refresh();
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // ===== ´¥Ãþ¼àÌý£¨ºËÐÄ£©=====
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);

    touchListener->onTouchBegan = [this](Touch* touch, Event*) {
        Vec2 localPos = this->convertToNodeSpace(touch->getLocation());
        int index = getSlotIndexByPos(localPos);
        if (index >= 0) {
            onSlotTouched(index);
            return true;
        }
        return false;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    return true;
}
void Hotbar::setSelectedIndex(int index)
{
    _selectedIndex = index;

    for (int i = 0; i < _slotBgs.size(); ++i) {
        _slotBgs[i]->clear();

        // ±³¾°
        Color4F bgColor = (i == _selectedIndex)
            ? Color4F(1, 1, 0, 0.4f)   // Ñ¡ÖÐ£º»ÆÉ«
            : Color4F(0, 0, 0, 0.5f);  // ÆÕÍ¨

        _slotBgs[i]->drawSolidRect(
            Vec2(-_slotSize / 2, -_slotSize / 2),
            Vec2(_slotSize / 2, _slotSize / 2),
            bgColor
        );

        // ±ß¿ò
        Vec2 verts[] = {
            Vec2(-_slotSize / 2, -_slotSize / 2),
            Vec2(_slotSize / 2, -_slotSize / 2),
            Vec2(_slotSize / 2, _slotSize / 2),
            Vec2(-_slotSize / 2, _slotSize / 2)
        };
        _slotBgs[i]->drawPolygon(
            verts, 4, Color4F(0, 0, 0, 0), 1, Color4F::WHITE
        );
    }
}
void Hotbar::setupLayout() {
    float totalWidth = _slotCount * _slotSize + (_slotCount - 1) * _padding;
    Vec2 startPos = Vec2(-totalWidth / 2, 0);

    for (int i = 0; i < _slotCount; ++i) {
        Vec2 pos = startPos + Vec2(
            i * (_slotSize + _padding) + _slotSize / 2,
            _slotSize / 2
        );

        auto bg = DrawNode::create();
        _slotBgs.push_back(bg);
        // ±³¾°
        bg->drawSolidRect(
            Vec2(-_slotSize / 2, -_slotSize / 2),
            Vec2(_slotSize / 2, _slotSize / 2),
            Color4F(0, 0, 0, 0.5f)
        );

        // ±ß¿ò
        Vec2 verts[] = {
            Vec2(-_slotSize / 2, -_slotSize / 2),
            Vec2(_slotSize / 2, -_slotSize / 2),
            Vec2(_slotSize / 2, _slotSize / 2),
            Vec2(-_slotSize / 2, _slotSize / 2)
        };
        bg->drawPolygon(verts, 4, Color4F(0, 0, 0, 0), 1, Color4F::WHITE);


        bg->setPosition(pos);
        this->addChild(bg, -1);
    }
}

void Hotbar::refresh() {
    _itemsContainer->removeAllChildren();

    auto items = InventoryManager::getInstance()->getItems();
    float totalWidth = _slotCount * _slotSize + (_slotCount - 1) * _padding;
    Vec2 startPos = Vec2(-totalWidth / 2, 0);

    for (int i = 0; i < _slotCount && i < items.size(); ++i) {
        if (!items[i] || items[i]->id == 0) continue;

        Vec2 pos = startPos + Vec2(
            i * (_slotSize + _padding) + _slotSize / 2,
            _slotSize / 2
        );

        auto sprite = Sprite::create(items[i]->iconPath);
        if (!sprite) continue;

        sprite->setPosition(pos);
        float targetSize = _slotSize * 0.8f;
        sprite->setScale(
            std::min(
                targetSize / sprite->getContentSize().width,
                targetSize / sprite->getContentSize().height
            )
        );
        _itemsContainer->addChild(sprite);

        if (items[i]->count > 1) {
            auto label = Label::createWithSystemFont(
                std::to_string(items[i]->count),
                "Arial",
                12
            );
            label->setAnchorPoint(Vec2(1, 0));
            label->setPosition(pos + Vec2(_slotSize / 2 - 2, -_slotSize / 2 + 2));
            _itemsContainer->addChild(label);
        }
    }
}

int Hotbar::getSlotIndexByPos(const Vec2& localPos) {
    float totalWidth = _slotCount * _slotSize + (_slotCount - 1) * _padding;
    float left = -totalWidth / 2;
    float bottom = 0;

    if (localPos.x < left || localPos.x > left + totalWidth ||
        localPos.y < bottom || localPos.y > bottom + _slotSize) {
        return -1;
    }

    int index = (int)((localPos.x - left) / (_slotSize + _padding));
    return (index >= 0 && index < _slotCount) ? index : -1;
}

void Hotbar::onSlotTouched(int index)
{
    auto scene = Director::getInstance()->getRunningScene();
    auto gameScene = dynamic_cast<GameScene*>(scene);
    if (!gameScene) return;

    gameScene->selectHotbarSlot(index);

    CCLOG("[Hotbar] Touch select slot %d", index);
}

void Hotbar::onEnter() {
    Node::onEnter();
    refresh();
}
