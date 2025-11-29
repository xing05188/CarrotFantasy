#pragma once
#include "cocos2d.h"
#include "Obstacle.h"
#include <string>
#include <map>

USING_NS_CC;
// 前向声明
class BaseLevelScene;

class ObstacleFactory {
public:
    virtual ~ObstacleFactory() {}
    
    //创建障碍物
    virtual Obstacle* createObstacle(int index, BaseLevelScene* scene, int x, int y) = 0;
    //获取工厂类型
    virtual std::string getFactoryType() const = 0;
};

/**
 * 普通障碍物工厂
 */
class NormalObstacleFactory : public ObstacleFactory {
public:
    Obstacle* createObstacle(int index, BaseLevelScene* scene, int x, int y) override;
    std::string getFactoryType() const override { return "Normal"; }
};

/**
 * 强化障碍物工厂
 */
class ReinforcedObstacleFactory : public ObstacleFactory {
public:
    Obstacle* createObstacle(int index, BaseLevelScene* scene, int x, int y) override;
    std::string getFactoryType() const override { return "Reinforced"; }
};

/**
 * 障碍物工厂提供者
 * 管理所有障碍物工厂，提供统一的创建接口
 */
class ObstacleFactoryProvider {
private:
    static std::map<std::string, ObstacleFactory*> factories;
    
public:
    // 获取指定类型的工厂
    static ObstacleFactory* getFactory(const std::string& factoryType);

    // 使用指定工厂类型创建障碍物
    static Obstacle* createObstacle(const std::string& factoryType, int index, BaseLevelScene* scene, int x, int y);
    
    // 使用默认工厂创建障碍物
    static Obstacle* createObstacle(int index, BaseLevelScene* scene, int x, int y);
    
    // 初始化所有工厂
    static void initFactories();
    // 清理所有工厂
    static void cleanup();
};