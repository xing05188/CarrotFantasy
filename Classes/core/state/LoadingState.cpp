// Refactored with State Pattern
// 使用状态模式重构
#include "LoadingState.h"

#include "BaseLevelScene.h"
#include "LevelState.h"
#include "cocos2d.h"

namespace carrot {
namespace core {
namespace state {

LoadingState::LoadingState(int levelId) : levelId_(levelId) {}

void LoadingState::Enter(GameStateContext& context) {
    auto director = cocos2d::Director::getInstance();
    auto scene = BaseLevelScene::createScene(levelId_);
    if (!scene) {
        CCLOG("LoadingState: failed to create level scene for %d", levelId_);
        return;
    }
    context.SetState(std::make_shared<LevelState>(scene));
}

void LoadingState::Exit(GameStateContext& /*context*/) {
    // Loading-specific cleanup hook.
}

}  // namespace state
}  // namespace core
}  // namespace carrot


