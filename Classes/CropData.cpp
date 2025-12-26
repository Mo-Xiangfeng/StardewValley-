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
    auto parsnip = std::make_shared<CropInfo>();
    parsnip->id = 1100;
    parsnip->name = "Parsnip";
    parsnip->season = CropSeason::SPRING;
    parsnip->growthDays = 4;
    parsnip->sellPrice = 35;
    parsnip->seedPrice = 20;
    parsnip->canRegrow = false;  
    parsnip->maxStack = 99;
    crops[parsnip->id] = parsnip;

    auto cauliflower = std::make_shared<CropInfo>();
    cauliflower->id = 1101;
    cauliflower->name = "Cauliflower";
    cauliflower->season = CropSeason::SPRING;
    cauliflower->growthDays = 12;
    cauliflower->sellPrice = 175;
    cauliflower->seedPrice = 80;
    cauliflower->canRegrow = false;
    crops[cauliflower->id] = cauliflower;

    auto potato = std::make_shared<CropInfo>();
    potato->id = 1102;
    potato->name = "Potato";
    potato->season = CropSeason::SPRING;
    potato->growthDays = 6;
    potato->sellPrice = 80;
    potato->seedPrice = 50;
    potato->canRegrow = false;
    crops[potato->id] = potato;

    // ========== 夏季作物 ==========
    auto tomato = std::make_shared<CropInfo>();
    tomato->id = 1103;
    tomato->name = "Tomato";
    tomato->season = CropSeason::SUMMER;
    tomato->growthDays = 11;
    tomato->sellPrice = 60;
    tomato->seedPrice = 50;
    tomato->canRegrow = true;
    tomato->regrowDays = 4;
    crops[tomato->id] = tomato;

    auto melon = std::make_shared<CropInfo>();
    melon->id = 1104;
    melon->name = "Melon";
    melon->season = CropSeason::SUMMER;
    melon->growthDays = 12;
    melon->sellPrice = 250;
    melon->seedPrice = 80;
    melon->canRegrow = false;
    crops[melon->id] = melon;

    // ========== 秋季作物 ==========
    auto pumpkin = std::make_shared<CropInfo>();
    pumpkin->id = 1105;
    pumpkin->name = "Pumpkin";
    pumpkin->season = CropSeason::AUTUMN;
    pumpkin->growthDays = 13;
    pumpkin->sellPrice = 320;
    pumpkin->seedPrice = 100;
    pumpkin->canRegrow = false;
    crops[pumpkin->id] = pumpkin;

    // ========== 全季节作物 ==========
    auto wheat = std::make_shared<CropInfo>();
    wheat->id = 1106;
    wheat->name = "Wheat";
    wheat->season = CropSeason::ALL_SEASON;
    wheat->growthDays = 4;
    wheat->sellPrice = 25;
    wheat->seedPrice = 10;
    wheat->canRegrow = false;
    crops[wheat->id] = wheat;
}

const CropInfo& CropDatabase::getCropInfo(int type) const {
    auto it = crops.find(type);
    if (it != crops.end()) {
        return *(it->second);
    }
    return emptyCrop;
}
