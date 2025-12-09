// Refactored with State Pattern
// 使用状态模式重构
#pragma once

#include "GameState.h"

namespace carrot {
namespace core {
namespace state {

class LoadingState : public GameState {
public:
    explicit LoadingState(int levelId);

    void Enter(GameStateContext& context) override;
    void Exit(GameStateContext& context) override;

private:
    int levelId_;
};

}  // namespace state
}  // namespace core
}  // namespace carrot


