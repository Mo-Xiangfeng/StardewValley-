// 文件名: GameScene.cpp (Classes/GameScene.cpp)
#include "BagScene.h"
#include "GameScene.h"
#include "HelloWorldScene.h" 
#include "Player.h"
#include "TimeDisplayScene.h"
#include "InventoryManager.h"
#include "ShopLayer.h"
#include "Hotbar.h"
#include "GameWorld.h"
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

    auto time = TimeDisplayScene::createScene();
    this->addChild(time, 10);

    //物品栏
    auto hotbar = Hotbar::create();

    // 放置在屏幕底部中心，留出 20 像素边距
    hotbar->setPosition(Vec2(visibleSize.width / 2, 500));
    this->addChild(hotbar, 99);

    //商店
   /* auto shopLabel = Label::createWithSystemFont("[SHOP]", "Arial", 32);
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
    this->addChild(menu, 10);*/

    //建立背包空间和物品初始化
    auto inv = InventoryManager::getInstance();

    // 第一步：初始化格子空间
    inv->initInventory(20);

    // 第二步：加载物品模版库
    inv->loadItemDatabase();

    // 第三步：现在你可以非常轻松地添加物品了，不需要再写一遍属性
    inv->addItemByID(1110, 1);  // 加钓鱼竿
    inv->addItemByID(1111, 1);
    inv->addItemByID(1112, 1);
    inv->addItemByID(1113, 1);
    inv->addItemByID(2200, 5);
    inv->addItemByID(2207, 1);

    // 1. 初始化地图世界 (从 HelloWorldScene 迁移)
    _gameWorld = GameWorld::create("tilemap.txt", "Map.png");
    this->addChild(_gameWorld);

    // 2. 初始化主角
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("down_side.plist");
    _playerSprite = Player::createWithSpriteFrameName("down_side-0.png");

    if (_playerSprite)
    {
        // 设置缩放与初始状态
        _playerSprite->setScale(4.0f);
        _playerSprite->direction = 0;

        // 绑定世界引用 (关键：为了让 Player 内部的 update 能调用 isWalkable)
        _playerSprite->setGameWorld(_gameWorld);

        // 3. 将主角添加到地图层而不是 Scene 层 (这样相机移动时角色才会随地图移动)
        _gameWorld->addChild(_playerSprite, 10);
        _gameWorld->setPlayer(_playerSprite);

        // 4. 加载动画数据
        _playerSprite->createAndRunActionAnimation();

        // 5. 设置初始地图和出生点
        _gameWorld->switchMap("Map", "Spawn");

        // 6. 注册输入监听
        auto listener = EventListenerKeyboard::create();
        listener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
        listener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

        auto mouseListener = EventListenerMouse::create();
        mouseListener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

        // 7. 开启主循环
        scheduleUpdate();
    }

    return true;
}

void GameScene::onMouseDown(EventMouse* event)
{

    if (event->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_LEFT)
    {
        if (_gameWorld && _gameWorld->getCurrentMapId() == "Shop")
        {
            // 获取点击位置并转换为地图坐标
            Vec2 touchPos = Director::getInstance()->convertToGL(event->getLocationInView());
            Vec2 posInMap = _gameWorld->convertToNodeSpace(touchPos);

            // 执行交互判定
            _gameWorld->handleInteraction(posInMap);
        }
        else if (_playerSprite && !_playerSprite->_isAction && _playerSprite->what_in_hand_now == 1112) // 【? 避免重复攻击】
        {
            _playerSprite->farm();
        }
        else if (_playerSprite && !_playerSprite->_isAction && _playerSprite->what_in_hand_now == 1113) {
            _playerSprite->water();
        }
        else if (_playerSprite && !_playerSprite->_isAction && _playerSprite->what_in_hand_now == 1110) {
            _playerSprite->cut();
        }
        else if (_playerSprite->what_in_hand_now == 1111) {
            _gameWorld->startFishingMinigame();
            
        }
        else{
            if (_playerSprite && !_playerSprite->_isAction && _playerSprite->what_in_hand_now != 0) {
                _playerSprite->onInteract();
            }
        }
    }
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    auto items = InventoryManager::getInstance()->getItems();
    int num = items.size();
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
        if (items[0]!=nullptr) {
            _playerSprite->what_in_hand_now = items[0]->id;
            CCLOG("使用物品1");
        }
        else {
            _playerSprite->what_in_hand_now = 0;
        }
        break;
    case EventKeyboard::KeyCode::KEY_2:
        if (items[1]!=nullptr) {
            _playerSprite->what_in_hand_now = items[1]->id;
            CCLOG("使用物品2");
        }
        else {
            _playerSprite->what_in_hand_now = 0;
        }
        break;
    case EventKeyboard::KeyCode::KEY_3:
        if (items[2]!=nullptr) {
            _playerSprite->what_in_hand_now = items[2]->id;
            CCLOG("使用物品3");
        }
        else {
            _playerSprite->what_in_hand_now = 0;
        }
        break;
    case EventKeyboard::KeyCode::KEY_4:
        if (items[3]!=nullptr) {
            _playerSprite->what_in_hand_now = items[3]->id;
            CCLOG("使用物品4");
        }
        else {
            _playerSprite->what_in_hand_now = 0;
        }
        break;
    case EventKeyboard::KeyCode::KEY_5:
        if (items[4]!=nullptr) {
            _playerSprite->what_in_hand_now = items[4]->id;
            CCLOG("使用物品5");
        }
        else {
            _playerSprite->what_in_hand_now = 0;
        }
        break;
    case EventKeyboard::KeyCode::KEY_6:
        if (items[5]!=nullptr) {
            _playerSprite->what_in_hand_now = items[5]->id;
            CCLOG("使用物品6");
        }
        else {
            _playerSprite->what_in_hand_now = 0;
        }
        break;
    case EventKeyboard::KeyCode::KEY_7:
        if (items[6]!=nullptr) {
            _playerSprite->what_in_hand_now = items[6]->id;
            CCLOG("使用物品7");
        }
        else {
            _playerSprite->what_in_hand_now = 0;
        }
        break;
    case EventKeyboard::KeyCode::KEY_8:
        if (items[7]!=nullptr) {
            _playerSprite->what_in_hand_now = items[7]->id;
            CCLOG("使用物品8");
        }
        else {
            _playerSprite->what_in_hand_now = 0;
        }
        break;
    case EventKeyboard::KeyCode::KEY_9:
        if (items[8]!=nullptr) {
            _playerSprite->what_in_hand_now = items[8]->id;
            CCLOG("使用物品9");
        }
        else {
            _playerSprite->what_in_hand_now = 0;
        }
        break;
    case EventKeyboard::KeyCode::KEY_E: // 假设 B 是背包快捷键
    {
        auto bag = BagScene::createScene();
        // 使用 pushScene 而不是 replaceScene
        Director::getInstance()->pushScene(TransitionFade::create(0.5, bag));
        break;
    }
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
    
        // 1. 处理键盘输入，这会触发 _playerSprite->start_move
        processMovement();

        // 2. 更新地图逻辑 (传送阵检测、Logic 更新)
        if (_gameWorld)
        {
            _gameWorld->update(dt);

            // 3. 更新相机 (让画面跟随主角)
            _gameWorld->updateCamera();
        }
    
}

GameScene::~GameScene()
{
    ;
}