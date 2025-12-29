#include "Daylight.h"
#include "TimeManager.h" // 引入你的时间管理器

USING_NS_CC;

// 定义关键颜色
const Color3B COLOR_DAWN = Color3B(255, 180, 100); // 清晨橙色
const Color3B COLOR_NOON = Color3B(255, 255, 255); // 白天（颜色不重要，因为透明度是0）
const Color3B COLOR_DUSK = Color3B(140, 100, 120); // 黄昏红紫色
const Color3B COLOR_NIGHT = Color3B(10, 10, 50);    // 深夜深蓝

bool Daylight::init() {
    if (!Layer::init()) {
        return false;
    }

    // 1. 创建全屏遮罩
    // 初始颜色给全黑，后续 update 会立即修正它
    _overlay = LayerColor::create(Color4B(0, 0, 0, 0));
    this->addChild(_overlay);

    // 2. 开启每帧更新
    this->scheduleUpdate();

    // 3. 立即执行一次更新，防止刚进入场景时闪烁
    this->update(0);

    return true;
}

void Daylight::update(float dt) {
    auto tm = TimeManager::getInstance();
    if (tm->isTimePaused()) return;

    // 抵消坐标逻辑保持不变
    if (this->getParent()) {
        Vec2 worldPos = this->getParent()->getPosition();
        this->setPosition(-worldPos);
    }

    float exactHour = tm->getHour() + (tm->getMinute() / 60.0f);

    Color3B targetColor;
    GLubyte targetOpacity;

    // --- 全天 24 小时平滑闭环逻辑 ---

    if (exactHour >= 0.0f && exactHour < 5.0f) {
        // [00:00 - 05:00] 深夜锁定
        targetColor = COLOR_NIGHT;
        targetOpacity = 190;
    }
    else if (exactHour >= 5.0f && exactHour < 7.0f) {
        // [05:00 - 07:00] 黎明：深夜 -> 清晨橙色
        float ratio = (exactHour - 5.0f) / 2.0f;
        targetColor = lerpColor(COLOR_NIGHT, COLOR_DAWN, ratio);
        targetOpacity = lerpOpacity(190, 80, ratio);
    }
    else if (exactHour >= 7.0f && exactHour < 10.0f) {
        // [07:00 - 10:00] 日出：清晨 -> 纯透明
        float ratio = (exactHour - 7.0f) / 3.0f;
        targetColor = lerpColor(COLOR_DAWN, COLOR_NOON, ratio);
        targetOpacity = lerpOpacity(80, 0, ratio);
    }
    else if (exactHour >= 10.0f && exactHour < 16.5f) {
        // [10:00 - 16:30] 纯白天：保持透明
        targetColor = COLOR_NOON;
        targetOpacity = 0;
    }
    else if (exactHour >= 16.5f && exactHour < 19.0f) {
        // [16:30 - 19:00] 黄昏入场：透明 -> 柔和紫褐 (COLOR_DUSK)
        float ratio = (exactHour - 16.5f) / 2.5f;
        targetColor = lerpColor(COLOR_NOON, COLOR_DUSK, ratio);
        targetOpacity = lerpOpacity(0, 130, ratio);
    }
    else if (exactHour >= 19.0f && exactHour < 21.0f) {
        // [19:00 - 21:00] 入夜：紫褐 -> 深蓝
        float ratio = (exactHour - 19.0f) / 2.0f;
        targetColor = lerpColor(COLOR_DUSK, COLOR_NIGHT, ratio);
        targetOpacity = lerpOpacity(130, 190, ratio);
    }
    else {
        // [21:00 - 24:00] 深夜锁定
        targetColor = COLOR_NIGHT;
        targetOpacity = 190;
    }

    _overlay->setColor(targetColor);
    _overlay->setOpacity(targetOpacity);
}

// 线性插值颜色
Color3B Daylight::lerpColor(const Color3B& start, const Color3B& end, float p) {
    GLubyte r = start.r + (end.r - start.r) * p;
    GLubyte g = start.g + (end.g - start.g) * p;
    GLubyte b = start.b + (end.b - start.b) * p;
    return Color3B(r, g, b);
}

// 线性插值透明度
GLubyte Daylight::lerpOpacity(GLubyte start, GLubyte end, float p) {
    return start + (end - start) * p;
}