#pragma once
#ifndef __BASE_NAVIGABLE_SCENE_H__
#define __BASE_NAVIGABLE_SCENE_H__

#include "cocos2d.h"
#include "audio/music.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class BaseNavigableScene : public Layer {
public:
    // 【模板方法】：定义了场景初始化的标准流程
    virtual bool init() override {
        if (!Layer::init()) return false;

        // 1. 初始化通用变量
        visibleSize = Director::getInstance()->getVisibleSize();
        origin = Director::getInstance()->getVisibleOrigin();
        screenSize = Director::getInstance()->getWinSize();

        // 2. 加载背景 (路径由子类提供)
        std::string bgPath = getBackgroundImagePath();
        if (!bgPath.empty()) {
            addBackground(bgPath);
        }

        // 3. 初始化菜单容器
        menu = Menu::create();
        menu->setPosition(Vec2::ZERO);
        this->addChild(menu, 1);

        // 4. 添加返回按钮
        addReturnButton();

        // 5. 【钩子方法】：子类在此处添加自己特有的 UI
        initCustomUI();

        return true;
    }

    // 通用的返回回调（默认是弹出当前场景）
    virtual void onReturn(Ref* pSender) {
        Music::getInstance()->button_music();
        Director::getInstance()->popScene();
    }

protected:
    // --- 必须由子类实现的方法 ---
    virtual std::string getBackgroundImagePath() = 0;

    // --- 子类可选择重写的钩子 ---
    virtual void initCustomUI() {} // 默认为空

    // --- 可配置项（子类可以通过重写这些函数来修改默认行为） ---
    virtual std::string getReturnBtnNormal() { return "CarrotGuardRes/UI/SettingReturnNormal.png"; }
    virtual std::string getReturnBtnSelected() { return "CarrotGuardRes/UI/SettingReturnSelected.png"; }
    virtual float getReturnBtnScale() { return 1.5f; } // 默认缩放

    // --- 通用功能实现 ---
    void addBackground(const std::string& path) {
        auto bg = Sprite::create(path);
        if (bg) {
            scaleX = screenSize.width / bg->getContentSize().width;
            scaleY = screenSize.height / bg->getContentSize().height;
            bg->setScaleX(scaleX);
            bg->setScaleY(scaleY);
            bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
            this->addChild(bg, 0);
        }
    }

    virtual void addReturnButton() {
        auto returnBtn = MenuItemImage::create(
            getReturnBtnNormal(),
            getReturnBtnSelected(),
            CC_CALLBACK_1(BaseNavigableScene::onReturn, this)); // 绑定到虚函数 onReturn
        
        if (returnBtn) {
            // 通用的位置计算逻辑
            float x = origin.x + returnBtn->getContentSize().width / 2 + visibleSize.width * 0.05f;
            float y = origin.y + visibleSize.height - returnBtn->getContentSize().width / 2 - visibleSize.height * 0.04f;
            
            returnBtn->setScale(getReturnBtnScale() * (scaleX > 0 ? scaleX : 1.0f)); // 适配缩放
            returnBtn->setPosition(Vec2(x, y));
            menu->addChild(returnBtn);
        }
    }

    // 成员变量
    Size visibleSize;
    Vec2 origin;
    Size screenSize;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    Menu* menu;
};

#endif // __BASE_NAVIGABLE_SCENE_H__