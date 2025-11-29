# 障碍物工厂模式设计文档

## 概述

本文档描述了游戏中障碍物系统的工厂模式实现。通过工厂模式，我们将障碍物的创建过程封装起来，使得在新增障碍物类型时无需修改客户端代码，提高了代码的可扩展性和可维护性。

## 设计模式说明

### 工厂方法模式

工厂方法模式定义了一个用于创建对象的接口，但让子类决定实例化哪一个类。在本项目中，我们使用工厂方法模式来创建不同类型的障碍物。

### 抽象工厂模式

抽象工厂模式提供一个接口，用于创建一系列相关或相互依赖的对象，而无需指定它们具体的类。在我们的实现中，`ObstacleFactoryProvider`充当抽象工厂，管理所有具体的障碍物工厂。

## 类结构

### 核心类

1. **Obstacle** - 障碍物基类
   - 包含障碍物的基本属性：生命值、护盾、位置、大小等
   - 提供障碍物的基本行为：受伤、死亡、更新血条等

2. **ObstacleFactory** - 抽象工厂接口
   - 定义创建障碍物的接口方法
   - 提供获取工厂类型的方法

3. **NormalObstacleFactory** - 普通障碍物工厂
   - 实现标准障碍物的创建逻辑
   - 创建具有基础属性的障碍物

4. **ReinforcedObstacleFactory** - 强化障碍物工厂
   - 实现强化障碍物的创建逻辑
   - 创建带有护盾的障碍物，护盾值为生命值的一半

5. **ObstacleFactoryProvider** - 工厂提供者
   - 管理所有障碍物工厂实例
   - 提供统一的创建接口
   - 支持随机创建不同类型的障碍物

## 使用示例

### 创建普通障碍物

```cpp
// 使用普通工厂创建障碍物
Obstacle* obstacle = ObstacleFactoryProvider::createObstacle("Normal", index, scene, x, y);
```

### 创建强化障碍物

```cpp
// 使用强化工厂创建障碍物
Obstacle* obstacle = ObstacleFactoryProvider::createObstacle("Reinforced", index, scene, x, y);
```

### 随机创建障碍物

```cpp
// 随机选择工厂类型创建障碍物
Obstacle* obstacle = ObstacleFactoryProvider::createObstacle(index, scene, x, y);
```

## 优势

1. **解耦创建与使用**：客户端代码无需直接实例化障碍物对象，降低了耦合度
2. **易于扩展**：新增障碍物类型只需添加新的工厂类，无需修改现有代码
3. **统一管理**：通过工厂提供者统一管理所有工厂，便于维护
4. **灵活创建**：支持指定类型创建和随机创建两种方式

## 扩展指南

### 添加新的障碍物类型

1. 创建新的工厂类，继承自`ObstacleFactory`
2. 实现`createObstacle`和`getFactoryType`方法
3. 在`ObstacleFactoryProvider::initFactories`中注册新工厂

示例：

```cpp
class SpecialObstacleFactory : public ObstacleFactory {
public:
    Obstacle* createObstacle(int index, BaseLevelScene* scene, int x, int y) override {
        Obstacle* obstacle = new Obstacle(index);
        // 设置特殊属性
        obstacle->setAllShield(obstacle->getHealth()); // 全护盾
        obstacle->Produce(scene, x, y);
        return obstacle;
    }
    
    std::string getFactoryType() const override { return "Special"; }
};

// 在initFactories中添加
factories["Special"] = new SpecialObstacleFactory();
```

## 注意事项

1. **内存管理**：工厂创建的对象需要客户端负责释放
2. **工厂初始化**：首次使用工厂时会自动初始化，也可以手动调用`initFactories`
3. **资源清理**：程序退出时应调用`cleanup`方法清理工厂资源

## 总结

通过工厂模式的实现，我们成功地将障碍物的创建过程与使用过程分离，提高了代码的可维护性和可扩展性。这种设计使得添加新的障碍物类型变得简单，同时保持了代码的整洁和一致性。