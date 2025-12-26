#pragma once
// PlayerCharacter.h
#include "cocos2d.h"
#include<iostream>
#include"GameWorld.h"
enum Direction {
    DIR_DOWN = 0,
    DIR_LEFT = 1,
    DIR_UP = 2,
    DIR_RIGHT = 3
};
class GameWorld; // 前向声明
class Player : public cocos2d::Sprite
{
public:
    // 静态创建方法
    static Player* create(const std::string& filename);
    static Player* createWithSpriteFrameName(const std::string& spriteFrameName);
    // 角色属性
    int maxHP;      // 最大生命值
    int currentHP;  // 当前生命值
    int maxStamina; // 最大体力值
    int currentStamina; // 当前体力值
    int luck;       // 运气值 (百分比或数值)
    int farmPower; // 攻击力
    int direction; //方向
    int what_in_hand_now; //手持物
    // 行为方法
    void takeDamage(int damage);
    void start_move(int directionX, int directionY);
    void stop_move(int directionX, int directionY);
    virtual void update(float dt) override;
    cocos2d::Action* _walkAction_down = nullptr;
    cocos2d::Action* _walkAction_left = nullptr;
    cocos2d::Action* _walkAction_up = nullptr;
    cocos2d::Action* _walkAction_right = nullptr;
    cocos2d::Action* _currentWalkAction = nullptr;

    cocos2d::Action* _farmAction_down = nullptr;
    cocos2d::Action* _farmAction_left = nullptr;
    cocos2d::Action* _farmAction_up = nullptr;
    cocos2d::Action* _farmAction_right = nullptr;

    cocos2d::Action* _waterAction_down = nullptr;
    cocos2d::Action* _waterAction_left = nullptr;
    cocos2d::Action* _waterAction_up = nullptr;
    cocos2d::Action* _waterAction_right = nullptr;

    cocos2d::Action* _cutAction_down = nullptr;
    cocos2d::Action* _cutAction_left = nullptr;
    cocos2d::Action* _cutAction_up = nullptr;
    cocos2d::Action* _cutAction_right = nullptr;

    cocos2d::Vec2 _currentVelocity; // 用于 update 计算位置
    bool _isAction = false;
    void setGameWorld(GameWorld* world);
    void setTilePosition(int tx, int ty);
    void farm();
    void water();
    void cut();
    void onInteract();
    void useStamina(int cost);
    bool isDead() const;
    // 构造函数和初始化方法
    float getPlayerScale() const { return _playerScale; }
    Player();
    virtual bool initWithSpriteFrameName(const std::string& spriteFrameName) override;
    void createAndRunActionAnimation(); // 专门用于创建和启动图集动画
    GameWorld* getGameWorld() const { return _world; }
private:
    // 私有方法或成员
    float _playerScale = 4.5f;
    GameWorld* _world = nullptr;
    cocos2d::Animation* createAnimation(const std::string& framePrefix, int frameCount, float delay);
};


