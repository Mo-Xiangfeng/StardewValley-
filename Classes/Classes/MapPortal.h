#ifndef __MAP_PORTAL_H__
#define __MAP_PORTAL_H__

#include "cocos2d.h"
#include <string>
struct PortalInfo
{
    cocos2d::Vec2 tile;      // 门所在 tile 坐标
    std::string toMap;       // 目标地图
    std::string toEntry;     // 目标地图的出生点
};

struct MapPortal
{
    cocos2d::Rect triggerArea;   // 世界坐标触发区
    std::string fromMap;         // 当前地图 ID
    std::string toMap;           // 目标地图 ID
    std::string toEntry;         // 目标入口名

    MapPortal(const cocos2d::Rect& area,
        const std::string& from,
        const std::string& to,
        const std::string& entry)
        : triggerArea(area)
        , fromMap(from)
        , toMap(to)
        , toEntry(entry)
    {
    }
};

#endif

