#ifndef __ITEM_H__
#define __ITEM_H__

#include "cocos2d.h"
#include <string>
#include <memory>

enum class ItemType { TOOL, CONSUMABLE, MATERIAL, SEED };

class Item {
public:
    int id;
    std::string name;
    std::string iconPath;
    std::string description;
    ItemType type;
    int count;
    int maxStack;

    Item() : id(0), name(""), iconPath(""), type(ItemType::MATERIAL), count(0), maxStack(1) {}

    // 必须有虚析构函数，确保子类内存正确释放
    virtual ~Item() {}

    // 虚函数：子类重写具体逻辑
    virtual void use() {
    }

    // 克隆函数：确保从数据库拷贝时保留子类身份
    virtual std::shared_ptr<Item> clone() const {
        return std::make_shared<Item>(*this);
    }
};

// 工具类子类
class ToolItem : public Item {
public:
    int durability;
    ToolItem() : durability(100) { type = ItemType::TOOL; }

    virtual void use() override {
    }

    virtual std::shared_ptr<Item> clone() const override {
        return std::make_shared<ToolItem>(*this);
    }
};
#endif