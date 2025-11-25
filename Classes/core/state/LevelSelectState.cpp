#include "LevelSelectState.h"

#include "themeScene.h"
#include "cocos2d.h"

namespace carrot {
namespace core {
namespace state {

void LevelSelectState::Enter(GameStateContext& /*context*/) {
    auto director = cocos2d::Director::getInstance();
    auto scene = themescene::createScene();
    if (!director->getRunningScene()) {
        director->runWithScene(scene);
    } else {
        director->replaceScene(scene);
    }
}

void LevelSelectState::Exit(GameStateContext& /*context*/) {
    // Level select currently has no exit logic.
}

}  // namespace state
}  // namespace core
}  // namespace carrot


