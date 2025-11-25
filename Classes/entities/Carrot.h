#pragma once

#ifndef CARROT_H
#define CARROT_H

#include "cocos2d.h"
#include "core/EventBusProvider.h"
#include "gameplay/events/CarrotEvents.h"

class Carrot : public cocos2d::Node {
public:
    static Carrot* create(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos);
    virtual bool init(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos);

    void changeHP(int change);
    void getDamage(int damage);
    void getRecover();

    int getHP() const { return hp; }
    int getMaxHP() const { return maxHp; }

private:
    void updateHPDisplay();
    void publishHpEvent(int delta);
    void publishDeathEvent();
    void gameOver();

    int hp;
    int maxHp;
    bool isDead;
    cocos2d::Sprite* carrotSprite;
    cocos2d::Sprite* hpSprite;
};

#endif  // CARROT_H

