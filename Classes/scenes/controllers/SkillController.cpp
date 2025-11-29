#include "SkillController.h"
#include "BaseLevelScene.h"
#include "core/GameManager.h"
#include "audio/music.h"

// 全局变量声明
extern float beishu;  // 怪物行进速度倍率
extern float tower_jiasu;  // 塔攻击速度倍率

SkillController::SkillController()
    : gameManager_(nullptr)
    , initialized_(false)
{
}

SkillController::~SkillController()
{
}

void SkillController::init(const SkillCallbacks& callbacks, GameManager* gameManager)
{
    callbacks_ = callbacks;
    gameManager_ = gameManager;
    initialized_ = true;
}

void SkillController::executeSkill1(Ref* pSender)
{
    if (!initialized_) {
        CCLOG("SkillController: Not initialized!");
        return;
    }

    Music::getInstance()->button_music();
    if (callbacks_.getMoney() >= 200) {
        callbacks_.updateMoney(-200);
        if (gameManager_) {
            gameManager_->Jineng1();
        }
    }
}

void SkillController::executeSkill2(Ref* pSender)
{
    if (!initialized_) {
        CCLOG("SkillController: Not initialized!");
        return;
    }

    Music::getInstance()->button_music();
    if (callbacks_.getMoney() >= 200) {
        callbacks_.updateMoney(-200);
        applyMonsterSpeedInternal(0.01f);
        beishu = 0.01f;
        
        auto delayaction = Sequence::create(
            DelayTime::create(5.0f),
            CallFunc::create([this] {
                applyMonsterSpeedInternal(1.0f);
                beishu = 1.0f;
            }),
            nullptr);
        callbacks_.runActionOnScene(delayaction);
    }
}

void SkillController::executeSkill3(Ref* pSender)
{
    if (!initialized_) {
        CCLOG("SkillController: Not initialized!");
        return;
    }

    Music::getInstance()->button_music();
    if (callbacks_.getMoney() >= 500) {
        auto bong = Sprite::create();
        if (!bong) {
            CCLOG("Failed to create bong sprite.");
            return;
        }
        bong->setPosition(480, 320);
        bong->setScale(2);
        callbacks_.addChildToScene(bong);
        
        cocos2d::Vector<cocos2d::SpriteFrame*> frames;
        for (int i = 0; i <= 3; ++i) {
            std::string frameName = "Carrot/bong/bong_" + std::to_string(i) + ".png";
            auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 164, 160));
            if (frame) {
                frames.pushBack(frame);
            }
            else {
                CCLOG("Failed to load frame: %s", frameName.c_str());
            }
        }
        if (frames.empty()) {
            CCLOG("No frames found for bong, skipping.");
            return;
        }
        auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
        auto animate = cocos2d::Animate::create(animation);
        auto onbong = cocos2d::CallFunc::create([bong]() {
            CCLOG("bong.");
            bong->removeFromParent();
        });
        Music::getInstance()->bongSound();
        bong->runAction(cocos2d::Sequence::create(animate, onbong, nullptr));
        
        callbacks_.updateMoney(-500);
        if (gameManager_) {
            gameManager_->KillAllMonsters();
        }
        else {
            auto instance = GameManager::getInstance();
            if (instance) {
                instance->KillAllMonsters();
            }
        }
    }
}

void SkillController::executeSkill4(Ref* pSender)
{
    if (!initialized_) {
        CCLOG("SkillController: Not initialized!");
        return;
    }

    Music::getInstance()->button_music();
    if (callbacks_.getMoney() >= 200) {
        callbacks_.updateMoney(-200);
        tower_jiasu = 2;
        auto delayaction = Sequence::create(
            DelayTime::create(5.0f),
            CallFunc::create([] {
                tower_jiasu = 1;
            }),
            nullptr);
        callbacks_.runActionOnScene(delayaction);
    }
}

void SkillController::executeSkill5(Ref* pSender)
{
    if (!initialized_) {
        CCLOG("SkillController: Not initialized!");
        return;
    }

    Music::getInstance()->button_music();
    if (callbacks_.getMoney() >= 150) {
        callbacks_.updateMoney(-150);
        applyMonsterSpeedInternal(0.5f);
        beishu = 0.5f;
        auto delayaction = Sequence::create(
            DelayTime::create(5.0f),
            CallFunc::create([this] {
                applyMonsterSpeedInternal(1.0f);
                beishu = 1.0f;
            }),
            nullptr);
        callbacks_.runActionOnScene(delayaction);
    }
}

void SkillController::executeSkill6(Ref* pSender)
{
    if (!initialized_) {
        CCLOG("SkillController: Not initialized!");
        return;
    }

    Music::getInstance()->button_music();
    if (callbacks_.getMoney() >= 200) {
        callbacks_.updateMoney(-200);
        if (gameManager_) {
            gameManager_->Jineng6();
        }
    }
}

void SkillController::applyMonsterSpeedInternal(float speedMultiplier)
{
    if (callbacks_.applyMonsterSpeed) {
        callbacks_.applyMonsterSpeed(speedMultiplier);
    }
    else if (gameManager_) {
        gameManager_->ApplyMonsterSpeed(speedMultiplier);
    }
    else {
        auto instance = GameManager::getInstance();
        if (instance) {
            instance->ApplyMonsterSpeed(speedMultiplier);
        }
    }
}

