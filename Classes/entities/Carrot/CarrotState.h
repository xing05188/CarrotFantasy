#pragma once

#include "cocos2d.h"

class Carrot;
class InvincibleState;

class CarrotState {
public:
    virtual ~CarrotState() = default;
    virtual void enter(Carrot* carrot) = 0;
    virtual void handleDamage(Carrot* carrot, int damage) = 0;
    virtual void handleRecover(Carrot* carrot) = 0;
};

// 普通状态类
class NormalState : public CarrotState {
public:
    void enter(Carrot* carrot) override;
    void handleDamage(Carrot* carrot, int damage) override;
    void handleRecover(Carrot* carrot) override;
};

// 无敌状态类
class InvincibleState : public CarrotState {
public:
    InvincibleState();
    virtual ~InvincibleState();

    void enter(Carrot* carrot) override;
    void handleDamage(Carrot* carrot, int damage) override;
    void handleRecover(Carrot* carrot) override;

private:
    float _remainingTime; // 剩余无敌时间
    cocos2d::Scheduler* _scheduler;
    Carrot* _carrot; // 保存对Carrot的引用
    
    void update(float dt);
};

class DeadState : public CarrotState {
public:
    void enter(Carrot* carrot) override;
    void handleDamage(Carrot* carrot, int damage) override;
    void handleRecover(Carrot* carrot) override;
};