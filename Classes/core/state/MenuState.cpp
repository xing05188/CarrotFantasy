#include "MenuState.h"

#include "MenuScene.h"
#include "cocos2d.h"

namespace carrot {
namespace core {
namespace state {

void MenuState::Enter(GameStateContext& /*context*/) {
    auto director = cocos2d::Director::getInstance();
    auto scene = MenuScene::createScene();
    if (!director->getRunningScene()) {
        director->runWithScene(scene);
    } else {
        director->replaceScene(scene);
    }
}

void MenuState::Exit(GameStateContext& /*context*/) {
    // Menu state currently has no dedicated exit handling.
}

}  // namespace state
}  // namespace core
}  // namespace carrot


