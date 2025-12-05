# Monster设计模式重构说明

## 概述

本项目使用了多种设计模式来优化怪物系统的架构：
- **工厂模式**: 通过`MonsterFactory`及其子类解耦了怪物创建过程
- **装饰器模式**: 通过`SkillDecorator`及其子类实现了Boss技能的动态组合和扩展

这些设计模式的应用显著提高了代码的可扩展性、可维护性和灵活性。

## 工厂模式结构

### 抽象工厂类
- `MonsterFactory`: 定义了创建怪物的抽象接口

### 具体工厂类
- `NormalMonsterFactory`: 创建普通怪物
- `BossMonsterFactory`: 创建Boss怪物

### 工厂提供者
- `MonsterFactoryProvider`: 根据怪物名称提供相应的工厂实例


## 使用方法

### 创建怪物
```cpp
// 使用工厂创建怪物
Monster* monster = Monster::create("NormalMonster", path, 0, false);
Monster* boss = Monster::create("BossYellow", path, 0, false);
Monster* elite = Monster::create("EliteWarrior", path, 0, false);
```

### 怪物初始化器使用
```cpp
// 直接使用初始化器初始化已创建的怪物对象
Monster* monster = new Monster();
if (MonsterInitializer::initializeMonsterWithHealthBar(monster, "Monster1", path, 0, false)) {
    // 初始化成功，可以使用monster
} else {
    // 初始化失败，处理错误
    CC_SAFE_DELETE(monster);
}
```

## 扩展新的怪物类型

### 步骤1：创建新的怪物类
```cpp
class NewMonsterType : public Monster {
public:
    virtual void SpecialAttack() override {
        // 实现特殊攻击逻辑
    }
    
    static NewMonsterType* create(const std::string& monsterName, 
                                 const std::vector<Vec2>& path, 
                                 int startIndex, 
                                 bool pause);
};
```

### 步骤2：创建新的工厂类
```cpp
class NewMonsterTypeFactory : public MonsterFactory {
public:
    Monster* createMonster(const std::string& monsterName, 
                          const std::vector<Vec2>& path, 
                          int startIndex, 
                          bool pause) override;
    
    std::string getFactoryType() const override { return "NewMonsterType"; }
};
```

### 步骤3：实现工厂方法
```cpp
Monster* NewMonsterTypeFactory::createMonster(const std::string& monsterName, 
                                             const std::vector<Vec2>& path, 
                                             int startIndex, 
                                             bool pause) {
    return NewMonsterType::create(monsterName, path, startIndex, pause);
}
```

### 步骤4：更新MonsterFactoryProvider
在`MonsterFactoryProvider::getFactory`方法中添加对新怪物类型的支持：
```cpp
MonsterFactory* MonsterFactoryProvider::getFactory(const std::string& monsterName) {
    if (monsterName.find("Boss") == 0) {
        return new (std::nothrow) BossMonsterFactory();
    } else if (monsterName.find("Elite") == 0) {
        return new (std::nothrow) EliteMonsterFactory();
    } else if (monsterName.find("NewType") == 0) {
        return new (std::nothrow) NewMonsterTypeFactory();
    } else {
        return new (std::nothrow) NormalMonsterFactory();
    }
}
```

## 工厂模式的优势

1. **解耦创建逻辑**: 将怪物创建逻辑从使用方分离，降低耦合度
2. **易于扩展**: 添加新怪物类型无需修改现有代码，只需添加新的工厂类
3. **统一管理**: 所有怪物创建逻辑集中在工厂中，便于管理和维护
4. **符合开闭原则**: 对扩展开放，对修改关闭

## MonsterInitializer设计

### 设计模式
- **策略模式**: 将怪物初始化逻辑封装在MonsterInitializer中，使工厂类可以灵活选择不同的初始化策略

### 优势
1. **代码复用性提升**: 消除了NormalMonsterFactory和BossMonsterFactory中的重复代码
2. **可维护性提升**: 怪物初始化逻辑集中在一个地方，便于修改和维护
3. **可扩展性提升**: 如果需要添加新的怪物工厂类，可以直接使用MonsterInitializer
4. **符合单一职责原则**: MonsterFactory专注于创建对象，MonsterInitializer专注于初始化对象

### 核心方法
```cpp
static bool initializeMonsterWithHealthBar(Monster* monster, 
                                          const std::string& monsterName, 
                                          const std::vector<Vec2>& path, 
                                          int startIndex, 
                                          bool pause);
```

## 装饰器模式结构

### 抽象组件接口
- `BossSkill`: 定义了Boss技能的抽象接口，包含`use(Monster*)`方法

### 装饰器基类
- `SkillDecorator`: 技能装饰器的基类，维护一个指向被装饰技能的指针，支持技能的组合和叠加

### 具体装饰器类
- `SlowDownTowerSkill`: BossYellow的减速塔技能装饰器，降低所有防御塔的攻击速度
- `DestroyTowerSkill`: BossSheep的摧毁防御塔技能装饰器，摧毁一半的防御塔

## 装饰器模式的使用方法

### Boss技能的执行
```cpp
// Boss技能通过Monster::SpecialAttack()方法触发
Monster* boss = Monster::create("BossYellow", path, 0, false);
boss->SpecialAttack(); // 自动调用绑定的技能装饰器
```

### 技能装饰器的绑定
技能装饰器在`BossMonsterFactory`中自动绑定：
```cpp
// BossMonsterFactory::createMonster 中
if (monsterName == "BossYellow") {
    monster->setBossSkill(new SlowDownTowerSkill());
} else if (monsterName == "BossSheep") {
    monster->setBossSkill(new DestroyTowerSkill());
}
```

### 技能装饰器的组合使用
装饰器模式支持技能的动态组合，可以叠加多个技能效果：
```cpp
// 示例：创建一个组合技能（先减速，再摧毁）
BossSkill* combinedSkill = new DestroyTowerSkill(
    new SlowDownTowerSkill()
);
monster->setBossSkill(combinedSkill);
```

## 扩展新的Boss技能

### 步骤1：创建新的技能装饰器类
```cpp
class NewBossSkill : public SkillDecorator
{
public:
    using SkillDecorator::SkillDecorator;
    void use(Monster* owner) override
    {
        // 先执行被装饰的技能（如果有）
        SkillDecorator::use(owner);
        
        // 实现新技能的具体逻辑
        // ...
    }
};
```

### 步骤2：在工厂中绑定新技能
在`BossMonsterFactory::createMonster`方法中添加对新Boss类型的支持：
```cpp
Monster* BossMonsterFactory::createMonster(...) {
    // ...
    if (monsterName == "BossNewType") {
        monster = new (std::nothrow) BossNewType();
        // 绑定新技能
        monster->setBossSkill(new (std::nothrow) NewBossSkill());
    }
    // ...
}
```

### 步骤3：创建对应的Boss类（如果需要）
```cpp
class BossNewType : public Monster
{
public:
    void SpecialAttack() override
    {
        Monster::SpecialAttack(); // 调用装饰器技能
    }
};
```

## 装饰器模式的优势

1. **动态组合**: 可以在运行时动态组合多个技能效果，无需修改现有代码
2. **单一职责**: 每个技能装饰器只负责一个具体的技能效果，符合单一职责原则
3. **易于扩展**: 添加新技能只需创建新的装饰器类，无需修改现有技能代码
4. **解耦技能逻辑**: 将技能实现从Monster类中分离，降低耦合度
5. **符合开闭原则**: 对扩展开放，对修改关闭
6. **灵活性强**: 支持技能的任意组合，可以创建复杂的技能效果

## 装饰器模式与工厂模式的协作

- **工厂模式**: 负责创建Monster对象，并在创建Boss时自动绑定对应的技能装饰器
- **装饰器模式**: 负责实现Boss的特殊技能，通过装饰器模式实现技能的灵活组合

两种模式的结合使用，使得怪物系统的创建和技能管理都得到了良好的解耦和扩展性。

## 注意事项

1. 工厂对象使用后需要手动释放内存
2. 新增怪物类型时，需要确保怪物名称前缀与工厂类型匹配
3. 工厂方法中的错误处理需要完善，确保资源正确释放
4. MonsterInitializer中的初始化逻辑适用于所有怪物类型，如果有特殊初始化需求，可以考虑扩展该方法或创建专门的初始化器
5. **技能装饰器内存管理**: Monster的析构函数会自动释放绑定的技能装饰器，无需手动管理
6. **技能装饰器的组合**: 当使用组合技能时，内层装饰器会在外层装饰器析构时自动释放
7. **技能执行顺序**: 装饰器模式中，内层技能先执行，外层技能后执行（通过`SkillDecorator::use`的调用顺序控制）