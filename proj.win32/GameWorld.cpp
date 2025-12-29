#include "GameWorld.h"
#include "Player.h"
#include "MainMapLogic.h"
#include "HomeLogic.h"
#include "ShopLogic.h"
#include "NPC1HouseLogic.h"
#include "NPC2HouseLogic.h"
#include "MineLogic.h"
#include "GameScene.h"
#include "CropData.h"
#include "InventoryManager.h"
#include "WeatherManager.h"
#include "fishing_game.h"`
#include "TreeManager.h"
#include "RockManager.h"
USING_NS_CC;

/* =========================
   创建 & 初始化
   ========================= */

GameWorld* GameWorld::create(const std::string& txtFile,
    const std::string& imgFile)
{
    GameWorld* pRet = new (std::nothrow) GameWorld();
    if (pRet && pRet->init(txtFile, imgFile))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool GameWorld::init(const std::string&,
    const std::string&)
{
    if (!Node::init()) return false;
    /* ========= 注册所有地图 ========= */

    // ================= 主地图 =================
    _maps["Map"] = {
        "tilemap.txt",
        "Map.png",
        {
            { "Spawn",    {13, 45} },
            { "HomeDoor", {32, 25} },
            { "ShopDoor", {53, 27} },
            { "NPC1Door", {20, 33} },
            { "NPC2Door", {8,  30} },
            { "MineDoor", {13, 45} }
        },
        {
            { {32, 25}, "Home",      "Exit" },
            { {53, 27}, "Shop",      "Exit" },
            { {20, 33}, "NPC1House", "Exit" },
            { {8,  30}, "NPC2House", "Exit" },
            { {13, 45}, "Mine",      "Exit" }
        }
    };

    // ================= 主角家 =================
    _maps["Home"] = {
        "Home.txt",
        "Home.png",
        {
            { "Exit", {13, 0} }
        },
        {
            { {13, 0}, "Map", "HomeDoor" }
        }
    };

    // ================= 商店 =================
    _maps["Shop"] = {
        "Shop.txt",
        "Shop.png",
        {
            { "Exit", {16, 4} }
        },
        {
            { {16, 4}, "Map", "ShopDoor" }
        }
    };

    // ================= NPC1 家 =================
    _maps["NPC1House"] = {
        "NPC1House.txt",
        "NPC1House.png",
        {
            { "Exit", {13, 0} }
        },
        {
            { {13, 0}, "Map", "NPC1Door" }
        }
    };

    // ================= NPC2 家 =================
    _maps["NPC2House"] = {
        "NPC2House.txt",
        "NPC2House.png",
        {
            { "Exit", {13, 0} }
        },
        {
            { {13, 0}, "Map", "NPC2Door" }
        }
    };

    // ================= 矿洞 =================
    _maps["Mine"] = {
        "Mine.txt",
        "Mine.png",
        {
            { "Exit", {36, 42} }
        },
        {
            { {36, 42}, "Map", "MineDoor" }
        }
    };


   
    switchMap("Map", "Spawn");
    CCLOG("DEBUG: Map Loaded. Width: %d, Height: %d", _map.width, _map.height);
    _daylightLayer = Daylight::create();
    this->addChild(_daylightLayer, 999); // 确保在最顶层
    _daylightLayer->setVisible(false);

    // 地图加载完成后
    _treeLayer = Node::create();
    this->addChild(_treeLayer, 70);
    _rockLayer = Node::create();
    this->addChild(_rockLayer, 71); // 稍微高于树木或保持一致
    _rockLayer->setVisible(false);
    scheduleUpdate();
    TreeManager::getInstance()->init(this);
    
    return true;
}

void GameWorld::handleInteraction(const Vec2& posInMap)
{
    if (!_player) return;

    // 1. 获取物理属性
    float baseTileSize = getTileWidth();
    float mapScale = getMapScale();
    float actualTileSize = baseTileSize * mapScale;

    // 2. 计算 X 索引 (通常不需要反转)
    int tx = std::floor(posInMap.x / actualTileSize);

    // 3. 【核心修复】计算 Y 索引：必须用地图高度减去点击位置
    // _map.height - 1 是最大行索引。这样可以将左下角原点转换为左上角原点。
    int ty = (_map.height - 1) - std::floor(posInMap.y / actualTileSize);

    // 4. 越界检查
    if (!_map.inBounds(tx, ty)) return;

    // 5. 获取图块 ID 并判断
    int tileId = _map.getTile(tx, ty);

    if (tileId == 11) // Businessman = 11
    {
        // 6. 【核心修复】重算中心点坐标用于距离判定（Y 也要翻转回去）
        float centerX = (tx + 0.5f) * actualTileSize;
        float centerY = (_map.height - 1 - ty + 0.5f) * actualTileSize;
        Vec2 merchantCenter = Vec2(centerX, centerY);

        // 7. 距离判定
        float playerDist = _player->getPosition().distance(merchantCenter);

        if (playerDist < actualTileSize * 2.5f)
        {
            if (_logic) {
                _logic->onInteract(_player);
            }
        }
        else {
            CCLOG("太远了，走近点再点商人");
        }
    }
}
// GameWorld.cpp
void GameWorld::addTreeSprite(int tx, int ty) {
    auto tree = Sprite::create("tree.png");
    tree->setAnchorPoint(Vec2(0.5f, 0.2f));

    

    const float TILE_SIZE = 16.0f;
    const float MAP_SCALE = 2.7f;

    // 计算显示位置（不要覆盖 tx, ty 变量）
    float posX = (tx * TILE_SIZE + TILE_SIZE * 0.5f) * MAP_SCALE;
    float posY = (ty * TILE_SIZE) * MAP_SCALE;
    tree->setPosition(posX, posY);

    tree->setScale(1.1f);

    // 使用原始的 tx, ty 生成 Tag
    int tag = 200000 + tx * 1000 + ty;
    tree->setTag(tag);

    _treeLayer->addChild(tree);
}

void GameWorld::removeTreeSprite(const cocos2d::Vec2& tilePos) {
    int tx = (int)tilePos.x;
    int ty = (int)tilePos.y;
    int tag = 200000 + tx * 1000 + ty;

    // 关键修正：去 _treeLayer 里找，而不是 this
    if (_treeLayer) {
        auto tree = _treeLayer->getChildByTag(tag);
        if (tree) {
            tree->removeFromParent(); // 或者 _treeLayer->removeChildByTag(tag);
            //CCLOG("Tree Sprite at %d, %d removed from _treeLayer", tx, ty);
        }
    }
}
void GameWorld::addRockSprite(int tx, int ty) {
    CCLOG("5555555555555555");
    auto rock = Sprite::create("RRock.png"); 
    float ts = 16.0f * _mapScale;
    rock->setPosition(Vec2((tx + 0.5f) * ts, (ty + 0.5f) * ts));
    // 使用 300,000 级别的 Tag 避免冲突
    int tag = 300000 + tx * 1000 + ty;
	cocos2d::log("Adding rock sprite with tag: %d", tag);
    rock->setTag(tag);

    // 添加到专门的图层
    if (_rockLayer) {
        _rockLayer->addChild(rock);
    }
}

void GameWorld::removeRockSprite(const Vec2& tilePos) {
    int tx = (int)tilePos.x;
    int ty = (int)tilePos.y;
    int tag = 300000 + tx * 1000 + ty;
	cocos2d::log("Removing rock sprite with tag: %d", tag);
    if (_rockLayer) {
        cocos2d::log("Current _rockLayer pointer: %p", _rockLayer);
        cocos2d::log("_rockLayer child count: %d", (int)_rockLayer->getChildrenCount());
        _rockLayer->removeChildByTag(tag);
    }
}

/* =========================
   Player 绑定
   ========================= */

void GameWorld::setPlayer(Player* player) {
    _player = player;
    if (_player) {
        // 让人物的显示大小跟随地图的缩放比例
		auto _playerScale = _player->getPlayerScale();
        _player->setScale(_playerScale);
    }
}

/* =========================
   地图加载
   ========================= */

void GameWorld::reload(const std::string& txtFile,
    const std::string& imgFile)
{
    if (_mapSprite)
    {
        removeChild(_mapSprite);
        _mapSprite = nullptr;
    }

    _map.load(txtFile, 16, 16);

    _mapSprite = Sprite::create(imgFile);
    _mapSprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    _mapSprite->setScale(_mapScale);
    addChild(_mapSprite, 0);

    _mapSizeScaled = _mapSprite->getContentSize() * _mapScale;

    _portals.clear();

    float ts = _map.tileWidth * _mapScale;

    const auto& info = _maps.at(_currentMapId);

    for (const auto& p : info.portals)
    {
        _portals.emplace_back(
            Rect(p.tile.x * ts, p.tile.y * ts, ts, ts),
            _currentMapId,
            p.toMap,
            p.toEntry
        );
    }
    
    drawFarmGrid();
   
}

/* =========================
   切换地图（核心）
   ========================= */

void GameWorld::switchMap(const std::string& mapId,
    const std::string& entry)
{
    // 离开旧地图逻辑
    if (_logic)
        _logic->onExit(this, _player);

    const auto& info = _maps.at(mapId);
    _currentMapId = mapId;

    reload(info.txt, info.img);
    if (_rockLayer) {
        if (mapId == "Mine") {
            _rockLayer->setVisible(true);
            _rockLayer->removeAllChildren(); // 关键：进入矿洞前清空旧的“红石头”视觉
            RockManager::getInstance()->init(this);
            RockManager::getInstance()->refreshRocks();
        }
        else {
            _rockLayer->setVisible(false);
        }
    }
    if (_treeLayer) {
        if (_currentMapId == "Map") {
            _treeLayer->setVisible(true);   // 回到主地图，显示树木
        }
        else {
            _treeLayer->setVisible(false);  // 进入室内/矿洞，隐藏树木
        }
    }
    if (_player)
    {
        Vec2 spawn = info.entries.at(entry);
        _player->setTilePosition(spawn.x, spawn.y);
    }
    if (mapId == "Mine") {
        // 1. 第一次进入时创建 rockLayer
        if (_rockLayer == nullptr) {
            _rockLayer = Node::create();
            // 70 是为了确保它在瓦片图层之上，且可以与玩家/树木进行 ZOrder 排序
            this->addChild(_rockLayer, 70);
            CCLOG("RockLayer created for the first time.");
        }

        // 2. 确保图层是可见的
        _rockLayer->setVisible(true);
    }
    else {
        // 如果离开了矿洞，建议隐藏 rockLayer 以节省渲染性能
        if (_rockLayer) {
            _rockLayer->setVisible(false);
        }
    }
    // 创建对应地图逻辑
    if (mapId == "Map")
        _logic = std::make_unique<MainMapLogic>();
    else if (mapId == "Home")
        _logic = std::make_unique<HomeLogic>();
    else if (mapId == "Shop")
        _logic = std::make_unique<ShopLogic>();
    else if (mapId == "NPC1House")
        _logic = std::make_unique<NPC1HouseLogic>();
    else if (mapId == "NPC2House")
        _logic = std::make_unique<NPC2HouseLogic>();
    else if (mapId == "Mine")
        _logic = std::make_unique<MineLogic>();
    if (_daylightLayer) {
        if (mapId != "Mine") {
            _daylightLayer->setVisible(true);  // 回到主地图，开启黑夜
        }
        else {
            _daylightLayer->setVisible(false); // 进入室内，强行变亮
        }
    }
    // 进入新地图逻辑
    if (_logic)
        _logic->onEnter(this, _player);

    _currentPortal = nullptr;
    _portalStayTime = 0.0f;
    updateLandVisuals();
}

/* =========================
   交互
   ========================= */

void GameWorld::onInteract() {
    if (_activePortal) {
        // 触发带黑屏过渡的切换
        fadeSwitch(_activePortal->toMap, _activePortal->toEntry);
    }
}
void GameWorld::fadeSwitch(const std::string& mapId, const std::string& entry) {
    auto mask = LayerColor::create(Color4B::BLACK);
    mask->setOpacity(0);
    Director::getInstance()->getRunningScene()->addChild(mask, 999);

    mask->runAction(Sequence::create(
        FadeIn::create(0.2f),
        CallFunc::create([=]() { this->switchMap(mapId, entry); }),
        FadeOut::create(0.2f),
        RemoveSelf::create(),
        nullptr
    ));
}

/* =========================
   更新（Portal + Logic）
   ========================= */

void GameWorld::update(float dt) {
    if (!_player) return;
    Vec2 pos = _player->getPosition();
    _activePortal = nullptr; // 每帧重置

    for (auto& p : _portals) {
        if (p.triggerArea.containsPoint(pos)) {
            _activePortal = &p; // 发现玩家站在门上
            break;
        }
    }
    if (_logic) _logic->update(dt);
}

/* =========================
   碰撞判定
   ========================= */
int GameWorld::debugGetTile(int tx, int ty)
{
    return _map.getTile(tx, ty);
}

/*bool GameWorld::isWalkable(const Vec2& worldPos)
{
    float ts = _map.tileWidth * _mapScale;

    int tx = worldPos.x / ts;
    int ty = worldPos.y / ts;

    int tileId = _map.getTile(tx, ty);

    // Debug（现在你会看到正常的 tileId 了）
    //CCLOG("tx=%d ty=%d tileId=%d", tx, ty, tileId);

    if (tileId < 0) return false;
    return _map.isWalkable(tileId);
}*/

bool GameWorld::isWalkable(const cocos2d::Vec2& posInMap)
{
    float ts = _map.tileWidth * _mapScale;

    // 使用 floor 确保负数也能正确转换，并加上一个微小的偏移防止边界误差
    int tx = std::floor((posInMap.x + 0.1f) / ts);
    int ty = std::floor((posInMap.y + 0.1f) / ts);

    // 打印地图实际宽高，看看是不是 txt 没读全
    // CCLOG("Map Size: %d x %d", _map.width, _map.height);

    int tileId = _map.getTile(tx, ty);

    // 如果是 -1，说明 tx/ty 超过了 _map.width/height
    if (tileId < 0) {
        // 临时修改：如果是边缘越界，先允许通行，看看人能不能出来
        // return true; 
        return false;
    }

    return _map.isWalkable(tileId);
}


void GameWorld::updateCamera()
{
    if (!_player) return;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto mapSize = _mapSizeScaled;
    auto playerPos = _player->getPosition();

    // -------------------
    // 目标世界位置（平滑跟随）
    // -------------------
    Vec2 screenCenter(visibleSize.width / 2, visibleSize.height / 2);
    Vec2 targetPos = screenCenter - playerPos;
    Vec2 currentPos = this->getPosition();
    Vec2 newWorldPos = currentPos + (targetPos - currentPos) * 0.1f; // 缓动

    // -------------------
    // 边界限制 & 小地图居中
    // -------------------
    float worldX, worldY;

    // 横向
    if (mapSize.width < visibleSize.width)
        worldX = (visibleSize.width - mapSize.width) / 2.0f; // 居中
    else {
        float minX = visibleSize.width - mapSize.width;
        worldX = clampf(newWorldPos.x, minX, 0);
    }

    // 纵向
    if (mapSize.height < visibleSize.height)
        worldY = (visibleSize.height - mapSize.height) / 2.0f; // 居中
    else {
        float minY = visibleSize.height - mapSize.height;
        worldY = clampf(newWorldPos.y, minY, 0);
    }

    // -------------------
    // 应用位置
    // -------------------
    this->setPosition(Vec2(worldX, worldY));
}

void GameWorld::drawFarmGrid()
{
    // 1. 查找或创建 DrawNode
    auto drawNode = dynamic_cast<DrawNode*>(this->getChildByTag(999));
    if (!drawNode) {
        drawNode = DrawNode::create();
        this->addChild(drawNode, 100, 999); // 将 ZOrder 调高到 100，确保在最顶层
    }
    else {
        drawNode->clear();
    }

    // 2. 检查缩放倍数 (如果这个值是 0，线条会重叠在原点看不见)
    float scale = getMapScale();
    if (scale <= 0.0f) {
        CCLOG("Warning: Map scale is 0, grid might not show!");
        scale = 1.0f; // 给个默认值防止消失
    }
    float ts = 16.0f * scale;


    Color4F lineColor(0.42f, 0.24f, 0.16f, 0.20f);


    int count = 0; // 用于调试计数

    // 4. 遍历
    for (int ty = 0; ty < _map.height; ++ty)
    {
        for (int tx = 0; tx < _map.width; ++tx)
        {
            if (_map.getTile(tx, ty) == 8)
            {
                //cocos2d::log("Paint!!!!!!!");
                Vec2 bottomLeft = Vec2(tx * ts, ty * ts);
                Vec2 topRight = Vec2((tx + 1) * ts, (ty + 1) * ts);

                drawNode->drawRect(bottomLeft, topRight, lineColor);
                count++;

            }
        }
    }

    //CCLOG("DrawGrid finished. Total farm tiles drawn: %d", count);
}

void GameWorld::interactWithLand(int tx, int ty, int itemID) {
    // 1. 判定地图ID：只有 ID 为 8 的格子才能交互
    if (_map.getTile(tx, ty) != 8) {
        return;
    }

    std::string key = getLandKey(tx, ty);
    LandTileData& land = _farmlandData[key];

    // 2. 根据 itemID 判断行为
    if (itemID == 1112) { // 假设 1112 是锄头
        if (land.state == LandState::NONE) {
            land.state = LandState::TILLED;
        }
    }
    else if (itemID == 1113) { // 假设 1113 是水壶
        if (land.state == LandState::TILLED) {
            land.state = LandState::WATERED;
        }
    }
    else {
        // 3. 播种判断：从你的 CropDatabase 查找 itemID 是否为种子
        auto cropInfo = CropDatabase::getInstance()->getCrop(itemID);
        if (cropInfo && land.state != LandState::NONE && land.cropId == -1) {
            // 只有耕过且没种过东西的地才能播种
            land.cropId = itemID;
            land.currentGrowth = 0;
            land.isHarvestable = false;

            // 播种后，需要消耗背包里的种子数量 (假设你有这个方法)
            InventoryManager::getInstance()->removeItemByID(itemID, 1);
        }

        // 4. 收获判断
        if (land.isHarvestable) {
            // 技巧：假设种子 ID 是 1100，对应的果实 ID 是 2200 (你的库里定义的)
            int fruitId = land.cropId + 1100;

            // 2. 给予玩家物品
            bool success = InventoryManager::getInstance()->addItemByID(fruitId, 1);

            if (success) {
                CCLOG("收获成功！获得了果实 ID: %d", fruitId);
                // 3. 重置土地
                land.cropId = -1;
                land.isHarvestable = false;
                land.state = LandState::TILLED;
            }
            else {
                CCLOG("背包已满，无法收获！");
            }
        }
    }

    updateLandVisuals();
}

void GameWorld::updateLandVisuals() {
    auto drawNode = dynamic_cast<DrawNode*>(this->getChildByTag(888));
    if (!drawNode) {
        drawNode = DrawNode::create();
        this->addChild(drawNode, 1, 888);
    }
    drawNode->clear();

    float ts = 16.0f * getMapScale();

    for (auto it = _farmlandData.begin(); it != _farmlandData.end(); ++it) {
        const std::string& key = it->first;
        const LandTileData& land = it->second;

        // 解析坐标
        size_t sep = key.find('_');
        int tx = std::atoi(key.substr(0, sep).c_str());
        int ty = std::atoi(key.substr(sep + 1).c_str());

        Vec2 origin(tx * ts, ty * ts);
        Vec2 dest((tx + 1) * ts, (ty + 1) * ts);

        // --- 绘制土地颜色 ---
        Color4F landColor;
        if (land.state == LandState::TILLED) landColor = Color4F(0.4f, 0.25f, 0.15f, 0.7f);
        else if (land.state == LandState::WATERED) landColor = Color4F(0.2f, 0.15f, 0.1f, 0.9f);
        else continue;

        drawNode->drawSolidRect(origin, dest, landColor);

        // --- 绘制作物标记 ---
        if (land.cropId != -1) {
            Vec2 center((tx + 0.5f) * ts, (ty + 0.5f) * ts);
            if (land.isHarvestable) {
                // 成熟了画个金色的圆 (代表可收获)
                drawNode->drawDot(center, ts * 0.4f, Color4F::YELLOW);
            }
            else {
                // 生长中画个绿色的点，点的大小随生长进度变大
                auto info = CropDatabase::getInstance()->getCrop(land.cropId);
                float progress = (info && info->growthDays > 0) ? (float)land.currentGrowth / info->growthDays : 0.1f;
                drawNode->drawDot(center, ts * (0.1f + progress * 0.2f), Color4F::GREEN);
            }
        }
    }
}

bool GameWorld::isFarmable(int tx, int ty) {
    // 1. 首先必须在地图边界内
    if (!_map.inBounds(tx, ty)) return false;

    int tileId = _map.getTile(tx, ty);
    return (tileId == 8); // 假设 8 号图块是可耕种的泥土

    return false;
}

void GameWorld::nextDay() {
    CCLOG("DEBUG: nextDay() has been triggered!");

    // 1. 获取当前天气
    auto weather = WeatherManager::getInstance()->getCurrentWeather();
    bool isRaining = (weather == WeatherType::RAINY || weather == WeatherType::STORMY);

    if (isRaining) {
        CCLOG("It's raining! All tilled land will be watered automatically.");
    }

    for (auto it = _farmlandData.begin(); it != _farmlandData.end(); ++it) {
        LandTileData& land = it->second;

        // 【新增逻辑】：如果是雨天/雷暴，且土地是已耕种状态，自动变湿润
        if (isRaining && land.state == LandState::TILLED) {
            land.state = LandState::WATERED;
        }

        // 2. 成长判定：只有浇了水的土地（包括被雨淋湿的），作物才会生长
        if (land.cropId != -1 && !land.isHarvestable) {
            if (land.state == LandState::WATERED) {
                land.currentGrowth++;

                auto info = CropDatabase::getInstance()->getCrop(land.cropId);
                if (info && land.currentGrowth >= info->growthDays) {
                    land.isHarvestable = true;
                }
            }
        }

        // 3. 每日结算：湿润的土地变干
        if (land.state == LandState::WATERED) {
            land.state = LandState::TILLED;
        }
        else if (land.state == LandState::TILLED) {
            // 如果没播种，第二天变回普通地块
            if (land.cropId == -1) {
                land.state = LandState::NONE;
            }
        }
    }

    updateLandVisuals();
    this->drawFarmGrid();
}

void GameWorld::startFishingMinigame() {
    if (!_player) return;

    // 1. 判定玩家面前是否为水 (复用之前的逻辑)
    float ts = 16.0f * _mapScale;
    Vec2 pos = _player->getPosition();
    int tx = std::floor(pos.x / ts);
    int ty = std::floor(pos.y / ts);

    if (_player->direction == 0) ty--;
    else if (_player->direction == 3) ty++;
    else if (_player->direction == 1) tx--;
    else if (_player->direction == 2) tx++;

    if (!isWater(tx, ty)) return; // 如果面前不是水，直接返回

    // 2. 锁定玩家，停止移动
    _player->stop_move(0, 0);
    _player->_isAction = true;

    // 3. 创建小游戏
    auto fishingNode = FishingGame::create();

    // 关键修正：将小游戏添加到 Scene 层，而不是 GameWorld 层
    // 这样 UI 就会根据屏幕分辨率居中，而不是跟着地图跑
    auto currentScene = Director::getInstance()->getRunningScene();
    if (currentScene) {
        currentScene->addChild(fishingNode, 9999);
    }

    // 4. 处理回调
    fishingNode->onGameOver = [this, fishingNode](bool success) {
        if (success) {
            CCLOG("Catch!");
            // 这里可以添加奖励逻辑
        }

        // 解锁玩家
        if (this->_player) {
            this->_player->_isAction = false;
        }

        // 移除小游戏
        fishingNode->removeFromParent();
        };
}
bool GameWorld::isWater(int tx, int ty) {
    if (!_map.inBounds(tx, ty)) return false;

    int tileId = _map.getTile(tx, ty);
    // 根据 TileType.h，Water 的 ID 是 2
    return (tileId == 2);
}
void GameWorld::playTreeHitEffect(int tx, int ty)
{
    int tag = 200000 + tx * 1000 + ty;

    // 从 _treeLayer 中获取树的精灵
    auto tree = _treeLayer->getChildByTag(tag);

    if (tree) {
        // 先停止树木当前正在运行的动作，防止连续点击导致动作叠加变不回来
        tree->stopAllActionsByTag(999); // 假设给受击动作设个标签

        // 1. 定义变暗动作 (120, 120, 120 是深灰色)
        auto tintDark = TintTo::create(0.05f, 120, 120, 120);

        // 2. 定义恢复原色动作 (255, 255, 255 是图片原始色彩)
        auto tintBack = TintTo::create(0.05f, 255, 255, 255);

        // 3. 组合动作序列：变暗 -> 恢复
        auto hitSequence = Sequence::create(tintDark, tintBack, nullptr);

        // 给动作设置一个 tag，方便上面 stopAllActionsByTag 调用
        hitSequence->setTag(999);

        tree->runAction(hitSequence);
    }
}
void GameWorld::playRockHitEffect(int tx, int ty)
{
    int tag = 300000 + tx * 1000 + ty;

    // 从 _treeLayer 中获取树的精灵
    auto rock = _rockLayer->getChildByTag(tag);

    if (rock) {
       
        rock->stopAllActionsByTag(999); // 假设给受击动作设个标签

        // 1. 定义变暗动作 (120, 120, 120 是深灰色)
        auto tintDark = TintTo::create(0.05f, 120, 120, 120);

        // 2. 定义恢复原色动作 (255, 255, 255 是图片原始色彩)
        auto tintBack = TintTo::create(0.05f, 255, 255, 255);

        // 3. 组合动作序列：变暗 -> 恢复
        auto hitSequence = Sequence::create(tintDark, tintBack, nullptr);

        // 给动作设置一个 tag，方便上面 stopAllActionsByTag 调用
        hitSequence->setTag(999);

        rock->runAction(hitSequence);
    }
}