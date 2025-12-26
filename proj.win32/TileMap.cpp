#include "TileMap.h"
#include <fstream>
#include <sstream>
#include "cocos2d.h"
/*bool TileMap::load(const std::string& path, int tileW, int tileH)
{
    tileWidth = tileW;
    tileHeight = tileH;

    std::ifstream file(path);
    if (!file.is_open()) return false;

    _data.clear();

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::vector<int> row;
        int v;

        while (ss >> v)
            row.push_back(v);

        if (!row.empty())
            _data.push_back(row);
    }

    // 原始行列
    height = _data.size();
    width = height > 0 ? _data[0].size() : 0;

    if (height == 0 || width == 0) return false;


    // =======================================

    return true;
}*/

bool TileMap::load(const std::string& path, int tileW, int tileH)
{
    tileWidth = tileW;
    tileHeight = tileH;

    std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(path);
    CCLOG("Loading map: %s", fullPath.c_str()); // 看看路径对不对

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        CCLOG("ERROR: File not found!");
        return false;
    }

    _data.clear();
    std::string line;
    while (std::getline(file, line))
    {
        // 移除行末可能的 \r (Windows 换行符处理)
        if (!line.empty() && line.back() == '\r') line.pop_back();

        std::stringstream ss(line);
        std::vector<int> row;
        int v;
        while (ss >> v) {
            row.push_back(v);
            if (ss.peek() == ',') ss.ignore();
        }

        if (!row.empty()) _data.push_back(row);
    }

    height = (int)_data.size();
    width = height > 0 ? (int)_data[0].size() : 0;

    CCLOG("Map Success: Width %d, Height %d", width, height);
    return (height > 0 && width > 0);
}



bool TileMap::inBounds(int x, int y) const
{
    return x >= 0 && y >= 0 && x < width && y < height;
}

int TileMap::getTile(int x, int y) const
{
    if (!inBounds(x, y)) return -1;
    int flippedY = height - 1 - y;
    return _data[flippedY][x];
}

/*int TileMap::getTile(int x, int y) const
{
    if (!inBounds(x, y)) return -1;

    // 关键修正：数组的第0行是地图的最顶端
    // height - 1 - y 将 Cocos 的 y 坐标转换为数组的行索引
    int arrayRow = (height - 1) - y;

    return _data[arrayRow][x];
}*/


bool TileMap::isWalkable(int tileId) const
{
    // ★ 根据你的 tilemap.txt 判定
    // 例如：0 = 草地可走，1 = 道路可走，其他均不可走
    return (tileId == 0 || tileId == 1 || tileId == 8 || tileId == 9);
}
