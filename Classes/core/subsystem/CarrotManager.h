#pragma once

#include <memory>

class Carrot;
class GameManager;

class CarrotManager {
public:
    explicit CarrotManager(GameManager* owner);

    void InitCarrot();
    void Doudong();
    Carrot* GetCarrot() const;
    void Jineng1();
    void Jineng6();

private:
    GameManager* owner_;
    Carrot* carrot_ = nullptr;
};

