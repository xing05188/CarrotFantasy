#include "Carrot.h"
#include "Music.h"
#include "CarrotState.h"

USING_NS_CC;

Carrot::~Carrot() {
    if (state) {
        delete state;
        state = nullptr;
    }
}

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
    state = nullptr;

    auto frameCache = SpriteFrameCache::getInstance();
    frameCache->addSpriteFramesWithFile("CarrotGuardRes/Carrots.plist", "CarrotGuardRes/Carrots.png");
    frameCache->addSpriteFramesWithFile("CarrotGuardRes/Health.plist", "CarrotGuardRes/Health.png");

    carrotSprite = Sprite::createWithSpriteFrameName(StringUtils::format("Carrot_%d.png", (hp + 1) / 2));
    carrotSprite->setScale(1.5F);
    carrotSprite->setPosition(carrotPos);
    this->addChild(carrotSprite, 99);

    hpSprite = Sprite::createWithSpriteFrameName(StringUtils::format("Health_%d.png", hp));
    hpSprite->setScale(1.5F);
    hpSprite->setPosition(hpPos);
    this->addChild(hpSprite, 99);

    // 初始化无敌精灵
    invincibleSprite = Sprite::create("Carrot/wudi.png");
    if (invincibleSprite) {
        invincibleSprite->setScale(1.3F);
        invincibleSprite->setPosition(carrotPos);
        invincibleSprite->setVisible(false); // 初始状态为隐藏
        this->addChild(invincibleSprite, 100); // 设置较高的z-order确保显示在其他精灵之上
    }

    // 初始状态
    state = new NormalState();
    state->enter(this);

    publishHpEvent(0);
    return true;
}

void Carrot::setState(CarrotState* newState) {
    if (state) delete state;
    state = newState;
    state->enter(this);
}

void Carrot::changeHP(int change) {
    int oldHp = hp;
    hp += change;
    if (hp < 0) hp = 0;
    if (hp > maxHp) hp = maxHp;

    if (hp == oldHp) return;

    updateHPDisplay();
    publishHpEvent(hp - oldHp);
}

void Carrot::updateHPDisplay() {
    int carrotFrame = std::max(1, (hp + 1) / 2);
    carrotSprite->setSpriteFrame(StringUtils::format("Carrot_%d.png", carrotFrame));

    int clampedHpFrame = hp <= 0 ? 1 : std::min(hp, maxHp);
    hpSprite->setSpriteFrame(StringUtils::format("Health_%d.png", clampedHpFrame));
    hpSprite->setVisible(hp > 0);
}

void Carrot::getDamage(int damage) {
    state->handleDamage(this, damage);
}

void Carrot::getRecover() {
    state->handleRecover(this);
}

void Carrot::gameOver() {
    Music::getInstance()->gameOverSound();
    CCLOG("Carrot game over");
}

//接入无敌状态
void Carrot::enterInvincibleState() {
    setState(new InvincibleState());
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