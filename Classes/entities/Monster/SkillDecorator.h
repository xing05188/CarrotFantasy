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


