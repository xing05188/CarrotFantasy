#pragma once
#ifndef __SEE_MENU_H__
#define __SEE_MENU_H__

#include "BaseNavigableScene.h"

class seescene : public BaseNavigableScene {
public:
    static Scene* createScene() {
        auto scene = Scene::create();
        auto layer = seescene::create();
        scene->addChild(layer);
        return scene;
    };
    CREATE_FUNC(seescene);

protected:
    // 实现基类接口
    virtual std::string getBackgroundImagePath() override {
        return "see.png"; 
    }
};

#endif // __SEE_MENU_H__

