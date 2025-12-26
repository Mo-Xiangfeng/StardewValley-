#include "InventoryManager.h"
#include "CropData.h"
InventoryManager* InventoryManager::_instance = nullptr;

InventoryManager* InventoryManager::getInstance() {
    if (!_instance) {
        _instance = new InventoryManager();
        _instance->initInventory(20);
    }
    return _instance;
}

void InventoryManager::initInventory(int capacity) {
    _capacity = capacity;
    _inventory.clear();
    _inventory.resize(capacity, nullptr); // 初始化为 nullptr
}

bool InventoryManager::addItem(std::shared_ptr<Item> newItem) {
    if (!newItem || newItem->id == 0) return false;

    // 1. 尝试堆叠 (工具不堆叠)
    if (newItem->type != ItemType::TOOL) {
        for (auto& slot : _inventory) {
            if (slot && slot->id == newItem->id && slot->count < slot->maxStack) {
                int canAdd = slot->maxStack - slot->count;
                int toAdd = std::min(canAdd, newItem->count);
                slot->count += toAdd;
                newItem->count -= toAdd;
                if (newItem->count <= 0) return true;
            }
        }
    }

    // 2. 寻找空位
    for (auto& slot : _inventory) {
        if (slot == nullptr) {
            slot = newItem; // 直接接管指针
            return true;
        }
    }
    return false;
}

void InventoryManager::loadItemDatabase() {
    _itemDatabase.clear();

    // 使用 make_shared 创建并存入数据库
    /*auto parsnip = std::make_shared<Item>();
    parsnip->id = 1100;
    parsnip->name = "防风草种子";
    parsnip->iconPath = "item/1100.png";
    parsnip->maxStack = 99;
    parsnip->type = ItemType::SEED;
    _itemDatabase[parsnip->id] = parsnip;*/

    /*auto axe = std::make_shared<ToolItem>();
    axe->id = 1101;
    axe->name = "石斧";
    axe->iconPath = "item/1101.png";
    axe->type = ItemType::TOOL;
    _itemDatabase[axe->id] = axe;*/
}

bool InventoryManager::addItemByID(int id, int count) {
    auto it = _itemDatabase.find(id);
    if (it == _itemDatabase.end()) return false;

    // 调用 clone() 产生子类正确的副本
    auto newItem = it->second->clone();
    newItem->count = count;
    return this->addItem(newItem);
}

void InventoryManager::swapItems(int idx1, int idx2) {
    if (idx1 >= 0 && idx1 < _inventory.size() && idx2 >= 0 && idx2 < _inventory.size()) {
        std::swap(_inventory[idx1], _inventory[idx2]);
    }
}

bool InventoryManager::removeItem(int index) {
    if (index >= 0 && index < _inventory.size()) {
        // 由于使用了 shared_ptr，将其设为 nullptr 会自动减少引用计数
        // 如果没有其他地方引用该对象，它会被自动销毁
        _inventory[index] = nullptr;
        return true;
    }
    return false;
}