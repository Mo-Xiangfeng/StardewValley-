#include "NPCManager.h"
#include "GameWorld.h"  // 包含完整定义

NPCManager* NPCManager::instance = nullptr;

NPCManager::NPCManager() {
}

NPCManager* NPCManager::getInstance() {
    if (!instance) {
        instance = new NPCManager();
    }
    return instance;
}

void NPCManager::init() {
    // 清空现有 NPC
    npcMap.clear();
    CCLOG("[NPCManager] Initialized");
}

NPC* NPCManager::createNPC(const std::string& npcId, const std::string& name, const std::string& spriteFrame) {
    // 检查是否已存在
    if (npcMap.find(npcId) != npcMap.end()) {
        CCLOG("[NPCManager] NPC %s already exists!", npcId.c_str());
        return npcMap[npcId];
    }

    // 创建新 NPC
    NPC* npc = NPC::create(npcId, name, spriteFrame);
    if (npc) {
        npc->retain();  // 保持引用
        npcMap[npcId] = npc;
        CCLOG("[NPCManager] Created NPC: %s (%s)", npcId.c_str(), name.c_str());
    }
    return npc;
}

NPC* NPCManager::getNPC(const std::string& npcId) {
    auto it = npcMap.find(npcId);
    if (it != npcMap.end()) {
        return it->second;
    }
    return nullptr;
}

void NPCManager::removeNPC(const std::string& npcId) {
    auto it = npcMap.find(npcId);
    if (it != npcMap.end()) {
        it->second->release();
        npcMap.erase(it);
        CCLOG("[NPCManager] Removed NPC: %s", npcId.c_str());
    }
}

void NPCManager::addNPCToWorld(const std::string& npcId, GameWorld* world,
    int tx, int ty, const std::string& mapId)
{
    NPC* npc = getNPC(npcId);
    if (!npc || !world) {
        CCLOG("[NPCManager] Failed to add NPC to world: NPC or World is null");
        return;
    }

    // 【关键】保存 NPC 所属的地图 ID
    npc->currentMapId = mapId;

    // 绑定世界引用
    npc->setGameWorld(world);

    // 设置瓦片位置（会自动转换为像素坐标）
    npc->setTilePosition(tx, ty);

    // 添加到世界
    world->addChild(npc, 999);

    // 【关键】只有当前地图匹配时才显示
    npc->setVisible(world->getCurrentMapId() == mapId);

    CCLOG("[NPCManager] Added NPC %s to map '%s' at tile (%d, %d)",
        npcId.c_str(), mapId.c_str(), tx, ty);
}

void NPCManager::updateAllNPCSchedules(int currentHour) {
    for (auto& pair : npcMap) {
        pair.second->updateSchedule(currentHour);
    }
}
