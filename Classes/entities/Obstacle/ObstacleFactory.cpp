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
#include "ObstacleFactory.h"
#include "BaseLevelScene.h"
#include <map>
#include <cstdlib>

// 静态成员初始化
std::map<std::string, ObstacleFactory*> ObstacleFactoryProvider::factories;

// NormalObstacleFactory 实现
Obstacle* NormalObstacleFactory::createObstacle(int index, BaseLevelScene* scene, int x, int y) {
    // 创建标准障碍物
    Obstacle* obstacle = new Obstacle(index);
    obstacle->Produce(scene, x, y);
    
    return obstacle;
}

// ReinforcedObstacleFactory 实现
Obstacle* ReinforcedObstacleFactory::createObstacle(int index, BaseLevelScene* scene, int x, int y) {
    // 创建强化障碍物，增加50%生命值
    Obstacle* obstacle = new Obstacle(index);
    // 设置护盾值为障碍物血量的一半
    int shieldValue = obstacle->getHealth() / 2;
    obstacle->setAllShield(shieldValue);
    obstacle->Produce(scene, x, y);
    
    return obstacle;
}

// ObstacleFactoryProvider 实现
ObstacleFactory* ObstacleFactoryProvider::getFactory(const std::string& factoryType) {
    // 如果工厂未初始化，先初始化
    if (factories.empty()) {
        initFactories();
    }
    
    auto it = factories.find(factoryType);
    if (it != factories.end()) {
        return it->second;
    }
    // 如果找不到指定类型，返回默认的普通工厂
    return factories["Normal"];
}

Obstacle* ObstacleFactoryProvider::createObstacle(const std::string& factoryType, int index, BaseLevelScene* scene, int x, int y) {
    ObstacleFactory* factory = getFactory(factoryType);
    return factory->createObstacle(index, scene, x, y);
}

Obstacle* ObstacleFactoryProvider::createObstacle(int index, BaseLevelScene* scene, int x, int y) {
    // 随机选择工厂类型创建障碍物
    std::string factoryType = (rand() % 2 == 0) ? "Normal" : "Reinforced";
    return createObstacle(factoryType, index, scene, x, y);
}

void ObstacleFactoryProvider::initFactories() {
    // 清理现有工厂
    cleanup();
    
    // 创建并注册工厂
    factories["Normal"] = new NormalObstacleFactory();
    factories["Reinforced"] = new ReinforcedObstacleFactory();
}

void ObstacleFactoryProvider::cleanup() {
    // 删除所有工厂实例
    for (auto& pair : factories) {
        delete pair.second;
    }
    factories.clear();
}