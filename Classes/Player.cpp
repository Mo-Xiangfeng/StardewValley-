#include "cocos2d.h"
#include"Player.h"
#include "GameScene.h"
#include "HelloWorldScene.h" 
#include "GameWorld.h"
USING_NS_CC;
#define WALK_ACTION_TAG 100 // 定义统一的动作标签
#define farm_ACTION_TAG 200 
#define water_ACTION_TAG 200 
#define cut_ACTION_TAG 200
class GameWorld;
Player::Player() {
    ;
}
void Player::setGameWorld(GameWorld* world) {
    _world = world;
}

void Player::setTilePosition(int tx, int ty) {
    if (!_world) return;

    // 1. 获取地图的基础格子大小 (例如 16 或 32)
    float baseTileSize = _world->getTileWidth();

    // 2. 获取地图的放大倍率 (就是那个 2.7f)
    float mapScale = _world->getMapScale();

    // 3. 最终在屏幕上的像素位置 = 索引 * 基础大小 * 缩放
    float x = (tx + 0.5f) * baseTileSize * mapScale;
    float y = (ty + 0.5f) * baseTileSize * mapScale;

    this->setPosition(Vec2(x, y));
}
// 实现 createWithSpriteFrameName 方法
Player* Player::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    // 1. 创建 PlayerCharacter 的实例
    Player* pRet = new (std::nothrow) Player();

    // 2. 调用通用的初始化方法，这里调用 initWithSpriteFrameName
    if (pRet && pRet->initWithSpriteFrameName(spriteFrameName))
    {
        // 3. 自动内存管理并返回
        pRet->autorelease();
        return pRet;
    }
    else
    {
        // 初始化失败，清理内存
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

// 实现从 SpriteFrameName 初始化的方法
bool Player::initWithSpriteFrameName(const std::string& spriteFrameName)
{
    // 【关键点】调用基类的初始化方法来设置精灵的视觉部分
    if (!Sprite::initWithSpriteFrameName(spriteFrameName))
    {
        return false;
    }
    // ------------------------------------------
    // 在这里添加你角色属性的默认初始化
    // ------------------------------------------
    maxHP = 100;      // 最大生命值
    currentHP = 100;  // 当前生命值
    maxStamina = 100; // 最大体力值
    currentStamina = 100; // 当前体力值
    luck = 1;       // 运气值 (百分比或数值)
    farmPower = 5; // 攻击力
    direction = 0; //0->下，1->左，2->上，3->右
    what_in_hand_now = 0;//0->空手，1->锄头，2->水壶
    // ... 其他初始化逻辑 ...

    return true;
}

void Player::createAndRunActionAnimation()
{
    const int FRAME_COUNT = 4; // 假设每个方向有 4 帧动画
    const float FRAME_DELAY = 0.1f;
    const int Action_FRAME_COUNT = 2;
    const float Action_DELAY = 0.1f;
    // 1. 创建并存储四个方向的动画动作和其他动作动画
    _walkAction_down = cocos2d::RepeatForever::create(cocos2d::Animate::create(
        createAnimation("down_side-", FRAME_COUNT, FRAME_DELAY)));

    _walkAction_up = cocos2d::RepeatForever::create(cocos2d::Animate::create(
        createAnimation("up_side-", FRAME_COUNT, FRAME_DELAY)));

    _walkAction_left = cocos2d::RepeatForever::create(cocos2d::Animate::create(
        createAnimation("left_side-", FRAME_COUNT, FRAME_DELAY)));

    _walkAction_right = cocos2d::RepeatForever::create(cocos2d::Animate::create(
        createAnimation("right_side-", FRAME_COUNT, FRAME_DELAY)));

    cocos2d::ActionInterval* farmAnimate_down = cocos2d::Animate::create(
        createAnimation("down_side_action-", Action_FRAME_COUNT, Action_DELAY));

    cocos2d::ActionInterval* farmAnimate_up = cocos2d::Animate::create(
        createAnimation("up_side_action-", Action_FRAME_COUNT, Action_DELAY));

    cocos2d::ActionInterval* farmAnimate_left = cocos2d::Animate::create(
        createAnimation("left_side_action-", Action_FRAME_COUNT, Action_DELAY));

    cocos2d::ActionInterval* farmAnimate_right = cocos2d::Animate::create(
        createAnimation("right_side_action-", Action_FRAME_COUNT, Action_DELAY));

    cocos2d::ActionInterval* waterAnimate_down = cocos2d::Animate::create(
        createAnimation("down_side_water-", Action_FRAME_COUNT, Action_DELAY));

    cocos2d::ActionInterval* waterAnimate_up = cocos2d::Animate::create(
        createAnimation("up_side_water-", Action_FRAME_COUNT, Action_DELAY));

    cocos2d::ActionInterval* waterAnimate_left = cocos2d::Animate::create(
        createAnimation("left_side_water-", Action_FRAME_COUNT, Action_DELAY));

    cocos2d::ActionInterval* waterAnimate_right = cocos2d::Animate::create(
        createAnimation("right_side_water-", Action_FRAME_COUNT, Action_DELAY));

    cocos2d::ActionInterval* cutAnimate_down = cocos2d::Animate::create(
        createAnimation("down_side_cut-", Action_FRAME_COUNT, Action_DELAY));

    cocos2d::ActionInterval* cutAnimate_up = cocos2d::Animate::create(
        createAnimation("up_side_cut-", Action_FRAME_COUNT, Action_DELAY));

    cocos2d::ActionInterval* cutAnimate_left = cocos2d::Animate::create(
        createAnimation("left_side_cut-", Action_FRAME_COUNT, Action_DELAY));

    cocos2d::ActionInterval* cutAnimate_right = cocos2d::Animate::create(
        createAnimation("right_side_cut-", Action_FRAME_COUNT, Action_DELAY));

    auto stopAction = cocos2d::CallFunc::create([this]() {
        // 动作结束后，切换到当前方向的待机帧
        std::string frameName;
        switch (this->direction) {
        case DIR_UP:    frameName = "up_side-0.png"; break;
        case DIR_DOWN:  frameName = "down_side-0.png"; break;
        case DIR_LEFT:  frameName = "left_side-0.png"; break;
        case DIR_RIGHT: frameName = "right_side-0.png"; break;
        default: frameName = "down_side-0.png"; break;
        }
        this->setSpriteFrame(frameName);
        // 【新增】清除攻击状态
        this->_isAction = false;
        });

    _farmAction_down = cocos2d::Sequence::create(farmAnimate_down, stopAction, nullptr);
    _farmAction_up = cocos2d::Sequence::create(farmAnimate_up, stopAction, nullptr);
    _farmAction_left = cocos2d::Sequence::create(farmAnimate_left, stopAction, nullptr);
    _farmAction_right = cocos2d::Sequence::create(farmAnimate_right, stopAction, nullptr);

    _waterAction_down = cocos2d::Sequence::create(waterAnimate_down, stopAction, nullptr);
    _waterAction_up = cocos2d::Sequence::create(waterAnimate_up, stopAction, nullptr);
    _waterAction_left = cocos2d::Sequence::create(waterAnimate_left, stopAction, nullptr);
    _waterAction_right = cocos2d::Sequence::create(waterAnimate_right, stopAction, nullptr);

    _cutAction_down = cocos2d::Sequence::create(cutAnimate_down, stopAction, nullptr);
    _cutAction_up = cocos2d::Sequence::create(cutAnimate_up, stopAction, nullptr);
    _cutAction_left = cocos2d::Sequence::create(cutAnimate_left, stopAction, nullptr);
    _cutAction_right = cocos2d::Sequence::create(cutAnimate_right, stopAction, nullptr);
    // 2. 必须 Retain，因为它们是成员变量且是 RepeatForever
    _walkAction_down->retain();
    _walkAction_up->retain();
    _walkAction_left->retain();
    _walkAction_right->retain();
    _farmAction_down->retain();
    _farmAction_up->retain();
    _farmAction_left->retain();
    _farmAction_right->retain();
    _waterAction_down->retain();
    _waterAction_up->retain();
    _waterAction_left->retain();
    _waterAction_right->retain();
    _cutAction_down->retain();
    _cutAction_up->retain();
    _cutAction_left->retain();
    _cutAction_right->retain();
}


cocos2d::Animation* Player::createAnimation(const std::string& framePrefix, int frameCount, float delay)
{
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    for (int i = 0; i < frameCount; i++) // 假设帧名是从 0 或 1 开始的
    {
        // 假设帧名格式为: "down_side-0.png", "down_side-1.png", ...
        std::string frameName = framePrefix + std::to_string(i) + ".png";
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame)
        {
            frames.pushBack(frame);
        }
        else
        {
            CCLOGERROR("Missing animation frame: %s", frameName.c_str());
        }
    }
    return cocos2d::Animation::createWithSpriteFrames(frames, delay);
}

void Player::start_move(int directionX, int directionY)
{
    // 1. 确定新的移动方向
    Direction newDirection = DIR_DOWN; // 默认向下
    if (directionY > 0) newDirection = DIR_UP;
    else if (directionY < 0) newDirection = DIR_DOWN;
    else if (directionX < 0) newDirection = DIR_LEFT;
    else if (directionX > 0) newDirection = DIR_RIGHT;

    // 2. 确定要播放的动作
    cocos2d::Action* nextAction = nullptr;
    switch (newDirection) {
    case DIR_UP:    nextAction = _walkAction_up; break;
    case DIR_DOWN:  nextAction = _walkAction_down; break;
    case DIR_LEFT:  nextAction = _walkAction_left; break;
    case DIR_RIGHT: nextAction = _walkAction_right; break;
    }

    // 3. 只有当动作不同时才切换动画
    if (nextAction != nullptr && nextAction != _currentWalkAction)
    {
        const float speed = 100.0f;
        _currentVelocity.x = directionX * speed;
        _currentVelocity.y = directionY * speed;
        // 停止旧动作
        this->stopActionByTag(WALK_ACTION_TAG);

        // 运行新动作并设置 Tag
        nextAction->setTag(WALK_ACTION_TAG);
        this->runAction(nextAction);
        _currentWalkAction = nextAction;
    }

    // 4. 更新玩家的方向属性 (用于 update 移动逻辑)
    this->direction = newDirection;

    // 5. 确保 update 函数被调度
    this->scheduleUpdate();
}

void Player::stop_move(int directionX, int directionY)
{
    if (directionX == 0 && directionY == 0)
    {
        // 停止动画
        _currentVelocity = cocos2d::Vec2::ZERO;
        this->stopActionByTag(WALK_ACTION_TAG);
        _currentWalkAction = nullptr; // 清除当前动作引用

        // 设置为当前方向的第一帧（待机）
        std::string frameName;
        switch (this->direction) {
        case DIR_UP:    frameName = "up_side-0.png"; break;
        case DIR_DOWN:  frameName = "down_side-0.png"; break;
        case DIR_LEFT:  frameName = "left_side-0.png"; break;
        case DIR_RIGHT: frameName = "right_side-0.png"; break;
        default: frameName = "down_side-0.png"; break;
        }
        this->setSpriteFrame(frameName);

        // 停止 update 调度，直到再次移动
        this->unscheduleUpdate(); // 建议在 GameScene::update 中检查速度是否为零
    }
}

void Player::update(float dt)
{
    // 1. 计算预期速度和位置
   // 1. 计算预期速度和位置
    const float speed = 100.0f
        ;
    Vec2 velocity = Vec2::ZERO;

    if
        (direction == DIR_UP) velocity.y += speed;
    else if
        (direction == DIR_DOWN) velocity.y -= speed;
    else if
        (direction == DIR_LEFT) velocity.x -= speed;
    else if
        (direction == DIR_RIGHT) velocity.x += speed;

    Vec2 currentPos =this->getPosition();
    Vec2 newPos = currentPos + velocity * dt;

    // ======== 核心：在此处插入碰撞检测逻辑 ========
    if
        (_world) {
        // 检查新位置是否可以通行
        if
            (_world->isWalkable(newPos)) {
            // 如果可以通行，直接设置新位置
            this
                ->setPosition(newPos);
        }
        else
        {
            // [进阶逻辑]：尝试“滑行” (防止被墙完全卡死)
            // 尝试只在 X 轴移动
            Vec2 xOnlyPos = Vec2(newPos.x, currentPos.y);
            if
                (_world->isWalkable(xOnlyPos)) {
                this
                    ->setPosition(xOnlyPos);
            }
            else
            {
                // 尝试只在 Y 轴移动
                Vec2 yOnlyPos = Vec2(currentPos.x, newPos.y);
                if
                    (_world->isWalkable(yOnlyPos)) {
                    this
                        ->setPosition(yOnlyPos);
                }
            }
        }

        // [可选]：地图边界检查
        Size mapSize = _world->getMapSizeScaled();
        float halfW = this->getContentSize().width * 0.5f
            ;
        float halfH = this->getContentSize().height * 0.5f
            ;
        Vec2 clampedPos =
            this
            ->getPosition();
        clampedPos.x = clampf(clampedPos.x, halfW, mapSize.width - halfW);
        clampedPos.y = clampf(clampedPos.y, halfH, mapSize.height - halfH);
        this
            ->setPosition(clampedPos);
    }
    else
    {
        // 如果没有关联 _world，则维持原样移动
        this->setPosition(newPos);
    }
    // ============================================

    if (velocity == Vec2::ZERO && _currentWalkAction == nullptr) {
        this ->unscheduleUpdate();
    }
}

void Player::farm()
{
    stop_move(0, 0);
    // 1. 停止所有移动相关的动作
    this->stopActionByTag(WALK_ACTION_TAG); // 停止行走动画
    this->stopActionByTag(farm_ACTION_TAG); // 停止可能正在运行的旧攻击动画

    // 攻击时，速度应为零，但我们不再取消 Player 的 update 调度。
    // 因为 GameScene::update 依赖它来做移动计算。
    _currentVelocity = cocos2d::Vec2::ZERO;
    //this->unscheduleUpdate();
    this->_isAction = true;
    // 2. 根据当前方向选择攻击动作
    cocos2d::Action* farmAction = nullptr;
    switch (this->direction) {
    case DIR_UP:    farmAction = _farmAction_up; break;
    case DIR_DOWN:  farmAction = _farmAction_down; break;
    case DIR_LEFT:  farmAction = _farmAction_left; break;
    case DIR_RIGHT: farmAction = _farmAction_right; break;
    default: return; // 默认方向不执行
    }

    // 3. 运行攻击动作
    if (farmAction) {
        // 必须使用 clone()
        cocos2d::Action* clonedAction = farmAction->clone();
        clonedAction->setTag(farm_ACTION_TAG);
        this->runAction(clonedAction);
    }
    //this->scheduleUpdate();
}

void Player::water()
{
    stop_move(0, 0);
    // 1. 停止所有移动相关的动作
    this->stopActionByTag(WALK_ACTION_TAG); // 停止行走动画
    this->stopActionByTag(farm_ACTION_TAG); // 停止可能正在运行的旧攻击动画

    // 攻击时，速度应为零，但我们不再取消 Player 的 update 调度。
    // 因为 GameScene::update 依赖它来做移动计算。
    _currentVelocity = cocos2d::Vec2::ZERO;
    //this->unscheduleUpdate();
    this->_isAction = true;
    // 2. 根据当前方向选择攻击动作
    cocos2d::Action* waterAction = nullptr;
    switch (this->direction) {
    case DIR_UP:    waterAction = _waterAction_up; break;
    case DIR_DOWN:  waterAction = _waterAction_down; break;
    case DIR_LEFT:  waterAction = _waterAction_left; break;
    case DIR_RIGHT: waterAction = _waterAction_right; break;
    default: return; // 默认方向不执行
    }

    // 3. 运行攻击动作
    if (waterAction) {
        // 必须使用 clone()
        cocos2d::Action* clonedAction = waterAction->clone();
        clonedAction->setTag(water_ACTION_TAG);
        this->runAction(clonedAction);
    }
    //this->scheduleUpdate();
}

void Player::cut()
{
    stop_move(0, 0);
    // 1. 停止所有移动相关的动作
    this->stopActionByTag(WALK_ACTION_TAG); // 停止行走动画
    this->stopActionByTag(farm_ACTION_TAG); // 停止可能正在运行的旧攻击动画

    // 攻击时，速度应为零，但我们不再取消 Player 的 update 调度。
    // 因为 GameScene::update 依赖它来做移动计算。
    _currentVelocity = cocos2d::Vec2::ZERO;
    //this->unscheduleUpdate();
    this->_isAction = true;
    // 2. 根据当前方向选择攻击动作
    cocos2d::Action* cutAction = nullptr;
    switch (this->direction) {
    case DIR_UP:    cutAction = _cutAction_up; break;
    case DIR_DOWN:  cutAction = _cutAction_down; break;
    case DIR_LEFT:  cutAction = _cutAction_left; break;
    case DIR_RIGHT: cutAction = _cutAction_right; break;
    default: return; // 默认方向不执行
    }

    // 3. 运行攻击动作
    if (cutAction) {
        // 必须使用 clone()
        cocos2d::Action* clonedAction = cutAction->clone();
        clonedAction->setTag(cut_ACTION_TAG);
        this->runAction(clonedAction);
    }
    //this->scheduleUpdate();
}