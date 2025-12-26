#ifndef __GAME_WORLD_H__
#define __GAME_WORLD_H__

#include "cocos2d.h"
#include "TileMap.h"
#include "MapPortal.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

class Player;
class MapLogic;

struct MapInfo
{
    std::string txt;   // tilemap.txt
    std::string img;   // png

    // 出生点（名字 → tile 坐标）
    std::unordered_map<std::string, cocos2d::Vec2> entries;

    // 本地图拥有的所有门
    std::vector<PortalInfo> portals;
};

enum class LandState {
    NONE,       // 荒地
    TILLED,     // 已耕种（深褐色）
    WATERED     // 已浇水（最深色）
};

// 土地详细信息
struct LandTileData {
    LandState state = LandState::NONE;
    int cropId = -1;        // 对应 CropInfo 的 ID，-1 表示没种东西
    int currentGrowth = 0;  // 当前生长天数
    bool isHarvestable = false; // 是否可收获
};

// 用来存储每一块地的坐标和状态
struct LandTile {
    int tx, ty;
    LandState state;
};

class GameWorld : public cocos2d::Node
{
public:
    void startFishingMinigame();
    bool isFarmable(int tx, int ty);
    bool GameWorld::isWater(int tx, int ty);
    // 被 Player 调用：耕种或浇水
    void interactWithLand(int tx, int ty, int itemID); // 核心交互逻辑
    void nextDay(); // 每天清晨调用  
    void updateLandVisuals();   // 更新渲染

    static GameWorld* create(const std::string& txtFile,
        const std::string& imgFile);
    virtual bool init(const std::string& txtFile,
        const std::string& imgFile);
    void updateCamera();
    void reload(const std::string& txtFile, const std::string& imgFile);
    int debugGetTile(int tx, int ty);

    int getMapWidth() const { return _map.width; }
    int getMapHeight() const { return _map.height; }

    bool isWalkable(const cocos2d::Vec2& worldPos);
    void fadeSwitch(const std::string& mapId, const std::string& entry);
    cocos2d::Size getMapSizeScaled() const { return _mapSizeScaled; }
    float getMapScale() const { return _mapScale; }
    float getTileWidth() const { return _map.tileWidth; }
    void setPlayer(Player* player);
    void update(float dt) override;
    void onInteract();
    void GameWorld::drawFarmGrid();
    void switchMap(const std::string& mapId,
        const std::string& entry);

private:
    std::unordered_map<std::string, LandTileData> _farmlandData;
    std::string getLandKey(int tx, int ty) { return std::to_string(tx) + "_" + std::to_string(ty); }

    void setupPortalsForMap(const std::string& mapId);
    TileMap _map;
    cocos2d::Sprite* _mapSprite = nullptr;
    float _mapScale = 2.7f;
    cocos2d::Size _mapSizeScaled;

    Player* _player = nullptr;
    std::string _currentMapId;

    std::unordered_map<std::string, MapInfo> _maps;
    std::vector<MapPortal> _portals;

    std::unique_ptr<MapLogic> _logic;
    MapPortal* _activePortal = nullptr;
    MapPortal* _currentPortal = nullptr;
    float _portalStayTime = 0.0f;

};

#endif
