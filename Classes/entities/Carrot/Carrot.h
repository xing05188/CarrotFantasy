#pragma once

#ifndef CARROT_H
#define CARROT_H

#include "cocos2d.h"
#include "core/EventBusProvider.h"
#include "gameplay/events/CarrotEvents.h"

class CarrotState;

class Carrot : public cocos2d::Node {
public:
    static Carrot* create(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos);
    virtual bool init(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos);

    ~Carrot();

    void changeHP(int change);
    void getDamage(int damage);
    void getRecover();

    int getHP() const { return hp; }
    int getMaxHP() const { return maxHp; }

    void setState(CarrotState* newState);
    void updateHPDisplay();
    void publishHpEvent(int delta);
    void publishDeathEvent();
    void gameOver();

    //接入无敌状态
    void enterInvincibleState();

    cocos2d::Sprite* getCarrotSprite() const { return carrotSprite; }
    cocos2d::Sprite* getHpSprite() const { return hpSprite; }
    cocos2d::Sprite* getInvincibleSprite() const { return invincibleSprite; }

private:
    int hp;
    int maxHp;
    CarrotState* state;
    cocos2d::Sprite* carrotSprite;
    cocos2d::Sprite* hpSprite;
    //无敌罩
    cocos2d::Sprite* invincibleSprite;
};

#endif  // CARROT_H