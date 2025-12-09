/**
 * Refactored with State Pattern
 * 使用状态模式重构
 * 
 * Design Pattern: State Pattern
 * 设计模式：状态模式
 * 
 * Benefits:
 * 1. Allows the carrot to change its behavior when its internal state changes
 * 2. Encapsulates state-specific behavior into separate state classes
 * 3. Makes state transitions explicit and easier to manage
 * 4. Eliminates large conditional statements that depend on the carrot's state
 * 
 * 好处：
 * 1. 允许萝卜在其内部状态改变时改变其行为
 * 2. 将特定于状态的行为封装到单独的状态类中
 * 3. 使状态转换显式且更易于管理
 * 4. 消除了依赖于萝卜状态的大型条件语句
 * 
 * Problems Solved:
 * 1. Avoids code duplication when implementing similar behaviors in different states
 * 2. Makes it easier to add new states without modifying existing code
 * 3. Provides a clean way to manage complex state transitions for the carrot
 * 4. Separates state logic from the main carrot object, improving maintainability
 * 
 * 解决的问题：
 * 1. 避免在不同状态下实现类似行为时的代码重复
 * 2. 使添加新状态而无需修改现有代码变得更容易
 * 3. 提供了一种管理萝卜复杂状态转换的清晰方式
 * 4. 将状态逻辑与主萝卜对象分离，提高了可维护性
 */
 // Refactored with State Pattern
 // 使用状态模式重构
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