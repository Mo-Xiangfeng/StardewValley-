#ifndef __MONEY_MANAGER_H__
#define __MONEY_MANAGER_H__

#include "cocos2d.h"
USING_NS_CC;

class MoneyManager {
private:
    static MoneyManager* instance;
    int currentMoney;

    MoneyManager();

public:
    static MoneyManager* getInstance();

    // 金钱操作
    void init(int startMoney = 500);
    int getMoney() const { return currentMoney; }
    bool addMoney(int amount);
    bool spendMoney(int amount);
    bool hasMoney(int amount) const;

    // 保存/加载（可选）
    void save();
    void load();
};

#endif
