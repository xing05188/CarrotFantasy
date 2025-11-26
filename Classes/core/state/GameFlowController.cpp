#include "GameFlowController.h"

#include "GameState.h"
#include "GameStateProvider.h"
#include "LevelSelectState.h"
#include "MenuState.h"
#include "cocos2d.h"

namespace carrot {
namespace core {
namespace state {

namespace {
void LogMissingContext(const char* targetState) {
    CCLOG("GameFlowController: missing GameStateContext for %s transition", targetState);
}
}  // namespace

GameFlowController::GameFlowController(std::shared_ptr<GameStateContext> context) {
    SetContext(context);
}

void GameFlowController::SetContext(const std::shared_ptr<GameStateContext>& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    context_ = context;
}

void GameFlowController::TransitionToMenu() {
    auto context = EnsureContextLocked();
    if (!context) {
        LogMissingContext("Menu");
        return;
    }
    context->SetState(std::make_shared<MenuState>());
}

void GameFlowController::TransitionToLevelSelect() {
    auto context = EnsureContextLocked();
    if (!context) {
        LogMissingContext("LevelSelect");
        return;
    }
    context->SetState(std::make_shared<LevelSelectState>());
}

std::shared_ptr<GameStateContext> GameFlowController::EnsureContextLocked() {
    std::lock_guard<std::mutex> lock(mutex_);
    auto context = context_.lock();
    if (!context) {
        context = GameStateProvider::Get();
        context_ = context;
    }
    return context;
}

}  // namespace state
}  // namespace core
}  // namespace carrot


