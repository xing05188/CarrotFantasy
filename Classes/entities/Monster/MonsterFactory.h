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