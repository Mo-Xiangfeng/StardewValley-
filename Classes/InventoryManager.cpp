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
    auto stone = std::make_shared<Item>();
    stone->id = 4402;
    stone->name = "石头";
    stone->iconPath = "item/4402.png";
    stone->maxStack = 99;
    stone->price = 80;
    stone->type = ItemType::MATERIAL;
    _itemDatabase[stone->id] = stone;

    auto slime_mud = std::make_shared<Item>();
    slime_mud->id = 4401;
    slime_mud->name = "史莱姆泥";
    slime_mud->iconPath = "item/4401.png";
    slime_mud->maxStack = 99;
    slime_mud->price = 80;
    slime_mud->type = ItemType::MATERIAL;
    _itemDatabase[slime_mud->id] = slime_mud;

    auto fish_1 = std::make_shared<Item>();
    fish_1->id = 3126;
    fish_1->name = "鱼获_1";
    fish_1->iconPath = "item/3126.png";
    fish_1->maxStack = 99;
    fish_1->price = 100;
    fish_1->type = ItemType::MATERIAL;
    _itemDatabase[fish_1->id] = fish_1;

    auto fish_2 = std::make_shared<Item>();
    fish_2->id = 3125;
    fish_2->name = "鱼获_2";
    fish_2->iconPath = "item/3125.png";
    fish_2->maxStack = 99;
    fish_2->price = 150;
    fish_2->type = ItemType::MATERIAL;
    _itemDatabase[fish_2->id] = fish_2;

    auto fish_3 = std::make_shared<Item>();
    fish_3->id = 3124;
    fish_3->name = "鱼获_3";
    fish_3->iconPath = "item/3120.png";
    fish_3->maxStack = 99;
    fish_3->price = 10;
    fish_3->type = ItemType::MATERIAL;
    _itemDatabase[fish_3->id] = fish_3;

    auto wood = std::make_shared<Item>();
    wood->id = 4400;
    wood->name = "木头";
    wood->iconPath = "item/4400.png";
    wood->maxStack = 99;
    wood->price = 20;
    wood->type = ItemType::MATERIAL;
    _itemDatabase[wood->id] = wood;

    auto parsnip = std::make_shared<Item>();
    parsnip->id = 2200;
    parsnip->name = "防风草果实";
    parsnip->iconPath = "item/2200.png";
    parsnip->maxStack = 99;
    parsnip->price = 40;
    parsnip->type = ItemType::MATERIAL;
    _itemDatabase[parsnip->id] = parsnip;

    auto cauliflower = std::make_shared<Item>();
    cauliflower->id = 2201;
    cauliflower->name = "西蓝花果实";
    cauliflower->iconPath = "item/2201.png";
    cauliflower->maxStack = 99;
    cauliflower->price = 180;
    cauliflower->type = ItemType::MATERIAL;
    _itemDatabase[cauliflower->id] = cauliflower;

    auto potato = std::make_shared<Item>();
    potato->id = 2202;
    potato->name = "成熟的土豆";
    potato->iconPath = "item/2202.png";
    potato->maxStack = 99;
    potato->price = 80;
    potato->type = ItemType::MATERIAL;
    _itemDatabase[potato->id] = potato;

    auto tomato = std::make_shared<Item>();
    tomato->id = 2203;
    tomato->name = "成熟的西红柿";
    tomato->iconPath = "item/2203.png";
    tomato->maxStack = 99;
    tomato->price = 90;
    tomato->type = ItemType::MATERIAL;
    _itemDatabase[tomato->id] = tomato;

    auto melon = std::make_shared<Item>();
    melon->id = 2204;
    melon->name = "成熟的甜瓜";
    melon->iconPath = "item/2204.png";
    melon->maxStack = 99;
    melon->price = 120;
    melon->type = ItemType::MATERIAL;
    _itemDatabase[melon->id] = melon;

    auto pumpkin = std::make_shared<Item>();
    pumpkin->id = 2205;
    pumpkin->name = "成熟的南瓜";
    pumpkin->iconPath = "item/2205.png";
    pumpkin->maxStack = 99;
    pumpkin->price = 200;
    pumpkin->type = ItemType::MATERIAL;
    _itemDatabase[pumpkin->id] = pumpkin;

    auto wheat = std::make_shared<Item>();
    wheat->id = 2206;
    wheat->name = "成熟的小麦";
    wheat->iconPath = "item/2206.png";
    wheat->maxStack = 99;
    wheat->price = 30;
    wheat->type = ItemType::MATERIAL;
    _itemDatabase[wheat->id] = wheat;

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
    kettle->name = "水壶";
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

bool InventoryManager::hasAllDatabaseItems() {
    // 使用传统的迭代器遍历 std::map
    std::map<int, std::shared_ptr<Item>>::iterator it;

    for (it = _itemDatabase.begin(); it != _itemDatabase.end(); ++it) {
        // it->first 是 Key (物品ID), it->second 是 Value (物品对象指针)
        int dbID = it->first;

        // 过滤掉 ID 为 0 的无效物品
        if (dbID <= 0) continue;

        bool found = false;

        // 使用原始下标遍历背包 vector
        for (int i = 0; i < (int)_inventory.size(); ++i) {
            if (_inventory[i] != NULL && _inventory[i]->id == dbID) {
                // 只要背包里有这个 ID，且数量大于 0 (假设有就是至少1个)
                if (_inventory[i]->count > 0) {
                    found = true;
                    break;
                }
            }
        }

        // 只要有一个数据库里的物品没找到，立即返回失败
        if (!found) {
            CCLOG("Sacrifice Failed: Missing Item ID %d", dbID);
            return false;
        }
    }

    return true; // 全部通过
}