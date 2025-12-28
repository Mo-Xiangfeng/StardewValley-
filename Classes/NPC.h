#ifndef __NPC_H__
#define __NPC_H__

#include "cocos2d.h"
#include <vector>
#include <string>

USING_NS_CC;

class GameWorld;

class NPC : public cocos2d::Sprite {
public:
    static NPC* create(const std::string& id, const std::string& name, const std::string& spriteFrame);

    // ===== 基础属性 =====
    std::string npcId;
    std::string npcName;

    // ===== 对话系统 =====
    std::vector<std::string> dialogues;
    int currentDialogueIndex = 0;

    std::string getNextDialogue();
    void resetDialogue();
    bool hasNextDialogue() const;

    // ===== 移动相关属性 =====
    bool isMoving = false;              // 是否正在移动
    std::vector<Vec2> currentPath;      // 当前寻路路径
    int pathIndex = 0;                  // 当前路径点索引
    float moveSpeed = 100.0f;           // 移动速度（像素/秒）

    // ===== 日程系统 =====
    struct SchedulePoint {
        int hour;
        int tx, ty;
        int direction;
        std::string mapId;  // 指定在哪个地图
    };
    std::vector<SchedulePoint> schedule;

    void addSchedulePoint(int hour, int tx, int ty, int direction, const std::string& mapId = "Map");
    void updateSchedule(int currentHour);  // 根据当前时间更新位置

    // ===== 移动系统 =====
    void moveToTile(int tx, int ty);           // 移动到目标格子
    void updateMovement(float dt);             // 每帧更新移动
    void stopMoving();                         // 停止移动
    Vec2 getTilePosition() const;              // 获取当前格子坐标

    // ===== 朝向系统 =====
    void setDirection(int dir);
    int getDirection() const { return direction; }
    int direction = 1;  // 0=上, 1=下, 2=左, 3=右

    std::string currentMapId = "";   // 添加这个
    std::string moveMapId = "";      // 添加这个

    // ===== 世界交互 =====
    void setGameWorld(GameWorld* world);
    GameWorld* getGameWorld() const { return _gameWorld; }
    void setTilePosition(int tx, int ty);  // 直接设置格子位置（瞬移）

protected:
    bool init(const std::string& id, const std::string& name, const std::string& spriteFrame);
    void update(float dt) override;  // Cocos2d 每帧更新

private:
    GameWorld* _gameWorld = nullptr;

    // ===== 动画辅助方法 =====
    void startWalkingAnimation(int dir);
    void stopWalkingAnimation();
};

#endif // __NPC_H__
