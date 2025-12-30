#include "FishingGame.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocos2d::ui;

bool FishingGame::init()
{
    if (!Layer::init()) return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 初始状态
    _isPressing = false;
    _barVel = 0;
    _progress = 30.0f;

    // 半透明遮罩
    auto mask = LayerColor::create(Color4B(0, 0, 0, 150));
    this->addChild(mask);

    // 容器
    _container = Node::create();
    _container->setPosition(visibleSize / 2);
    this->addChild(_container);

    // 背景
    auto bg = Sprite::create("fishing_pole.png");
    bg->setScale(0.7f);
    _container->addChild(bg);

    Size bgSize = bg->getContentSize();

    // 绿条
    _fishBar = Sprite::create();
    _fishBar->setTextureRect(Rect(0, 0, 35, BAR_H));
    _fishBar->setColor(Color3B::GREEN);
    _fishBar->setAnchorPoint(Vec2(0, 0));

    float barPosX = bgSize.width * 0.44f;
    _barY = 100.0f;
    _fishBar->setPosition(barPosX, _barY);
    bg->addChild(_fishBar);

    // 鱼
    _fishY = _barY;
    _fish = Sprite::create("fishing.png");
    _fish->setScale(0.2f);
    _fish->setPosition(barPosX + 15, _fishY);
    bg->addChild(_fish);

    // 进度条
    _prog = LoadingBar::create("GREEN.png");
    _prog->setDirection(LoadingBar::Direction::RIGHT);
    _prog->setRotation(90);
    _prog->ignoreContentAdaptWithSize(false);
    _prog->setContentSize(Size(AREA_H, 12));
    _prog->setPercent(_progress);
    _prog->setPosition(Vec2(bgSize.width * 0.9f, bgSize.height * 0.5f));
    bg->addChild(_prog, 10);

    // =========================
    // Android 触摸输入（唯一）
    // =========================
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);

    touchListener->onTouchBegan = [this](Touch* touch, Event* event) {
        _isPressing = true;
        return true;
        };

    touchListener->onTouchEnded = [this](Touch* touch, Event* event) {
        _isPressing = false;
        };

    touchListener->onTouchCancelled = [this](Touch* touch, Event* event) {
        _isPressing = false;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    this->scheduleUpdate();
    return true;
}

void FishingGame::update(float dt)
{
    // ======================
    // A. 绿条物理
    // ======================
    float accel = _isPressing ? BUOYANCY : GRAVITY;
    _barVel += accel * dt;
    _barY += _barVel * dt;

    if (_barY < 0) {
        _barY = 0;
        _barVel = 0;
    }
    if (_barY > AREA_H - BAR_H) {
        _barY = AREA_H - BAR_H;
        _barVel = 0;
    }

    _fishBar->setPositionY(_barY);

    // ======================
    // B. 鱼 AI
    // ======================
    _fishTimer -= dt;
    if (_fishTimer <= 0) {
        _fishTargetY = CCRANDOM_0_1() * (AREA_H - 20);
        _fishTimer = 1.0f + CCRANDOM_0_1() * 1.5f;
    }

    _fishY += (_fishTargetY - _fishY) * 1.2f * dt;
    _fish->setPositionY(_fishY + 10);

    // ======================
    // C. 进度判定
    // ======================
    bool inside = (_fishY >= _barY && _fishY <= _barY + BAR_H);
    _progress += (inside ? 25.0f : -15.0f) * dt;
    _progress = clampf(_progress, 0.0f, 100.0f);

    _prog->setPercent(_progress);
    _prog->setColor(_progress < 25 ? Color3B::RED : Color3B::WHITE);

    // ======================
    // D. 结束判定（安全版）
    // ======================
    if (!_ending && (_progress <= 0 || _progress >= 100)) {
        _ending = true;
        _winResult = (_progress >= 100);

        // 立刻停止 update（非常重要）
        this->unscheduleUpdate();

        // ⭐ 关键修复：延迟到下一帧再处理结束
        this->runAction(Sequence::create(
            DelayTime::create(0),
            CallFunc::create([this]() {
                if (onGameOver) {
                    onGameOver(_winResult);
                }
                this->removeFromParent();
                }),
            nullptr
        ));
    }
}
