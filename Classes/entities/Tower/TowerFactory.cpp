#include "TowerFactory.h"

// 具体工厂实现
Tower* BottleFactory::createTower(int grade) {
    return new Bottle(0, grade);
}

Tower* SunFactory::createTower(int grade) {
    return new Sun(1, grade);
}

Tower* PlaneFactory::createTower(int grade) {
    return new MyPlane(2, grade);
}

Tower* ShitFactory::createTower(int grade) {
    return new Shit(3, grade);
}

// 工厂提供者实现
TowerFactory* TowerFactoryProvider::getFactory(int towerType) {
    static BottleFactory bottleFactory;
    static SunFactory sunFactory;
    static PlaneFactory planeFactory;
    static ShitFactory shitFactory;

    switch (towerType) {
    case 0: return &bottleFactory;
    case 1: return &sunFactory;
    case 2: return &planeFactory;
    case 3: return &shitFactory;
    default: return nullptr;
    }
}

Tower* TowerFactoryProvider::createTower(int towerType, int grade) {
    TowerFactory* factory = getFactory(towerType);
    if (factory) {
        return factory->createTower(grade);
    }
    return nullptr;
}