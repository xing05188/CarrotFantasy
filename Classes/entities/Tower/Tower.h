#pragma once
#include "cocos2d.h"
#include"entities/Monster/Monster.h"
#include"entities/Obstacle/Obstacle.h"
#include"AttackStrategy.h"
#include<vector>
#include<string>
#include<map>
USING_NS_CC;

// 塔的总数量
#define TOWER_NUM 6

// 前向声明
class BaseLevelScene;
class Tower;
class Obstacle;
class Fan;
class MagicTower;

// 塔基类 - 所有防御塔的父类
class Tower {
protected:
    int index, grade;        // 塔的类型索引和等级
    int range, demage;       // 攻击范围和攻击力
    AttackStrategy* attackStrategy;  // 攻击策略指针（策略模式）
public:
    // 构造函数，初始化塔的基本属性
    Tower(int index_,int _grade=0) :index(index_), grade(_grade) { 
        range = range_table[index][grade]; 
        demage = demage_table[index][grade];
        attackStrategy = nullptr;
    }
    
    // 虚析构函数，确保派生类正确析构
    virtual ~Tower() {
        if (attackStrategy) {
            delete attackStrategy;
        }
    }
    
    // 静态数据表，存储不同类型和等级塔的属性
    static std::string tower_table[TOWER_NUM][3];    // 塔的图片资源表
    static std::string base_table[TOWER_NUM];        // 塔基图片资源表
    static Vec2 anchorpoint_table[TOWER_NUM][2];    // 锚点表
    static int build_cost[TOWER_NUM];                // 建造成本表
    static int up_cost[TOWER_NUM][2];                // 升级成本表
    static int range_table[TOWER_NUM][3];            // 攻击范围表
    static int demage_table[TOWER_NUM][3];           // 攻击力表
    static int sell_money[TOWER_NUM][3];             // 出售价格表
    static std::map<int, std::string>sale_graph;     // 出售按钮图片映射
    static std::map<int, std::string>up_graph;       // 升级按钮图片映射
    static std::map<int, std::string>noup_graph;     // 不可升级按钮图片映射
    static float interval_table[TOWER_NUM];          // 攻击间隔表

    // 塔的视觉元素和位置
    Sprite* sprite_mark, * sp_base;  // 标记精灵和基座精灵
    Vec2 pos;                        // 塔的位置
    float interval;                  // 当前攻击间隔

    // 塔的基本操作方法
    void build(BaseLevelScene*, Vec2);      // 建造塔
    void destroy(BaseLevelScene*);           // 销毁塔
    
    // UI相关静态精灵
    static Sprite* curr_up, * curr_sale, * curr_range;
    
    // 塔的更新和UI交互方法
    void update(BaseLevelScene*, Vec2);              // 更新塔的状态
    void UpMenuAppear(BaseLevelScene* my_scene, Vec2& position);  // 显示升级菜单
    void UpMenuGone(BaseLevelScene*);                // 隐藏升级菜单
    
    // 获取塔属性的访问器方法
    int GetIndex()const { return index; }
    int GetGrade()const {return grade;}
	int GetRange()const { return range; }
    int GetDemage()const { return demage; }
    
    // 设置攻击策略（策略模式）
    void setAttackStrategy(AttackStrategy* strategy) {
        if (attackStrategy) {
            delete attackStrategy;
        }
        attackStrategy = strategy;
    }
    
    // 虚攻击方法，使用策略模式进行攻击
    virtual void attack(BaseLevelScene* scene, std::vector<Monster*>& monsters, 
                       char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu) {
        if (attackStrategy) {
            attackStrategy->attack(this, scene, monsters, isTarget, tar_m, tar_o, jiasu);
        }
    }
};

// 瓶子塔类 - 继承自Tower，发射炮弹攻击
class Bottle :public Tower {
public:
    float tower_angle;      // 塔的旋转角度
    Sprite* curr_shell;     // 当前炮弹精灵
    float curr_dis, de_time;  // 当前距离和衰减时间
    
    Bottle(int index_,int grade) :Tower(index_,grade), tower_angle(0) { }
    static std::string bottle_shell[3];  // 炮弹图片资源表
};

// 太阳塔类 - 继承自Tower，发射光环攻击
class Sun :public Tower {
public:
    Sprite* curr_halo;     // 当前光环精灵
    Sun(int index_, int grade) :Tower(index_, grade) {}
};

// 飞机塔类 - 继承自Tower，发射光线攻击
class MyPlane :public Tower {
public:
    Sprite* curr_ray;      // 当前光线精灵
    float tower_angle;     // 塔的旋转角度
    float de_ang;          // 角度衰减
    
    MyPlane(int index_, int grade) :Tower(index_, grade), tower_angle(90) {}
    static std::string ray[3];  // 光线图片资源表
};

// 粪便塔类 - 继承自Tower，发射粪便攻击
class Shit :public Tower {
public:
    Sprite* curr_shell;    // 当前炮弹精灵
    Sprite* sprite_mark;   // 标记精灵
    float de_time;         // 衰减时间
    
    Shit(int index_, int grade) :Tower(index_, grade) {}
    static std::string shit_shell[3];  // 粪便图片资源表
};

// 风扇塔类 - 继承自Tower，发射旋转的四叶草攻击
class Fan :public Tower {
public:
    Sprite* curr_clover;   // 当前四叶草精灵
    float de_time;         // 四叶草飞行时间
    
    Fan(int index_, int grade) :Tower(index_, grade) {}
    static std::string clover[3];  // 四叶草图片资源表
};

// 魔法塔类 - 继承自Tower，发射持续光束攻击
class MagicTower :public Tower {
public:
    Sprite* curr_beam;     // 当前光束精灵
    Monster* lockedTarget; // 锁定的目标怪物
    float beamDamageInterval; // 光束伤害间隔
    float lastDamageTime;  // 上次造成伤害的时间
    
    MagicTower(int index_, int grade) :Tower(index_, grade), curr_beam(nullptr), lockedTarget(nullptr), beamDamageInterval(0.5f), lastDamageTime(0.0f) {}
    
    // 析构函数，清理光束资源
    virtual ~MagicTower() {clearBeam();}
    // 添加清理方法
    void clearBeam() {
        if (curr_beam) {
            curr_beam->removeFromParent();
            curr_beam = nullptr;
        }
        lockedTarget = nullptr;
    }
    static std::string magic_beam[3];  // 光束图片资源表
};