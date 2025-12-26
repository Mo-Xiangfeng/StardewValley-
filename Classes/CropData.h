#ifndef __CROP_DATA_H__
#define __CROP_DATA_H__

#include <string>
#include <map>
#include "Item.h"
// 作物类型枚举
enum class CropType {
    PARSNIP,      // 防风草 - 春季
    CAULIFLOWER,  // 花椰菜 - 春季
    POTATO,       // 土豆 - 春季
    TOMATO,       // 番茄 - 夏季
    MELON,        // 甜瓜 - 夏季
    CORN,         // 玉米 - 夏季/秋季
    PUMPKIN,      // 南瓜 - 秋季
    WHEAT         // 小麦 - 全季节
};

// 作物季节枚举
enum class CropSeason {
    SPRING = 0,
    SUMMER = 1,
    AUTUMN = 2,
    WINTER = 3,
    ALL_SEASON = 99
};

// 作物信息结构体
class CropInfo :public Item {
public:
    std::string name;          // 作物名称（英文）             // 作物类型
    CropSeason season;         // 适合的季节
    int growthDays;            // 生长天数
    int sellPrice;             // 出售价格
    int seedPrice;             // 种子价格
    bool canRegrow;            // 是否可重复收获
    int regrowDays;            // 重新生长天数

    CropInfo() :season(CropSeason::SPRING),
        growthDays(0), sellPrice(0), seedPrice(0),
        canRegrow(false), regrowDays(0) {
    }
};

// 作物数据库（单例）
class CropDatabase {
public:
    static CropDatabase* getInstance();

    void init();
    const CropInfo& getCropInfo(int type) const;

private:
    CropDatabase();
    static CropDatabase* instance;
    std::map<int, std::shared_ptr<CropInfo>> crops;
    CropInfo emptyCrop;
};
#endif
