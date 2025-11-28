#include "CarrotState.h"
#include "Carrot.h"
#include "Music.h"

USING_NS_CC;

void NormalState::enter(Carrot *carrot)
{
    Music::getInstance()->born_music();
    // 可以在这里播放正常状态的动画
}

void NormalState::handleDamage(Carrot *carrot, int damage)
{
    carrot->changeHP(-damage);
    Music::getInstance()->carrotSound();
    CCLOG("Carrot getDamage: %d", damage);

    if (carrot->getHP() <= 0)
    {
        carrot->setState(new DeadState());
    }
}

void NormalState::handleRecover(Carrot *carrot)
{
    if (carrot->getHP() < carrot->getMaxHP())
    {
        carrot->changeHP(1);
        CCLOG("Carrot getRecover");
    }
}

InvincibleState::InvincibleState() : _remainingTime(5.0f), _scheduler(Director::getInstance()->getScheduler()), _carrot(nullptr)
{
}

InvincibleState::~InvincibleState()
{
    // 确保在销毁时取消调度器，使用与schedule相同的键
    if (_scheduler && _carrot != nullptr) // 只有在_carrot不为nullptr时才取消，避免重复取消
    {
        _scheduler->unschedule("invincible_update", this);
    }
}

void InvincibleState::enter(Carrot *carrot)
{
    _carrot = carrot;
    
    // 显示无敌精灵
    if (_carrot->getInvincibleSprite()) {
        _carrot->getInvincibleSprite()->setVisible(true);
    }
    Music::getInstance()->kaiSound();
    // 开始计时，使用与unschedule相同的键
    _scheduler->schedule([this](float dt)
                         { this->update(dt); }, this, 0.1f, false, "invincible_update");
}

void InvincibleState::handleDamage(Carrot *carrot, int damage)
{
    // 无敌状态下不受到伤害
    CCLOG("Carrot is invincible, damage ignored!");
}

void InvincibleState::handleRecover(Carrot *carrot)
{
    carrot->getRecover();
}

void InvincibleState::update(float dt)
{
    _remainingTime -= dt;
    if (_remainingTime <= 0) {
        // 先取消调度器，避免在状态切换后继续调用
        _scheduler->unschedule("invincible_update", this);
        
        // 隐藏无敌精灵
        if (_carrot && _carrot->getInvincibleSprite()) {
            _carrot->getInvincibleSprite()->setVisible(false);
        }
        
        // 保存对carrot的引用，因为setState会删除当前状态
        Carrot* carrotRef = _carrot;
        _carrot = nullptr; // 防止在析构函数中再次尝试取消调度器
        
        // 切换状态
        if (carrotRef) {
            carrotRef->setState(new NormalState());
        }
    }
}

void DeadState::enter(Carrot *carrot)
{
    carrot->gameOver();
    carrot->publishDeathEvent();
}

void DeadState::handleDamage(Carrot *carrot, int damage)
{
    // 死亡状态下不处理伤害
}

void DeadState::handleRecover(Carrot *carrot)
{
    // 死亡状态下不处理恢复
}