#pragma once

#include "GameState.h"

namespace carrot {
namespace core {
namespace state {

// State Pattern: Menu screen controller.
class MenuState : public GameState {
public:
    void Enter(GameStateContext& context) override;
    void Exit(GameStateContext& context) override;
};

}  // namespace state
}  // namespace core
}  // namespace carrot


