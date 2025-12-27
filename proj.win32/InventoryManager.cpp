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
                notifyUpdate();
                if (newItem->count <= 0) return true;
            }
        }
    }

    // 2. 寻找空位
    for (auto& slot : _inventory) {
        if (slot == nullptr) {
            slot = newItem; // 直接接管指针
            notifyUpdate();
            return true;
        }
    }
    return false;
}

void InventoryManager::loadItemDatabase() {
    _itemDatabase.clear();

    // 使用 make_shared 创建并存入数据库
    auto parsnip = std::make_shared<Item>();
    parsnip->id = 2200;
    parsnip->name = "防风草果实";
    parsnip->iconPath = "item/2200.png";
    parsnip->maxStack = 99;
    parsnip->price = 40;
    parsnip->type = ItemType::MATERIAL;
    _itemDatabase[parsnip->id] = parsnip;

    auto cauliflower = std::make_shared<Item>();
    cauliflower->id = 2207;
    cauliflower->name = "西蓝花果实";
    cauliflower->iconPath = "item/2207.png";
    cauliflower->maxStack = 99;
    cauliflower->price = 150;
    cauliflower->type = ItemType::MATERIAL;
    _itemDatabase[cauliflower->id] = cauliflower;

    auto wheat = std::make_shared<Item>();
    wheat->id = 2201;
    wheat->name = "成熟的小麦";
    wheat->iconPath = "item/2201.png";
    wheat->maxStack = 99;
    wheat->price = 20;
    wheat->type = ItemType::MATERIAL;
    _itemDatabase[wheat->id] = wheat;

    auto potato = std::make_shared<Item>();
    potato->id = 2202;
    potato->name = "成熟的土豆";
    potato->iconPath = "item/2202.png";
    potato->maxStack = 99;
    potato->price = 20;
    potato->type = ItemType::MATERIAL;
    _itemDatabase[potato->id] = potato;

    auto axe = std::make_shared<ToolItem>();
    axe->id = 1110;
    axe->name = "石斧";
    axe->iconPath = "item/1110.png";
    axe->type = ItemType::TOOL;
    _itemDatabase[axe->id] = axe;

    auto pole = std::make_shared<ToolItem>();
    pole->id = 1111;
    pole->name = "钓鱼竿";
    pole->iconPath = "item/1111.png";
    pole->type = ItemType::TOOL;
    _itemDatabase[pole->id] = pole;

    auto hoe = std::make_shared<ToolItem>();
    hoe->id = 1112;
    hoe->name = "锄头";
    hoe->iconPath = "item/1112.png";
    hoe->type = ItemType::TOOL;
    _itemDatabase[hoe->id] = hoe;

    auto kettle = std::make_shared<ToolItem>();
    kettle->id = 1113;
    kettle->name = "锄头";
    kettle->iconPath = "item/1113.png";
    kettle->type = ItemType::TOOL;
    _itemDatabase[kettle->id] = kettle;
}

bool InventoryManager::addItemByID(int id, int count) {
    auto it = _itemDatabase.find(id);
    if (it == _itemDatabase.end()) return false;

    // 调用 clone() 产生子类正确的副本
    auto newItem = it->second->clone();
    newItem->count = count;
    notifyUpdate();
    return this->addItem(newItem);
}

void InventoryManager::swapItems(int idx1, int idx2) {
    if (idx1 >= 0 && idx1 < _inventory.size() && idx2 >= 0 && idx2 < _inventory.size()) {
        std::swap(_inventory[idx1], _inventory[idx2]);
        notifyUpdate();
    }
}

bool InventoryManager::removeItem(int index, int num) {
    // 1. 检查索引合法性
    if (index >= 0 && index < _inventory.size() && _inventory[index] != nullptr) {

        // 2. 如果 num 为 0，表示直接清空该格子
        if (num == 0) {
            _inventory[index] = nullptr;
            notifyUpdate();
            return true;
        }
        else {
            // 3. 检查数量是否足够扣除
            if (_inventory[index]->count >= num) {
                _inventory[index]->count -= num;

                // --- 新增逻辑：如果扣除后数量为 0，则清空格子 ---
                if (_inventory[index]->count <= 0) {
                    _inventory[index] = nullptr;
                }

                notifyUpdate();
                return true;
            }
        }
    }
    return false;
}

bool InventoryManager::removeItemByID(int id, int num) {
    for (int i = 0; i < _inventory.size(); ++i) {
        if (_inventory[i] && _inventory[i]->id == id) {
            return removeItem(i, num); // 复用上面的逻辑
        }
    }
    return false;
}

int InventoryManager::getmoney(int index) {
    if (index >= 0 && index < _inventory.size() && _inventory[index] != nullptr) {
        return _inventory[index]->price * _inventory[index]->count;
    }
    return 0;
}

void InventoryManager::notifyUpdate() {
    // 发送一个自定义事件，名字可以叫 "inventory_changed"
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("inventory_changed");
}