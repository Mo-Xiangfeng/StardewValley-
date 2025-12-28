#include "Monster.h"
#include "GameWorld.h"
#include <string>

USING_NS_CC;

Monster* Monster::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    Monster* pRet = new (std::nothrow) Monster();

    if (pRet && pRet->initWithSpriteFrameName(spriteFrameName))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool Monster::initWithSpriteFrameName(const std::string& spriteFrameName)
{
    if (!Sprite::initWithSpriteFrameName(spriteFrameName))
    {
        return false;
    }

    // --- 初始化数值 ---
    this->maxHP = 60;
    this->currentHP = 60;
    this->attackPower = 5;
    this->setScale(0.3f);


    // --- 初始化 AI 参数 ---
    this->_detectRange = 80.0f; // 像素距离
    this->_moveSpeed = 0.5f;    // 史莱姆移动较慢
    this->_wanderTimer = 1.0f;
    this->_attackTimer = 2.0f;
    this->_stunTimer = 0.0f;
    this->_wanderDirection = Vec2::ZERO;

    // 创建并播放动画
    this->createAndRunSlimeAnimation();

    // 开启 Update
    this->scheduleUpdate();

    return true;
}

void Monster::createAndRunSlimeAnimation() {

    int frameCount = 3;
    Vector<SpriteFrame*> frames;
    for (int i = 0; i < frameCount; i++) {
        std::string name = "slime-" + std::to_string(i) + ".png";
        SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);

        // 如果找不到特定帧，就用当前帧防止崩溃（可选）
        if (frame) {
            frames.pushBack(frame);
        }
    }

    if (!frames.empty()) {
        Animation* animation = Animation::createWithSpriteFrames(frames, 0.2f);
        Animate* animate = Animate::create(animation);
        this->runAction(RepeatForever::create(animate));
    }
}
void Monster::update(float dt)
{
    if (this->currentHP <= 0) return
        ;

    if (_stunTimer > 0
        ) {
        _stunTimer -= dt;
        return
            ;
    }

    if
        (_wanderDirection != Vec2::ZERO) {
        Vec2 currentPos =
            this
            ->getPosition();
        // 注意：Monster.h 中 _moveSpeed 若为 0.5f 建议在初始化时改为 40.0f 左右以获得明显移动
        Vec2 moveStep = _wanderDirection * _moveSpeed * dt;
        Vec2 nextPos = currentPos + moveStep;

        // 获取完整类类型的父节点
        auto world = dynamic_cast<GameWorld*>(this
            ->getParent());

        if
            (world) {
            if
                (world->isWalkable(nextPos)) {
                // 路径通畅，执行移动
                this
                    ->setPosition(nextPos);
            }
            else
            {
                // --- 碰撞掉头逻辑 ---
                // 1. 方向取反
                _wanderDirection = -_wanderDirection;

                // 2. 增加一点随机扰动（防止在狭窄通道来回抽搐）
                float randomAngle = CC_DEGREES_TO_RADIANS(cocos2d::random(-15, 15
                ));
                _wanderDirection.rotate(Vec2::ZERO, randomAngle);

                // 3. 重置游荡计时器，确保掉头后能走一段距离
                _wanderTimer =
                    1.5f
                    ;

                // 4. (可选) 立即同步一次翻转状态
                this->setFlippedX(_wanderDirection.x > 0
                );
            }
        }
        else
        {
            this ->setPosition(nextPos);
        }

        // 常规翻转更新
        if (_wanderDirection.x > 0) this->setFlippedX(true
        );
        else if (_wanderDirection.x < 0) this->setFlippedX(false
        );
    }

    if (_attackTimer > 0
        ) _attackTimer -= dt;
    if (_wanderTimer > 0
        ) _wanderTimer -= dt;
}
void Monster::takeDamage(int damage) {
    if (currentHP <= 0) return;

    this->currentHP -= damage;

    // 受伤闪烁红光
    auto tintRed = TintTo::create(0.1f, 255, 100, 100);
    auto tintBack = TintTo::create(0.1f, 255, 255, 255);
    this->runAction(Sequence::create(tintRed, tintBack, nullptr));

    //CCLOG("Slime took %d dmg. Current HP: %d", damage, currentHP);

    if (this->currentHP <= 0) {
        // 死亡逻辑
        this->stopAllActions();
        // 禁用物理/更新
        this->unscheduleUpdate();

        // 死亡动画：缩小消失
        auto shrink = ScaleTo::create(0.3f, 0.01f);
        this->runAction(shrink);
    }
}