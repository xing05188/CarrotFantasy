#pragma once
#ifndef __SET_SCENE_H__
#define __SET_SCENE_H__

#include "BaseNavigableScene.h"

class setscene : public BaseNavigableScene {
public:
    static Scene* createScene() {
        auto scene = Scene::create();
        auto layer = setscene::create();
        scene->addChild(layer);
        return scene;
    };
    CREATE_FUNC(setscene);
    
    // 特有回调保留
    void paly_music(Ref* pSender);

protected:
    virtual std::string getBackgroundImagePath() override {
        return "CarrotGuardRes/UI/SettingBackground.png";
    }
    
    // 覆盖默认缩放，因为原代码里设置界面的返回按钮缩放是 1.8
    virtual float getReturnBtnScale() override { return 1.8f; }

    // 在这里写特有的 UI 初始化
    virtual void initCustomUI() override;
};

#endif // __SET_SCENE_H__