#include "LevelState.h"

namespace carrot {
namespace core {
namespace state {

LevelState::LevelState(cocos2d::Scene* levelScene) : levelScene_(levelScene) {
    if (levelScene_) {
        levelScene_->retain();
    }
}

LevelState::~LevelState() {
    if (levelScene_) {
        levelScene_->release();
        levelScene_ = nullptr;
    }
}

void LevelState::Enter(GameStateContext& /*context*/) {
    if (!levelScene_) {
        CCLOG("LevelState::Enter: scene is null");
        return;
    }
    auto director = cocos2d::Director::getInstance();
    if (!director->getRunningScene()) {
        director->runWithScene(levelScene_);
    } else {
        director->replaceScene(levelScene_);
    }
}

void LevelState::Exit(GameStateContext& /*context*/) {
    // Future hook: snapshot, analytics, cleanup, etc.
}

}  // namespace state
}  // namespace core
}  // namespace carrot


