#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "GameWorld.h"
#include "Player.h"
#include"GameScene.h"
class HelloWorldScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void update(float dt) override;

    CREATE_FUNC(HelloWorldScene);

private:
    GameWorld* _gameWorld = nullptr;
    Player* _player = nullptr;
};

#endif
