// Refactored with State Pattern
// 使用状态模式重构
#pragma once

#include "GameState.h"

namespace carrot {
namespace core {
namespace state {

class PausedState : public GameState {
public:
    void Enter(GameStateContext& context) override;
    void Exit(GameStateContext& context) override;
};

}  // namespace state
}  // namespace core
}  // namespace carrot

