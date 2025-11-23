#include "Carrot.h"
#include"Music.h"
#define schedule_selector(_SELECTOR) static_cast<SEL_SCHEDULE>(&_SELECTOR)
USING_NS_CC;
extern Music a;
Carrot* Carrot::create(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos) {
    Carrot* ret = new (std::nothrow) Carrot();
    if (ret && ret->init(initialHP, carrotPos, hpPos)) {
        ret->autorelease();  // �����ڴ����
        return ret;
    }
    else {
        delete ret;
        return nullptr;
    }
}

bool Carrot::init(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos) {
    if (!Node::init()) { // ȷ�������init()���óɹ�
        return false;
    }

    // ��ʼ��Ѫ��
    hp = initialHP;

    // ���뾫��֡����
    auto frameCache = SpriteFrameCache::getInstance();
    frameCache->addSpriteFramesWithFile("CarrotGuardRes/Carrots.plist", "CarrotGuardRes/Carrots.png");
    frameCache->addSpriteFramesWithFile("CarrotGuardRes/Health.plist", "CarrotGuardRes/Health.png");

    // ��ʼ���ܲ�ͼƬ
    carrotSprite = Sprite::createWithSpriteFrameName(StringUtils::format("Carrot_%d.png", (hp+1)/2));
    carrotSprite->setScale(1.5);
    carrotSprite->setPosition(carrotPos);
    this->addChild(carrotSprite, 2); // ���ܲ�ͼƬ����ΪCarrot���ӽڵ�

    // ��ʼ��Ѫ��ͼƬ
    hpSprite = Sprite::createWithSpriteFrameName(StringUtils::format("Health_%d.png", hp));
    hpSprite->setScale(1.5);
    hpSprite->setPosition(hpPos);
    this->addChild(hpSprite, 2); // ��Ѫ��ͼƬ����ΪCarrot���ӽڵ�

    return true;
}

void Carrot::changeHP(int change) {
    hp += change;
    if (hp <= 0) {
        hp == 0;
        updateHPDisplay(); // �����ܲ�ͼƬ��Ѫ��ͼƬ
    }
    else if (hp > 0 && hp <= 10) {
        updateHPDisplay(); // �����ܲ�ͼƬ��Ѫ��ͼƬ
    }
}

void Carrot::updateHPDisplay() {
    carrotSprite->setSpriteFrame(StringUtils::format("Carrot_%d.png", (hp + 1) /2));
    hpSprite->setSpriteFrame(StringUtils::format("Health_%d.png", hp));
}





void Carrot::getDamage(int damage) {
    if(hp<=0)
        return;
    a.carrotSound();
    changeHP(0-damage);
    CCLOG("getDamage...");
}
void Carrot::getRecover()
{
    if (hp== 10)
        return;
    changeHP(1);
    CCLOG("getRecover...");
}
void Carrot::gameOver() {
    CCLOG("game over");
}

int Carrot::getHP() const {
    return hp;
}