#include "TimeDisplayScene.h"
#include "MoneyManager.h"

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
    weatherOverlay = LayerColor::create(Color4B(255, 255, 255, 0));

    // 1. 确保大小完全等于可见区域
    weatherOverlay->setContentSize(visibleSize);

    // 2. 【核心修复】将位置设置到可见区域的原点
    weatherOverlay->setPosition(origin);

    this->addChild(weatherOverlay, 0);

    // 先用英文测试
    float paddingX = 20.0f; // 距离右边缘的距离
    float topMargin = 30.0f; // 第一个标签距离顶部的距离
    float spacingY = 90.0f;  // 标签之间的垂直间距



    auto timeBg = Sprite::create("UI1.png");

    // 设置背景的锚点为右上角 (1.0, 1.0)
    // 这样直接设置坐标为屏幕右上角时，图片就会乖乖挂在那个角上
    timeBg->setAnchorPoint(Vec2(1.0f, 1.0f));

    // 设置背景在屏幕上的位置 (右上角)
    // paddingX 和 topMargin 是控制背景距离屏幕边缘的距离
    timeBg->setPosition(Vec2(origin.x + visibleSize.width - paddingX,
        origin.y + visibleSize.height - topMargin));

    // 将背景添加到场景中，Z轴设为1 (或其他层级)
    this->addChild(timeBg, 1);

    // --- 获取背景图片的尺寸，用于计算内部标签的位置 ---
    Size bgSize = timeBg->getContentSize();

    // 定义内部的边距 (文字距离背景图片边缘的距离)
    float innerPaddingX = 25.0f; // 文字距离图片右边的距离
    float innerTopMargin = 40.0f; // 第一行文字距离图片顶部的距离


    // 2. 日期标签 
    dateLabel = Label::createWithSystemFont("Year 1 Spring Day 1", "Arial", 17);
    dateLabel->setAnchorPoint(Vec2(1.0f, 0.5f));
    // 相对于上一个标签的位置向下移动
    dateLabel->setPosition(Vec2(bgSize.width - innerPaddingX, bgSize.height - innerTopMargin));
    dateLabel->setTextColor(Color4B::BLACK);
    timeBg->addChild(dateLabel);

    
    // 3. 时间标签 
    // 注意：现在是相对于 timeBg 定位，所以不再涉及 origin.x 或 visibleSize
    timeLabel = Label::createWithSystemFont("06:00", "Arial", 26);
    timeLabel->setAnchorPoint(Vec2(1.0f, 0.5f)); // 依然保持右对齐
    // 坐标 X: 图片宽度 - 内部边距
    // 坐标 Y: 图片高度 - 顶部边距
    timeLabel->setPosition(Vec2(bgSize.width - innerPaddingX - 45, dateLabel->getPositionY() - spacingY));
    timeLabel->setTextColor(Color4B::BLACK);
    // 【关键点】添加到 timeBg 上，而不是 this
    timeBg->addChild(timeLabel);

    moneyLabel = Label::createWithSystemFont("0", "Arial",45);
    moneyLabel->setAnchorPoint(Vec2(1.0f, 0.5f));
    moneyLabel->setPosition(Vec2(bgSize.width - innerPaddingX-4, timeLabel->getPositionY() - spacingY+10));
    moneyLabel->setColor(Color3B(160, 82, 45));
    timeBg->addChild(moneyLabel);
    updateMoneyDisplay();
}

void TimeDisplayScene::updateMoneyDisplay() {
    int money = MoneyManager::getInstance()->getMoney();
    moneyLabel->setString(std::to_string(money));
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

    updateMoneyDisplay();

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
        rain->setStartSize(1.5f);
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
