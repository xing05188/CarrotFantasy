#pragma once

class GameManager;

class GameStateChecker {
public:
    explicit GameStateChecker(GameManager* owner);

    void Reset();
    bool CheckLose();
    bool CheckWin();
    void PublishGameWonEvent();
    void PublishGameLostEvent();

private:
    GameManager* owner_;
    bool hasGameWon_ = false;
    bool hasGameLost_ = false;
};

