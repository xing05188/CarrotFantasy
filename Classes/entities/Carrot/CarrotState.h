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