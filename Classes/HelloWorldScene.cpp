#include "HelloWorldScene.h"
USING_NS_CC;

Scene* HelloWorldScene::createScene()
{
    return HelloWorldScene::create();
}

bool HelloWorldScene::init()
{
    if (!Scene::init()) return false;

    _gameWorld = GameWorld::create("tilemap.txt", "Map.png");
    addChild(_gameWorld);

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("down_side.plist");
    _player = Player::createWithSpriteFrameName("down_side-0.png");
    _player->createAndRunActionAnimation();
    _player->setGameWorld(_gameWorld);
    _gameWorld->addChild(_player, 10);
    _gameWorld->setPlayer(_player);
    _gameWorld->switchMap("Map", "Spawn");
    
    scheduleUpdate();
    return true;
}

void HelloWorldScene::update(float dt)
{
    if (!_player || !_gameWorld) return;

    // 更新游戏逻辑（portal + map logic）
    _gameWorld->update(dt);

    // 更新相机
    _gameWorld->updateCamera();
}

