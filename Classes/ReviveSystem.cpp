#include "ReviveSystem.h"
#include "GameScene.h"
#include "TimeManager.h"
#include "GameWorld.h"
#include "Player.h"
#include "WeatherManager.h"
// 单例实例
ReviveSystem* ReviveSystem::_instance = nullptr;

ReviveSystem::ReviveSystem()
    : _isReviving(false)
{
}

ReviveSystem::~ReviveSystem()
{
}

ReviveSystem* ReviveSystem::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new (std::nothrow) ReviveSystem();
        if (_instance)
        {
            _instance->autorelease();
            _instance->retain();
        }
    }
    return _instance;
}

void ReviveSystem::triggerRevive(cocos2d::Scene* currentScene,
    const std::string& reason,
    const std::function<void()>& onComplete)
{
    if (_isReviving)
    {
        CCLOG("[ReviveSystem] Already reviving, ignoring. Reason: %s", reason.c_str());
        return;
    }

    CCLOG("[ReviveSystem] Revive triggered. Reason: %s", reason.c_str());
    _isReviving = true;

    executeReviveSequence(currentScene, reason, onComplete);
}

void ReviveSystem::executeReviveSequence(cocos2d::Scene* currentScene,
    const std::string& reason,
    const std::function<void()>& onComplete)
{
    stepFadeOut(currentScene, [this, onComplete]() {
        stepSwitchSceneAndTime([this, onComplete]() {
            auto director = cocos2d::Director::getInstance();
            auto newScene = director->getRunningScene();

            stepFadeIn(newScene, [this, onComplete]() {
                _isReviving = false;
                CCLOG("[ReviveSystem] Revive sequence completed.");

                if (onComplete)
                {
                    onComplete();
                }
                });
            });
        });
}

void ReviveSystem::stepFadeOut(cocos2d::Scene* scene, const std::function<void()>& onComplete)
{
    CCLOG("[ReviveSystem] Step 1: Fading out...");

    auto fadeLayer = cocos2d::LayerColor::create(
        cocos2d::Color4B(
            FADE_COLOR_R * 255,
            FADE_COLOR_G * 255,
            FADE_COLOR_B * 255,
            0
        )
    );
    fadeLayer->setName("ReviveFadeLayer");
    scene->addChild(fadeLayer, 9999);

    auto fadeOut = cocos2d::FadeTo::create(FADE_OUT_DURATION, 255);
    auto callback = cocos2d::CallFunc::create([onComplete]() {
        if (onComplete)
        {
            onComplete();
        }
        });

    fadeLayer->runAction(cocos2d::Sequence::create(fadeOut, callback, nullptr));
}

void ReviveSystem::stepSwitchSceneAndTime(const std::function<void()>& onComplete)
{
    CCLOG("[ReviveSystem] Step 2: Advancing time and switching map...");

    // 时间推进
    auto timeManager = TimeManager::getInstance();
    timeManager->pauseTime();

    int newYear = timeManager->getYear();
    int newSeason = timeManager->getSeason();
    int newDay = timeManager->getDay() + 1;

    if (newDay > 28)
    {
        newDay = 1;
        newSeason++;
        if (newSeason > 3)
        {
            newSeason = 0;
            newYear++;
        }
    }

    timeManager->setDateTime(newYear, newSeason, newDay, 6, 0);
    timeManager->resumeTime();

    CCLOG("[ReviveSystem] Time: Year %d, Season %d, Day %d, 06:00",
        newYear, newSeason, newDay);

    WeatherManager::getInstance()->updateWeather(newSeason);
    auto director = cocos2d::Director::getInstance();
    auto currentScene = director->getRunningScene();
    auto gameScene = dynamic_cast<GameScene*>(currentScene);

    if (gameScene) {
        auto gameWorld = gameScene->getGameWorld();
        if (gameWorld) {
            gameWorld->updateWeatherVisuals(); // 现在可以正确调用了
        }
    }
    director->getScheduler()->schedule(
        [onComplete](float dt) {
            auto currentScene = cocos2d::Director::getInstance()->getRunningScene();
            auto gameScene = dynamic_cast<GameScene*>(currentScene);

            if (gameScene)
            {
                auto gameWorld = gameScene->getGameWorld();
                auto player = gameScene->getPlayer();

                if (gameWorld && player)
                {
                    gameWorld->switchMap("Home", "Exit");
                    player->direction = DIR_DOWN; 
                    player->stop_move(0, 0);       

                    CCLOG("[ReviveSystem] Player at Home.");
                }
            }

            if (onComplete)
            {
                onComplete();
            }
        },
        director->getScheduler(),
        0.0f,
        0,
        0.0f,
        false,
        "ReviveCallback"
    );
}

void ReviveSystem::stepFadeIn(cocos2d::Scene* scene, const std::function<void()>& onComplete)
{
    CCLOG("[ReviveSystem] Step 3: Fading in...");

    auto fadeLayer = scene->getChildByName("ReviveFadeLayer");

    if (fadeLayer)
    {
        auto fadeIn = cocos2d::FadeTo::create(FADE_IN_DURATION, 0);
        auto removeLayer = cocos2d::RemoveSelf::create();
        auto callback = cocos2d::CallFunc::create([onComplete]() {
            if (onComplete)
            {
                onComplete();
            }
            });

        fadeLayer->runAction(cocos2d::Sequence::create(fadeIn, removeLayer, callback, nullptr));
    }
    else
    {
        CCLOG("[ReviveSystem] Warning: Fade layer not found!");
        if (onComplete)
        {
            onComplete();
        }
    }
}
