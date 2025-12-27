#ifndef __CROP_DATA_H__
#define __CROP_DATA_H__

#include <string>
#include <map>
#include "Item.h"
// 作物类型枚举
/*enum class CropType {
    PARSNIP,      // 防风草 - 春季
    CAULIFLOWER,  // 花椰菜 - 春季
    POTATO,       // 土豆 - 春季
    TOMATO,       // 番茄 - 夏季
    MELON,        // 甜瓜 - 夏季
    CORN,         // 玉米 - 夏季/秋季
    PUMPKIN,      // 南瓜 - 秋季
    WHEAT         // 小麦 - 全季节
};*/

// 作物季节枚举
enum class CropSeason {
    SPRING = 0,
    SUMMER = 1,
    AUTUMN = 2,
    WINTER = 3,
    ALL_SEASON = 99
};

// 作物信息结构体
// CropData.h
class CropInfo : public Item {
public:
    CropSeason season;
    int growthDays;
    int seedPrice;
    bool canRegrow;
    int regrowDays;

    CropInfo() : season(CropSeason::SPRING),
        growthDays(0), seedPrice(0),
        canRegrow(false), regrowDays(0) {
        price = 0; type = ItemType::SEED;// 明确使用父类的 price
    }

    virtual std::shared_ptr<Item> clone() const override {
        return std::make_shared<CropInfo>(*this);
    }
};

// 作物数据库（单例）
class CropDatabase {
public:
    static CropDatabase* getInstance();

    void init();
    const CropInfo& getCropInfo(int type) const;
    std::shared_ptr<CropInfo> getCrop(int id);

private:
    CropDatabase();
    static CropDatabase* instance;
    std::map<int, std::shared_ptr<CropInfo>> crops;
    CropInfo emptyCrop;
};

#endif
