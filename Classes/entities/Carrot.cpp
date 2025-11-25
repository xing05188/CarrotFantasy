#include "Carrot.h"
#include "Music.h"

USING_NS_CC;
extern Music a;

Carrot* Carrot::create(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos) {
    Carrot* ret = new (std::nothrow) Carrot();
    if (ret && ret->init(initialHP, carrotPos, hpPos)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool Carrot::init(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos) {
    if (!Node::init()) {
        return false;
    }

    hp = initialHP;
    maxHp = initialHP;
    isDead = (hp <= 0);

    auto frameCache = SpriteFrameCache::getInstance();
    frameCache->addSpriteFramesWithFile("CarrotGuardRes/Carrots.plist", "CarrotGuardRes/Carrots.png");
    frameCache->addSpriteFramesWithFile("CarrotGuardRes/Health.plist", "CarrotGuardRes/Health.png");

    carrotSprite = Sprite::createWithSpriteFrameName(StringUtils::format("Carrot_%d.png", (hp + 1) / 2));
    carrotSprite->setScale(1.5F);
    carrotSprite->setPosition(carrotPos);
    this->addChild(carrotSprite, 2);

    hpSprite = Sprite::createWithSpriteFrameName(StringUtils::format("Health_%d.png", hp));
    hpSprite->setScale(1.5F);
    hpSprite->setPosition(hpPos);
    this->addChild(hpSprite, 2);

    publishHpEvent(0);
    if (isDead) {
        publishDeathEvent();
    }
    return true;
}

void Carrot::changeHP(int change) {
    int oldHp = hp;
    hp += change;
    if (hp < 0) {
        hp = 0;
    }
    if (hp > maxHp) {
        hp = maxHp;
    }

    if (hp == oldHp) {
        return;
    }

    updateHPDisplay();
    publishHpEvent(hp - oldHp);

    if (hp <= 0 && !isDead) {
        isDead = true;
        publishDeathEvent();
    } else if (hp > 0) {
        isDead = false;
    }
}

void Carrot::updateHPDisplay() {
    int carrotFrame = std::max(1, (hp + 1) / 2);
    carrotSprite->setSpriteFrame(StringUtils::format("Carrot_%d.png", carrotFrame));

    int clampedHpFrame = hp <= 0 ? 1 : std::min(hp, maxHp);
    hpSprite->setSpriteFrame(StringUtils::format("Health_%d.png", clampedHpFrame));
    hpSprite->setVisible(hp > 0);
}

void Carrot::getDamage(int damage) {
    if (hp <= 0) {
        return;
    }
    a.carrotSound();
    changeHP(-damage);
    CCLOG("Carrot getDamage: %d", damage);
}

void Carrot::getRecover() {
    if (hp >= maxHp) {
        return;
    }
    changeHP(1);
    CCLOG("Carrot getRecover");
}

void Carrot::gameOver() {
    CCLOG("Carrot game over");
}

void Carrot::publishHpEvent(int delta) {
    carrot::gameplay::events::CarrotHpChangedEvent evt;
    evt.current = hp;
    evt.delta = delta;
    evt.max = maxHp;
    carrot::core::EventBusProvider::Get()->Publish(carrot::gameplay::events::kCarrotHpChangedEventId, evt);
}

void Carrot::publishDeathEvent() {
    carrot::gameplay::events::CarrotDiedEvent evt;
    carrot::core::EventBusProvider::Get()->Publish(carrot::gameplay::events::kCarrotDiedEventId, evt);
    gameOver();
}

