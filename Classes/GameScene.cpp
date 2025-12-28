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
#include "ReviveSystem.h"
#include "MapLogic.h"
#include "HomeLogic.h"
#include "DialogueBox.h"  
#include "NPC.h"          
#include "NPCManager.h"
USING_NS_CC;
using namespace cocos2d::ui;
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









    auto StaminaContainer = Node::create();
    StaminaContainer->setPosition(Vec2(origin.x + 1750, origin.y + visibleSize.height - 1050)); // 设置整体位置
    this->addChild(StaminaContainer, 100);

    // 2. 添加背景图片
    auto barbg = Sprite::create("HP_bar.png");
    barbg->setScale(0.92f);
    StaminaContainer->addChild(barbg);

    // 2. 创建进度条
    auto LoadingBar = ui::LoadingBar::create("GREEN.png");
    LoadingBar->setDirection(ui::LoadingBar::Direction::LEFT);
    LoadingBar->setRotation(-90);
    // --- 调整大小的核心代码 ---
    LoadingBar->ignoreContentAdaptWithSize(false);
    // 假设你的背景图宽 200，你希望进度条稍微窄一点，比如 180
    LoadingBar->setContentSize(Size(165.0f, 25.0f));
    // -----------------------

    LoadingBar->setPercent(100);
    LoadingBar->setColor(Color3B::GREEN);

    // 设置相对于背景图中心的位置
    // 如果背景图很大，Vec2(0, 0) 会让进度条正好盖在背景图中心
    LoadingBar->setPosition(Vec2(1, -22));

    StaminaContainer->addChild(LoadingBar);
    this->_StaminaBar = LoadingBar; // 记得赋值给成员变量








    auto staminaContainer = Node::create();
    staminaContainer->setPosition(Vec2(origin.x + 1850, origin.y + visibleSize.height - 1050)); // 设置整体位置
    this->addChild(staminaContainer, 100);

    // 2. 添加背景图片
    auto barBG = Sprite::create("resistance_bar.png");
    staminaContainer->addChild(barBG);

    // 2. 创建进度条
    auto loadingBar = ui::LoadingBar::create("GREEN.png");
    loadingBar->setDirection(ui::LoadingBar::Direction::LEFT);
    loadingBar->setRotation(-90);
    // --- 调整大小的核心代码 ---
    loadingBar->ignoreContentAdaptWithSize(false);
    // 假设你的背景图宽 200，你希望进度条稍微窄一点，比如 180
    loadingBar->setContentSize(Size(160.0f, 25.0f));
    // -----------------------

    loadingBar->setPercent(100);
    loadingBar->setColor(Color3B::GREEN);

    // 设置相对于背景图中心的位置
    // 如果背景图很大，Vec2(0, 0) 会让进度条正好盖在背景图中心
    loadingBar->setPosition(Vec2(1, -24));

    staminaContainer->addChild(loadingBar);
    this->_staminaBar = loadingBar; // 记得赋值给成员变量


    //物品栏
    auto hotbar = Hotbar::create();

    // 放置在屏幕底部中心，留出 20 像素边距
    hotbar->setPosition(Vec2(visibleSize.width / 2, 0));
    this->addChild(hotbar, 99);

    //商店
    /*auto shopLabel = Label::createWithSystemFont("[SHOP]", "Arial", 32);
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
        _gameWorld->addChild(_playerSprite, 1000);
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

        initNPCs();
        _dialogueBox = DialogueBox::create("", "");
        this->addChild(_dialogueBox, 9999);
        _currentTalkingNPC = nullptr;
        // 7. 开启主循环
        scheduleUpdate();
    }
    auto timeManager = TimeManager::getInstance();
    timeManager->registerExhaustionCallback([this]() {
        CCLOG("[GameScene] Exhaustion callback triggered at 02:00!");

        auto reviveSystem = ReviveSystem::getInstance();
        if (reviveSystem && !reviveSystem->isReviving())
        {
            // 暂停时间流逝
            auto timeManager = TimeManager::getInstance();
            if (timeManager) {
                timeManager->pauseTime();
                CCLOG("[GameScene] Time paused during revive.");
            }

            reviveSystem->triggerRevive(this, "exhaustion", [this]() {
                CCLOG("[GameScene] Exhaustion revive completed.");

                // 恢复玩家状态
                if (_playerSprite) {
                    _playerSprite->currentHP = _playerSprite->maxHP;
                    _playerSprite->currentStamina = _playerSprite->maxStamina;
                }

                // 恢复时间流逝
                auto timeManager = TimeManager::getInstance();
                if (timeManager) {
                    timeManager->resumeTime();
                    CCLOG("[GameScene] Time resumed after revive.");
                }
                });
        }
    });
    // --- 关键修正：注册日期变更回调 ---
    timeManager->registerDayCallback([this](int y, int s, int d) {
        CCLOG("[GameScene] DayCallback RECEIVED! New Day: %d", d);
        if (this->_gameWorld) {
            CCLOG("[GameScene] Notifying _gameWorld to process nextDay logic...");
            this->_gameWorld->nextDay();
        }
        else {
            CCLOG("[GameScene] ERROR: DayCallback received but _gameWorld is NULL!");
        }
        });
    timeManager->registerHourCallback([this](int hour) {
        NPCManager::getInstance()->updateAllNPCSchedules(hour);
        });
    return true;
}

void GameScene::initNPCs() {
    auto npcMgr = NPCManager::getInstance();
    npcMgr->init();

    // 示例：创建一个农夫 NPC
    NPC* farmer = npcMgr->createNPC(
        "npc_farmer",           // NPC ID
        "Lao Wang",                  // 显示名称
        "down_side-0.png"       // 使用 Player 的精灵帧作为临时占位符
    );

    if (farmer) {
        // 设置对话
        farmer->dialogues.clear();
        farmer->dialogues.push_back("Remember to water the crops!");
        farmer->dialogues.push_back("Hello!I am Lao Wang.");
        farmer->dialogues.push_back("Today is a fine day for farming!");

        // 设置日程：
        // 6:00-12:00 在农田 (假设坐标 10, 15)
       // 06:00 - 从 NPC1Door（自己家门口）出来
        farmer->addSchedulePoint(6, 20, 33, 1, "Map");
        farmer->addSchedulePoint(7, 16, 13, 2, "Map");
        farmer->addSchedulePoint(10, 30, 16, 0, "Map");
        farmer->addSchedulePoint(13, 45, 10, 0, "Map");
        farmer->addSchedulePoint(16, 32, 20, 3, "Map");
        farmer->addSchedulePoint(20, 20, 33, 1, "Map");


        // 初始化时放在 NPC1Door
        npcMgr->addNPCToWorld("npc_farmer", _gameWorld, 20, 33, "Map");

        NPC* fisherman = npcMgr->createNPC(
            "npc_fisherman",
            "Xiao Li",
            "down_side-0.png"
        );

        if (fisherman) {
            fisherman->dialogues.clear();
            fisherman->dialogues.push_back("Want to learn some fishing tips?");
            fisherman->dialogues.push_back("Hello! I'm Xiao Li, the fisherman.");
            fisherman->dialogues.push_back("The fish are biting well today!");

            fisherman->addSchedulePoint(6, 8, 30, 1, "Map");
            fisherman->addSchedulePoint(8, 10, 13, 2, "Map");
            fisherman->addSchedulePoint(12, 20, 8, 0, "Map");
            fisherman->addSchedulePoint(16, 35, 19, 3, "Map");
            fisherman->addSchedulePoint(20, 8, 30, 1, "Map");

            // 初始化时放在 NPC2Door
            npcMgr->addNPCToWorld("npc_fisherman", _gameWorld, 8, 30, "Map");
        }
    }

    CCLOG("[GameScene] NPCs initialized");
}

void GameScene::onMouseDown(EventMouse* event)
{
    if (event->getMouseButton() != EventMouse::MouseButton::BUTTON_LEFT) {
        return;
    }

    // ===== 1. 对话框优先处理（点击显示下一句）=====
    if (_dialogueBox && _dialogueBox->isVisible()) {
        if (_currentTalkingNPC) {
            std::string nextDialogue = _currentTalkingNPC->getNextDialogue();
            _dialogueBox->setText(nextDialogue);
            CCLOG("显示下一句对话: %s", nextDialogue.c_str());
        }
        return;  // ← 处理完对话后直接返回
    }

    // ===== 2. 阻止动作期间的交互 =====
    if (_playerSprite && _playerSprite->_isAction) {
        return;
    }

    // ===== 3. 坐标转换 =====
    Vec2 clickPos = Director::getInstance()->convertToGL(event->getLocationInView());
    Vec2 worldPos = _gameWorld->convertToNodeSpace(clickPos);

    // ===== 4. 商店地图特殊处理 =====
    if (_gameWorld && _gameWorld->getCurrentMapId() == "Shop") {
        _gameWorld->handleInteraction(worldPos);
        return;
    }

    // ===== 5. NPC 交互检测 =====
    auto npcMgr = NPCManager::getInstance();
    for (auto& pair : npcMgr->getAllNPCs()) {
        NPC* npc = pair.second;

        if (!npc->isVisible()) continue;
        if (npc->getParent() != _gameWorld) continue;

        Rect npcRect = npc->getBoundingBox();

        if (npcRect.containsPoint(worldPos)) {
            float distance = _playerSprite->getPosition().distance(npc->getPosition());
            float interactRadius = 64.0f * _gameWorld->getMapScale();

            if (distance <= interactRadius) {
                _currentTalkingNPC = npc;
                npc->resetDialogue();  // 从第一句开始

                std::string dialogue = npc->getNextDialogue();
                _dialogueBox->init(npc->npcName, dialogue);
                _dialogueBox->show();

                CCLOG("开始与 %s 对话: %s", npc->npcName.c_str(), dialogue.c_str());
            }
            else {
                CCLOG("太远了，走近点再点击");
            }
            return;
        }
    }

    if (event->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_LEFT)
    {
        if (_playerSprite && _playerSprite->_isAction) {
            return;
        }
        if (_gameWorld && _gameWorld->getCurrentMapId() == "Home")
        {
            Vec2 touchPos = Director::getInstance()->convertToGL(event->getLocationInView());
            Vec2 posInMap = _gameWorld->convertToNodeSpace(touchPos);
            auto logic = _gameWorld->getLogic();
            if (logic) {
                logic->onInteract(_playerSprite, posInMap);
            }
        }
        else if (_gameWorld && _gameWorld->getCurrentMapId() == "Shop")
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
            _playerSprite->useStamina(2);
        }
        else if (_playerSprite && !_playerSprite->_isAction && _playerSprite->what_in_hand_now == 1113) {
            _playerSprite->water();
            _playerSprite->useStamina(1);
        }
        else if (_playerSprite && !_playerSprite->_isAction && _playerSprite->what_in_hand_now == 1110) {
            _playerSprite->cut();
            _playerSprite->useStamina(2);
        }
        else if (_playerSprite && _playerSprite->what_in_hand_now == 1111) {
            _gameWorld->startFishingMinigame();
            _playerSprite->useStamina(5);

        }
        else if (_playerSprite && _playerSprite->what_in_hand_now <= 2209 && _playerSprite->what_in_hand_now >= 2200) {
            _playerSprite->eat(10, 20);
            InventoryManager::getInstance()->removeItemByID(_playerSprite->what_in_hand_now, 1);
        }
        else {
            if (_playerSprite && !_playerSprite->_isAction) {
                _playerSprite->onInteract();
            }
        }
    }
}


void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    // ... 其他逻辑 ...
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
        if (_dialogueBox && _dialogueBox->isVisible()) {
            _dialogueBox->hide();
            if (_currentTalkingNPC) {
                _currentTalkingNPC->resetDialogue();
            }
            _currentTalkingNPC = nullptr;
            return;
        }
    }
    // 1. 处理数字键 1-9
    if (keyCode >= EventKeyboard::KeyCode::KEY_1 && keyCode <= EventKeyboard::KeyCode::KEY_9) {
        // 计算索引：KEY_1 对应 0，KEY_2 对应 1...
        int idx = (int)keyCode - (int)EventKeyboard::KeyCode::KEY_1;
        _selectedSlotIndex = idx;

        auto items = InventoryManager::getInstance()->getItems();
        if (idx < items.size() && items[idx] != nullptr) {
            _playerSprite->what_in_hand_now = items[idx]->id;
            CCLOG("选中快捷栏 %d, 物品ID: %d", idx + 1, items[idx]->id);
        }
        else {
            _playerSprite->what_in_hand_now = 0;
            CCLOG("选中快捷栏 %d, 但该格为空", idx + 1);
        }
        return;
    }

    // 2. 处理 0 (空手)
    if (keyCode == EventKeyboard::KeyCode::KEY_0) {
        _selectedSlotIndex = -1;
        _playerSprite->what_in_hand_now = 0;
        CCLOG("切换为空手");
        return;
    }

    // 3. 原有的其他按键处理
    switch (keyCode) {
        case EventKeyboard::KeyCode::KEY_W: _isWPressed = true; break;
        case EventKeyboard::KeyCode::KEY_A: _isAPressed = true; break;
        case EventKeyboard::KeyCode::KEY_S: _isSPressed = true; break;
        case EventKeyboard::KeyCode::KEY_D: _isDPressed = true; break;
        case EventKeyboard::KeyCode::KEY_T: _isTPressed = true; break;
        case EventKeyboard::KeyCode::KEY_E: {
            auto existingBag = this->getChildByName("BagLayerNode");
            if (!existingBag) {
                auto bag = BagScene::create();
                this->addChild(bag, 100, "BagLayerNode");
            }
            else {
                existingBag->removeFromParent();
            }
            break;
        }
        default: break;
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

        if (_playerSprite && _StaminaBar) {
            // 计算百分比：(当前值 / 最大值) * 100
            float percent = (float)_playerSprite->currentHP / _playerSprite->maxHP * 100.0f;
            _StaminaBar->setPercent(percent);

            // 进阶：体力不足时变红
            if (percent < 25.0f) {
                _StaminaBar->setColor(Color3B::RED);
            }
            else {
                _StaminaBar->setColor(Color3B::GREEN);
            }
        }

        if (_playerSprite && _staminaBar) {
            // 计算百分比：(当前值 / 最大值) * 100
            float percent = (float)_playerSprite->currentStamina / _playerSprite->maxStamina * 100.0f;
            _staminaBar->setPercent(percent);

            // 进阶：体力不足时变红
            if (percent < 25.0f) {
                _staminaBar->setColor(Color3B::RED);
            }
            else {
                _staminaBar->setColor(Color3B::GREEN);
            }
        }



        if (_selectedSlotIndex != -1) {
            auto items = InventoryManager::getInstance()->getItems();
            // 检查当前选中的格子是否变成了空指针，或者 ID 是否发生了变化
            if (_selectedSlotIndex < items.size()) {
                auto currentItem = items[_selectedSlotIndex];
                if (currentItem == nullptr) {
                    // 关键点：如果物品用完了，自动重置 ID
                    if (_playerSprite->what_in_hand_now != 0) {
                        _playerSprite->what_in_hand_now = 0;
                        CCLOG("检测到物品已消耗殆尽，自动清空手持。");
                    }
                }
                else {
                    // 确保 ID 同步（防止背包格位被替换了）
                    _playerSprite->what_in_hand_now = currentItem->id;
                }
            }
        }
}

GameScene::~GameScene()
{
    ;
}