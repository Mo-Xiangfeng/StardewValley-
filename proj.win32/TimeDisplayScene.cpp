#include "TimeDisplayScene.h"

Scene* TimeDisplayScene::createScene() {
    return TimeDisplayScene::create();
}

bool TimeDisplayScene::init() {
    if (!Scene::init()) {
        return false;
    }

    // 初始化管理器
    TimeManager::getInstance()->init();
    WeatherManager::getInstance()->init();

    // 设置时间流速：每真实秒 = 10游戏分钟（可调整）
    TimeManager::getInstance()->setTimeScale(10.0f);

    // 注册日期变化回调
    TimeManager::getInstance()->registerDayCallback([this](int y, int s, int d) {
        this->onDayChanged(y, s, d);
        });

    setupUI();


    // 启动更新
    scheduleUpdate();

    return true;
}

void TimeDisplayScene::setupUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 天气覆盖层
// 创建半透明覆盖层
    weatherOverlay = LayerColor::create(Color4B(255, 255, 255, 50));

    // 1. 确保大小完全等于可见区域
    weatherOverlay->setContentSize(visibleSize);

    // 2. 【核心修复】将位置设置到可见区域的原点
    weatherOverlay->setPosition(origin);

    this->addChild(weatherOverlay, 0);

    // 先用英文测试
    float paddingX = 20.0f; // 距离右边缘的距离
    float topMargin = 30.0f; // 第一个标签距离顶部的距离
    float spacingY = 50.0f;  // 标签之间的垂直间距

    // 1. 时间标签 (最上方)
    timeLabel = Label::createWithSystemFont("06:00", "Arial", 50);
    // 设置锚点为右侧中心，这样设置位置时，坐标点代表标签的右边缘中心
    timeLabel->setAnchorPoint(Vec2(1.0f, 0.5f));
    timeLabel->setPosition(Vec2(origin.x + visibleSize.width - paddingX,
        origin.y + visibleSize.height - topMargin));
    timeLabel->setTextColor(Color4B::BLACK);
    this->addChild(timeLabel, 1);

    // 2. 日期标签 (中间)
    dateLabel = Label::createWithSystemFont("Year 1 Spring Day 1", "Arial", 50);
    dateLabel->setAnchorPoint(Vec2(1.0f, 0.5f));
    dateLabel->setPosition(Vec2(origin.x + visibleSize.width - paddingX,
        timeLabel->getPositionY() - spacingY));
    dateLabel->setTextColor(Color4B::BLACK);
    this->addChild(dateLabel, 1);

    // 3. 天气标签 (下方)
    weatherLabel = Label::createWithSystemFont("Weather: Sunny", "Arial", 50);
    weatherLabel->setAnchorPoint(Vec2(1.0f, 0.5f));
    weatherLabel->setPosition(Vec2(origin.x + visibleSize.width - paddingX,
        dateLabel->getPositionY() - spacingY));
    weatherLabel->setTextColor(Color4B::BLACK);
    this->addChild(weatherLabel, 1);

}


void TimeDisplayScene::update(float dt) {
    TimeManager::getInstance()->update(dt);
    updateDisplay();
}

void TimeDisplayScene::updateDisplay() {
    auto timeMgr = TimeManager::getInstance();
    auto weatherMgr = WeatherManager::getInstance();

    timeLabel->setString(timeMgr->getFormattedTime());
    dateLabel->setString(timeMgr->getFormattedDate());

    std::string weatherText = "Weather: " + weatherMgr->getWeatherName();
    if (weatherLabel->getString() != weatherText) {
        weatherLabel->setString(weatherText);
        // 当天气名称变化时，同步更新视觉特效
        applyWeatherEffects(weatherMgr->getCurrentWeather());
    }

    auto tint = weatherMgr->getWeatherTint();
    weatherOverlay->setColor(Color3B(tint.r, tint.g, tint.b));
}

void TimeDisplayScene::onDayChanged(int year, int season, int day) {
    // 每天更新天气
    WeatherManager::getInstance()->updateWeather(season);
    CCLOG("新的一天! 年:%d 季节:%d 日:%d", year, season, day);
}

void TimeDisplayScene::applyWeatherEffects(WeatherType type) {
    // 1. 清理旧特效
    if (weatherEffectNode) {
        weatherEffectNode->removeFromParent();
        weatherEffectNode = nullptr;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    switch (type) {
    case WeatherType::RAINY: {
        // 使用内置雨粒子（不需要贴图，默认使用内置的微小像素块）
        auto rain = ParticleRain::create();
        rain->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height));
        rain->setSpeed(400.0f);
        rain->setGravity(Vec2(-100, -200)); // 倾斜下雨
        weatherEffectNode = rain;
        break;
    }
    case WeatherType::SNOWY: {
        // 使用内置雪粒子
        auto snow = ParticleSnow::create();
        snow->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height));
        snow->setSpeed(60.0f);
        snow->setStartSize(4.0f); // 像素雪花稍大一点
        weatherEffectNode = snow;
        break;
    }
    case WeatherType::STORMY: {
        // 暴风雨：更急的雨 + 闪电逻辑
        auto storm = ParticleRain::create();
        storm->setSpeed(500.0f);
        storm->setEmissionRate(storm->getEmissionRate() * 2);
        storm->setGravity(Vec2(-200, -400));
        weatherEffectNode = storm;

        // 启动随机闪电
        auto lightningAction = RepeatForever::create(Sequence::create(
            DelayTime::create(3.0f + rand() % 5),
            CallFunc::create(CC_CALLBACK_0(TimeDisplayScene::createLightning, this)),
            nullptr
        ));
        weatherEffectNode->runAction(lightningAction);
        break;
    }
    case WeatherType::CLOUDY: {
        // 多云：使用 DrawNode 绘制几个半透明圆形块移动
        auto drawNode = DrawNode::create();
        for (int i = 0; i < 5; i++) {
            drawNode->drawDot(Vec2(rand() % 300, rand() % 100 + visibleSize.height - 100),
                40 + rand() % 20, Color4F(1, 1, 1, 0.3f));
        }
        // 让云缓慢飘动
        drawNode->runAction(RepeatForever::create(Sequence::create(
            MoveBy::create(10.0f, Vec2(-200, 0)),
            MoveTo::create(0.0f, Vec2(0, 0)),
            nullptr
        )));
        weatherEffectNode = drawNode;
        break;
    }
    default:
        break; // 晴天不需要额外粒子
    }

    if (weatherEffectNode) {
        this->addChild(weatherEffectNode, 2); // 放在 UI 下方，遮罩上方
    }
}

void TimeDisplayScene::createLightning() {
    // 闪电视觉效果：通过快速改变 Overlay 的透明度实现
    auto originalOpacity = weatherOverlay->getOpacity();
    auto flash = Sequence::create(
        FadeTo::create(0.05f, 255), // 瞬间极亮
        FadeTo::create(0.05f, 100), // 稍暗
        FadeTo::create(0.05f, 230), // 再次亮起
        FadeTo::create(0.2f, originalOpacity), // 回到原天气亮度
        nullptr
    );
    weatherOverlay->runAction(flash);

    // 可以在这里添加震屏效果
    auto shake = Sequence::create(
        MoveBy::create(0.05f, Vec2(5, 5)),
        MoveBy::create(0.05f, Vec2(-10, -10)),
        MoveBy::create(0.05f, Vec2(5, 5)),
        nullptr
    );
    this->runAction(shake);
}
