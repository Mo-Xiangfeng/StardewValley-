#ifndef __REVIVE_SYSTEM_H__
#define __REVIVE_SYSTEM_H__

#include "cocos2d.h"
#include <functional>
#include <string>

USING_NS_CC;

class ReviveSystem : public cocos2d::Ref
{
public:
    // 单例获取
    static ReviveSystem* getInstance();

    // 触发复活流程
    void triggerRevive(Scene* currentScene,
        const std::string& reason = "unknown",
        const std::function<void()>& onComplete = nullptr);

    // 检查是否正在复活
    bool isReviving() const { return _isReviving; }

protected:
    ReviveSystem();
    virtual ~ReviveSystem();

private:
    static ReviveSystem* _instance;
    bool _isReviving;

    // 内部流程步骤
    void executeReviveSequence(Scene* currentScene,
        const std::string& reason,
        const std::function<void()>& onComplete);

    void stepFadeOut(Scene* scene, const std::function<void()>& onComplete);
    void stepSwitchSceneAndTime(const std::function<void()>& onComplete);
    void stepFadeIn(Scene* scene, const std::function<void()>& onComplete);

    // 过渡效果参数
    static constexpr float FADE_OUT_DURATION = 1.5f;
    static constexpr float FADE_IN_DURATION = 1.0f;
    static constexpr float FADE_COLOR_R = 0.0f;
    static constexpr float FADE_COLOR_G = 0.0f;
    static constexpr float FADE_COLOR_B = 0.0f;
};

#endif // __REVIVE_SYSTEM_H__
