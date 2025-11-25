#pragma once

#include <memory>

namespace carrot {
namespace core {
namespace state {

class GameStateContext;

class GameState {
public:
    virtual ~GameState() {}
    virtual void Enter(GameStateContext& context) = 0;
    virtual void Exit(GameStateContext& context) = 0;
    virtual void Pause(GameStateContext& context) {}
    virtual void Resume(GameStateContext& context) {}
};

class GameStateContext {
public:
    GameStateContext();

    void SetState(std::shared_ptr<GameState> state);
    void Pause();
    void Resume();

private:
    std::shared_ptr<GameState> currentState_;
};

}  // namespace state
}  // namespace core
}  // namespace carrot

