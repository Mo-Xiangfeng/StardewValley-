#include "GameWorld.h"
#include "Player.h"
#include "MainMapLogic.h"
#include "HomeLogic.h"
#include "ShopLogic.h"
#include "NPC1HouseLogic.h"
#include "NPC2HouseLogic.h"
#include "MineLogic.h"
#include "GameScene.h"

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
            { "Spawn",    {33, 25} },
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


    /* ========= 初始进入主地图 ========= */
    //switchMap("Map", "Spawn");

    scheduleUpdate();
    return true;
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

    if (_player)
    {
        Vec2 spawn = info.entries.at(entry);
        _player->setTilePosition(spawn.x, spawn.y);
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

    // 进入新地图逻辑
    if (_logic)
        _logic->onEnter(this, _player);

    _currentPortal = nullptr;
    _portalStayTime = 0.0f;
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

bool GameWorld::isWalkable(const Vec2& worldPos)
{
    float ts = _map.tileWidth * _mapScale;

    int tx = worldPos.x / ts;
    int ty = worldPos.y / ts;

    int tileId = _map.getTile(tx, ty);

    // Debug（现在你会看到正常的 tileId 了）
    //CCLOG("tx=%d ty=%d tileId=%d", tx, ty, tileId);

    if (tileId < 0) return false;
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
