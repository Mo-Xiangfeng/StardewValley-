#ifndef __DAYLIGHT_H__
#define __DAYLIGHT_H__

#include "cocos2d.h"

class Daylight : public cocos2d::Layer {
public:
    virtual bool init() override;
    virtual void update(float dt) override;

    // 标准的 create() 宏
    CREATE_FUNC(Daylight);

private:
    cocos2d::LayerColor* _overlay; // 全屏的有色遮罩层

    // 辅助函数：颜色插值（用于计算两个颜色中间的过渡色）
    cocos2d::Color3B lerpColor(const cocos2d::Color3B& start, const cocos2d::Color3B& end, float p);

    // 辅助函数：透明度插值
    GLubyte lerpOpacity(GLubyte start, GLubyte end, float p);
};

#endif // __DAYLIGHT_H__