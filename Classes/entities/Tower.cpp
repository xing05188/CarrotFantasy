#include"Tower.h"
#include"BaseLevelScene.h"
#include"music.h"
#include<cmath>

#define CELL_SIZE 64
const double PI = 3.1415926;
std::string Tower::tower_table[TOWER_NUM][3] = { {"Towers/huo0.png","Towers/huo1.png","Towers/huo2.png"},
	{"Towers/sun0.png","Towers/sun1.png","Towers/sun2.png"}, {"Towers/plane0.png","Towers/plane1.png"
,"Towers/plane2.png"},{{"Towers/shit0.png"},{"Towers/shit1.png"},{"Towers/shit2.png"}} };
std::string Tower::base_table[TOWER_NUM] = { "Towers/huobase.png" ,"Towers/sunbase.png","Towers/planebase.png",
"Towers/shitbase.png" };
Vec2 Tower::anchorpoint_table[TOWER_NUM][2] = { {Vec2(0.5,0.5),Vec2(0.4,0.46)},{Vec2(0.5,0.5),Vec2(0.5,0.5)},
{Vec2(0.5,0.5),Vec2(0.5,0.5)},{Vec2(0.5,1.0),Vec2(0.5,0.5)} };

int Tower::build_cost[TOWER_NUM] = { 100,180 ,220,120 };
int Tower::demage_table[TOWER_NUM][3] = { {20,35,50},{30,45,60},{50,80,110},{5,10,20} };
int Tower::range_table[TOWER_NUM][3] = { {200,250,300},{200,250,350},{250,375,400},{200,250,300} };

int Tower::up_cost[TOWER_NUM][2] = { {180,260},{260,320},{320,380},{220,260} };
int Tower::sell_money[TOWER_NUM][3] = { {80,224,432},{144,352,608},{176,432,736},{96,272,480} };

std::map<int, std::string> Tower::sale_graph = { {80,"Towers/sale_80.png"}  , {96,"Towers/sale_96.png"},
	{ 144,"Towers/sale_144.png" } ,{176,"Towers/sale_176.png"}, { 224,"Towers/sale_224.png" },
	{272,"Towers/sale_272.png"}, {352,"Towers/sale_352.png"} ,{432,"Towers/sale_432.png"}  ,
	{480,"Towers/sale_480.png"}, {608,"Towers/sale_608.png"},{736,"Towers/sale_736.png"} };

std::map<int, std::string> Tower::up_graph = { {180,"Towers/up_180.png"},{220,"Towers/up_220.png"},
	{260,"Towers/up_260.png"},{320,"Towers/up_320.png"},{380,"Towers/up_380.png"} };

std::map<int, std::string> Tower::noup_graph = { {180,"Towers/noup_180.png"},{220,"Towers/noup_220.png"},
	{260,"Towers/noup_260.png"},{320,"Towers/noup_320.png"} ,{380,"Towers/noup_380.png"} };

Sprite* Tower::curr_sale;
Sprite* Tower::curr_up;
Sprite* Tower::curr_range;

const float Bottle::speed = 800;

const float Shit::speed = 500;

float Tower::interval_table[TOWER_NUM] = { 0.8,1.5,2.5,1.0 };

std::string Bottle::bottle_shell[3] = { "Towers/shell1-1.png","Towers/shell1-2.png" ,"Towers/shell1-3.png" };
//��Ϸ�����в���Ҫ�䶯�����ĳ�ʼ��

std::string MyPlane::ray[3] = { "Towers/ray0.png","Towers/ray1.png","Towers/ray2.png" };

std::string Shit::shit_shell[3] = { "Towers/shitshell1.png","Towers/shitshell2.png","Towers/shitshell3.png" };


Tower* createTower(int index,int grade)
{
	switch (index) {
	case 0:return new Bottle(index,grade);
	case 1:return new Sun(index,grade);
	case 2:return new MyPlane(index, grade);
	case 3:return new Shit(index, grade);
	default:return nullptr;
	}
}

int count = 0;

float getAngleBetweenVec2(const Vec2& v1, const Vec2& v2);//��ȡ��������֮��ļн�
bool isColliding(Sprite* spriteA, Sprite* spriteB);//������ײ���

void Tower::build(BaseLevelScene* my_scene, Vec2 position)
{//�ж�Ǯ�������ģ���������ʵ�֣�Ǯ����ʱ�����ᴴ���������
	auto base = Sprite::create(base_table[index]);
	base->setAnchorPoint(anchorpoint_table[index][0]);
	base->setPosition(position);
	my_scene->addChild(base);
	sp_base = base;
	auto tower = Sprite::create(tower_table[index][grade]);
	tower->setAnchorPoint(anchorpoint_table[index][1]);
	tower->setPosition(position);
	if (index == 1) tower->setScale(0.8);
	my_scene->addChild(tower);
	sprite_mark = tower;
	pos = position;
	interval = 0;
}

void Tower::destroy(BaseLevelScene* my_scene)
{
	my_scene->removeChild(sprite_mark);
	my_scene->removeChild(sp_base);
	sprite_mark->release();
	sp_base->release();
	my_scene->updateMoney(sell_money[index][grade]);
	Music::getInstance()->TowerSell();
}

void Tower::update(BaseLevelScene* my_scene, Vec2 position)
{
	if (grade >= 2 || my_scene->getMoney() < up_cost[index][grade]) return;
	my_scene->updateMoney(-up_cost[index][grade]);
	my_scene->removeChild(sprite_mark);
	sprite_mark->release();
	grade += 1;
	auto tower = Sprite::create(tower_table[index][grade]);
	if (index == 1) tower->setScale(0.8);
	tower->setAnchorPoint(anchorpoint_table[index][1]);
	tower->setPosition(position);
	my_scene->addChild(tower);
	sprite_mark = tower;
	range = range_table[index][grade];
	demage = demage_table[index][grade];
	Music::getInstance()->TowerUpdata();
}

void Tower::UpMenuAppear(BaseLevelScene* my_scene, Vec2& position)
{
	Sprite* up;
	if (grade < 2 && my_scene->getMoney() >= up_cost[index][grade]) up = Sprite::create(up_graph[up_cost[index][grade]]);
	else if (grade < 2) up = Sprite::create(noup_graph[up_cost[index][grade]]);
	else up = Sprite::create("Towers/upgrade_top.png");
	auto sale = Sprite::create(sale_graph[sell_money[index][grade]]);
	auto range = Sprite::create("Towers/range.png");
	range->setScale(range_table[index][grade] / range->getContentSize().width);
	up->setScale(CELL_SIZE / up->getContentSize().width);
	sale->setScale(CELL_SIZE / sale->getContentSize().width);
	range->setPosition(position);
	up->setPosition(Vec2(position.x, position.y + CELL_SIZE));
	sale->setPosition(Vec2(position.x, position.y - CELL_SIZE));
	my_scene->addChild(up);
	my_scene->addChild(sale);
	my_scene->addChild(range);
	curr_up = up; curr_sale = sale; curr_range = range;
}

void Tower::UpMenuGone(BaseLevelScene* my_scene)
{
	my_scene->removeChild(curr_up);
	my_scene->removeChild(curr_sale);
	my_scene->removeChild(curr_range);
	curr_up->release();
	curr_sale->release();
	curr_range->release();
}

template<class T>
void DemageSprite(T*sp,int demage)
{
	
	sp->getHurt(demage);
	if (!sp->getHpVisibleState()) {
		sp->setHpVisible(true);
	}
	sp->updateHealthBar();
}


void Bottle::attack(BaseLevelScene* my_scene, std::vector<Monster*>& monsters, char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu)
{
	
	/*Director::getInstance()->end();*/
	/*Director::getInstance()->end();*/
	interval = 0;
	if (isTarget == 1) {
		if (AttackSprite(tar_m, my_scene,jiasu))
			return;//������ˣ����أ�����������
	}
	if (isTarget == 2) {
		if (AttackSprite(tar_o, my_scene,jiasu))
			return;
	}


	int size = monsters.size();
	for (auto it = monsters.begin(); it != monsters.end(); it++) {
		if ((*it)->getHealth() <= 0) continue;
		if (AttackSprite((*it), my_scene,jiasu))
			break;
	}


}


template<class T>
bool Bottle::AttackSprite(T* sp, BaseLevelScene* my_scene, float jiasu)
{
	Vec2 m_pos = sp->getPosition();
	//�������
	double distance = sqrt((m_pos.x - pos.x) * (m_pos.x - pos.x) + (m_pos.y - pos.y) * (m_pos.y - pos.y));
	if (distance < range / 2) {//���ڷ�Χ�ڣ��򷢶�����
		Vec2 direction = m_pos - pos;
		direction.normalize();
		auto ang = direction.getAngle();
		ang = m_pos.y < pos.y ? (ang < 0 ? fabs(ang / PI) * 180 : ang / PI * 180) : (ang > 0 ? -ang / PI * 180 : ang / PI * 180);
		auto action1 = RotateTo::create(fabs(ang / (360*jiasu)), ang);//����ת��



		auto callback2 = CallFunc::create([this, my_scene, sp, m_pos, ang,jiasu]() {

			tower_angle = ang;//�Ƕȸ���
			ShellProduct(my_scene);//�ӵ�������
			//Vec2 start_pos = curr_shell->getPosition();//��ȡ��ʼλ��
			//curr_dis = sqrt((m_pos.x - start_pos.x) * (m_pos.x - start_pos.x) + (m_pos.y - start_pos.y) * (m_pos.y - start_pos.y)); 
			//auto action2 = MoveTo::create(curr_dis / speed, m_pos);
			//auto callback1 = CallFunc::create([this, my_scene, &monsters, it, ang]() {
			//	ShellDemage(my_scene, monsters, it); });
			//auto sequence1 = Sequence::create(action2, callback1, nullptr);
			//curr_shell->runAction(sequence1); 

			auto callback3 = CallFunc::create([sp, this,jiasu]() {//�ӵ�׷��
				auto m_pos = sp->getPosition();
				auto s_pos = curr_shell->getPosition();

				auto move = m_pos - s_pos;
				move.normalize();
				move = move * 10;
				//����ӵ�λ�ú͹���λ�ã������λ���ƶ�10�����ص�
				auto action3 = MoveBy::create(10 / (speed*jiasu), move);
				curr_shell->runAction(action3);
				});
			auto callback4 = CallFunc::create([my_scene, sp, this]() {
				if ((sp->getPosition() - curr_shell->getPosition()).distance(Vec2(0, 0)) < 10.f) {
					curr_shell->stopAllActions();//һ�������㹻С����ֹͣ����
					ShellDemage(my_scene, sp);
				}
				});
			de_time = 10 / (speed*jiasu);//�����ظ�������ƶ�����ʵ��׷��Ч��
			auto sequence3 = Sequence::create(callback3, DelayTime::create(de_time), callback4, nullptr);
			auto repeat = RepeatForever::create(sequence3);
			curr_shell->runAction(repeat);
			});
		auto sequence2 = Sequence::create(action1, callback2, nullptr);

		sprite_mark->runAction(sequence2);
		return true;
	}
	return false;//����ڷ�Χ�⣬˵��û�򣬷���0
}


void Bottle::ShellProduct(Scene* my_scene)//�����ӵ�
{
	auto shell = Sprite::create(bottle_shell[grade]);
	Vec2 start_pos;
	start_pos.x = pos.x;/*+ (sprite_mark->getContentSize().height+shell->getContentSize().width)* cos(PI*tower_angle/180)/2;*/
	start_pos.y = pos.y;/* + (sprite_mark->getContentSize().height+shell->getContentSize().width)*sin(PI * tower_angle/180)/ 2;*/
	shell->setPosition(start_pos);
	shell->setScale(0.6);
	/*shell->setAnchorPoint(Vec2(0.5, 0.5));*/
	/*shell->runAction(RotateBy::create(0.001, -tower_angle));*/
	my_scene->addChild(shell);
	curr_shell = shell;
	Music::getInstance()->bottleSound();
}


template<class T>
void Bottle::ShellDemage(BaseLevelScene* my_scene, T* sp)//�ӵ��Թ�������˺����ӵ���ʧ
{
	curr_shell->removeFromParent();//���ӵ���������������ü�����һ���Զ��ͷ�
	auto c = cocos2d::Sprite::create();
	if (!c) {
		CCLOG("Failed to create c sprite.");
		return;
	}
	my_scene->addChild(c);
	c->setPosition(sp->getPosition());  // ����������������λ���������ͬ
	c->setScale(1.4f);
	// 4. �������������� 4 ��ͼƬ
	cocos2d::Vector<cocos2d::SpriteFrame*> frames;
	for (int i = 0; i <= 1; ++i) {
		std::string frameName = "Towers/texiao_" + std::to_string(i) + ".png";
		auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 44, 45)); // ����ÿ��ͼƬ�Ĵ�С�� 64x64
		if (frame) {
			frames.pushBack(frame);
		}
		else {
			CCLOG("Failed to load frame: %s", frameName.c_str());
		}
	}
	// 5. ���û�м��ص�����֡��ֱ�ӷ���
	if (frames.empty()) {
		CCLOG("No frames found for death animation, skipping.");
		return;
	}
	// 6. ����������ÿ֡���� 0.2 ��
	auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
	auto animate = cocos2d::Animate::create(animation);
	// 7. ������ɺ�ɾ����ʱ��������������
	auto onDeathComplete = cocos2d::CallFunc::create([c]() {
		c->removeFromParent();
		});
	// 8. ������������������������������ʱ����
	//Music::getInstance()->duanSound();
	c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
	DemageSprite(sp, demage);
	if (sp->getHealth() <= 0) {
		(sp)->toDie(my_scene);
	}

}


float getAngleBetweenVec2(const Vec2& v1, const Vec2& v2)
{
	// ������
	float dot = v1.dot(v2);

	// �������������ĳ���
	float v1Length = v1.length();
	float v2Length = v2.length();

	// ʹ�÷����Һ�������нǣ����ȣ�
	float angle = acos(dot / (v1Length * v2Length));

	// ������ת��Ϊ�Ƕ�
	return CC_RADIANS_TO_DEGREES(angle);
}





















//sun------------------------------------------------------------------------------------------------------------
void Sun::attack(BaseLevelScene* my_scene, std::vector<Monster*>& monsters, char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu)
{
	interval = 0;
	//�����ϰ����ڷ�Χ�ڣ����й����ڷ�Χ�ڣ����ᴥ����Χ�Ե��˺�
	if (isTarget == 2) {
		Vec2 o_pos = tar_o->getPosition();
		float distance = sqrt((o_pos.x - pos.x) * (o_pos.x - pos.x) + (o_pos.y - pos.y) * (o_pos.y - pos.y));
		if (distance < range / 2) {
			SunAttack(my_scene, monsters,jiasu);
			return;
		}
	}
	for (auto it = monsters.begin(); it != monsters.end(); it++) {
		if ((*it)->getHealth() <= 0) continue;
		Vec2 m_pos = (*it)->getPosition();
		float distance = sqrt((m_pos.x - pos.x) * (m_pos.x - pos.x) + (m_pos.y - pos.y) * (m_pos.y - pos.y));
		if (distance < range / 2) {
			SunAttack(my_scene, monsters,jiasu);
			return;
		}
	}
}

//�������䷶Χ�ڵ����й��޺��ϰ���
void Sun::SunAttack(BaseLevelScene* my_scene, std::vector<Monster*>& monsters, float jiasu)
{
	Music::getInstance()->sunSound();
	curr_halo = Sprite::create("Towers/sunhalo.png");
	curr_halo->setPosition(pos);
	curr_halo->setScale(0.8 * range / curr_halo->getContentSize().width);
	my_scene->addChild(curr_halo);
	auto scale_up = ScaleBy::create(0.3/jiasu, 1.25);//��Ȧ�Ŵ�Ķ���

	auto callback = CallFunc::create([this, my_scene, &monsters]() {
		for (auto it = my_scene->Obstacles.begin(); it != my_scene->Obstacles.end(); it++) {
			Vec2 o_pos = (*it).second->getPosition();
			float distance = sqrt((o_pos.x - pos.x) * (o_pos.x - pos.x) + (o_pos.y - pos.y) * (o_pos.y - pos.y));
			if (distance < range / 2&& (*it).second->getHealth() > 0) {
				auto c = cocos2d::Sprite::create();
				my_scene->addChild(c);
				c->setPosition(o_pos);
				c->setScale(1.6f);
				cocos2d::Vector<cocos2d::SpriteFrame*> frames;
				std::string frameName = "Towers/texiao_2.png";
				auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 26, 15));
				if (frame) {
					frames.pushBack(frame);
				}
				auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
				auto animate = cocos2d::Animate::create(animation);
				auto onDeathComplete = cocos2d::CallFunc::create([c]() {
					c->removeFromParent();
					});
				//Music::getInstance()->duanSound();
				c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
				DemageSprite((*it).second, demage);
				if ((*it).second->getHealth() <= 0) (*it).second->toDie(my_scene);
			}
		}
		//�ڷ�Χ���ڵģ������ܵ��˺�
		for (auto it = monsters.begin(); it != monsters.end(); it++) {
			Vec2 m_pos = (*it)->getPosition();
			float distance = sqrt((m_pos.x - pos.x) * (m_pos.x - pos.x) + (m_pos.y - pos.y) * (m_pos.y - pos.y));
			if (distance < range / 2&& (*it)->getHealth() > 0) {
				auto c = cocos2d::Sprite::create();
				my_scene->addChild(c);
				c->setPosition(m_pos);
				c->setScale(1.8f);
				cocos2d::Vector<cocos2d::SpriteFrame*> frames;
				std::string frameName = "Towers/texiao_2.png";
				auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 26, 15)); 
				if (frame) {
					frames.pushBack(frame);
				}
				auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
				auto animate = cocos2d::Animate::create(animation);
				auto onDeathComplete = cocos2d::CallFunc::create([c]() {
					c->removeFromParent();
					});
				//Music::getInstance()->duanSound();
				c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
				DemageSprite((*it), demage);
				if ((*it)->getHealth() <= 0) (*it)->toDie(my_scene);
			}
		}
		curr_halo->removeFromParent();
		});
	auto seq = Sequence::create(scale_up, callback, nullptr);
	curr_halo->runAction(seq);
	/*Director::getInstance()->end();
		return;*/
}















//Plane-----------------------------------------------------------------------------------------------------------
void MyPlane::attack(BaseLevelScene* my_scene, std::vector<Monster*>& monsters, char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu)//��������
{

	/*if (isTarget == 2) {
		Vec2 o_pos = tar_o->getPosition();
	}*/
	interval = 0;
	if (isTarget == 1) {
		if (AttackSprite(tar_m, my_scene, monsters,jiasu))
			return;//������ˣ����أ�����������
	}
	if (isTarget == 2) {
		if (AttackSprite(tar_o, my_scene, monsters,jiasu))
			return;
	}

	int size = monsters.size();
	for (auto it = monsters.begin(); it != monsters.end(); it++) {
		if ((*it)->getHealth() <= 0) continue;
		if (AttackSprite((*it), my_scene, monsters,jiasu))
			break;
	}
}


template<class T>
bool  MyPlane::AttackSprite(T* sp, BaseLevelScene* my_scene, std::vector<Monster*>& monsters, float jiasu)
{
	Vec2 m_pos = sp->getPosition();
	//�������
	double distance = sqrt((m_pos.x - pos.x) * (m_pos.x - pos.x) + (m_pos.y - pos.y) * (m_pos.y - pos.y));
	if (distance > range / 2) return false;//�����ڷ�Χ�ڣ��򷵻�


	auto callback1 = CallFunc::create([this, m_pos, sp, my_scene, &monsters,jiasu]() {
		de_ang = 0;
		Vec2 tool = m_pos - pos;
		auto ang = CC_RADIANS_TO_DEGREES((tool).getAngle());
		auto m_ang = ang >= 0 ? ang : 360 + ang;
		int Clockwise = m_ang - tower_angle > 0 ? (m_ang - tower_angle > 180 ? 1 : -1) : (tower_angle - m_ang > 180 ? -1 : 1);
		//��ȡת����

		auto TurnCallback = CallFunc::create([this, sp, Clockwise, my_scene, &monsters,jiasu]() {
			sprite_mark->setRotation(-(tower_angle - 90));
			Vec2 m_pos = sp->getPosition();//��ȡ���ﵱǰλ��
			Vec2 tool = m_pos - pos;
			auto ang = CC_RADIANS_TO_DEGREES((tool).getAngle());
			auto m_ang = ang >= 0 ? ang : 360 + ang;
			CCLOG("----------------------------%f,%f", tower_angle, m_ang);
			if (fabs(tower_angle - m_ang) < 5) {
				CCLOG("-----------------------------------------------------------------------stttooopppp");

				sprite_mark->stopAllActions();
				/*sprite_mark->runAction(seq);*/
				auto callback2 = CallFunc::create([this, my_scene]() {
					curr_ray = Sprite::create(ray[grade]);
					curr_ray->setAnchorPoint(Vec2(0.5, 0));
					curr_ray->setScale(CELL_SIZE / curr_ray->getContentSize().width);
					Vec2 ray_pos;
					ray_pos.x = pos.x + 0.5 * sprite_mark->getContentSize().height * cos(CC_DEGREES_TO_RADIANS(tower_angle));
					ray_pos.y = pos.y + 0.5 * sprite_mark->getContentSize().height * sin(CC_DEGREES_TO_RADIANS(tower_angle));
					curr_ray->setPosition(ray_pos);//��������
					curr_ray->setRotation(90 - tower_angle);
					my_scene->addChild(curr_ray);
					});
				auto callback3 = CallFunc::create([this, my_scene, &monsters]() {
					PlaneDemage(my_scene, monsters);
					curr_ray->removeFromParent();
					});
				auto seq = Sequence::create(callback2, DelayTime::create(0.3/jiasu), callback3, nullptr);
				sprite_mark->runAction(seq);


				return;
			}
			tower_angle = tower_angle - Clockwise * 5 < 0 ? 360 + tower_angle - Clockwise * 5 : (tower_angle - Clockwise * 5 >= 360 ? tower_angle - Clockwise * 5 - 360 : tower_angle - Clockwise * 5);
			//�ѽǶȱ�Ϊ��ʱ��ת�������ӵ���ʽ�������˵�һ������
			auto TurnLittle = RotateBy::create(5 / (360.0f*jiasu), Clockwise * 5);
			sprite_mark->runAction(TurnLittle);
			de_ang += 5;
			});
		auto turn_sequence = Sequence::create(TurnCallback, DelayTime::create(5 / (360.0f*jiasu)), nullptr);
		auto action1 = RepeatForever::create(turn_sequence);
		sprite_mark->runAction(action1);
		});

	sprite_mark->runAction(callback1);
	Music::getInstance()->planeSound();
	return true;
}


void MyPlane::PlaneDemage(BaseLevelScene* my_scene, std::vector<Monster*>& monsters)
{
	for (auto it = my_scene->Obstacles.begin(); it != my_scene->Obstacles.end(); it++) {

		if (isColliding(it->second->curr, curr_ray)&& (*it).second->getHealth() > 0) {
			CCLOG("--------------------------------------------------------------cccc");
			auto c = cocos2d::Sprite::create();
			my_scene->addChild(c);
			c->setPosition((*it).second->getPosition());  // ����������������λ���������ͬ
			c->setScale(1.4f);
			cocos2d::Vector<cocos2d::SpriteFrame*> frames;
			for (int i = 3; i <= 4; ++i) {
				std::string frameName = "Towers/texiao_" + std::to_string(i) + ".png";
				auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 44, 47)); // ����ÿ��ͼƬ�Ĵ�С�� 64x64
				if (frame) {
					frames.pushBack(frame);
				}
			}
			auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
			auto animate = cocos2d::Animate::create(animation);
			auto onDeathComplete = cocos2d::CallFunc::create([c]() {
				c->removeFromParent();
				});
			//Music::getInstance()->duanSound();
			c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
			DemageSprite((*it).second, demage);
			if ((*it).second->getHealth() <= 0) (*it).second->toDie(my_scene);
		}
	}
	//�ڷ�Χ���ڵģ������ܵ��˺�
	for (auto it = monsters.begin(); it != monsters.end(); it++) {
		if (isColliding(*it, curr_ray)&& (*it)->getHealth() > 0) {
			CCLOG("--------------------------------------------------------------cccc");
			auto c = cocos2d::Sprite::create();
			my_scene->addChild(c);
			c->setPosition((*it)->getPosition());  // ����������������λ���������ͬ
			c->setScale(1.4f);
			// 4. �������������� 4 ��ͼƬ
			cocos2d::Vector<cocos2d::SpriteFrame*> frames;
			for (int i = 3; i <= 4; ++i) {
				std::string frameName = "Towers/texiao_" + std::to_string(i) + ".png";
				auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 44, 47)); // ����ÿ��ͼƬ�Ĵ�С�� 64x64
				if (frame) {
					frames.pushBack(frame);
				}
			}
			auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
			auto animate = cocos2d::Animate::create(animation);
			auto onDeathComplete = cocos2d::CallFunc::create([c]() {
				c->removeFromParent();
				});
			//Music::getInstance()->duanSound();
			c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
			DemageSprite((*it), demage);
			if ((*it)->getHealth() <= 0) (*it)->toDie(my_scene);
		}
	}

}
bool isColliding(Sprite* spriteA, Sprite* spriteB) {//������ײ���
	return spriteA->getBoundingBox().intersectsRect(spriteB->getBoundingBox());
}



//shit---------------------------------------------------------------------------------------------------------------
void Shit::attack(BaseLevelScene* my_scene, std::vector<Monster*>& monsters, char isTarget, Monster* tar_m, Obstacle* tar_o, float jiasu)//��������
{
	interval = 0;
	if (isTarget == 1) {
		if (AttackSprite(tar_m, my_scene, jiasu))
			return;//������ˣ����أ�����������
	}
	if (isTarget == 2) {
		if (AttackSprite(tar_o, my_scene, jiasu))
			return;
	}


	int size = monsters.size();
	for (auto it = monsters.begin(); it != monsters.end(); it++) {
		if ((*it)->getHealth() <= 0) continue;
		if (AttackSprite((*it), my_scene, jiasu))
			break;
	}
}


template<class T>
bool  Shit::AttackSprite(T* sp, BaseLevelScene* my_scene, float jiasu)
{
	Vec2 m_pos = sp->getPosition();
	//�������
	double distance = sqrt((m_pos.x - pos.x) * (m_pos.x - pos.x) + (m_pos.y - pos.y) * (m_pos.y - pos.y));
	if (distance < range / 2) {//���ڷ�Χ�ڣ��򷢶�����

		auto callback2 = CallFunc::create([this, my_scene, sp, jiasu]() {
			/*Director::getInstance()->end();
			return;*/
			ShellProduct(my_scene);//�ӵ�������

			auto callback3 = CallFunc::create([sp, this, jiasu]() {//�ӵ�׷��
				auto m_pos = sp->getPosition();
				auto s_pos = curr_shell->getPosition();

				auto move = m_pos - s_pos;
				move.normalize();
				move = move * 10;
				//����ӵ�λ�ú͹���λ�ã������λ���ƶ�10�����ص�
				auto action3 = MoveBy::create(10 / (speed * jiasu), move);
				curr_shell->runAction(action3);
				});
			auto callback4 = CallFunc::create([my_scene, sp, this]() {
				if ((sp->getPosition() - curr_shell->getPosition()).distance(Vec2(0, 0)) < 10.f) {
					curr_shell->stopAllActions();//һ�������㹻С����ֹͣ����
					ShellDemage(my_scene, sp);
				}
				});
			de_time = 10 / (speed * jiasu);//�����ظ�������ƶ�����ʵ��׷��Ч��
			auto sequence3 = Sequence::create(callback3, DelayTime::create(de_time), callback4, nullptr);
			auto repeat = RepeatForever::create(sequence3);
			curr_shell->runAction(repeat);
			});

		callback2->execute();//�ú�ƿ��һ���ķ�ʽ�ǻ������
		return true;
	}
	return false;//����ڷ�Χ�⣬˵��û�򣬷���0
}

template<class T>
void Shit::ShellDemage(BaseLevelScene* my_scene, T* sp)
{
	curr_shell->removeFromParent();//���ӵ���������������ü�����һ���Զ��ͷ�
	DemageSprite(sp, demage);

	if (sp->getHealth() <= 0) {
		(sp)->toDie(my_scene);
	}
}

template<>
void Shit::ShellDemage(BaseLevelScene* my_scene, Monster* sp)
{
	curr_shell->removeFromParent();//���ӵ���������������ü�����һ���Զ��ͷ�
	auto c = cocos2d::Sprite::create();
	if (!c) {
		CCLOG("Failed to create c sprite.");
		return;
	}
	sp->addChild(c);
	c->setPosition(40, 0);  
	c->setScale(1.4f);
	cocos2d::Vector<cocos2d::SpriteFrame*> frames;
	for (int i = 5; i <= 6; ++i) {
		std::string frameName = "Towers/texiao_" + std::to_string(i) + ".png";
		auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 55, 18));
		if (frame) {
			frames.pushBack(frame);
		}
		else {
			CCLOG("Failed to load frame: %s", frameName.c_str());
		}
	}
	if (frames.empty()) {
		CCLOG("No frames found for death animation, skipping.");
		return;
	}
	auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 2.0f);
	auto animate = cocos2d::Animate::create(animation);
	auto onDeathComplete = cocos2d::CallFunc::create([c]() {
		c->removeFromParent();
		});
	c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
	DemageSprite(sp, demage);
	if (sp->getHealth() > 0)
		sp->speedaction->setSpeed(0.5f);
	auto delayaction = Sequence::create(
		DelayTime::create(4.0f),
		CallFunc::create([=] {sp->speedaction->setSpeed(1.0f);  }),
		nullptr);
	my_scene->runAction(delayaction);
	if (sp->getHealth() <= 0) {
		(sp)->toDie(my_scene);
	}
}

void Shit::ShellProduct(Scene* my_scene)//�����ڵ�
{
	auto shell = Sprite::create(shit_shell[grade]);
	Vec2 start_pos;
	start_pos.x = pos.x;
	start_pos.y = pos.y;
	shell->setPosition(start_pos);
	shell->setScale(1.0);
	my_scene->addChild(shell);
	curr_shell = shell;
	Music::getInstance()->ShitSound();
}
