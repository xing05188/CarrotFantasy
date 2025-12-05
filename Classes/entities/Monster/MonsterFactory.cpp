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