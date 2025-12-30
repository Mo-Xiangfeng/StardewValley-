#ifndef __TREE_MANAGER_H__
#define __TREE_MANAGER_H__

#include <vector>
#include "Tree.h"

class GameWorld;

class TreeManager {
public:
    static TreeManager* getInstance();

    void init(GameWorld* world);

    // 每天凌晨调用
    void dailyUpdate();

    // 玩家交互
    bool hasTree(int tx, int ty);
    bool removeTreeAt(int tx, int ty,int damage);

    int getTreeCount() const;

private:
    TreeManager() = default;

    void generateOneTree();
    void tryGenerateTrees();

private:
    GameWorld* _world = nullptr;
    std::vector<Tree> _trees;
};

#endif
