#include "GameOverState.h"

#include <algorithm>

#include "BaseLevelScene.h"
#include "GameManager.h"
#include "core/state/GameFlowController.h"
#include "core/state/GameFlowProvider.h"
#include "core/state/GameStateProvider.h"
#include "core/state/LevelSelectState.h"
#include "audio/music.h"

extern bool level_is_win[3];

namespace carrot {
namespace core {
namespace state {

namespace {
constexpr int kOverlayZOrder = 1000;
constexpr int kTotalLevels = 3;

void stopGameplayLoops() {
    auto manager = GameManager::getInstance();
    if (manager) {
        manager->stopAllSchedulers();
        manager->removeListener();
    }
}

std::shared_ptr<GameFlowController> ResolveFlowController() {
    auto controller = GameFlowProvider::Get();
    if (!controller) {
        CCLOG("GameOverState: GameFlowController unavailable, using legacy state switches");
    }
    return controller;
}
}  // namespace

GameOverState::GameOverState(bool isWin, int levelId, int currentWave, int allWave)
    : isWin_(isWin),
      levelId_(levelId),
      currentWave_(currentWave),
      allWave_(allWave) {}

void GameOverState::Enter(GameStateContext& /*context*/) {
    auto director = cocos2d::Director::getInstance();
    auto runningScene = director->getRunningScene();
    if (!runningScene) {
        return;
    }

    stopGameplayLoops();

    if (auto baseScene = dynamic_cast<BaseLevelScene*>(runningScene)) {
        baseScene->unscheduleUpdate();
        auto manager = GameManager::getInstance();
        if (manager) {
            manager->saveGameState();
        }
    }

    overlay_ = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 0));
    overlay_->setName("GameOverOverlay");
    runningScene->addChild(overlay_, kOverlayZOrder);

    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](cocos2d::Touch*, cocos2d::Event*) {
        return true;
    };
    overlayTouchListener_ = listener;
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, overlay_);

    menu_ = cocos2d::Menu::create();
    menu_->setPosition(cocos2d::Vec2::ZERO);
    overlay_->addChild(menu_, 1);

    if (isWin_) {
        if (levelId_ > 0 && levelId_ <= 3) {
            level_is_win[levelId_ - 1] = true;
        }
        Music::getInstance()->carrotSound();
        buildWinUI();
    } else {
        Music::getInstance()->downSound();
        buildLoseUI();
    }

    addLevelSelectButton(menu_);
}

void GameOverState::Exit(GameStateContext& /*context*/) {
    if (overlayTouchListener_) {
        cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(overlayTouchListener_);
        overlayTouchListener_ = nullptr;
    }
    if (overlay_ && overlay_->getParent()) {
        overlay_->removeFromParent();
    }
    overlay_ = nullptr;
    menu_ = nullptr;
}

void GameOverState::buildWinUI() {
    if (!menu_) {
        return;
    }
    auto background = cocos2d::Sprite::create("CarrotGuardRes/UI/WinGame.png");
    background->setPosition({480.f, 320.f});
    background->setScale(1.5f);
    overlay_->addChild(background, 0);

    auto goldenCarrot = cocos2d::Sprite::create("CarrotGuardRes/UI/goldenCarrot.png");
    goldenCarrot->setPosition({960.f * 0.493f, 640.f * 0.7f});
    overlay_->addChild(goldenCarrot, 0);

    const int shownWave = std::min(currentWave_, allWave_) + 1;
    auto waveLabel = cocos2d::Label::createWithSystemFont(
        cocos2d::StringUtils::format("%d", shownWave),
        "Arial",
        32);
    waveLabel->setColor(cocos2d::Color3B::YELLOW);
    waveLabel->setPosition({960.f * 0.51f, 640.f * 0.54f});
    overlay_->addChild(waveLabel, 10);

    auto left = cocos2d::Label::createWithSystemFont("fought off", "Arial", 30);
    left->setPosition({960.f * 0.36f, 640.f * 0.54f});
    overlay_->addChild(left, 10);

    auto right = cocos2d::Label::createWithSystemFont("waves", "Arial", 30);
    right->setPosition({960.f * 0.60f, 640.f * 0.545f});
    overlay_->addChild(right, 10);

    auto continueButton = cocos2d::MenuItemImage::create(
        "CarrotGuardRes/UI/continueNormal.png",
        "CarrotGuardRes/UI/continueSelected.png",
        [this](cocos2d::Ref*) {
            Music::getInstance()->button_music();
            handleWinContinue();
        });
    continueButton->setPosition({960.f * 0.613f, 640.f * 0.375f});
    continueButton->setScale(1.38f);
    menu_->addChild(continueButton, 1);
}

void GameOverState::buildLoseUI() {
    if (!menu_) {
        return;
    }
    auto background = cocos2d::Sprite::create("CarrotGuardRes/UI/LoseGame.png");
    background->setPosition({960.f / 2.f + 960.f * 0.01f, 640.f / 2.f + 640.f * 0.015f});
    background->setScale(1.5f);
    overlay_->addChild(background, 0);

    auto waveLabel = cocos2d::Label::createWithSystemFont(
        cocos2d::StringUtils::format("%d", std::max(currentWave_, 0)),
        "Arial",
        32);
    waveLabel->setColor(cocos2d::Color3B::YELLOW);
    waveLabel->setPosition({960.f * 0.51f, 640.f * 0.54f});
    overlay_->addChild(waveLabel, 10);

    auto left = cocos2d::Label::createWithSystemFont("fought off", "Arial", 30);
    left->setPosition({960.f * 0.36f, 640.f * 0.54f});
    overlay_->addChild(left, 10);

    auto right = cocos2d::Label::createWithSystemFont("waves", "Arial", 30);
    right->setPosition({960.f * 0.60f, 640.f * 0.54f});
    overlay_->addChild(right, 10);

    auto againButton = cocos2d::MenuItemImage::create(
        "CarrotGuardRes/UI/AgainNormal.png",
        "CarrotGuardRes/UI/AgainSelected.png",
        [this](cocos2d::Ref*) {
            Music::getInstance()->button_music();
            restartLevel();
        });
    againButton->setPosition({960.f * 0.61f, 640.f * 0.37f});
    againButton->setScale(0.9f);
    menu_->addChild(againButton, 1);
}

void GameOverState::addLevelSelectButton(cocos2d::Menu* menu) {
    if (!menu) {
        return;
    }
    auto chooseButton = cocos2d::MenuItemImage::create(
        "CarrotGuardRes/UI/chooseLevelNormal.png",
        "CarrotGuardRes/UI/chooseLevelSelected.png",
        [this](cocos2d::Ref*) {
            Music::getInstance()->button_music();
            transitionToLevelSelect();
        });
    chooseButton->setPosition({960.f * 0.38f, 640.f * 0.37f});
    chooseButton->setScale(1.4f);
    menu->addChild(chooseButton, 1);
}

void GameOverState::transitionToLevelSelect() {
    if (auto controller = ResolveFlowController()) {
        controller->TransitionToLevelSelect();
        return;
    }

    auto context = GameStateProvider::Get();
    if (context) {
        context->SetState(std::make_shared<LevelSelectState>());
    }
}

void GameOverState::handleWinContinue() {
    if (levelId_ > 0 && levelId_ < kTotalLevels) {
        replaceWithLevel(levelId_ + 1);
    } else {
        transitionToLevelSelect();
    }
}

void GameOverState::restartLevel() {
    replaceWithLevel(levelId_);
}

void GameOverState::replaceWithLevel(int levelId) {
    if (auto controller = ResolveFlowController()) {
        controller->StartLoadingLevel(levelId);
        return;
    }

    auto director = cocos2d::Director::getInstance();
    auto scene = BaseLevelScene::createScene(levelId);
    director->replaceScene(scene);
}

}  // namespace state
}  // namespace core
}  // namespace carrot


