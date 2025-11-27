#pragma once

#include "cocos2d.h"
#include"MonsterConfigs.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace cocos2d::ui;
extern float beishu;

class BaseLevelScene;

class Monster : public cocos2d::Sprite {
protected:
    Vec2  endPos;                   //��ֹλ��
    int startPosIndex;              //��ʼ����
    int PathIndex = 0;              //·������
    std::string name;               //������
    int maxHp;                      //���Ѫ��
    int damage;                     //������
    LoadingBar* _HP;                // ������Ч����ʾѪ��
    Sprite* hpback;                 //Ѫ������
    bool ishpvs = false;            //HP�Ƿ�ɼ�
    float  speed;                   // �ƶ��ٶ�
    bool pause;                     //��ͣ
    int   reward;                   // ��ɱ����
    int   health;                   // ����ֵ
    bool  isDead = false;           //�Ƿ�����
public:
    Speed* speedaction;
    static Monster* create(const std::string& monsterName,  
        const std::vector<Vec2>& path, int startIndex,bool pause); //��̬�������������ݹ���������������ļ�����һ���������
    bool initWithPath(const std::string& monsterName,  
        const std::vector<Vec2>& path, int startIndex,bool pause); //�ù������͹ؿ�·������ʼ�����������
    void moveAlongPath(const std::vector<Vec2>& path);             //������ƶ��߼�
    void toDie(BaseLevelScene* my_scene);                          //�ù�������
    void getHurt(int value);                                       //�ù�������
    void updateHealthBar();                                        //����Ѫ��
    bool checkLive()const { return (!isDead); };                   //�������Ƿ�����
    //set����
    //���ù���Ѫ��
    void setHealth(int health) { this->health = health; };
    void setHpVisible(bool isVisible);                         //����Ѫ���ɼ�
    void setDamage(int value) { this->damage = value; }        //���ù����˺���ֵ
    void setSpeed(int value) { this->speed = value; }          //���ù����ٶ�
    void setPause(bool isPause){this->pause=isPause;}          //������ͣ״̬
    void setReward(int reward) { this->reward=reward; }        //������ͣ״̬
    //get����
    //��ȡ��������ֵ
    int getHealth()const{return health;}                       //��ȡѪ��
    bool getHpVisibleState()const{return ishpvs;}              //��ȡѪ����ʾ״̬
    std::string getMonsterName()const { return name; }         //��ȡ���޵�����
    int getDamage()const{return damage;}                       //��ȡ�����˺���ֵ
    int getPathIndex()const{return PathIndex;}                 //��ȡ���ﵱǰ��·������
    int getSpeed()const { return speed; }                      //��ȡ�����ٶ�
    bool getPause()const {return pause;}                       //��ȡ��ͣ״̬
    int  getReward()const{ return reward;}
    virtual void SpecialAttack() {  }

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
//BossYellow��
class BossYellow :public Monster
{
public:
    void SpecialAttack()override;
};
//BossSheep��
class BossSheep :public Monster
{
public:
    void SpecialAttack()override;
};
