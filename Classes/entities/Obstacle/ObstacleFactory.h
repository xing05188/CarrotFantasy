/**
 * Refactored with Factory Pattern
 * 使用工厂模式重构
 * 
 * Design Pattern: Factory Method Pattern
 * 设计模式：工厂方法模式
 * 
 * Benefits:
 * 1. Decouples client code from concrete obstacle classes, allowing for easier maintenance
 * 2. Enables adding new obstacle types without modifying existing code
 * 3. Centralizes obstacle creation logic, making it easier to manage
 * 4. Supports polymorphism through base class pointers
 * 
 * 好处：
 * 1. 将客户端代码与具体障碍物类解耦，便于维护
 * 2. 支持添加新障碍物类型而无需修改现有代码
 * 3. 集中管理障碍物的创建逻辑，更易于管理
 * 4. 通过基类指针支持多态性
 * 
 * Problems Solved:
 * 1. Eliminates direct instantiation of concrete obstacle classes throughout the codebase
 * 2. Provides a consistent interface for creating obstacles regardless of their type
 * 3. Simplifies object creation when there are multiple obstacle types with different initialization requirements
 * 
 * 解决的问题：
 * 1. 消除了在整个代码库中直接实例化具体障碍物类的问题
 * 2. 为创建不同类型的障碍物提供了一致的接口
 * 3. 简化了当有多种障碍物类型且有不同初始化需求时的对象创建过程
 */
 // Refactored with Factory Pattern
 // 使用工厂模式重构
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