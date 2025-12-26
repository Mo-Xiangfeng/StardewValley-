// 文件名: GameScene.h (Classes/GameScene.h)

#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include"Player.h"
class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);
private:
    // 【修改点 1】：动画精灵
    Player* _playerSprite;
    GameWorld* _gameWorld;
    // 【修改点 2】：动画动作（用于存储重复播放的动画）

    // 【新增/修改点 3】：动画相关函数

    // ... (保持其它成员变量和函数不变，如 onKeyPressed, update 等)
    bool _isWPressed = false;
    bool _isAPressed = false;
    bool _isSPressed = false;
    bool _isDPressed = false;
    bool _isTPressed = false;
    void onMouseDown(cocos2d::EventMouse* event);
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    virtual void update(float dt) override;
    void processMovement();
    ~GameScene();
};

#endif // __GAME_SCENE_H__#pragma once
