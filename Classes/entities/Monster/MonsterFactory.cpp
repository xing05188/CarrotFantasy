/**
 * Refactored with Factory Pattern
 * 使用工厂模式重构
 * 
 * Design Pattern: Factory Method Pattern
 * 设计模式：工厂方法模式
 * 
 * Benefits:
 * 1. Decouples client code from concrete monster classes, allowing for easier maintenance
 * 2. Enables adding new monster types without modifying existing code
 * 3. Centralizes monster creation logic, making it easier to manage
 * 4. Supports polymorphism through base class pointers
 * 
 * 好处：
 * 1. 将客户端代码与具体怪物类解耦，便于维护
 * 2. 支持添加新怪物类型而无需修改现有代码
 * 3. 集中管理怪物的创建逻辑，更易于管理
 * 4. 通过基类指针支持多态性
 * 
 * Problems Solved:
 * 1. Eliminates direct instantiation of concrete monster classes throughout the codebase
 * 2. Provides a consistent interface for creating monsters regardless of their type
 * 3. Simplifies object creation when there are multiple monster types with different initialization requirements
 * 
 * 解决的问题：
 * 1. 消除了在整个代码库中直接实例化具体怪物类的问题
 * 2. 为创建不同类型的怪物提供了一致的接口
 * 3. 简化了当有多种怪物类型且有不同初始化需求时的对象创建过程
 */
 // Refactored with Factory Pattern
 // 使用工厂模式重构
#include "MonsterFactory.h"
#include "config/MonsterConfigs.h"
#include "ui/CocosGUI.h"
#include "SkillDecorator.h"

// NormalMonsterFactory实现
Monster* NormalMonsterFactory::createMonster(const std::string& monsterName, 
                                           const std::vector<Vec2>& path, 
                                           int startIndex, 
                                           bool pause) {
    // 创建普通怪物对象
    Monster* monster = new (std::nothrow) Monster();
    if (!monster) {
        CCLOG("Failed to create normal monster: %s", monsterName.c_str());
        return nullptr;
    }
    
    // 使用工厂类的初始化方法初始化怪物
    if (Monster::initializeMonsterWithHealthBar(monster, monsterName, path, startIndex, pause)) {
        CCLOG("Normal monster created: %s", monsterName.c_str());
        return monster;
    }
    
    CC_SAFE_DELETE(monster);
    return nullptr;
}

// BossMonsterFactory实现
Monster* BossMonsterFactory::createMonster(const std::string& monsterName, 
                                          const std::vector<Vec2>& path, 
                                          int startIndex, 
                                          bool pause) {
    Monster* monster = nullptr;
    
    // 根据Boss类型创建不同的Boss对象
    if (monsterName == "BossYellow") {
        monster = new (std::nothrow) BossYellow();
    } else if (monsterName == "BossSheep") {
        monster = new (std::nothrow) BossSheep();
    } else {
        CCLOG("Unknown boss type: %s", monsterName.c_str());
        return nullptr;
    }
    
    if (!monster) {
        CCLOG("Failed to create boss monster: %s", monsterName.c_str());
        return nullptr;
    }
    
    // 使用工厂类的初始化方法初始化Boss
    if (Monster::initializeMonsterWithHealthBar(monster, monsterName, path, startIndex, pause)) {
        // 为不同 Boss 绑定对应的技能装饰器
        if (monsterName == "BossYellow") {
            monster->setBossSkill(new (std::nothrow) SlowDownTowerSkill());
        } else if (monsterName == "BossSheep") {
            monster->setBossSkill(new (std::nothrow) DestroyTowerSkill());
        }
        CCLOG("Boss monster created: %s", monsterName.c_str());
        return monster;
    }
    
    CC_SAFE_DELETE(monster);
    return nullptr;
}

// MonsterFactoryProvider实现
MonsterFactory* MonsterFactoryProvider::getFactory(const std::string& monsterName) {
    // 根据怪物名称判断怪物类型
    if (monsterName.find("Boss") == 0) {
        return new (std::nothrow) BossMonsterFactory();
    } else {
        return new (std::nothrow) NormalMonsterFactory();
    }
}