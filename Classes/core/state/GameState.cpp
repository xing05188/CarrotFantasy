#include "GameState.h"

namespace carrot {
namespace core {
namespace state {

GameStateContext::GameStateContext() {}

void GameStateContext::SetState(std::shared_ptr<GameState> state) {
    if (currentState_) {
        currentState_->Exit(*this);
    }
    currentState_ = state;
    if (currentState_) {
        currentState_->Enter(*this);
    }
}

void GameStateContext::Pause() {
    if (currentState_) {
        currentState_->Pause(*this);
    }
}

void GameStateContext::Resume() {
    if (currentState_) {
        currentState_->Resume(*this);
    }
}

}  // namespace state
}  // namespace core
}  // namespace carrot


