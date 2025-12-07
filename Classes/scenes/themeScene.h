#pragma once
#ifndef __THEME_SCENE_H__
#define __THEME_SCENE_H__

#include "BaseNavigableScene.h"

class themescene : public BaseNavigableScene {
public:
    static Scene* createScene() {
        auto scene = Scene::create();
        auto layer = themescene::create();
        scene->addChild(layer);
        return scene;
    };
    CREATE_FUNC(themescene);
    
    void clearRelatedButtons();
    void know_Callback(Ref* pSender);
    void loadGameState();

protected:
    virtual std::string getBackgroundImagePath() override {
        return "CarrotGuardRes/UI/AdvantureBackground.png";
    }

    // 覆盖默认的返回回调：themeScene 返回是 popScene，和默认一样
    // 但如果有特殊逻辑，可以重写 onReturn
    
    // 覆盖返回按钮样式
    virtual std::string getReturnBtnNormal() override { return "CarrotGuardRes/UI/ReturnButtonNormal.png"; }
    virtual std::string getReturnBtnSelected() override { return "CarrotGuardRes/UI/ReturnButtonSelected.png"; }
    virtual float getReturnBtnScale() override { return 3.0f * scaleX; }

    virtual void initCustomUI() override;
};

#endif
