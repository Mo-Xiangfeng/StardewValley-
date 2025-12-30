#include "DialogueBox.h"

DialogueBox* DialogueBox::create(const std::string& npcName, const std::string& text) {
    DialogueBox* box = new (std::nothrow) DialogueBox();
    if (box && box->init(npcName, text)) {
        box->autorelease();
        return box;
    }
    delete box;
    return nullptr;
}

bool DialogueBox::init(const std::string& npcName, const std::string& text) {
    if (!Layer::init()) {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建半透明背景
    _background = Sprite::create();
    _background->setTextureRect(Rect(0, 0, visibleSize.width - 100, 150));
    _background->setColor(Color3B(30, 30, 30));
    _background->setOpacity(220);
    _background->setPosition(Vec2(
        origin.x + visibleSize.width / 2,
        origin.y + 100
    ));
    this->addChild(_background);

    // NPC 名字标签
    _nameLabel = Label::createWithSystemFont(npcName, "Arial", 24);
    _nameLabel->setColor(Color3B(255, 215, 0));  // 金色
    _nameLabel->setPosition(Vec2(
        origin.x + 80,
        origin.y + 150
    ));
    this->addChild(_nameLabel);

    // 对话文本
    _textLabel = Label::createWithSystemFont(text, "Arial", 20);
    _textLabel->setColor(Color3B::WHITE);
    _textLabel->setDimensions(visibleSize.width - 140, 80);
    _textLabel->setAlignment(TextHAlignment::LEFT);
    _textLabel->setPosition(Vec2(
        origin.x + visibleSize.width / 2,
        origin.y + 90
    ));
    this->addChild(_textLabel);

    // 初始隐藏
    this->setVisible(false);

    return true;
}

void DialogueBox::show() {
    this->setVisible(true);
}

void DialogueBox::hide() {
    this->setVisible(false);
}

void DialogueBox::setText(const std::string& text) {
    _textLabel->setString(text);
}
