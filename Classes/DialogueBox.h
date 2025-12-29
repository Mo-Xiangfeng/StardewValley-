#ifndef __DIALOGUE_BOX_H__
#define __DIALOGUE_BOX_H__

#include "cocos2d.h"

USING_NS_CC;

class DialogueBox : public cocos2d::Layer {
public:
    static DialogueBox* create(const std::string& npcName, const std::string& text);

    virtual bool init(const std::string& npcName, const std::string& text);

    void show();
    void hide();
    void setText(const std::string& text);

private:
    cocos2d::Sprite* _background;
    cocos2d::Label* _nameLabel;
    cocos2d::Label* _textLabel;
};

#endif // __DIALOGUE_BOX_H__
