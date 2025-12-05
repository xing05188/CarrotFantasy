#include "SkillDecorator.h"
#include "Monster.h"
#include "music.h"
#include "BaseLevelScene.h"
#include "GameManager.h"
#include "cocos2d.h"

USING_NS_CC;

extern GameManager* manager;
extern float tower_jiasu;

namespace
{
    // 局部工具函数：在屏幕上显示 Boss 技能名
    void showBossSkill(const std::string& skillText)
    {
        auto skillLabel = Label::createWithTTF(skillText, "fonts/Marker Felt.ttf", 48);
        if (!skillLabel) return;

        skillLabel->setPosition(Director::getInstance()->getVisibleSize() / 2);
        skillLabel->setTextColor(Color4B::RED);

        manager->getScene()->addChild(skillLabel, 100);

        auto fadeIn = FadeIn::create(0.5f);
        auto delay = DelayTime::create(2.0f);
        auto fadeOut = FadeOut::create(0.5f);
        auto remove = CallFunc::create([=]() {
            manager->getScene()->removeChild(skillLabel);
        });

        auto sequence = Sequence::create(fadeIn, delay, fadeOut, remove, nullptr);
        skillLabel->runAction(sequence);
    }
}

//================ SkillDecorator ==================

SkillDecorator::SkillDecorator(BossSkill* inner)
    : _inner(inner)
{
}

SkillDecorator::~SkillDecorator()
{
    CC_SAFE_DELETE(_inner);
}

void SkillDecorator::setInner(BossSkill* inner)
{
    if (_inner == inner) return;
    CC_SAFE_DELETE(_inner);
    _inner = inner;
}

void SkillDecorator::use(Monster* owner)
{
    // 先执行被装饰的技能（如果有）
    if (_inner)
    {
        _inner->use(owner);
    }
}

//================ SlowDownTowerSkill ==================

void SlowDownTowerSkill::use(Monster* owner)
{
    // 先执行可能被装饰的其他技能
    SkillDecorator::use(owner);

    if (!owner) return;

    // boss 技能：降低所有塔的攻击速度
    CCLOG("BossYellow's Attack!");
    showBossSkill("SpecialAttack:Slow Speed!");

    Music::getInstance()->duanSound();
    tower_jiasu = 0.3f;

    auto delayaction = Sequence::create(
        DelayTime::create(20.0f),
        CallFunc::create([] {
            tower_jiasu = 1.0f;
        }),
        nullptr);

    owner->runAction(delayaction);
}

//================ DestroyTowerSkill ==================

#define CELL_SIZE 64

void DestroyTowerSkill::use(Monster* owner)
{
    // 先执行可能被装饰的其他技能
    SkillDecorator::use(owner);

    if (!owner) return;

    // boss 技能：摧毁一半的塔
    CCLOG("BossSheep's Attack!");
    showBossSkill("SpecialAttack:Destroy!");

    Music::getInstance()->duanSound();

    auto bong = Sprite::create();
    if (!bong)
    {
        CCLOG("Failed to create bong sprite.");
        return;
    }

    bong->setPosition(480, 320);
    bong->setScale(2);
    owner->addChild(bong);

    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    for (int i = 0; i <= 3; ++i)
    {
        std::string frameName = "Carrot/bong/bong_" + std::to_string(i) + ".png";
        auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 164, 160));
        if (frame)
        {
            frames.pushBack(frame);
        }
        else
        {
            CCLOG("Failed to load frame: %s", frameName.c_str());
        }
    }

    if (frames.empty())
    {
        CCLOG("No frames found for bong, skipping.");
        return;
    }

    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = cocos2d::Animate::create(animation);
    auto onbong = cocos2d::CallFunc::create([bong]() {
        CCLOG("bong.");
        bong->removeFromParent();
    });

    Music::getInstance()->bongSound();
    bong->runAction(cocos2d::Sequence::create(animate, onbong, nullptr));

    int num = 0;
    for (auto it = manager->getScene()->towers.begin(); it != manager->getScene()->towers.end();)
    {
        if (num % 2 == 0)
        {
            manager->getScene()->removeChild(it->second->sprite_mark);
            manager->getScene()->removeChild(it->second->sp_base);
            it->second->sprite_mark->release();
            it->second->sp_base->release();

            auto tool = ++it;
            --it;
            manager->getScene()->map_data[int(it->second->pos.x / CELL_SIZE)][int(it->second->pos.y / CELL_SIZE)].flag = 0;
            manager->getScene()->towers.erase(it);
            it = tool;
        }
        else
        {
            ++it;
        }
        ++num;
    }
}


