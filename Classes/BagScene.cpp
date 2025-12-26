#include "BagScene.h"
#include "GameScene.h"
#include "cocos2d.h"
#include "InventoryManager.h"
#include "Item.h"

USING_NS_CC;

Scene* BagScene::createScene() {
    return BagScene::create();
}

bool BagScene::init() {
    if (!Scene::init()) {
        return false;
    }

    _visibleSize = Director::getInstance()->getVisibleSize();

    // 1. 添加半透明背景层 (静态)
    auto bgLayer = LayerColor::create(Color4B(0, 0, 0, 180));
    this->addChild(bgLayer);

    // 2. 添加标题 (静态)
    auto title = Label::createWithTTF("PLAYER INVENTORY", "fonts/Marker Felt.ttf", 32);
    title->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height - 50));
    this->addChild(title);

    // 3. 添加返回按钮 (静态)
    auto backLabel = Label::createWithTTF("BACK", "fonts/Marker Felt.ttf", 24);
    auto backItem = MenuItemLabel::create(backLabel, CC_CALLBACK_1(BagScene::onBackToGame, this));
    backItem->setPosition(Vec2(_visibleSize.width - 60, 40));
    auto menu = Menu::create(backItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);

    // 4. 创建动态容器层 (非常重要：用于存放所有可刷新的物品图标)
    _itemsContainer = Node::create();
    this->addChild(_itemsContainer);

    // 5. 初始化 UI 布局
    initUI();

    // 6. 注册触摸监听器 (只注册一次)
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BagScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BagScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BagScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void BagScene::initUI() {
    // 计算布局起始点 (左下角)
    float totalWidth = _cols * _slotSize + (_cols - 1) * _padding;
    float totalHeight = _rows * _slotSize + (_rows - 1) * _padding;
    _startPos = Vec2((_visibleSize.width - totalWidth) / 2, (_visibleSize.height - totalHeight) / 2);
    // --- 新增：创建垃圾桶 ---
    _trashIcon = Sprite::create("trash_can.png"); // 确保资源路径正确
    if (_trashIcon) {
        // 将垃圾桶放在背包矩阵右侧 50 像素处
        _trashIcon->setPosition(_startPos + Vec2(totalWidth + 20, _slotSize / 2));
        this->addChild(_trashIcon);

        // 初始化垃圾桶的碰撞区域 (使用世界坐标)
        Size s = _trashIcon->getContentSize();
        _trashRect = Rect(_trashIcon->getPositionX() - s.width / 2,
            _trashIcon->getPositionY() - s.height / 2,
            s.width, s.height);
    }
    auto items = InventoryManager::getInstance()->getItems();
    int capacity = InventoryManager::getInstance()->getCapacity();

    // 绘制格子背景和初始物品
    for (int i = 0; i < capacity; ++i) {
        int r = i / _cols;
        int c = i % _cols;

        Vec2 pos = _startPos + Vec2(c * (_slotSize + _padding) + _slotSize / 2,
            r * (_slotSize + _padding) + _slotSize / 2);

        // A. 绘制格子背景 (格子框通常不动，可以直接加在 Scene 上或 container 上)
        auto drawNode = DrawNode::create();
        drawNode->drawRect(Vec2(-_slotSize / 2, -_slotSize / 2), Vec2(_slotSize / 2, _slotSize / 2), Color4F::WHITE);
        drawNode->setPosition(pos);
        this->addChild(drawNode);

        // B. 绘制物品内容
        updateSlotUI(i);
    }
}

void BagScene::updateSlotUI(int index) {
    if (!_itemsContainer) return;
    _itemsContainer->removeChildByTag(1000 + index);
    _itemsContainer->removeChildByTag(2000 + index);

    auto items = InventoryManager::getInstance()->getItems();
    // 修改：判断指针是否为空
    if (index >= items.size() || items[index] == nullptr) return;

    auto& item = items[index]; // item 是 std::shared_ptr<Item>
    int r = index / _cols;
    int c = index % _cols;
    Vec2 pos = _startPos + Vec2(c * (_slotSize + _padding) + _slotSize / 2,
        r * (_slotSize + _padding) + _slotSize / 2);

    auto itemSprite = Sprite::create(item->iconPath);
    if (itemSprite) {
        itemSprite->setPosition(pos);
        itemSprite->setTag(1000 + index);
        // 缩放适配
        float tw = itemSprite->getContentSize().width;
        float th = itemSprite->getContentSize().height;
        if (tw > 0 && th > 0) {
            float targetSize = _slotSize * 0.8f;
            itemSprite->setScale(std::min(targetSize / tw, targetSize / th));
        }
        _itemsContainer->addChild(itemSprite, 10);

        if (item->count > 1) {
            auto label = Label::createWithTTF(std::to_string(item->count), "fonts/arial.ttf", 10);
            label->setPosition(pos + Vec2(_slotSize / 2 - 2, -_slotSize / 2 + 2));
            label->setTag(2000 + index);
            _itemsContainer->addChild(label, 11);
        }
    }
}

bool BagScene::onTouchBegan(Touch* touch, Event* event) {
    Vec2 pos = touch->getLocation();
    _sourceIdx = getSlotIndexByPos(pos);
    auto& items = InventoryManager::getInstance()->getItems();

    if (_sourceIdx != -1 && _sourceIdx < items.size() && items[_sourceIdx] != nullptr) {
        // 1. 隐藏格子里本来的图标
        auto originalIcon = _itemsContainer->getChildByTag(1000 + _sourceIdx);
        if (originalIcon) originalIcon->setVisible(false);

        // 2. 创建拖拽镜像
        _dragSprite = Sprite::create(items[_sourceIdx]->iconPath);
        _dragSprite->setPosition(pos);
        _dragSprite->setOpacity(180); // 半透明效果

        // 3. 【修复】设置正确的缩放
        float scale = (_slotSize * 0.8f) / _dragSprite->getContentSize().width;
        _dragSprite->setScale(scale);

        this->addChild(_dragSprite, 100);
        return true;
    }
    return false;
}

void BagScene::onTouchMoved(Touch* touch, Event* event) {
    if (_dragSprite) _dragSprite->setPosition(touch->getLocation());
}

void BagScene::onTouchEnded(Touch* touch, Event* event) {
    if (!_dragSprite) return;
    int destIdx = getSlotIndexByPos(touch->getLocation());
    Vec2 dropPos = touch->getLocation(); // 获取触摸结束时的世界坐标

    // 1. 检查是否拖拽到了垃圾桶区域
    if (_trashRect.containsPoint(dropPos)) {
        CCLOG("Discard item from slot: %d", _sourceIdx);

        // 调用 Manager 移除数据
        InventoryManager::getInstance()->removeItem(_sourceIdx);

        // 刷新 UI (图标会因为数据变为 nullptr 而消失)
        updateSlotUI(_sourceIdx);
    }
    else if (destIdx != -1 && destIdx != _sourceIdx) {
        InventoryManager::getInstance()->swapItems(_sourceIdx, destIdx);
        updateSlotUI(_sourceIdx);
        updateSlotUI(destIdx);
    }
    else {
        auto original = _itemsContainer->getChildByTag(1000 + _sourceIdx);
        if (original) original->setVisible(true);
    }
    _dragSprite->removeFromParent();
    _dragSprite = nullptr;
}

int BagScene::getSlotIndexByPos(Vec2 touchPos) {
    float relativeX = touchPos.x - _startPos.x;
    float relativeY = touchPos.y - _startPos.y;

    float totalWidth = _cols * _slotSize + (_cols - 1) * _padding;
    float totalHeight = _rows * _slotSize + (_rows - 1) * _padding;

    if (relativeX < 0 || relativeX > totalWidth || relativeY < 0 || relativeY > totalHeight) {
        return -1;
    }

    int col = static_cast<int>(relativeX / (_slotSize + _padding));
    int row = static_cast<int>(relativeY / (_slotSize + _padding));

    if (fmod(relativeX, (_slotSize + _padding)) > _slotSize ||
        fmod(relativeY, (_slotSize + _padding)) > _slotSize) {
        return -1;
    }

    int index = row * _cols + col;
    return (index >= 0 && index < (_rows * _cols)) ? index : -1;
}

void BagScene::onBackToGame(Ref* sender) {
    Director::getInstance()->popScene();
}