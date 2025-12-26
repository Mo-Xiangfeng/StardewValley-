#ifndef __TILE_DATA_H__
#define __TILE_DATA_H__

#include "CropData.h"

// 土地状态枚举
enum class TileState {
    NORMAL,      // 普通土地（未耕地）
    TILLED,      // 已耕地
    WATERED,     // 已浇水
    PLANTED      // 已种植
};

// 单个土地块的数据
struct TileData {
    TileState state;           // 土地状态
    bool isWatered;            // 是否浇水
    CropType cropType;         // 种植的作物类型
    int growthStage;           // 生长阶段（0=刚种下，max=成熟）
    int daysSincePlanted;      // 种植后经过的天数
    bool isReadyToHarvest;     // 是否可收获

    TileData() : state(TileState::NORMAL), isWatered(false),
        cropType(CropType::NONE), growthStage(0),
        daysSincePlanted(0), isReadyToHarvest(false) {
    }

    // 重置土地状态
    void reset() {
        state = TileState::NORMAL;
        isWatered = false;
        cropType = CropType::NONE;
        growthStage = 0;
        daysSincePlanted = 0;
        isReadyToHarvest = false;
    }
};

#endif
