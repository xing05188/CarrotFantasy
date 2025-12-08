#pragma once
#ifndef __ADVANTURE_SCENE_H__
#define __ADVANTURE_SCENE_H__

#include "BaseNavigableScene.h"

class advanturescene : public BaseNavigableScene {
public:
    static Scene* createScene() {
        auto scene = Scene::create();
        auto layer = advanturescene::create();
        scene->addChild(layer);
        return scene;
    };
    CREATE_FUNC(advanturescene);
    
    void clearRelatedButtons();
    void know_Callback(Ref* pSender);

protected:
    virtual std::string getBackgroundImagePath() override {
        return "CarrotGuardRes/UI/AdvantureBackground.png";
    }

    virtual void initCustomUI() override;
};

#endif

