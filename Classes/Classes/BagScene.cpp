#include "BagScene.h"
#include "GameScene.h"
#include "cocos2d.h"
#include "InventoryManager.h"
#include "Item.h"
#include "MoneyManager.h"
#include "ShopLayer.h"
#include "Player.h"
#include "HelloWorldScene.h"
USING_NS_CC;


bool BagScene::init() {
    if (!Layer::init()) {
        return false;
    }

    _visibleSize = Director::getInstance()->getVisibleSize();

    // 1. 添加半透明背景层 (静态)
    auto bgLayer = LayerColor::create(Color4B(0, 0, 0, 180));
    this->addChild(bgLayer);

    // 2. 添加标题 (静态)
    auto title = Label::createWithTTF("PLAYER INVENTORY", "fonts/Marker Felt.ttf", 100);
    title->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height - 150));
    this->addChild(title);

    // 3. 添加返回按钮 (静态)
    auto homeItem = MenuItemImage::create(
        "ExitTitile.png",   // 未点击图片
        "ExitPressedTitile.png",   // 点击状态图片 (如果没有单独的点击图，可以重复填一样的)
        [](Ref* sender) {
            // 切换回 HelloWorldScene
            auto scene = HelloWorldScene::createScene();
            // 使用淡入淡出效果切换，体验更好
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
        });

    // B. 创建“退出程序”按钮
    // 请替换 "CloseNormal.png", "CloseSelected.png" 为你的图片路径
    auto closeItem = MenuItemImage::create(
        "ExitDesktop.png",    // 未点击图片
        "ExitPressedDesktop.png",    // 点击状态图片
        [](Ref* sender) {
            // 直接关闭程序
            Director::getInstance()->end();
        });

    // C. 创建菜单容器
    auto navMenu = Menu::create(homeItem, closeItem, nullptr);
    navMenu->setPosition(Vec2::ZERO); // 将原点设为左下角，方便定位
    this->addChild(navMenu, 100);     // zOrder 设高一点，防止被其他层遮挡

    // D. 设置按钮位置
    // 这里我把它们放在了屏幕左上角，垂直排列，你可以根据喜好修改坐标

    // 返回按钮：放在左上角
    homeItem->setPosition(Vec2(950, _visibleSize.height - 600));

    // 退出按钮：放在返回按钮下方
    closeItem->setPosition(Vec2(950, _visibleSize.height - 700));


    auto sacrificeBtn = ui::Button::create("Sacrifice_Normal.png", "Sacrifice_Pressed.png"); // 请确保你有这两张图片
    sacrificeBtn->setTitleFontSize(40);
    sacrificeBtn->setPosition(Vec2(_visibleSize.width - 200, 150));
    sacrificeBtn->addClickEventListener(CC_CALLBACK_1(BagScene::onSacrifice, this));
    this->addChild(sacrificeBtn);


    // 4. 创建动态容器层 (非常重要：用于存放所有可刷新的物品图标)
    _itemsContainer = Node::create();
    this->addChild(_itemsContainer);

    // 5. 初始化 UI 布局
    initUI();
    initAbilityUI();
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
    _startPos.y -= 400.0f;
    auto Ability = Sprite::create("Ability.png"); // 替换为你的文件名
    if (Ability) {
        Ability->setScale(1.2f);
        // 计算位置：放在格子矩阵的正上方
        float totalWidth = _cols * _slotSize + (_cols - 1) * _padding;
        float totalHeight = _rows * _slotSize + (_rows - 1) * _padding;

        // 设置在格子中心上方约 100 像素的位置
        Ability->setPosition(Vec2(_startPos.x + totalWidth / 2,
            _startPos.y + totalHeight + 400));
        this->addChild(Ability, 5); // 设置较小的 zOrder，确保在拖拽物品下方

    }
    // --- 新增：创建垃圾桶 ---
    _trashIcon = Sprite::create("trash_can.png"); // 确保资源路径正确
    if (_trashIcon) {
        // 将垃圾桶放在背包矩阵右侧 50 像素处
        _trashIcon->setPosition(_startPos + Vec2(totalWidth + 60, _slotSize / 2));
        this->addChild(_trashIcon);

        // 初始化垃圾桶的碰撞区域 (使用世界坐标)
        Size s = _trashIcon->getContentSize();
        _trashRect = Rect(_trashIcon->getPositionX() - s.width / 2,
            _trashIcon->getPositionY() - s.height / 2,
            s.width, s.height);
    }
    //新增：售卖箱
    _storeIcon = Sprite::create("store.png"); // 确保资源路径正确
    float scaleValue = 2.0f;
    _storeIcon->setScale(scaleValue);
    if (_storeIcon) {

        _storeIcon->setPosition(_startPos + Vec2(totalWidth + 60, _slotSize / 2 + 150));
        this->addChild(_storeIcon);

        // 初始化垃圾桶的碰撞区域 (使用世界坐标)
        Size s = _storeIcon->getContentSize();
        _storeRect = Rect(_storeIcon->getPositionX() - s.width / 2,
            _storeIcon->getPositionY() - s.height / 2,
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
            auto label = Label::createWithTTF(std::to_string(item->count), "fonts/arial.ttf", 30);
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
        // --- 修复点 1：同时隐藏图标和数字标签 ---
        auto originalIcon = _itemsContainer->getChildByTag(1000 + _sourceIdx);
        auto originalLabel = _itemsContainer->getChildByTag(2000 + _sourceIdx);
        if (originalIcon) originalIcon->setVisible(false);
        if (originalLabel) originalLabel->setVisible(false);

        // 2. 创建拖拽镜像
        _dragSprite = Sprite::create(items[_sourceIdx]->iconPath);
        _dragSprite->setPosition(pos);
        _dragSprite->setOpacity(180);

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
    Vec2 dropPos = touch->getLocation();

    bool acted = false; // 标记是否执行了有效操作

    if (_trashRect.containsPoint(dropPos)) {
        InventoryManager::getInstance()->removeItem(_sourceIdx);
        acted = true;
    }
    else if (_storeRect.containsPoint(dropPos)) {
        auto item = InventoryManager::getInstance()->getItems()[_sourceIdx];
        if (item && item->type == ItemType::MATERIAL) {
            int money = InventoryManager::getInstance()->getmoney(_sourceIdx);
            MoneyManager::getInstance()->addMoney(money);
            InventoryManager::getInstance()->removeItem(_sourceIdx);
            acted = true;
        }
    }
    else if (destIdx != -1 && destIdx != _sourceIdx) {
        InventoryManager::getInstance()->swapItems(_sourceIdx, destIdx);
        updateSlotUI(destIdx); // 刷新目标位置
        acted = true;
    }

    // --- 修复点 2：无论成功失败，都强制刷新起始格子的 UI ---
    // 如果 acted 为 false，updateSlotUI 会重新根据数据生成图标，自然就“恢复”了
    // 如果 acted 为 true，数据已变，updateSlotUI 就会让图标真正“消失”
    updateSlotUI(_sourceIdx);

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
    this->removeFromParent();
}

void BagScene::initAbilityUI() {
    // 1. 获取当前玩家实例 (假设从 GameScene 获取)
    auto gameScene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
    if (gameScene) {
        _player = gameScene->getPlayer();
    }
    if (!_player) return;

    // 2. 定义起始坐标（你需要根据你的能力栏背景图位置调整这些数值）
    // 假设 Ability 区域在屏幕右侧
    float startX = _visibleSize.width * 0.4f;
    float startY = _visibleSize.height * 0.75f;
    float lineSpacing = 70.0f; // 行间距

    // 3. 数据结构方便循环创建
    struct AbilityData {
        std::string name;
        int level;
        int exp;
    };

    std::vector<AbilityData> abilities = {
        {"(Combat)", _player->combatLevel, _player->combatExp},
        {"(Farming)", _player->farmingLevel, _player->farmingExp},
        {"(Fishing)", _player->fishingLevel, _player->fishingExp},
        {"(Foraging)", _player->foragingLevel, _player->foragingExp}
    };

    // 4. 循环渲染文字
    for (int i = 0; i < abilities.size(); ++i) {
        std::string infoStr = abilities[i].name + ": Lv." + std::to_string(abilities[i].level) +
            "      (Exp: " + std::to_string(abilities[i].exp) + ")";

        auto label = Label::createWithSystemFont(infoStr, "Arial", 30);
        label->setAnchorPoint(Vec2(0, 0.5f)); // 左对齐
        label->setPosition(Vec2(startX, startY - (i * lineSpacing)));
        label->setColor(Color3B::BLACK);
        label->enableOutline(Color4B::BLACK, 2); // 加描边方便看清

        this->addChild(label, 100);
    }
}


void BagScene::onSacrifice(cocos2d::Ref* sender) {
    InventoryManager* invMgr = InventoryManager::getInstance();

    if (invMgr->hasAllDatabaseItems()) {
        // --- 1. 创建胜利背景遮罩 (深黑色半透明) ---
        auto overlay = LayerColor::create(Color4B(0, 0, 0, 220), _visibleSize.width, _visibleSize.height);
        this->addChild(overlay, 999);

        // --- 2. 创建“游戏胜利”文字 ---
        // 注意：如果显示中文乱码，请确保字体支持中文并将文件保存为 UTF-8 BOM 格式
        auto winLabel = Label::createWithTTF("GAME VICTORY!", "fonts/Marker Felt.ttf", 100);
        winLabel->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2));
        winLabel->setColor(Color3B(255, 215, 0));

        // 添加文字外边框让它更醒目
        winLabel->enableOutline(Color4B::BLACK, 3);
        winLabel->setOpacity(0); // 初始透明
        winLabel->setScale(0.2f); // 初始极小
        overlay->addChild(winLabel);

        // --- 3. 设置文字动画：从中心弹出并放大 ---
        auto fadeIn = FadeIn::create(0.5f);
        auto scaleUp = EaseBackOut::create(ScaleTo::create(0.6f, 1.2f)); // 带有回弹效果的放大
        auto spawn = Spawn::create(fadeIn, scaleUp, nullptr); // 同时执行

        // 让文字上下轻轻漂浮的循环动作
        auto moveUp = MoveBy::create(1.0f, Vec2(0, 20));
        auto moveDown = moveUp->reverse();
        auto floatSeq = Sequence::create(moveUp, moveDown, nullptr);
        auto repeatFloat = RepeatForever::create(floatSeq);

        // --- 4. 添加喷泉烟花粒子效果 ---
        auto fireworks = ParticleFireworks::create();
        fireworks->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2));
        overlay->addChild(fireworks);

        // --- 5. 执行序列动作：显示 -> 等待 -> 切换场景 ---
        auto delay = DelayTime::create(3.5f);
        auto callback = CallFunc::create([]() {
            // 切换回登录页面
            auto scene = HelloWorldScene::createScene();
            Director::getInstance()->replaceScene(TransitionFade::create(1.2f, scene));
            });

        winLabel->runAction(Sequence::create(spawn, delay, callback, nullptr));
        winLabel->runAction(repeatFloat); // 独立运行漂浮动作

    }
    else {
        // --- 失败提示：简单震动并显示提示 ---
        auto hint = Label::createWithTTF("Sacrifice Failed! Need more items.", "fonts/arial.ttf", 36);
        hint->setPosition(Vec2(_visibleSize.width / 2, 200));
        hint->setColor(Color3B::RED);
        this->addChild(hint, 1000);
        hint->runAction(Sequence::create(FadeIn::create(0.3f), DelayTime::create(1.5f), RemoveSelf::create(), nullptr));

        // 屏幕抖动逻辑
        auto shake = Sequence::create(MoveBy::create(0.05f, Vec2(10, 0)), MoveBy::create(0.05f, Vec2(-20, 0)), MoveBy::create(0.05f, Vec2(10, 0)), nullptr);
        this->runAction(shake);
    }
}