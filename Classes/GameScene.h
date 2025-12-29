// 文件名: GameScene.h (Classes/GameScene.h)

#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include"Player.h"
#include "ui/CocosGUI.h"
class DialogueBox;
class NPC;
class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

    GameWorld* getGameWorld() const { return _gameWorld; }
    Player* getPlayer() const { return _playerSprite; }
    void showFavorabilityGain(const std::string& npcName, int amount);
private:
    // 【修改点 1】：动画精灵
    Player* _playerSprite;
    GameWorld* _gameWorld;
    cocos2d::ui::LoadingBar* _staminaBar;
    cocos2d::ui::LoadingBar* _StaminaBar;
    //NPC
    DialogueBox* _dialogueBox = nullptr;
    NPC* _currentTalkingNPC = nullptr;
    cocos2d::EventListenerMouse* _mouseListener = nullptr;
    void initNPCs();  // 初始化 NPC

    // ... (保持其它成员变量和函数不变，如 onKeyPressed, update 等)
    bool _isWPressed = false;
    bool _isAPressed = false;
    bool _isSPressed = false;
    bool _isDPressed = false;
    bool _isTPressed = false;
    // 凌晨强制睡眠标志
    bool _hasTriggeredExhaustion = false;
    int _selectedSlotIndex = -1; // -1 表示空手，0-8 表示快捷栏格位
    void onMouseDown(cocos2d::EventMouse* event);
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    virtual void update(float dt) override;
    void processMovement();
    ~GameScene();
};

#endif // __GAME_SCENE_H__#pragma once
