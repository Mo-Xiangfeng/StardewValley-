#include "cocos2d.h"
#include "fishing_game.h"
#include "HelloWorldScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocos2d::ui;


bool FishingGame::init() {
    if (!Node::init()) return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();


    // 1. 半透明遮罩层（让背景地图变暗，突出小游戏）
    auto mask = LayerColor::create(Color4B(0, 0, 0, 150));
    this->addChild(mask);

    // 2. 游戏容器（居中显示）
    _container = Node::create();
    _container->setPosition(visibleSize / 2);
    this->addChild(_container);

    // 3. 绘制 UI (背景槽、绿条、鱼)
    auto bg = Sprite::create("fishing_pole.png");
    bg->setPosition(20, 0); 
    bg->setScale(0.7f);
    _container->addChild(bg);
    
   
    _fishBar = Sprite::create();
    _fishBar->setTextureRect(Rect(0, 0, 9, BAR_H));
    _fishBar->setColor(Color3B::GREEN);
    _fishBar->setAnchorPoint(Vec2(0, -0.1));

    Size bgSize = bg->getContentSize();
    float barPosX = bgSize.width * 0.44f; 
    float initialY = 50.0f;
    _barY = initialY;
    _fishY = initialY;
    _fishBar->setPosition(Vec2(barPosX, _barY ));
    bg->addChild(_fishBar);

    _fish = Sprite::create("fishing.png");
    _fish->setPosition(Vec2(_fishBar->getPositionX()+5, _fishY));
    _fish->setScale(0.2f);
    bg->addChild(_fish);

    // 1. 创建进度条，直接读取你的图片
    _prog = ui::LoadingBar::create("GREEN.png");


    _prog->setDirection(ui::LoadingBar::Direction::RIGHT);
    _prog->setRotation(90);

    _prog->ignoreContentAdaptWithSize(false);
    _prog->setContentSize(Size(135, 4)); 
    _prog->setPosition(Vec2(bgSize.width * 0.89f, bgSize.height * 0.5f));
    bg->addChild(_prog, 10);
  
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = [this](Event* event) {
        if (static_cast<EventMouse*>(event)->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
            _isPressing = true;
        };
    mouseListener->onMouseUp = [this](Event* event) {
        if (static_cast<EventMouse*>(event)->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
            _isPressing = false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    this->scheduleUpdate();
    return true;
}

void FishingGame::update(float dt) {
    // --- 物理逻辑 ---
    float accel = _isPressing ? BUOYANCY : GRAVITY;
    _barVel += accel * dt;
    _barY += _barVel * dt;

    // 碰撞边界处理
    if (_barY <= 0) {
        _barY = 0;
        _barVel *= -0.4f; // 反弹
    }
    else if (_barY >= AREA_H - BAR_H) {
        _barY = AREA_H - BAR_H;
        _barVel = 0;
    }
    _fishBar->setPositionY(_barY);

    // B. 鱼 AI：平滑移动到随机目标
    _fishTimer -= dt;
    if (_fishTimer <= 0) {
        _fishTargetY = CCRANDOM_0_1() * (AREA_H - 20);
        _fishTimer = 1.0f + CCRANDOM_0_1() * 1.5f;
    }
    _fishY = _fishY + (_fishTargetY - _fishY) * 1.8f * dt; // Lerp 平滑插值
    _fish->setPositionY(_fishY + 10); // +10 是为了对齐中心

    bool isInside = (_fishY >= _barY && _fishY <= _barY + BAR_H);
    _progress += (isInside ? 20.0f : -10.0f) * dt;
    _progress = std::max(0.0f, std::min(100.0f, _progress));
    if (_prog) {
        _prog->setPercent(_progress);

        // 视觉反馈：如果进度快掉光了，变个色提醒玩家
        if (_progress < 25.0f) {
            _prog->setColor(Color3B::RED);
        }
        else {
            _prog->setColor(Color3B::WHITE); // 恢复原图颜色（绿色）
        }
    }

    // D. 检查结束
    if (_progress >= 100.0f || _progress <= 0.0f) {
        if (_progress >= 100.0f || _progress <= 0.0f) {
            // 【关键】先停止调度，再执行回调
            this->unscheduleUpdate();

            if (onGameOver) {
                onGameOver(_progress >= 100.0f);
            }

            // 注意：回调之后不要再写任何代码，因为此时 this 可能已被销毁
            return;
        }
    }
}