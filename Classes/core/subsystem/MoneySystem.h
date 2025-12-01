#pragma once

#include <cstddef>

class GameManager;

class MoneySystem {
public:
    explicit MoneySystem(GameManager* owner);

    int GetMoney() const;
    void ChangeMoney(int delta);
    void SetMoney(int value, bool publishEvent = true);

private:
    void PublishMoneyChangedEvent(int delta);

    GameManager* owner_;
    int money_ = 0;
};

