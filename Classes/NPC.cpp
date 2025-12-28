#include "NPC.h"
#include "GameWorld.h"
#include "Pathfinding.h"

NPC* NPC::create(const std::string& id, const std::string& name, const std::string& spriteFrame) {
    NPC* npc = new NPC();
    if (npc && npc->init(id, name, spriteFrame)) {
        npc->autorelease();
        return npc;
    }
    CC_SAFE_DELETE(npc);
    return nullptr;
}

bool NPC::init(const std::string& id, const std::string& name, const std::string& spriteFrame) {
    if (!Sprite::initWithSpriteFrameName(spriteFrame)) {
        return false;
    }

    this->npcId = id;
    this->npcName = name;
    this->setScale(4.0f);

    // 添加名字标签
    auto nameLabel = Label::createWithSystemFont(name, "Arial", 12);
    nameLabel->setPosition(Vec2(this->getContentSize().width / 2,
        this->getContentSize().height + 10));
    this->addChild(nameLabel);

    // ===== 启用每帧更新 =====
    this->scheduleUpdate();

    return true;
}

// ========== 对话相关 ==========
std::string NPC::getNextDialogue() {
    if (dialogues.empty()) {
        CCLOG("[NPC] %s: dialogues is EMPTY!", npcName.c_str());
        return "";
    }

    int index = currentDialogueIndex % dialogues.size();
    std::string dialogue = dialogues[index];

    CCLOG("[NPC] %s: getNextDialogue() -> index=%d/%zu, text='%s'",
        npcName.c_str(), index, dialogues.size(), dialogue.c_str());

    currentDialogueIndex++;
    return dialogue;
}

void NPC::resetDialogue() {
    currentDialogueIndex = 0;
    CCLOG("[NPC] %s: resetDialogue() called", npcName.c_str());
}

bool NPC::hasNextDialogue() const {
    return !dialogues.empty();
}

// ========== 日程系统 ==========
void NPC::addSchedulePoint(int hour, int tx, int ty, int direction, const std::string& mapId) {
    schedule.push_back({ hour, tx, ty, direction, mapId });
    CCLOG("[NPC] %s: Added schedule point - %02d:00 -> Tile(%d, %d) on map '%s'",
        npcName.c_str(), hour, tx, ty, mapId.c_str());
}

void NPC::updateSchedule(int currentHour) {
    if (schedule.empty()) return;

    // 找到当前时间应该执行的日程点
    SchedulePoint* targetPoint = nullptr;
    for (int i = schedule.size() - 1; i >= 0; --i) {
        if (currentHour >= schedule[i].hour) {
            targetPoint = &schedule[i];
            break;
        }
    }

    if (!targetPoint) {
        targetPoint = &schedule[0];
    }

    if (!_gameWorld) {
        CCLOG("[NPC] %s: ERROR - _gameWorld is NULL!", npcName.c_str());
        return;
    }

    std::string currentMap = _gameWorld->getCurrentMapId();

    // 检查是否在正确的地图上
    if (currentMap != targetPoint->mapId) {
        this->setVisible(false);
        isMoving = false;  // 停止移动
        CCLOG("[NPC] %s: Hidden (should be on map '%s', current map is '%s')",
            npcName.c_str(), targetPoint->mapId.c_str(), currentMap.c_str());
        return;
    }

    // 在正确的地图上，显示并移动
    this->setVisible(true);

    // ===== 修改：使用平滑移动而不是瞬移 =====
    Vec2 currentTile = getTilePosition();

    if ((int)currentTile.x != targetPoint->tx ||
        (int)currentTile.y != targetPoint->ty) {

        if (!isMoving) {  // 避免重复触发
            setDirection(targetPoint->direction);
            moveToTile(targetPoint->tx, targetPoint->ty);
        }
    }

    // 20:00 特殊处理：回家后消失
    if (targetPoint->hour == 20 && !isMoving) {
        this->setVisible(false);
        CCLOG("[NPC] %s returned home and became invisible at 20:00",
            npcName.c_str());
    }
}

// ========== 移动相关 ==========

Vec2 NPC::getTilePosition() const {
    if (!_gameWorld) return Vec2(-1, -1);

    Vec2 pixelPos = this->getPosition();
    float tileSize = 16.0f;
    float scale = _gameWorld->getMapScale();

    int tx = (int)(pixelPos.x / (tileSize * scale));
    int ty = (int)(pixelPos.y / (tileSize * scale));

    return Vec2(tx, ty);
}

void NPC::moveToTile(int targetX, int targetY) {
    if (!_gameWorld) {
        CCLOG("[NPC] %s: Cannot move without GameWorld reference!", npcName.c_str());
        return;
    }

    // 记录当前地图
    std::string moveStartMap = _gameWorld->getCurrentMapId();

    Vec2 currentTile = getTilePosition();
    int startX = (int)currentTile.x;
    int startY = (int)currentTile.y;

    if (startX == targetX && startY == targetY) {
        CCLOG("[NPC] %s: Already at target (%d, %d)",
            npcName.c_str(), targetX, targetY);
        return;
    }

    // 使用 A* 寻路
    currentPath = Pathfinding::findPath(_gameWorld, startX, startY, targetX, targetY);

    if (currentPath.empty()) {
        CCLOG("[NPC] %s: No path found to (%d, %d)!",
            npcName.c_str(), targetX, targetY);
        return;
    }

    // 移除起点
    if (!currentPath.empty() &&
        (int)currentPath[0].x == startX &&
        (int)currentPath[0].y == startY) {
        currentPath.erase(currentPath.begin());
    }

    pathIndex = 0;
    isMoving = true;
    moveMapId = moveStartMap;  // 记录移动开始时的地图

    CCLOG("[NPC] %s: Started moving to (%d, %d), path length: %zu",
        npcName.c_str(), targetX, targetY, currentPath.size());
}


void NPC::updateMovement(float dt) {
    if (!isMoving || currentPath.empty()) {
        return;
    }

    // 检查地图是否切换了
    if (_gameWorld && _gameWorld->getCurrentMapId() != moveMapId) {
        CCLOG("[NPC] %s: Map changed during movement, stopping", npcName.c_str());
        stopMoving();
        return;
    }

    if (pathIndex >= currentPath.size()) {
        isMoving = false;
        stopWalkingAnimation();
        CCLOG("[NPC] %s: Reached destination", npcName.c_str());
        return;
    }

    // ... 原有的移动逻辑保持不变
    Vec2 targetTile = currentPath[pathIndex];

    float tileSize = 16.0f;
    float scale = _gameWorld->getMapScale();
    Vec2 targetPixel(
        (targetTile.x + 0.5f) * tileSize * scale,
        (targetTile.y + 0.5f) * tileSize * scale
    );

    Vec2 currentPos = this->getPosition();
    Vec2 moveDirection = targetPixel - currentPos;
    float distance = moveDirection.length();

    if (distance < 2.0f) {
        pathIndex++;
        return;
    }

    moveDirection.normalize();
    Vec2 movement = moveDirection * moveSpeed * dt;

    if (movement.length() > distance) {
        movement = moveDirection * distance;
    }

    Vec2 newPos = currentPos + movement;

    // 碰撞检测
    if (_gameWorld && !_gameWorld->isWalkable(newPos)) {
        CCLOG("[NPC] %s: Path blocked, recalculating...", npcName.c_str());

        Vec2 finalTarget = currentPath.back();
        int targetX = (int)finalTarget.x;
        int targetY = (int)finalTarget.y;

        moveToTile(targetX, targetY);
        return;
    }

    this->setPosition(newPos);

    // 更新朝向和动画
    int animDirection = 0;
    if (abs(moveDirection.y) > abs(moveDirection.x)) {
        animDirection = (moveDirection.y > 0) ? 3 : 1;
    }
    else {
        animDirection = (moveDirection.x > 0) ? 2 : 0;
    }

    if (animDirection != direction) {
        startWalkingAnimation(animDirection);
    }
}


void NPC::startWalkingAnimation(int dir) {
    direction = dir;

    // 停止当前动画
    this->stopAllActions();

    // 根据方向选择动画
    const char* frameName = nullptr;
    switch (dir) {
        case 0: frameName = "left_side-%d.png"; break;
        case 1: frameName = "down_side-%d.png"; break;
        case 2: frameName = "right_side-%d.png"; break;
        case 3: frameName = "up_side-%d.png"; break;
    }

    if (!frameName) return;

    // 创建动画
    Vector<SpriteFrame*> frames;
    for (int i = 0; i < 4; i++) {
        char name[64];
        sprintf(name, frameName, i);
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        if (frame) {
            frames.pushBack(frame);
        }
    }

    if (!frames.empty()) {
        auto animation = Animation::createWithSpriteFrames(frames, 0.15f);
        auto animate = Animate::create(animation);
        this->runAction(RepeatForever::create(animate));
    }
}

void NPC::stopWalkingAnimation() {
    this->stopAllActions();

    // 设置为静止帧
    const char* frameName = nullptr;
    switch (direction) {
        case 0: frameName = "left_side-0.png"; break;
        case 1: frameName = "down_side-0.png"; break;
        case 2: frameName = "right_side-0.png"; break;
        case 3: frameName = "up_side-0.png"; break;
    }

    if (frameName) {
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            this->setSpriteFrame(frame);
        }
    }
}

void NPC::stopMoving() {
    isMoving = false;
    currentPath.clear();
    pathIndex = 0;
    stopWalkingAnimation();
}

void NPC::update(float dt) {
    // Cocos2d 的每帧更新回调
    if (isMoving) {
        updateMovement(dt);
    }
}

void NPC::setDirection(int dir) {
    direction = dir;
    stopWalkingAnimation();  // 更新静止帧
}

// ========== 世界交互 ==========
void NPC::setGameWorld(GameWorld* world) {
    _gameWorld = world;
}

void NPC::setTilePosition(int tx, int ty) {
    if (!_gameWorld) return;

    float tileSize = 16.0f * _gameWorld->getMapScale();
    float x = (tx + 0.5f) * tileSize;
    float y = (ty + 0.5f) * tileSize;
    this->setPosition(Vec2(x, y));

    CCLOG("[NPC] %s: Teleported to tile (%d, %d) -> pixel (%.2f, %.2f)",
        npcName.c_str(), tx, ty, x, y);
}
