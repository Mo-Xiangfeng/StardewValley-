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

static const char* SAFE_TTF = "fonts/Roboto-Regular.ttf";
cocos2d::Scene* BagScene::createScene(Player* player)
{
    auto scene = cocos2d::Scene::create();

    auto bag = BagScene::create();
    bag->setPlayer(player);

    scene->addChild(bag);
    return scene;
}



bool BagScene::init() {
    if (!Layer::init()) {
        return false;
    }

    _visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 半透明背景
    auto bgLayer = LayerColor::create(Color4B(0, 0, 0, 180));
    this->addChild(bgLayer);

    // 标题（顶部居中，安全区）
    auto title = Label::createWithTTF(
        "PLAYER INVENTORY",
        SAFE_TTF,
        48
    );

    if (title) {
        title->setAnchorPoint(Vec2(0.5f, 1.0f));
        title->setPosition(Vec2(
            origin.x + _visibleSize.width / 2,
            origin.y + _visibleSize.height - 40
        ));
        this->addChild(title);
    }
    // 返回按钮
    auto homeItem = MenuItemImage::create(
        "ExitTitile.png",
        "ExitPressedTitile.png",
        [](Ref*) {
            Director::getInstance()->replaceScene(
                TransitionFade::create(0.5f, HelloWorldScene::createScene())
            );
        });

    // 退出按钮
    auto closeItem = MenuItemImage::create(
        "ExitDesktop.png",
        "ExitPressedDesktop.png",
        [](Ref*) {
            Director::getInstance()->end();
        });

    auto navMenu = Menu::create(homeItem, closeItem, nullptr);
    navMenu->setPosition(Vec2::ZERO);
    this->addChild(navMenu, 100);

    homeItem->setAnchorPoint(Vec2(0, 1));
    closeItem->setAnchorPoint(Vec2(0, 1));

    homeItem->setPosition(Vec2(
        origin.x + 40,
        origin.y + _visibleSize.height - 40
    ));
    closeItem->setPosition(Vec2(
        origin.x + 40,
        origin.y + _visibleSize.height - 140
    ));

    // Sacrifice 按钮（右下角）
    auto sacrificeBtn = ui::Button::create("Sacrifice_Normal.png", "Sacrifice_Pressed.png");
    sacrificeBtn->setAnchorPoint(Vec2(1, 0));
    sacrificeBtn->setPosition(Vec2(
        origin.x + _visibleSize.width - 40,
        origin.y + 40
    ));
    sacrificeBtn->addClickEventListener(CC_CALLBACK_1(BagScene::onSacrifice, this));
    this->addChild(sacrificeBtn);

    _itemsContainer = Node::create();
    this->addChild(_itemsContainer);

    initUI();
    //initAbilityUI();

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
    _startPos.y -= _visibleSize.height * 0.08f;
    auto Ability = Sprite::create("Ability.png");
    if (Ability) {
        Ability->setName("AbilityBG");   // ⭐⭐⭐ 关键中的关键
        Ability->setScale(1.2f);

        float totalWidth = _cols * _slotSize + (_cols - 1) * _padding;
        float totalHeight = _rows * _slotSize + (_rows - 1) * _padding;

        Ability->setPosition(Vec2(
            _visibleSize.width / 2,
            _visibleSize.height * 0.75f
        ));

        this->addChild(Ability, 5);
    }

    // --- 新增：创建垃圾桶 ---
    _trashIcon = Sprite::create("trash_can.png"); // 确保资源路径正确
    if (_trashIcon) {
        // 将垃圾桶放在背包矩阵右侧 50 像素处
        _trashIcon->setPosition(_startPos + Vec2(totalWidth + 60, _slotSize / 2));
        this->addChild(_trashIcon);

        // 初始化垃圾桶的碰撞区域 (使用世界坐标)
        Size s = _trashIcon->getContentSize() * _trashIcon->getScale();
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
            auto label = Label::createWithTTF(
                std::to_string(item->count),
                SAFE_TTF,
                30
            );

            if (label) {
                label->setPosition(pos + Vec2(_slotSize / 2 - 2, -_slotSize / 2 + 2));
                label->setTag(2000 + index);
                _itemsContainer->addChild(label, 11);
            }
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
void BagScene::onEnter()
{
    Layer::onEnter();

    CCLOG("BagScene onEnter CALLED");

    if (_player) {
        CCLOG("BagScene onEnter: player OK");
        initAbilityUI();
    }
    else {
        CCLOG("BagScene onEnter: _player IS NULL !!!");
    }
}
void BagScene::initAbilityUI()
{
    if (!_player) {
        CCLOG("Player is null in BagScene");
        return;
    }

    // 1️⃣ 找 Ability 背景图（你 initUI 里加的那个）
    auto abilityBg = this->getChildByName("AbilityBG");
    if (!abilityBg) {
        CCLOG("AbilityBG not found!");
        return;
    }

    Vec2 basePos = abilityBg->getPosition();
    Size bgSize = abilityBg->getContentSize() * abilityBg->getScale();

    float startX = basePos.x - bgSize.width * 0.35f;
    float startY = basePos.y + bgSize.height * 0.25f;
    float lineSpacing = 60.0f;

    struct AbilityData {
        std::string name;
        int level;
        int exp;
    };

    std::vector<AbilityData> abilities = {
        {"Combat",    _player->combatLevel,    _player->combatExp},
        {"Farming",   _player->farmingLevel,   _player->farmingExp},
        {"Fishing",   _player->fishingLevel,   _player->fishingExp},
        {"Foraging",  _player->foragingLevel,  _player->foragingExp}
    };

    for (int i = 0; i < abilities.size(); ++i) {
        std::string info =
            abilities[i].name +
            "  Lv." + std::to_string(abilities[i].level) +
            "  Exp:" + std::to_string(abilities[i].exp);

        auto label = Label::createWithSystemFont(info, "Arial", 28);

        if (!label) continue;

        label->setAnchorPoint(Vec2(0, 0.5f));
        label->setPosition(Vec2(
            startX,
            startY - i * lineSpacing
        ));
        label->setColor(Color3B::BLACK);          // ① 高对比色
        this->addChild(label, 1000);
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
        auto winLabel = Label::createWithTTF(
            "GAME VICTORY!",
            SAFE_TTF,
            64
        );

        if (winLabel) {
            winLabel->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2));
            winLabel->setColor(Color3B(255, 215, 0));
            winLabel->setOpacity(0);
            winLabel->setScale(0.2f);
            overlay->addChild(winLabel);
        }

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
        auto hint = Label::createWithTTF(
            "Sacrifice Failed! Need more items.",
            SAFE_TTF,
            36
        );

        if (hint) {
            hint->setPosition(Vec2(_visibleSize.width / 2, 200));
            hint->setColor(Color3B::RED);
            this->addChild(hint, 1000);
            hint->runAction(Sequence::create(
                FadeIn::create(0.3f),
                DelayTime::create(1.5f),
                RemoveSelf::create(),
                nullptr
            ));
        }

        // 屏幕抖动逻辑
        auto shake = Sequence::create(MoveBy::create(0.05f, Vec2(10, 0)), MoveBy::create(0.05f, Vec2(-20, 0)), MoveBy::create(0.05f, Vec2(10, 0)), nullptr);
        this->runAction(shake);
    }
}