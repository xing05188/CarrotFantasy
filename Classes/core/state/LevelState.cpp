/**
 * Refactored with State Pattern
 * 使用状态模式重构
 * 
 * Design Pattern: State Pattern
 * 设计模式：状态模式
 * 
 * Benefits:
 * 1. Allows the game to change its behavior when in level state
 * 2. Encapsulates level-specific behavior into a separate state class
 * 3. Makes state transitions explicit and easier to manage
 * 4. Eliminates large conditional statements that depend on the game's state
 * 
 * 好处：
 * 1. 允许游戏在关卡状态下改变其行为
 * 2. 将关卡特定行为封装到单独的状态类中
 * 3. 使状态转换显式且更易于管理
 * 4. 消除了依赖于游戏状态的大型条件语句
 * 
 * Problems Solved:
 * 1. Avoids code duplication when implementing level-specific behaviors
 * 2. Makes it easier to modify level behavior without affecting other states
 * 3. Provides a clean way to manage transitions to and from the level state
 * 4. Separates level logic from the main game object, improving maintainability
 * 
 * 解决的问题：
 * 1. 避免在实现关卡特定行为时的代码重复
 * 2. 使修改关卡行为而不影响其他状态变得更容易
 * 3. 提供了一种管理进出关卡状态转换的清晰方式
 * 4. 将关卡逻辑与主游戏对象分离，提高了可维护性
 */
 // Refactored with State Pattern
 // 使用状态模式重构
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


