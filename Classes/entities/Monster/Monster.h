#pragma once

#include "cocos2d.h"
#include"MonsterConfigs.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace cocos2d::ui;
extern float beishu;

// 前向声明 Boss 技能装饰器接口
class BossSkill;

class BaseLevelScene;

class Monster : public cocos2d::Sprite {
protected:
    Vec2  endPos;                   // 终点位置
    int startPosIndex;              // 起始索引
    int PathIndex = 0;              // 路径索引
    std::string name;               // 怪物名称
    int maxHp;                      // 最大血量
    int damage;                     // 攻击伤害
    LoadingBar* _HP;                // 血条UI控件
    Sprite* hpback;                 // 血条背景
    bool ishpvs = false;            // HP是否可见
    float  speed;                   // 移动速度
    bool pause;                     // 暂停
    int   reward;                   // 击杀奖励
    int   health;                   // 当前血量
    bool  isDead = false;           // 是否死亡
    BossSkill* bossSkill = nullptr; // Boss 技能装饰器
public:
    Speed* speedaction;
    static Monster* create(const std::string& monsterName,  
        const std::vector<Vec2>& path, int startIndex,bool pause); // 静态创建怪物对象，根据怪物名称、路径、起始索引和暂停状态创建怪物
    bool initWithPath(const std::string& monsterName,  
        const std::vector<Vec2>& path, int startIndex,bool pause); // 初始化怪物，根据怪物名称、路径、起始索引和暂停状态初始化怪物
    void moveAlongPath(const std::vector<Vec2>& path);             // 沿着路径移动逻辑
    void toDie(BaseLevelScene* my_scene);                          // 怪物死亡处理
    void getHurt(int value);                                       // 怪物受到伤害
    void updateHealthBar();                                        // 更新血条
    bool checkLive()const { return (!isDead); };                   // 检查怪物是否存活
    //set方法
    //设置怪物血量
    void setHealth(int health) { this->health = health; };
    void setHpVisible(bool isVisible);                         // 设置血条可见性
    void setDamage(int value) { this->damage = value; }        // 设置怪物攻击伤害值
    void setSpeed(int value) { this->speed = value; }          // 设置怪物移动速度
    void setPause(bool isPause){this->pause=isPause;}          // 设置怪物暂停状态
    void setReward(int reward) { this->reward=reward; }        // 设置怪物奖励
    //get方法
    //获取怪物的属性值
    int getHealth()const{return health;}                       // 获取血量
    bool getHpVisibleState()const{return ishpvs;}              // 获取血条显示状态
    std::string getMonsterName()const { return name; }         // 获取怪物名称
    int getDamage()const{return damage;}                       // 获取攻击伤害值
    int getPathIndex()const{return PathIndex;}                 // 获取怪物当前路径索引
    int getSpeed()const { return speed; }                      // 获取移动速度
    bool getPause()const {return pause;}                       // 获取暂停状态
    int  getReward()const{ return reward;}
    virtual ~Monster();

    // 设置 / 触发 Boss 技能（装饰器入口）
    void setBossSkill(BossSkill* skill) { bossSkill = skill; }
    virtual void SpecialAttack();

    /**
     * @brief 初始化怪物并创建血条
     * @param monster 怪物对象指针
     * @param monsterName 怪物名称
     * @param path 怪物移动路径
     * @param startIndex 起始位置索引
     * @param pause 是否暂停
     * @return 初始化是否成功
     */
    static bool initializeMonsterWithHealthBar(Monster *monster,
                                               const std::string &monsterName,
                                               const std::vector<Vec2> &path,
                                               int startIndex,
                                               bool pause);
    
};
//BossYellow类
class BossYellow :public Monster
{
public:
    void SpecialAttack() override;
};
//BossSheep类
class BossSheep :public Monster
{
public:
    void SpecialAttack() override;
};