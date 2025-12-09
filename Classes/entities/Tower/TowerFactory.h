/**
 * Refactored with Factory Pattern
 * 使用工厂模式重构
 * 
 * Design Pattern: Factory Method Pattern
 * 设计模式：工厂方法模式
 * 
 * Benefits:
 * 1. Decouples client code from concrete tower classes, allowing for easier maintenance
 * 2. Enables adding new tower types without modifying existing code
 * 3. Centralizes tower creation logic, making it easier to manage
 * 4. Supports polymorphism through base class pointers
 * 
 * 好处：
 * 1. 将客户端代码与具体塔类解耦，便于维护
 * 2. 支持添加新塔类型而无需修改现有代码
 * 3. 集中管理塔的创建逻辑，更易于管理
 * 4. 通过基类指针支持多态性
 * 
 * Problems Solved:
 * 1. Eliminates direct instantiation of concrete tower classes throughout the codebase
 * 2. Provides a consistent interface for creating towers regardless of their type
 * 3. Simplifies object creation when there are multiple tower types with different initialization requirements
 * 
 * 解决的问题：
 * 1. 消除了在整个代码库中直接实例化具体塔类的问题
 * 2. 为创建不同类型的塔提供了一致的接口
 * 3. 简化了当有多种塔类型且有不同初始化需求时的对象创建过程
 */
 // Refactored with Factory Pattern
 // 使用工厂模式重构
#pragma once
// 引入Cocos2d-x框架头文件
#include "cocos2d.h"
// 引入塔类头文件
#include "Tower.h"
// 引入攻击策略头文件
#include "AttackStrategy.h"

// 使用Cocos2d-x命名空间
USING_NS_CC;

// 抽象塔工厂接口 - 工厂模式中的抽象工厂类，定义创建塔的接口
class TowerFactory {
public:
    // 纯虚函数，用于创建塔对象，参数grade表示塔的等级
    virtual Tower* createTower(int grade = 0) = 0;
    // 虚析构函数，确保派生类对象正确销毁
    virtual ~TowerFactory() {}
};

// 具体工厂类 - 实现抽象工厂接口，创建特定类型的塔
// 瓶子塔工厂类 - 创建瓶子塔的具体工厂
class BottleFactory : public TowerFactory {
public:
    // 重写创建塔的方法，返回瓶子塔实例
    Tower* createTower(int grade = 0) override;
};

// 太阳塔工厂类 - 创建太阳塔的具体工厂
class SunFactory : public TowerFactory {
public:
    // 重写创建塔的方法，返回太阳塔实例
    Tower* createTower(int grade = 0) override;
};

// 飞机塔工厂类 - 创建飞机塔的具体工厂
class PlaneFactory : public TowerFactory {
public:
    // 重写创建塔的方法，返回飞机塔实例
    Tower* createTower(int grade = 0) override;
};

// 粪便塔工厂类 - 创建粪便塔的具体工厂
class ShitFactory : public TowerFactory {
public:
    // 重写创建塔的方法，返回粪便塔实例
    Tower* createTower(int grade = 0) override;
};

// 风扇塔工厂类 - 创建风扇塔的具体工厂
class FanFactory : public TowerFactory {
public:
    // 重写创建塔的方法，返回风扇塔实例
    Tower* createTower(int grade = 0) override;
};

// 魔法塔工厂类 - 创建魔法塔的具体工厂
class MagicFactory : public TowerFactory {
public:
    // 重写创建塔的方法，返回魔法塔实例
    Tower* createTower(int grade = 0) override;
};

// 工厂提供者类 - 用于管理所有工厂实例，提供统一的创建接口
class TowerFactoryProvider {
public:
    // 根据塔类型获取对应的工厂实例
    static TowerFactory* getFactory(int towerType);
    // 根据塔类型和等级直接创建塔实例，内部调用对应工厂的createTower方法
    static Tower* createTower(int towerType, int grade = 0);
};