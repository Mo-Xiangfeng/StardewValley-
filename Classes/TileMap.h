#ifndef __TILE_MAP_H__
#define __TILE_MAP_H__

#include <vector>
#include <string>

class TileMap
{
public:
    bool load(const std::string& path, int tileW, int tileH);
/*void loadMap(const std::string& txtFile,
    const std::string& imageFile,
    const Vec2& startTile);*/
    int getTile(int x, int y) const;
    bool isWalkable(int tileId) const;
    bool inBounds(int x, int y) const;

    int width = 0;
    int height = 0;

    int tileWidth = 0;   // ¡ï ÊµÀý³ÉÔ±
    int tileHeight = 0;

private:
    std::vector<std::vector<int>> _data;
};

#endif
