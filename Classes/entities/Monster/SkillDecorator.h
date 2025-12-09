/**
 * Refactored with Decorator Pattern
 * 使用装饰器模式重构
 * 
 * Design Pattern: Decorator Pattern
 * 设计模式：装饰器模式
 * 
 * Benefits:
 * 1. Allows adding new abilities to monsters without modifying their core classes
 * 2. Provides a flexible alternative to subclassing for extending functionality
 * 3. Enables combining multiple skills dynamically at runtime
 * 4. Maintains the original monster interface while adding new behaviors
 * 
 * 好处：
 * 1. 允许在不修改怪物核心类的情况下添加新能力
 * 2. 提供了扩展功能的灵活替代方案，避免了子类化
 * 3. 支持在运行时动态组合多个技能
 * 4. 在添加新行为的同时保持原始怪物接口
 * 
 * Problems Solved:
 * 1. Avoids class explosion when trying to create monsters with different skill combinations
 * 2. Allows skills to be added or removed from monsters during gameplay
 * 3. Separates skill logic from the core monster implementation
 * 4. Makes it easier to maintain and test individual skills in isolation
 * 
 * 解决的问题：
 * 1. 避免在尝试创建具有不同技能组合的怪物时出现类爆炸
 * 2. 允许在游戏过程中为怪物添加或移除技能
 * 3. 将技能逻辑与核心怪物实现分离
 * 4. 使单独维护和测试各个技能变得更加容易
 */
 // Refactored with Decorator Pattern
 // 使用装饰器模式重构
#pragma once

// 前向声明，避免循环依赖
class Monster;

/**
 * @brief Boss 技能接口
 * 作为被装饰对象的统一抽象
 */
class BossSkill
{
public:
    virtual ~BossSkill() = default;

    /**
     * @brief 施放技能
     * @param owner 技能所属的怪物
     */
    virtual void use(Monster* owner) = 0;
};

/**
 * @brief 技能装饰器基类
 * 设计模式：装饰器模式
 * 作用：可以在不修改原有技能的前提下，为 Boss 技能动态叠加效果
 */
class SkillDecorator : public BossSkill
{
protected:
    BossSkill* _inner;        // 被装饰的技能（可以为 nullptr，表示基础技能）

public:
    explicit SkillDecorator(BossSkill* inner = nullptr);
    virtual ~SkillDecorator();

    void setInner(BossSkill* inner);
    BossSkill* getInner() const { return _inner; }

    // 默认实现：先执行被装饰技能，再执行自己的逻辑（子类可选择 super 调用）
    void use(Monster* owner) override;
};

/**
 * @brief BossYellow 的减速塔技能装饰器
 */
class SlowDownTowerSkill : public SkillDecorator
{
public:
    using SkillDecorator::SkillDecorator;
    void use(Monster* owner) override;
};

/**
 * @brief BossSheep 的摧毁一半防御塔技能装饰器
 */
class DestroyTowerSkill : public SkillDecorator
{
public:
    using SkillDecorator::SkillDecorator;
    void use(Monster* owner) override;
};


