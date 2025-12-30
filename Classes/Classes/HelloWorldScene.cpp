#include "HelloWorldScene.h"
#include "GameScene.h"
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
USING_NS_CC;

Scene* HelloWorldScene::createScene() {
    return HelloWorldScene::create();
}

bool HelloWorldScene::init() {
    if (!Scene::init()) return false;
    // 播放背景音乐
    // 参数1: 音乐文件路径； 参数2: 是否循环播放
    auto audio = SimpleAudioEngine::getInstance();
    audio->playBackgroundMusic("stardew.mp3", true);
    audio->setBackgroundMusicVolume(0.5f); // 设置音量 (0.0 ~ 1.0)

    // 修改开始按钮的回调
    auto startItem = MenuItemImage::create(
        "StartButton.png",
        "StartPressedButton.png",
        [](Ref* sender) {

            auto scene = GameScene::createScene();
            Director::getInstance()->replaceScene(TransitionFade::create(0.8f, scene));
        });
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("cover.png"); // 替换为你的文件名
    if (background) {
        // 设置位置为屏幕中心
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

        // 调整图片大小以适应屏幕
        float scaleX = visibleSize.width / background->getContentSize().width;
        float scaleY = visibleSize.height / background->getContentSize().height;
        background->setScale(scaleX, scaleY);

        // 将背景添加到 Scene，并设置 z-order 为 -1 确保它在最底层
        this->addChild(background, -1);
    }


    // 3. 创建“退出游戏”按钮
    auto exitItem = MenuItemImage::create(
        "ExitButton.png",
        "ExitPressedButton.png",
        [](Ref* sender) {
            // 点击后的回调：关闭程序
            Director::getInstance()->end();
        });

    // 4. 创建菜单容器并将按钮加入
    // Menu 默认会将内部元素居中，我们需要手动调整位置
    auto menu = Menu::create(startItem, exitItem, nullptr);
    menu->setPosition(Vec2::ZERO); // 将 Menu 坐标原点设为左下角，方便精确控制按钮位置
    this->addChild(menu);

    // 5. 调整按钮位置
    // 启动按钮放在屏幕中心偏下
    float marginX = visibleSize.width * 0.3f; // 距离左右边缘 20% 的宽度
    float marginBottom = 300.0f;              // 距离底部 100 像素

    // 1. “启动游戏”按钮：放在左下方
    // 坐标：(左侧边距, 底部边距)
    startItem->setPosition(Vec2(origin.x + marginX, origin.y + marginBottom));

    // 2. “退出游戏”按钮：放在右下方
    // 坐标：(总宽度 - 右侧边距, 底部边距)
    exitItem->setPosition(Vec2(origin.x + visibleSize.width - marginX, origin.y + marginBottom));

    return true;
}