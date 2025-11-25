#pragma once

#include "GameState.h"

namespace carrot {
namespace core {
namespace state {

// State Pattern: Level selection screen.
class LevelSelectState : public GameState {
public:
    void Enter(GameStateContext& context) override;
    void Exit(GameStateContext& context) override;
};

}  // namespace state
}  // namespace core
}  // namespace carrot


