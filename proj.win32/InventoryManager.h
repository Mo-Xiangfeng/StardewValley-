#ifndef __INVENTORY_MANAGER_H__
#define __INVENTORY_MANAGER_H__

#include "Item.h"
#include <vector>
#include <map>
#include <memory>

class InventoryManager {
public:
    static InventoryManager* getInstance();

    void initInventory(int capacity);
    int getmoney(int index);
    bool removeItem(int index);
    bool addItem(std::shared_ptr<Item> newItem);
    bool addItemByID(int id, int count);

    const std::vector<std::shared_ptr<Item>>& getItems() const { return _inventory; }
    int getCapacity() const { return _capacity; }

    void loadItemDatabase();
    void swapItems(int idx1, int idx2);

    InventoryManager() : _capacity(20) {}
    static InventoryManager* _instance;

    void notifyUpdate();

    // 数据库和背包现在都存指针
    std::map<int, std::shared_ptr<Item>> _itemDatabase;
    std::vector<std::shared_ptr<Item>> _inventory;
    int _capacity;
};

#endif