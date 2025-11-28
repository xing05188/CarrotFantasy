#pragma once
#include "cocos2d.h"
#include "Tower.h"

USING_NS_CC;

// 抽象塔工厂接口
class TowerFactory {
public:
    virtual Tower* createTower(int grade = 0) = 0;
    virtual ~TowerFactory() {}
};

// 具体工厂类
class BottleFactory : public TowerFactory {
public:
    Tower* createTower(int grade = 0) override;
};

class SunFactory : public TowerFactory {
public:
    Tower* createTower(int grade = 0) override;
};

class PlaneFactory : public TowerFactory {
public:
    Tower* createTower(int grade = 0) override;
};

class ShitFactory : public TowerFactory {
public:
    Tower* createTower(int grade = 0) override;
};

// 工厂提供者类，用于管理所有工厂实例
class TowerFactoryProvider {
public:
    static TowerFactory* getFactory(int towerType);
    static Tower* createTower(int towerType, int grade = 0);
};