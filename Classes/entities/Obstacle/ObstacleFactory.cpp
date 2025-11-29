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