// 引入塔工厂头文件
#include "TowerFactory.h"

// 具体工厂实现 - 实现各种塔的创建方法

// 瓶子塔工厂实现 - 创建瓶子塔实例并设置攻击策略
Tower* BottleFactory::createTower(int grade) {
    // 创建瓶子塔实例，参数0表示塔的类型索引
    Bottle* bottle = new Bottle(0, grade);
    // 设置瓶子塔的攻击策略
    bottle->setAttackStrategy(new BottleAttackStrategy());
    return bottle;
}

// 太阳塔工厂实现 - 创建太阳塔实例并设置攻击策略
Tower* SunFactory::createTower(int grade) {
    // 创建太阳塔实例，参数1表示塔的类型索引
    Sun* sun = new Sun(1, grade);
    // 设置太阳塔的攻击策略
    sun->setAttackStrategy(new SunAttackStrategy());
    return sun;
}

// 飞机塔工厂实现 - 创建飞机塔实例并设置攻击策略
Tower* PlaneFactory::createTower(int grade) {
    // 创建飞机塔实例，参数2表示塔的类型索引
    MyPlane* plane = new MyPlane(2, grade);
    // 设置飞机塔的攻击策略
    plane->setAttackStrategy(new PlaneAttackStrategy());
    return plane;
}

// 粪便塔工厂实现 - 创建粪便塔实例并设置攻击策略
Tower* ShitFactory::createTower(int grade) {
    // 创建粪便塔实例，参数3表示塔的类型索引
    Shit* shit = new Shit(3, grade);
    // 设置粪便塔的攻击策略
    shit->setAttackStrategy(new ShitAttackStrategy());
    return shit;
}

// 风扇塔工厂实现 - 创建风扇塔实例并设置攻击策略
Tower* FanFactory::createTower(int grade) {
    // 创建风扇塔实例，参数4表示塔的类型索引
    Fan* fan = new Fan(4, grade);
    // 设置风扇塔的攻击策略
    fan->setAttackStrategy(new FanAttackStrategy());
    return fan;
}

// 魔法塔工厂实现 - 创建魔法塔实例并设置攻击策略
Tower* MagicFactory::createTower(int grade) {
    // 创建魔法塔实例，参数5表示塔的类型索引
    MagicTower* magic = new MagicTower(5, grade);
    // 设置魔法塔的攻击策略
    magic->setAttackStrategy(new MagicAttackStrategy());
    return magic;
}

// 工厂提供者实现 - 管理所有工厂实例并提供统一的创建接口

// 根据塔类型获取对应的工厂实例
TowerFactory* TowerFactoryProvider::getFactory(int towerType) {
    // 使用静态局部变量确保每种工厂只有一个实例（单例模式）
    static BottleFactory bottleFactory;
    static SunFactory sunFactory;
    static PlaneFactory planeFactory;
    static ShitFactory shitFactory;
    static FanFactory fanFactory;
    static MagicFactory magicFactory;

    // 根据塔类型返回对应的工厂实例
    switch (towerType) {
    case 0: return &bottleFactory;  // 返回瓶子塔工厂
    case 1: return &sunFactory;     // 返回太阳塔工厂
    case 2: return &planeFactory;   // 返回飞机塔工厂
    case 3: return &shitFactory;    // 返回粪便塔工厂
    case 4: return &fanFactory;     // 返回风扇塔工厂
    case 5: return &magicFactory;   // 返回魔法塔工厂
    default: return nullptr;        // 无效类型返回空指针
    }
}

// 根据塔类型和等级直接创建塔实例
Tower* TowerFactoryProvider::createTower(int towerType, int grade) {
    // 获取对应的工厂实例
    TowerFactory* factory = getFactory(towerType);
    if (factory) {
        // 调用工厂的创建方法创建塔实例
        return factory->createTower(grade);
    }
    return nullptr;  // 工厂不存在则返回空指针
}