#pragma once
#include "cocos2d.h"

// 移除 Player.h 的包含，除非 Monster 内部逻辑必须依赖 Player 类
// 这里通常不需要，因为 AI 逻辑在 Scene 层处理了位置对比
class Monster : public cocos2d::Sprite
{
public:
    // 静态创建方法
    static Monster* createWithSpriteFrameName(const std::string& spriteFrameName);

    // 初始化
    virtual bool initWithSpriteFrameName(const std::string& spriteFrameName) override;

    // 每帧更新
    virtual void update(float dt) override;

    // 受伤逻辑
    void takeDamage(int damage);

    // --- 属性变量 (公开以便 Scene 访问) ---
    int maxHP;
    int currentHP;
    int attackPower;
    float _stunTimer; // 【新增】受击硬直计时器

    // --- AI 相关变量 ---
    float _detectRange;       // 侦测玩家的范围
    float _attackTimer;       // 攻击冷却
    float _wanderTimer;       // 游荡计时器
    float _moveSpeed;         // 移动速度
    cocos2d::Vec2 _wanderDirection; // 当前移动方向

private:
    void createAndRunSlimeAnimation();
};