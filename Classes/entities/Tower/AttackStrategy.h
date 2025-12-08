#pragma once
#include "cocos2d.h"
#include "entities/Monster/Monster.h"
#include "entities/Obstacle/Obstacle.h"
#include "music.h"
#include <vector>
#include <cmath>

USING_NS_CC;

// 游戏单元格大小常量
#define CELL_SIZE 64
// 圆周率常量，用于角度计算
const double PI = 3.1415926;

// 前向声明
class Tower;
class Bottle;
class Sun;
class MyPlane;
class Shit;
class Fan;
class MagicTower;
class BaseLevelScene;

template<typename T> class ObjectPool;

// 辅助函数声明
// 检测两个精灵是否碰撞
bool isColliding(Sprite* spriteA, Sprite* spriteB);
// 对精灵造成伤害的模板函数
template<class T>
void DemageSprite(T* sp, int demage);

// 攻击策略接口 - 策略模式的抽象策略类
class AttackStrategy {
public:
    virtual ~AttackStrategy() {}
    // 纯虚函数，定义攻击行为
    // 参数：塔指针、场景指针、怪物列表、目标类型、目标怪物、目标障碍、加速倍数
    virtual void attack(Tower* tower, BaseLevelScene* scene, std::vector<Monster*>& monsters, 
                       char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu) = 0;
};

// 瓶子攻击策略 - 具体策略类，实现瓶子塔的攻击行为
class BottleAttackStrategy : public AttackStrategy {
private:
    //static const float speed;  // 炮弹飞行速度
    
public:
    // 实现基类的纯虚函数，定义瓶子塔的攻击逻辑
    void attack(Tower* tower, BaseLevelScene* scene, std::vector<Monster*>& monsters, 
                char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu) override;
    
private:
    // 模板函数，处理瓶子塔对目标的攻击
    template<class T>
    bool AttackSprite(Bottle* bottle, T* sp, BaseLevelScene* my_scene, float jiasu);
};

// 太阳攻击策略 - 具体策略类，实现太阳塔的攻击行为
class SunAttackStrategy : public AttackStrategy {
public:
    // 实现基类的纯虚函数，定义太阳塔的攻击逻辑
    void attack(Tower* tower, BaseLevelScene* scene, std::vector<Monster*>& monsters, 
                char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu) override;
    
private:
    // 太阳塔的攻击实现，范围攻击
    void SunAttack(Sun* sun, BaseLevelScene* my_scene, std::vector<Monster*>& monsters, float jiasu);
};

// 飞机攻击策略 - 具体策略类，实现飞机塔的攻击行为
class PlaneAttackStrategy : public AttackStrategy {
public:
    // 实现基类的纯虚函数，定义飞机塔的攻击逻辑
    void attack(Tower* tower, BaseLevelScene* scene, std::vector<Monster*>& monsters, 
                char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu) override;
    
private:
    // 模板函数，处理飞机塔对目标的攻击
    template<class T>
    bool AttackSprite(MyPlane* plane, T* sp, BaseLevelScene* my_scene, std::vector<Monster*>& monsters, float jiasu);
    
};

// 大便攻击策略 - 具体策略类，实现粪便塔的攻击行为
class ShitAttackStrategy : public AttackStrategy {
private:
    //static const float speed;  // 粪便飞行速度
    
public:
    // 实现基类的纯虚函数，定义粪便塔的攻击逻辑
    void attack(Tower* tower, BaseLevelScene* scene, std::vector<Monster*>& monsters, 
                char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu) override;
    
private:
    // 模板函数，处理粪便塔对目标的攻击
    template<class T>
    bool AttackSprite(Shit* shit, T* sp, BaseLevelScene* my_scene, float jiasu);
    
};









// 风扇攻击策略 - 具体策略类，实现风扇塔的攻击行为
class FanAttackStrategy : public AttackStrategy {
private:
    static const float speed;  // 四叶草飞行速度
    
public:
    // 实现基类的纯虚函数，定义风扇塔的攻击逻辑
    void attack(Tower* tower, BaseLevelScene* scene, std::vector<Monster*>& monsters, 
                char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu) override;
    
private:
    // 模板函数，处理风扇塔对目标的攻击
    template<class T>
    bool AttackSprite(Fan* fan, T* sp, BaseLevelScene* my_scene, float jiasu, std::vector<Monster*>& monsters);
    
    // 创建四叶草精灵
    void CloverProduct(Fan* fan, Scene* my_scene);
    
    // 处理四叶草对目标的伤害
    template<class T>
    void CloverDemage(Fan* fan, BaseLevelScene* my_scene, T* sp);
};

// 魔法攻击策略 - 具体策略类，实现魔法塔的攻击行为
class MagicAttackStrategy : public AttackStrategy {
public:
    // 实现基类的纯虚函数，定义魔法塔的攻击逻辑
    void attack(Tower* tower, BaseLevelScene* scene, std::vector<Monster*>& monsters, 
                char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu) override;
    
private:
    // 魔法塔的攻击实现，持续光束攻击（模板化，支持 Monster 和 Obstacle）
    template<class T>
    void MagicBeamAttack(MagicTower* magic, BaseLevelScene* my_scene, std::vector<Monster*>& monsters, T* sp, float jiasu);

    // 创建魔法光束（模板化目标类型）
    template<class T>
    void CreateMagicBeam(MagicTower* magic, BaseLevelScene* my_scene, T* target);

    // 更新光束位置和方向（模板化目标类型）
    template<class T>
    void UpdateBeamPosition(MagicTower* magic, T* target);

    // 处理光束对目标的持续伤害（模板化目标类型）
    template<class T>
    void ProcessBeamDamage(MagicTower* magic, BaseLevelScene* my_scene, T* target, float currentTime);
};