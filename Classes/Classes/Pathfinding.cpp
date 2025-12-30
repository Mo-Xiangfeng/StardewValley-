#include "Pathfinding.h"
#include "GameWorld.h"
#include <algorithm>
#include <cmath>

std::vector<Vec2> Pathfinding::findPath(
    GameWorld* world,
    int startX, int startY,
    int endX, int endY
) {
    if (!world) {
        CCLOG("[Pathfinding] ERROR: GameWorld is NULL!");
        return {};
    }

    // 1. 检查起点和终点是否有效
    if (!isWalkableTile(world, startX, startY)) {
        CCLOG("[Pathfinding] Start tile (%d, %d) is not walkable!", startX, startY);
        return {};
    }

    if (!isWalkableTile(world, endX, endY)) {
        CCLOG("[Pathfinding] End tile (%d, %d) is not walkable!", endX, endY);
        return {};
    }

    // 2. 如果起点就是终点
    if (startX == endX && startY == endY) {
        return { Vec2(startX, startY) };
    }

    // 3. 初始化开放列表（优先队列）和关闭列表
    auto cmp = [](PathNode* a, PathNode* b) { return *a > *b; };
    std::priority_queue<PathNode*, std::vector<PathNode*>, decltype(cmp)> openList(cmp);

    std::unordered_map<Vec2, bool, Vec2Hash, Vec2Equal> closedSet;
    std::unordered_map<Vec2, PathNode*, Vec2Hash, Vec2Equal> nodeMap;

    // 4. 创建起点节点
    PathNode* startNode = new PathNode(
        startX, startY,
        0.0f,  // g = 0
        heuristic(startX, startY, endX, endY),  // h
        nullptr
    );

    openList.push(startNode);
    nodeMap[Vec2(startX, startY)] = startNode;

    PathNode* endNode = nullptr;

    // 5. A* 主循环
    while (!openList.empty()) {
        // 取出 f 值最小的节点
        PathNode* current = openList.top();
        openList.pop();

        Vec2 currentPos(current->x, current->y);

        // 如果已经在关闭列表，跳过
        if (closedSet[currentPos]) {
            continue;
        }

        // 标记为已访问
        closedSet[currentPos] = true;

        // 找到终点
        if (current->x == endX && current->y == endY) {
            endNode = current;
            break;
        }

        // 6. 遍历相邻格子
        for (const Vec2& neighbor : getNeighbors(current->x, current->y)) {
            int nx = (int)neighbor.x;
            int ny = (int)neighbor.y;

            // 跳过不可通行或已访问的格子
            if (!isWalkableTile(world, nx, ny) || closedSet[Vec2(nx, ny)]) {
                continue;
            }

            // 计算新的 g 值
            float newG = current->g + 1.0f;  // 移动代价为 1

            // 检查是否已经在 nodeMap 中
            Vec2 neighborPos(nx, ny);
            PathNode* neighborNode = nodeMap[neighborPos];

            if (!neighborNode) {
                // 创建新节点
                neighborNode = new PathNode(
                    nx, ny,
                    newG,
                    heuristic(nx, ny, endX, endY),
                    current
                );
                nodeMap[neighborPos] = neighborNode;
                openList.push(neighborNode);
            }
            else if (newG < neighborNode->g) {
                // 找到更短路径，更新节点
                neighborNode->g = newG;
                neighborNode->f = newG + neighborNode->h;
                neighborNode->parent = current;
                openList.push(neighborNode);  // 重新加入队列
            }
        }
    }

    // 7. 回溯路径
    std::vector<Vec2> path;

    if (endNode) {
        PathNode* node = endNode;
        while (node) {
            path.push_back(Vec2(node->x, node->y));
            node = node->parent;
        }
        std::reverse(path.begin(), path.end());

        CCLOG("[Pathfinding] Path found! Length: %zu", path.size());
    }
    else {
        CCLOG("[Pathfinding] No path found from (%d, %d) to (%d, %d)",
            startX, startY, endX, endY);
    }

    // 8. 清理内存
    for (auto& pair : nodeMap) {
        delete pair.second;
    }

    return path;
}

bool Pathfinding::isWalkableTile(GameWorld* world, int tx, int ty) {
    float tileSize = 16.0f;
    float scale = world->getMapScale();

    // 转换为像素坐标（中心点）
    Vec2 pixelPos(
        (tx + 0.5f) * tileSize * scale,
        (ty + 0.5f) * tileSize * scale
    );

    return world->isWalkable(pixelPos);
}

std::vector<Vec2> Pathfinding::getNeighbors(int x, int y) {
    return {
        Vec2(x, y - 1),  // 下
        Vec2(x, y + 1),  // 上
        Vec2(x - 1, y),  // 左
        Vec2(x + 1, y)   // 右
    };
}
