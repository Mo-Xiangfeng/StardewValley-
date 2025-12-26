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
float spacingY = 25.0f;  // 标签之间的垂直间距

// 1. 时间标签 (最上方)
timeLabel = Label::createWithSystemFont("06:00", "Arial", 10);
// 设置锚点为右侧中心，这样设置位置时，坐标点代表标签的右边缘中心
timeLabel->setAnchorPoint(Vec2(1.0f, 0.5f)); 
timeLabel->setPosition(Vec2(origin.x + visibleSize.width - paddingX,
                            origin.y + visibleSize.height - topMargin));
timeLabel->setTextColor(Color4B::BLACK);
this->addChild(timeLabel, 1);

// 2. 日期标签 (中间)
dateLabel = Label::createWithSystemFont("Year 1 Spring Day 1", "Arial", 10);
dateLabel->setAnchorPoint(Vec2(1.0f, 0.5f));
dateLabel->setPosition(Vec2(origin.x + visibleSize.width - paddingX,
                            timeLabel->getPositionY() - spacingY));
dateLabel->setTextColor(Color4B::BLACK);
this->addChild(dateLabel, 1);

// 3. 天气标签 (下方)
weatherLabel = Label::createWithSystemFont("Weather: Sunny", "Arial", 10);
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
    weatherLabel->setString(weatherText);

    auto tint = weatherMgr->getWeatherTint();
    weatherOverlay->setColor(Color3B(tint.r, tint.g, tint.b));
}

void TimeDisplayScene::onDayChanged(int year, int season, int day) {
    // 每天更新天气
    WeatherManager::getInstance()->updateWeather(season);
    CCLOG("新的一天! 年:%d 季节:%d 日:%d", year, season, day);
}
