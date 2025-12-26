#include "MoneyManager.h"

MoneyManager* MoneyManager::instance = nullptr;

MoneyManager::MoneyManager() : currentMoney(0) {}

MoneyManager* MoneyManager::getInstance() {
    if (!instance) {
        instance = new MoneyManager();
    }
    return instance;
}

void MoneyManager::init(int startMoney) {
    currentMoney = startMoney;
    CCLOG("Money initialized: $%d", currentMoney);
}

bool MoneyManager::addMoney(int amount) {
    if (amount < 0) return false;

    currentMoney += amount;
    CCLOG("Money added: +$%d, Total: $%d", amount, currentMoney);
    return true;
}

bool MoneyManager::spendMoney(int amount) {
    if (amount < 0 || currentMoney < amount) {
        CCLOG("Not enough money! Need: $%d, Have: $%d", amount, currentMoney);
        return false;
    }

    currentMoney -= amount;
    CCLOG("Money spent: -$%d, Remaining: $%d", amount, currentMoney);
    return true;
}

bool MoneyManager::hasMoney(int amount) const {
    return currentMoney >= amount;
}

void MoneyManager::save() {
    UserDefault::getInstance()->setIntegerForKey("player_money", currentMoney);
    UserDefault::getInstance()->flush();
}

void MoneyManager::load() {
    currentMoney = UserDefault::getInstance()->getIntegerForKey("player_money", 500);
    CCLOG("Money loaded: $%d", currentMoney);
}
