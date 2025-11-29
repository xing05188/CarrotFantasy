#include "GameSpeedController.h"
#include "audio/music.h"

// 全局变量声明
extern float beishu;  // 怪物行进速度倍率
extern float tower_jiasu;  // 塔攻击速度倍率

GameSpeedController::GameSpeedController()
    : initialized_(false)
    , isDoubleSpeed_(false)
{
}

GameSpeedController::~GameSpeedController()
{
}

void GameSpeedController::init(const SpeedCallbacks& callbacks)
{
    callbacks_ = callbacks;
    initialized_ = true;
}

void GameSpeedController::toggleSpeed(Ref* pSender)
{
    if (!initialized_) {
        CCLOG("GameSpeedController: Not initialized!");
        return;
    }

    isDoubleSpeed_ = !isDoubleSpeed_;
    Music::getInstance()->button_music();
    
    MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
    if (isDoubleSpeed_) {
        setSpeedMultiplier(2.0f, button);
    } else {
        setSpeedMultiplier(1.0f, button);
    }
}

void GameSpeedController::setSpeedMultiplier(float multiplier, MenuItemImage* button)
{
    if (multiplier == 2.0f) {
        // 切换到 2倍速
        auto normalSprite = Sprite::create("CarrotGuardRes/UI/doubleSpeed.png");
        auto selectedSprite = Sprite::create("CarrotGuardRes/UI/doubleSpeed.png");
        button->setNormalImage(normalSprite);
        button->setSelectedImage(selectedSprite);
        tower_jiasu = 2;
        beishu = 2;
    } else {
        // 切换到 1倍速
        auto normalSprite = Sprite::create("CarrotGuardRes/UI/normalSpeed.png");
        auto selectedSprite = Sprite::create("CarrotGuardRes/UI/normalSpeed.png");
        button->setNormalImage(normalSprite);
        button->setSelectedImage(selectedSprite);
        tower_jiasu = 1;
        beishu = 1;
    }
    
    // 应用怪物速度倍率
    if (callbacks_.applyMonsterSpeed) {
        callbacks_.applyMonsterSpeed(multiplier);
    }
}

