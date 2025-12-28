#include "CropData.h"
#include "InventoryManager.h"
#include "map"
CropDatabase* CropDatabase::instance = nullptr;

CropDatabase::CropDatabase() {}

CropDatabase* CropDatabase::getInstance() {
    if (!instance) {
        instance = new CropDatabase();
    }
    return instance;
}

void CropDatabase::init() {
    // ========== 春季作物 ==========
    auto parsnip_seed = std::make_shared<CropInfo>();
    parsnip_seed->id = 1100;
    parsnip_seed->name = "parsnip_seed";
    parsnip_seed->iconPath= "item/1100.png";
    parsnip_seed->season = CropSeason::SPRING;
    parsnip_seed->growthDays = 1;
    parsnip_seed->price = 15;
    parsnip_seed->seedPrice = 20;
    parsnip_seed->canRegrow = false;  
    parsnip_seed->maxStack = 99;
    crops[parsnip_seed->id] = parsnip_seed;

    auto cauliflower_seed = std::make_shared<CropInfo>();
    cauliflower_seed->id = 1107;
    cauliflower_seed->name = "cauliflower_seed";
    cauliflower_seed->iconPath = "item/1107.png";
    cauliflower_seed->season = CropSeason::SPRING;
    cauliflower_seed->growthDays = 1;
    cauliflower_seed->price = 65;
    cauliflower_seed->seedPrice = 80;
    cauliflower_seed->canRegrow = false;
    crops[cauliflower_seed->id] = cauliflower_seed;

    auto patato_seed = std::make_shared<CropInfo>();
    patato_seed->id = 1102;
    patato_seed->name = "patato_seed";
    patato_seed->iconPath = "item/1102.png";
    patato_seed->season = CropSeason::SPRING;
    patato_seed->growthDays = 1;
    patato_seed->price = 40;
    patato_seed->seedPrice = 50;
    patato_seed->canRegrow = false;
    crops[patato_seed->id] = patato_seed;

    // ========== 夏季作物 ==========
    auto tomato_seed = std::make_shared<CropInfo>();
    tomato_seed->id = 1106;
    tomato_seed->name = "tomato_seed";
    tomato_seed->iconPath = "item/1106.png";
    tomato_seed->season = CropSeason::SUMMER;
    tomato_seed->growthDays = 1;
    tomato_seed->price = 40;
    tomato_seed->seedPrice = 50;
    tomato_seed->canRegrow = true;
    tomato_seed->regrowDays = 4;
    crops[tomato_seed->id] = tomato_seed;

    auto melon_seed = std::make_shared<CropInfo>();
    melon_seed->id = 1104;
    melon_seed->name = "melon_seed";
    melon_seed->iconPath = "item/1104.png";
    melon_seed->season = CropSeason::SUMMER;
    melon_seed->growthDays = 1;
    melon_seed->price = 50;
    melon_seed->seedPrice = 80;
    melon_seed->canRegrow = false;
    crops[melon_seed->id] = melon_seed;

    // ========== 秋季作物 ==========
    auto pumpkin_seed = std::make_shared<CropInfo>();
    pumpkin_seed->id = 1105;
    pumpkin_seed->name = "pumpkin_seed";
    pumpkin_seed->iconPath = "item/1105.png";
    pumpkin_seed->season = CropSeason::AUTUMN;
    pumpkin_seed->growthDays = 1;
    pumpkin_seed->price = 80;
    pumpkin_seed->seedPrice = 100;
    pumpkin_seed->canRegrow = false;
    crops[pumpkin_seed->id] = pumpkin_seed;

    // ========== 全季节作物 ==========
    auto wheat_seed = std::make_shared<CropInfo>();
    wheat_seed->id = 1101;
    wheat_seed->name = "wheat_seed";
    wheat_seed->iconPath = "item/1101.png";
    wheat_seed->season = CropSeason::ALL_SEASON;
    wheat_seed->growthDays = 1;
    wheat_seed->price = 5;
    wheat_seed->seedPrice = 10;
    wheat_seed->canRegrow = false;
    crops[wheat_seed->id] = wheat_seed;
}

const CropInfo& CropDatabase::getCropInfo(int type) const {
    auto it = crops.find(type);
    if (it != crops.end()) {
        return *(it->second);
    }
    return emptyCrop;
}

std::shared_ptr<CropInfo> CropDatabase::getCrop(int id) {
    // 在 map 中查找该 ID
    auto it = crops.find(id);

    // 如果找到了，返回对应的 shared_ptr
    if (it != crops.end()) {
        return it->second;
    }

    // 如果没找到，返回 nullptr
    return nullptr;
}