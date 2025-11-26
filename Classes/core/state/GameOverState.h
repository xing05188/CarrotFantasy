#pragma once

#include "GameState.h"

#include <memory>

#include "cocos2d.h"

namespace carrot {
namespace core {
namespace state {

class GameOverState : public GameState, public std::enable_shared_from_this<GameOverState> {
public:
    GameOverState(bool isWin, int levelId, int currentWave, int allWave);

    void Enter(GameStateContext& context) override;
    void Exit(GameStateContext& context) override;

private:
    void buildWinUI();
    void buildLoseUI();
    void addLevelSelectButton(cocos2d::Menu* menu);
    void transitionToLevelSelect();
    void handleWinContinue();
    void restartLevel();
    void replaceWithLevel(int levelId);

    bool isWin_;
    int levelId_;
    int currentWave_;
    int allWave_;
    cocos2d::Node* overlay_ = nullptr;
    cocos2d::Menu* menu_ = nullptr;
    cocos2d::EventListener* overlayTouchListener_ = nullptr;
};

}  // namespace state
}  // namespace core
}  // namespace carrot


