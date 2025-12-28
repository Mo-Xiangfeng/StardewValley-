#ifndef __PATHFINDING_H__
#define __PATHFINDING_H__

#include "cocos2d.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>

USING_NS_CC;

class GameWorld;

// A* 路径节点
struct PathNode {
    int x, y;           // 瓦片坐标
    float g;            // 从起点到当前点的实际代价
    float h;            // 启发式：当前点到终点的估计代价
    float f;            // f = g + h
    PathNode* parent;   // 父节点（用于回溯路径）

    PathNode(int _x, int _y, float _g, float _h, PathNode* _parent = nullptr)
        : x(_x), y(_y), g(_g), h(_h), parent(_parent) {
        f = g + h;
    }

    // 用于优先队列排序（f 值小的优先）
    bool operator>(const PathNode& other) const {
        return f > other.f;
    }
};

// 自定义哈希函数（用于 unordered_map）
struct Vec2Hash {
    std::size_t operator()(const Vec2& v) const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};

struct Vec2Equal {
    bool operator()(const Vec2& a, const Vec2& b) const {
        return a.x == b.x && a.y == b.y;
    }
};

class Pathfinding {
public:
    /**
     * A* 寻路核心函数
     * @param world GameWorld 引用（用于碰撞检测）
     * @param startX 起点 X（瓦片坐标）
     * @param startY 起点 Y（瓦片坐标）
     * @param endX 终点 X（瓦片坐标）
     * @param endY 终点 Y（瓦片坐标）
     * @return 路径点列表（如果失败返回空）
     */
    static std::vector<Vec2> findPath(
        GameWorld* world,
        int startX, int startY,
        int endX, int endY
    );

private:
    // 曼哈顿距离启发式
    static float heuristic(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    // 检查瓦片是否可通行
    static bool isWalkableTile(GameWorld* world, int tx, int ty);

    // 获取相邻格子（上下左右四方向）
    static std::vector<Vec2> getNeighbors(int x, int y);
};

#endif // __PATHFINDING_H__
