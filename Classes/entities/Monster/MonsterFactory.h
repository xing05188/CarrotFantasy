/**
 * Refactored with Factory Pattern
 * 使用工厂模式重构
 * 
 * Design Pattern: Factory Method Pattern
 * 设计模式：工厂方法模式
 * 
 * Benefits:
 * 1. Decouples client code from concrete monster classes, allowing for easier maintenance
 * 2. Enables adding new monster types without modifying existing code
 * 3. Centralizes monster creation logic, making it easier to manage
 * 4. Supports polymorphism through base class pointers
 * 
 * 好处：
 * 1. 将客户端代码与具体怪物类解耦，便于维护
 * 2. 支持添加新怪物类型而无需修改现有代码
 * 3. 集中管理怪物的创建逻辑，更易于管理
 * 4. 通过基类指针支持多态性
 * 
 * Problems Solved:
 * 1. Eliminates direct instantiation of concrete monster classes throughout the codebase
 * 2. Provides a consistent interface for creating monsters regardless of their type
 * 3. Simplifies object creation when there are multiple monster types with different initialization requirements
 * 
 * 解决的问题：
 * 1. 消除了在整个代码库中直接实例化具体怪物类的问题
 * 2. 为创建不同类型的怪物提供了一致的接口
 * 3. 简化了当有多种怪物类型且有不同初始化需求时的对象创建过程
 */
 // Refactored with Factory Pattern
 // 使用工厂模式重构
#pragma once

#include "Monster.h"
#include "cocos2d.h"
#include <vector>
#include <string>

USING_NS_CC;

// 前向声明
class BossYellow;
class BossSheep;

/**
 * @class MonsterFactory
 * @brief 怪物工厂抽象基类
 * @details 使用工厂方法模式创建怪物对象，解耦怪物创建逻辑
 * 设计模式：工厂方法模式
 * 原因：将怪物创建逻辑封装在工厂类中，使客户端代码与具体怪物类解耦
 * 好处：添加新怪物类型时无需修改现有代码，只需添加新的工厂子类
 */
class MonsterFactory
{
public:
    /**
     * @brief 创建怪物对象
     * @param monsterName 怪物名称
     * @param path 怪物移动路径
     * @param startIndex 起始位置索引
     * @param pause 是否暂停
     * @return 创建的怪物对象指针
     */
    virtual Monster *createMonster(const std::string &monsterName,
                                   const std::vector<Vec2> &path,
                                   int startIndex,
                                   bool pause) = 0;

    /**
     * @brief 获取工厂类型名称
     * @return 工厂类型名称
     */
    virtual std::string getFactoryType() const = 0;

    virtual ~MonsterFactory() = default;
};

/**
 * @class NormalMonsterFactory
 * @brief 普通怪物工厂
 * @details 用于创建普通类型的怪物
 */
class NormalMonsterFactory : public MonsterFactory
{
public:
    Monster *createMonster(const std::string &monsterName,
                           const std::vector<Vec2> &path,
                           int startIndex,
                           bool pause) override;

    std::string getFactoryType() const override { return "NormalMonster"; }
};

/**
 * @class BossMonsterFactory
 * @brief Boss怪物工厂
 * @details 用于创建Boss类型的怪物
 */
class BossMonsterFactory : public MonsterFactory
{
public:
    Monster *createMonster(const std::string &monsterName,
                           const std::vector<Vec2> &path,
                           int startIndex,
                           bool pause) override;

    std::string getFactoryType() const override { return "BossMonster"; }
};

/**
 * @class MonsterFactoryProvider
 * @brief 怪物工厂提供者
 * @details 根据怪物名称提供相应的工厂实例
 */
class MonsterFactoryProvider
{
public:
    /**
     * @brief 获取怪物工厂
     * @param monsterName 怪物名称
     * @return 对应的工厂实例
     */
    static MonsterFactory *getFactory(const std::string &monsterName);
};