// 文件名: GameScene.cpp (Classes/GameScene.cpp)
#include "BagScene.h"
#include "GameScene.h"
#include "HelloWorldScene.h" 
#include "Player.h"
#include "TimeDisplayScene.h"
#include "InventoryManager.h"
#include "ShopLayer.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

bool GameScene::init()
{
    if (!Scene::init())
    {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //时间
    auto time = TimeDisplayScene::createScene();
    this->addChild(time, 10);


    //商店
    auto shopLabel = Label::createWithSystemFont("[SHOP]", "Arial", 32);
    shopLabel->setColor(Color3B(255, 215, 0));  // 金色

    auto shopButton = MenuItemLabel::create(shopLabel, [this](Ref* sender) {
        // 打开商店
        auto shopLayer = ShopLayer::create();
        this->addChild(shopLayer, 100);  // 100 是 z-order，确保在最上层
        });

    // 设置按钮位置（屏幕右上角）
    shopButton->setPosition(Vec2(
        origin.x + 80,
        origin.y + visibleSize.height - 40
    ));

    auto menu = Menu::create(shopButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 10);

    //建立背包空间和物品初始化
    auto inv = InventoryManager::getInstance();

    // 第一步：初始化格子空间
    inv->initInventory(20);

    // 第二步：加载物品模版库
    inv->loadItemDatabase();

    // 第三步：现在你可以非常轻松地添加物品了，不需要再写一遍属性
    inv->addItemByID(1100, 5);  // 加5个防风草
    inv->addItemByID(1101, 5);
    // =========================================================
    // 【核心步骤 1：加载纹理图集数据到缓存】
    // 必须首先执行这一步，将 Action.plist 中的帧信息载入 SpriteFrameCache。
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("down_side.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("up_side.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("left_side.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("right_side.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("down_side_action.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("up_side_action.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("left_side_action.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("right_side_action.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("down_side_water.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("up_side_water.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("left_side_water.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("right_side_water.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("down_side_cut.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("up_side_cut.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("left_side_cut.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("right_side_cut.plist");
    // =========================================================

    // 【核心步骤 2：创建精灵】
    // 使用静止状态来创建精灵。
    _playerSprite = Player::createWithSpriteFrameName("down_side-0.png");
    float scaleValue = 4.0f;
    _playerSprite->setScale(scaleValue);
    if (_playerSprite)
    {
        // 设置精灵的初始位置
        _playerSprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        _playerSprite->direction = 0;//初始状态面朝下
        this->addChild(_playerSprite);

        // 3. 创建动画动作 (但不立即运行，由 update 控制)
        _playerSprite->createAndRunActionAnimation();

        // 4. 启用键盘事件监听
        auto listener = EventListenerKeyboard::create();
        listener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
        listener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

        auto mouseListener = EventListenerMouse::create();
        mouseListener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

        // 5. 注册 update 循环
        scheduleUpdate();
    }
    else
    {
        CCLOGERROR("Failed to create player sprite! Check 'down_side.plist' and 'down_side.png'.");
    }

    return true;
}

void GameScene::onMouseDown(EventMouse* event)
{
    if (event->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_LEFT)
    {
        if (_playerSprite && !_playerSprite->_isAction && _playerSprite->what_in_hand_now == 1) // 【? 避免重复攻击】
        {
            _playerSprite->farm();
        }
        else if (_playerSprite && !_playerSprite->_isAction && _playerSprite->what_in_hand_now == 2) {
            _playerSprite->water();
        }
        else if (_playerSprite && !_playerSprite->_isAction && _playerSprite->what_in_hand_now == 3) {
            _playerSprite->cut();
        }
    }
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    // 【修改点】：设置按键状态为 true
    switch (keyCode)
    {
    case EventKeyboard::KeyCode::KEY_W:
		cocos2d::log("按下W键");
        _isWPressed = true;
        break;
    case EventKeyboard::KeyCode::KEY_A:
        _isAPressed = true;
        break;
    case EventKeyboard::KeyCode::KEY_S:
        _isSPressed = true;
        break;
    case EventKeyboard::KeyCode::KEY_D:
        _isDPressed = true;
        break;
    case EventKeyboard::KeyCode::KEY_T:
        _isTPressed = true;
        break;
    case EventKeyboard::KeyCode::KEY_0:
        _playerSprite->what_in_hand_now = 0;
        CCLOG("空手");
        break;
    case EventKeyboard::KeyCode::KEY_1:
        _playerSprite->what_in_hand_now = 1;
        CCLOG("使用物品1");
        break;
    case EventKeyboard::KeyCode::KEY_2:
        _playerSprite->what_in_hand_now = 2;
        CCLOG("使用物品2");
        break;
    case EventKeyboard::KeyCode::KEY_3:
        _playerSprite->what_in_hand_now = 3;
        CCLOG("使用物品3");
        break;
    case EventKeyboard::KeyCode::KEY_4:
        _playerSprite->what_in_hand_now = 4;
        CCLOG("使用物品4");
        break;
    case EventKeyboard::KeyCode::KEY_5:
        _playerSprite->what_in_hand_now = 5;
        CCLOG("使用物品5");
        break;
    case EventKeyboard::KeyCode::KEY_6:
        _playerSprite->what_in_hand_now = 6;
        CCLOG("使用物品6");
        break;
    case EventKeyboard::KeyCode::KEY_7:
        _playerSprite->what_in_hand_now = 7;
        CCLOG("使用物品7");
        break;
    case EventKeyboard::KeyCode::KEY_8:
        _playerSprite->what_in_hand_now = 8;
        CCLOG("使用物品8");
        break;
    case EventKeyboard::KeyCode::KEY_9:
        _playerSprite->what_in_hand_now = 9;
        CCLOG("使用物品9");
        break;
    /*case EventKeyboard::KeyCode::KEY_E: // 假设 B 是背包快捷键
    {
        auto bag = BagScene::createScene();
        // 使用 pushScene 而不是 replaceScene
        Director::getInstance()->pushScene(TransitionFade::create(0.5, bag));
        break;
    }*/
    default:
        break;
    }
}


void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    // 【修改点】：设置按键状态为 false
    switch (keyCode)
    {
    case EventKeyboard::KeyCode::KEY_W:
        _isWPressed = false;
        break;
    case EventKeyboard::KeyCode::KEY_A:
        _isAPressed = false;
        break;
    case EventKeyboard::KeyCode::KEY_S:
        _isSPressed = false;
        break;
    case EventKeyboard::KeyCode::KEY_D:
        _isDPressed = false;
        break;
    case EventKeyboard::KeyCode::KEY_T:
        _isTPressed = false;
		break;
    default:
        break;
    }
}

// GameScene.cpp

// GameScene.cpp

void GameScene::processMovement()
{
    if (_playerSprite && _playerSprite->_isAction)
    {
        return;
    }

    int dirX = 0;
    int dirY = 0;

    // 1. 计算方向输入
    if (_isWPressed) dirY += 1; // 上
    if (_isSPressed) dirY -= 1; // 下
    if (_isAPressed) dirX -= 1; // 左
    if (_isDPressed) dirX += 1; // 右
    if (_isTPressed) {
        auto world = _playerSprite->getGameWorld();
        if (world) {
            world->onInteract();
        }
    }
    if (dirX != 0 || dirY != 0)
    {
        // 正在移动：根据 X/Y 的绝对值优先级确定动画方向，然后开始移动
        if (abs(dirY) >= abs(dirX))
        {
            // 垂直方向占优 (动画使用上下方向)
            _playerSprite->start_move(dirX, dirY);
        }
        else
        {
            // 水平方向占优 (动画使用左右方向)
            _playerSprite->start_move(dirX, dirY);
        }
    }
    else
    {
        // 停止移动
        _playerSprite->stop_move(0, 0);
    }
}


void GameScene::update(float dt)
{
    // 1. 处理输入和动画切换
    processMovement();

    // 2. 移动精灵
    // 如果 _playerSprite->update(dt) 被调度，它会负责移动。
    // 如果不调度 Player::update，你需要在这里计算最终的位置并设置给 _playerSprite。

    // 当前 Player::update(dt) 依赖于 direction 属性，我们只需要确保它被调度即可。
}

GameScene::~GameScene()
{
    ;
}