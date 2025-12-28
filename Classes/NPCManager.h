#ifndef __NPC_MANAGER_H__
#define __NPC_MANAGER_H__

#include "cocos2d.h"
#include "NPC.h"
#include <map>
#include <string>

USING_NS_CC;

class GameWorld;  // 前向声明

class NPCManager {
public:
    static NPCManager* getInstance();

    // 初始化
    void init();

    // NPC 管理
    NPC* createNPC(const std::string& npcId, const std::string& name, const std::string& spriteFrame);
    NPC* getNPC(const std::string& npcId);
    void removeNPC(const std::string& npcId);

    // 批量操作
    void addNPCToWorld(const std::string& npcId, GameWorld* world,
        int tx, int ty, const std::string& mapId = "Map");
    void updateAllNPCSchedules(int currentHour);

    // 获取所有 NPC
    const std::map<std::string, NPC*>& getAllNPCs() const { return npcMap; }

private:
    NPCManager();
    static NPCManager* instance;

    std::map<std::string, NPC*> npcMap;  // 存储所有 NPC
};

#endif // __NPC_MANAGER_H__
