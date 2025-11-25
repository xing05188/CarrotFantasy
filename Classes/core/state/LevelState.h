#pragma once

#include "GameState.h"

#include "cocos2d.h"

namespace carrot {
namespace core {
namespace state {

class LevelState : public GameState {
public:
    explicit LevelState(cocos2d::Scene* levelScene);
    ~LevelState() override;

    void Enter(GameStateContext& context) override;
    void Exit(GameStateContext& context) override;

private:
    cocos2d::Scene* levelScene_;
};

}  // namespace state
}  // namespace core
}  // namespace carrot


