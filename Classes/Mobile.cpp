#include "Mobile.h"
USING_NS_CC;

bool Joystick::init() {
    if (!Node::init()) return false;

    // === µ×ÅÌ ===
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // === ºËÐÄ£º°´ÆÁÄ»¿í¶È±ÈÀý·Å´ó ===
    float scale = visibleSize.width / 1920.0f; // ÄãÔ­Éè¼Æ·Ö±æÂÊ

    _radius = 100.0f * scale;   
    _base = DrawNode::create();
    _base->drawSolidCircle(
        Vec2::ZERO,
        _radius,
        0,
        60,
        Color4F(0, 0, 0, 0.3f)
    );
    addChild(_base);

    // === Ò¡¸Ë ===
    _thumb = DrawNode::create();
    _thumb->drawSolidCircle(
        Vec2::ZERO,
        _radius * 0.4f,
        0,
        60,
        Color4F(1, 1, 1, 0.7f)
    );
    addChild(_thumb);
    _base->setScale(scale * 1.2f);
    _thumb->setScale(scale * 1.2f);
    // === ´¥Ãþ¼àÌý ===
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(Joystick::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(Joystick::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(Joystick::onTouchEnded, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

bool Joystick::onTouchBegan(Touch* touch, Event*) {
    Vec2 pos = convertToNodeSpace(touch->getLocation());
    if (pos.length() > _radius) return false;

    _active = true;
    return true;
}

void Joystick::onTouchMoved(Touch* touch, Event*) {
    if (!_active) return;

    Vec2 pos = convertToNodeSpace(touch->getLocation());
    if (pos.length() > _radius) {
        pos.normalize();
        pos *= _radius;
    }

    _thumb->setPosition(pos);
    _direction = pos / _radius; // ·¶Î§ [-1, 1]
}

void Joystick::onTouchEnded(Touch*, Event*) {
    _active = false;
    _thumb->setPosition(Vec2::ZERO);
    _direction = Vec2::ZERO;
}
