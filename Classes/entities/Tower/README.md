# 塔工厂模式重构

## 概述

我们已经对塔防游戏中的塔系统进行了第一步重构，应用了工厂方法模式来封装塔的创建逻辑。

## 工厂模式实现

### 文件结构

- `TowerFactory.h` - 定义了抽象工厂接口和具体工厂类
- `TowerFactory.cpp` - 实现了工厂类
- `TowerFactoryExample.h` - 使用示例
- `Tower.cpp` - 修改了原有的createTower函数，现在使用工厂模式

### 设计说明

1. **抽象工厂接口** (`TowerFactory`)
   - 定义了创建塔的统一接口

2. **具体工厂类**
   - `BottleFactory` - 创建瓶子塔
   - `SunFactory` - 创建太阳塔
   - `PlaneFactory` - 创建飞机塔
   - `ShitFactory` - 创建便便塔

3. **工厂提供者** (`TowerFactoryProvider`)
   - 管理所有工厂实例
   - 提供统一的访问接口

### 使用方式

```cpp
// 方式1：使用工厂提供者直接创建塔
Tower* tower = TowerFactoryProvider::createTower(0, 1); // 创建1级瓶子塔

// 方式2：获取具体工厂，然后创建塔
TowerFactory* factory = TowerFactoryProvider::getFactory(0);
Tower* tower = factory->createTower(1); // 创建1级瓶子塔
```

## 重构收益

1. **解耦创建逻辑** - 将塔的创建逻辑从使用代码中分离出来
2. **易于扩展** - 添加新塔类型时，只需添加新的工厂类，无需修改现有代码
3. **符合开闭原则** - 对扩展开放，对修改关闭
4. **统一接口** - 所有塔的创建都通过统一接口进行

## 后续计划

接下来，我们可以逐步应用其他设计模式：

1. **策略模式** - 重构塔的攻击行为
2. **组件模式** - 将塔的功能拆分为独立组件
3. **装饰器模式** - 重构塔的升级系统
4. **建造者模式** - 重构塔的属性管理

每一步都会保持代码的简洁性和可维护性，确保系统逐步优化而不是一次性变得复杂。