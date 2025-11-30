#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include<string>
USING_NS_CC;
using namespace cocos2d::ui;

// 前向声明
class BaseLevelScene;

struct ObData {
	std::string road_path; // 图片路径
	int size; // 尺寸,1表示1*1，2表示1*2，4表示2*2
	int money; // 被破坏后得到的钱
	int hp; // 血量
};

class Obstacle {
private:
	
	int index;
	int size; // 尺寸,1表示1*1，2表示1*2，4表示2*2
	int health; // 血量
	int maxHp;
	int shield; // 护盾值
	int maxShield; // 最大护盾值
	int x, y; // 指定障碍物在地图网格中的位置
	bool isDead = false;
	LoadingBar* _HP;       // 进度条有效显示血量
	LoadingBar* _ShieldBar; // 护盾条
public:
	
	Sprite* hpback;
	Sprite* shieldback; // 护盾背景
	bool ishpvs=false; // HP是否可见
	bool isShieldVisible = false; // 护盾血条是否可见
	Sprite* curr;
	Sprite* shieldSprite; // 护盾精灵
	int reward; // 被破坏后得到的钱
	static ObData Ob[10];
	int GetIndex(){return index;}
	Obstacle(int index) :index(index),size(Ob[index].size),maxHp(Ob[index].hp), reward(Ob[index].money), health(Ob[index].hp), shield(0), maxShield(0) {}
	virtual ~Obstacle() {} // 添加虚析构函数，使类成为多态类型
	void Produce(BaseLevelScene* my_scene, int x_, int y_);//生成障碍物

	Vec2 getPosition() { return curr->getPosition(); }

	void toDie(BaseLevelScene* my_scene);

	void updateHealthBar(); 
	void updateShieldBar(); // 更新护盾条

	void addChild(Sprite* c) {
		curr->addChild(c);
	}
	void setHpVisible(bool isVisible){ _HP->setVisible(isVisible); hpback->setVisible(isVisible); ishpvs = isVisible;}	//设置血条可见
	void setShieldVisible(bool isVisible){ _ShieldBar->setVisible(isVisible); shieldback->setVisible(isVisible); isShieldVisible = isVisible;}	//设置护盾条可见
	bool getShieldVisibleState()const { return isShieldVisible; }
	bool getHpVisibleState()const { return ishpvs; }
	int  GetSize()const{return size;}
	void getHurt(int value); // 修改为处理护盾和血量的伤害
	int getHealth()const { return health; }                       //获取血量
	void setHealth(int health) { this->health = health; }
	int getShield()const { return shield; } // 获取护盾值
	void setAllShield(int shield) { this->shield = shield; this->maxShield = shield; } // 设置护盾值和最大护盾值
	bool hasShield()const { return maxShield > 0; } // 判断是否有护盾
};