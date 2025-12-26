#include "ShopLayer.h"
#include "MoneyManager.h"
#include "InventoryManager.h"
#include "TimeManager.h"

bool ShopLayer::init() {
    if (!Layer::init()) {
        return false;
    }

    TimeManager::getInstance()->pauseTime();

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    scrollOffset = 0;
    minScrollOffset = 0;

    // 半透明黑色背景
    auto bg = LayerColor::create(Color4B(0, 0, 0, 150));
    this->addChild(bg);

    //  商店面板
    float panelWidth = visibleSize.width * 0.5f;
    float panelHeight = visibleSize.height * 0.6f;

    auto shopPanel = LayerColor::create(
        Color4B(101, 67, 33, 255),
        panelWidth,
        panelHeight
    );
    shopPanel->setPosition(Vec2(
        origin.x + visibleSize.width / 2 - panelWidth / 2,
        origin.y + visibleSize.height / 2 - panelHeight / 2
    ));
    this->addChild(shopPanel);

    //  标题
    float titleFontSize = panelWidth * 0.06f;
    auto titleLabel = Label::createWithSystemFont("=== SEED SHOP ===", "Arial", titleFontSize);
    titleLabel->setPosition(Vec2(
        origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height / 2 + panelHeight * 0.4f
    ));
    titleLabel->setColor(Color3B(255, 215, 0));
    this->addChild(titleLabel);

    //  金钱显示
    float moneyFontSize = panelWidth * 0.045f;
    moneyLabel = Label::createWithSystemFont("Money: $0", "Arial", moneyFontSize);
    moneyLabel->setPosition(Vec2(
        origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height / 2 + panelHeight * 0.3f
    ));
    moneyLabel->setColor(Color3B(255, 255, 100));
    this->addChild(moneyLabel);
    updateMoneyDisplay();

    //  滚动提示
    float hintFontSize = panelWidth * 0.03f;
    auto scrollHint = Label::createWithSystemFont("[Scroll to view more]", "Arial", hintFontSize);
    scrollHint->setPosition(Vec2(
        origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height / 2 + panelHeight * 0.22f
    ));
    scrollHint->setColor(Color3B(180, 180, 180));
    this->addChild(scrollHint);

    //  关闭按钮
    float closeFontSize = panelWidth * 0.045f;
    auto closeLabel = Label::createWithSystemFont("[X] CLOSE", "Arial", closeFontSize);
    closeLabel->setColor(Color3B(255, 100, 100));
    auto closeButton = MenuItemLabel::create(closeLabel, [this](Ref* sender) {
        TimeManager::getInstance()->resumeTime();
        this->removeFromParent();
        });
    closeButton->setPosition(Vec2(
        origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height / 2 - panelHeight * 0.42f
    ));

    auto menu = Menu::create(closeButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 100);

    // ============================================
    //  创建裁剪节点（关键修复！）
    // ============================================
    float clipWidth = panelWidth * 0.9f;
    float clipHeight = panelHeight * 0.5f;  // 可视区域高度

    // 创建裁剪模板（矩形区域）
    auto stencil = DrawNode::create();
    Vec2 clipOrigin = Vec2(
        origin.x + visibleSize.width / 2 - clipWidth / 2,
        origin.y + visibleSize.height / 2 - clipHeight / 2
    );
    Vec2 clipRect[] = {
        clipOrigin,
        Vec2(clipOrigin.x + clipWidth, clipOrigin.y),
        Vec2(clipOrigin.x + clipWidth, clipOrigin.y + clipHeight),
        Vec2(clipOrigin.x, clipOrigin.y + clipHeight)
    };
    stencil->drawSolidPoly(clipRect, 4, Color4F(1, 1, 1, 1));

    // 创建裁剪节点
    auto clippingNode = ClippingNode::create(stencil);
    clippingNode->setAlphaThreshold(0.05f);
    this->addChild(clippingNode, 1);

    //  滚动容器添加到裁剪节点内
    scrollContainer = Layer::create();
    clippingNode->addChild(scrollContainer);

    //  物品列表
    float startY = origin.y + visibleSize.height / 2 + panelHeight * 0.15f;
    float itemSpacing = panelHeight * 0.12f;

    createShopItem(1100/*PARSNIP*/, Vec2(origin.x + visibleSize.width / 2, startY));
    createShopItem(1101/*POTATO*/, Vec2(origin.x + visibleSize.width / 2, startY - itemSpacing));
    createShopItem(1102/*CAULIFLOWER*/, Vec2(origin.x + visibleSize.width / 2, startY - itemSpacing * 2));
    createShopItem(1103, Vec2(origin.x + visibleSize.width / 2, startY - itemSpacing * 3));
    createShopItem(1104, Vec2(origin.x + visibleSize.width / 2, startY - itemSpacing * 4));
    createShopItem(1105, Vec2(origin.x + visibleSize.width / 2, startY - itemSpacing * 5));
    createShopItem(1106, Vec2(origin.x + visibleSize.width / 2, startY - itemSpacing * 6));

    //  计算滚动范围
    maxScrollOffset = itemSpacing * 8 - clipHeight;
    if (maxScrollOffset < 0) maxScrollOffset = 0;

    // ============================================
    //  鼠标滚轮监听
    // ============================================
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseScroll = [this](Event* event) {
        EventMouse* mouseEvent = static_cast<EventMouse*>(event);
        float scrollY = mouseEvent->getScrollY();

        scrollOffset += scrollY * 30;

        if (scrollOffset < minScrollOffset) scrollOffset = minScrollOffset;
        if (scrollOffset > maxScrollOffset) scrollOffset = maxScrollOffset;

        scrollContainer->setPositionY(scrollOffset);
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    // ============================================
    //  触摸滚动支持
    // ============================================
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(false);

    static Vec2 lastTouchPos;

    touchListener->onTouchBegan = [](Touch* touch, Event* event) {
        lastTouchPos = touch->getLocation();
        return true;
        };

    touchListener->onTouchMoved = [this](Touch* touch, Event* event) {
        Vec2 currentPos = touch->getLocation();
        float deltaY = currentPos.y - lastTouchPos.y;

        scrollOffset += deltaY;

        if (scrollOffset < minScrollOffset) scrollOffset = minScrollOffset;
        if (scrollOffset > maxScrollOffset) scrollOffset = maxScrollOffset;

        scrollContainer->setPositionY(scrollOffset);
        lastTouchPos = currentPos;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    return true;
}

ShopLayer::~ShopLayer() {
    if (TimeManager::getInstance()->isTimePaused()) {
        TimeManager::getInstance()->resumeTime();
    }
}


void ShopLayer::createShopItem(int cropType, Vec2 position) {
   const CropInfo& info = CropDatabase::getInstance()->getCropInfo(cropType);

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 物品尺寸
    float itemWidth = visibleSize.width * 0.5f * 0.85f;
    float itemHeight = visibleSize.height * 0.08f;  // 高度也自适应

    auto itemBg = LayerColor::create(Color4B(80, 50, 20, 255), itemWidth, itemHeight);
    itemBg->setPosition(position - Vec2(itemWidth / 2, itemHeight / 2));
    scrollContainer->addChild(itemBg);

    // 图标字体 = 物品高度的 45%
    float iconFontSize = itemHeight * 0.45f;
    std::string icon;
    switch (cropType) {
        case 1100: icon = "[P]"; break;
        case 1101/*POTATO*/: icon = "[PT]"; break;
        case 1102/*CAULIFLOWER*/: icon = "[C]"; break;
        case 1103/*TOMATO*/: icon = "[T]"; break;
        case 1104/*CORN*/: icon = "[CR]"; break;
        case 1105/*PUMPKIN*/: icon = "[PK]"; break;
        case 1106/*WHEAT*/: icon = "[W]"; break;
        default: icon = "[?]"; break;
    }

    auto seedIcon = Label::createWithSystemFont(icon, "Arial", iconFontSize);
    seedIcon->setPosition(position + Vec2(-itemWidth * 0.4f, 0));
    seedIcon->setColor(Color3B(100, 255, 100));
    scrollContainer->addChild(seedIcon);

    // 名称字体 = 物品高度的 33%
    float nameFontSize = itemHeight * 0.33f;
    std::string nameText = info.name + " (" + std::to_string(info.growthDays) + "d)";
    if (info.canRegrow) {
        nameText += " [R]";
    }
    auto nameLabel = Label::createWithSystemFont(nameText, "Arial", nameFontSize);
    nameLabel->setAnchorPoint(Vec2(0, 0.5));
    nameLabel->setPosition(position + Vec2(-itemWidth * 0.3f, itemHeight * 0.15f));
    nameLabel->setColor(Color3B::WHITE);
    scrollContainer->addChild(nameLabel);

    // 价格字体 = 物品高度的 27%
    float priceFontSize = itemHeight * 0.27f;
    int profit = info.sellPrice - info.seedPrice;
    std::string priceText = "$" + std::to_string(info.seedPrice) +
        " | +$" + std::to_string(profit);
    auto priceLabel = Label::createWithSystemFont(priceText, "Arial", priceFontSize);
    priceLabel->setAnchorPoint(Vec2(0, 0.5));
    priceLabel->setPosition(position + Vec2(-itemWidth * 0.3f, -itemHeight * 0.15f));

    if (profit > 0) {
        priceLabel->setColor(Color3B(100, 255, 100));
    }
    else {
        priceLabel->setColor(Color3B(255, 100, 100));
    }
    scrollContainer->addChild(priceLabel);

    // 购买按钮字体 = 物品高度的 33%
    float buyFontSize = itemHeight * 0.33f;
    auto buyLabel = Label::createWithSystemFont("[ BUY ]", "Arial", buyFontSize);
    buyLabel->setColor(Color3B(150, 255, 150));
    auto buyButton = MenuItemLabel::create(buyLabel, [this, cropType](Ref* sender) {
        onBuyButtonClicked(cropType);
        });
    buyButton->setPosition(position + Vec2(itemWidth * 0.35f, 0));

    auto menu = Menu::create(buyButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    scrollContainer->addChild(menu);
}

void ShopLayer::onBuyButtonClicked(int cropType) {
    // 1. 获取作物数据库中的配置信息
    const CropInfo& info = CropDatabase::getInstance()->getCropInfo(cropType);

    // 2. 检查玩家金钱是否足够
    if (!MoneyManager::getInstance()->hasMoney(info.seedPrice)) {
        CCLOG("Not enough money to buy %s seeds!", info.name.c_str());

        // 显示余额不足提示
        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto hint = Label::createWithSystemFont("[$] Not enough money!", "Arial", 30);
        hint->setPosition(visibleSize / 2);
        hint->setColor(Color3B::RED);
        this->addChild(hint, 100);

        hint->runAction(Sequence::create(
            Spawn::create(ScaleTo::create(0.2f, 1.2f), FadeIn::create(0.2f), nullptr),
            DelayTime::create(1.2f),
            Spawn::create(ScaleTo::create(0.2f, 0.8f), FadeOut::create(0.2f), nullptr),
            RemoveSelf::create(),
            nullptr
        ));
        return;
    }

    // 3. 扣款并尝试将物品放入背包
    if (MoneyManager::getInstance()->spendMoney(info.seedPrice)) {

        // --- 核心逻辑：直接构造 Item 指针 ---
        // 这里利用 CropInfo 的数据填充 Item 属性
        auto seedItem = std::make_shared<Item>();

        seedItem->id = cropType; // 直接用枚举值作为临时ID
        seedItem->name = info.name;
        seedItem->iconPath = "item/" + std::to_string(cropType) + ".png"; // 假设你的资源路径规则
        seedItem->type = ItemType::SEED;
        seedItem->count = 1;
        seedItem->maxStack = 99;
        seedItem->description = "种在土里可以长出" + info.name;

        // 4. 调用 InventoryManager 的 addItem 函数
        if (InventoryManager::getInstance()->addItem(seedItem)) {
            // 购买成功
            updateMoneyDisplay();
            CCLOG("Bought 1 %s seed successfully", info.name.c_str());

            // 显示成功提示
            auto hint = Label::createWithSystemFont("[OK] Bought 1 " + info.name + " seed!", "Arial", 28);
            hint->setPosition(Director::getInstance()->getVisibleSize() / 2);
            hint->setColor(Color3B(100, 255, 100));
            this->addChild(hint, 100);

            hint->runAction(Sequence::create(
                Spawn::create(ScaleTo::create(0.2f, 1.1f), FadeIn::create(0.2f), nullptr),
                DelayTime::create(1.2f),
                Spawn::create(ScaleTo::create(0.2f, 0.9f), FadeOut::create(0.2f), nullptr),
                RemoveSelf::create(),
                nullptr
            ));
        }
        else {
            // 背包已满，退还金钱
            MoneyManager::getInstance()->addMoney(info.seedPrice);

            auto hint = Label::createWithSystemFont("Inventory Full!", "Arial", 30);
            hint->setPosition(Director::getInstance()->getVisibleSize() / 2);
            hint->setColor(Color3B::ORANGE);
            this->addChild(hint, 100);
            hint->runAction(Sequence::create(DelayTime::create(1.0f), RemoveSelf::create(), nullptr));

            //CCLOG("Purchase failed: Inventory Full. Refunded $%d", info.seedPrice);
        }
    }
}       
void ShopLayer::updateMoneyDisplay() {
    int money = MoneyManager::getInstance()->getMoney();
    moneyLabel->setString("Money: $" + std::to_string(money));
}

ShopLayer* ShopLayer::create() {
    ShopLayer* ret = new ShopLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void ShopLayer::refreshShop() {
    updateMoneyDisplay();
}
