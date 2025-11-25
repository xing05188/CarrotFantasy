#include"Obstacle.h"
#include"BaseLevelScene.h"//��ΪBaseLevelScene���õ���Obstacle,Ϊ��ֹѭ��Ƕ�ף�����include�����������.h��
#include"music.h"
#define CELL_SIZE 64

ObData Obstacle::Ob[10] = { {"Obstacles/obstacle1.png",1,50,300},{"Obstacles/obstacle2.png",1,50,300} ,{"Obstacles/obstacle3.png",1,100,400} ,
{"Obstacles/obstacle4.png",1,100,400} ,{"Obstacles/obstacle5.png",1,100,400} ,{"Obstacles/obstacle6.png",2,150,500} ,
{"Obstacles/obstacle7.png",4,200,600} ,{"Obstacles/obstacle8.png",4,200,600} ,{"Obstacles/obstacle9.png",4,200,600} ,
{"Obstacles/obstacle10.png",4,200,600} };

void Obstacle::Produce(Scene* my_scene, int x_, int y_) {
	curr = Sprite::create(Ob[index].road_path);//����ͼƬ
	x = x_; y = y_;
	float scale;
	Vec2 pos;
	if (size == 4) {//����size��ͬ����λ�����С
		pos = Vec2((x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
		scale = CELL_SIZE * 2 / curr->getContentSize().width;
	}
	else if (size == 2) {
		pos = Vec2((x + 1) * CELL_SIZE, (y + 0.5) * CELL_SIZE);
		scale = CELL_SIZE * 2 / curr->getContentSize().width;
	}
	else {
		pos = Vec2((x + 0.5) * CELL_SIZE, (y + 0.5) * CELL_SIZE);
		scale = CELL_SIZE / curr->getContentSize().width;
	}
	curr->setPosition(pos);
	curr->setScale(scale);
	my_scene->addChild(curr);//����
	_HP = cocos2d::ui::LoadingBar::create("Monsters/HPbar.png");
	_HP->setPercent(100); // ��ʼѪ��Ϊ��Ѫ
	_HP->setPosition(Vec2(curr->getContentSize().width / 2, curr->getContentSize().height * 1.1f));
	curr->addChild(_HP, 10);
	//����Ѫ������
	hpback= Sprite::create("Monsters/HPBackground.png");
	hpback->setPosition(Vec2(curr->getContentSize().width / 2, curr->getContentSize().height * 1.1f));
	curr->addChild(hpback, 9);
	_HP->setVisible(false);
	hpback->setVisible(false);
}


void Obstacle::toDie(BaseLevelScene*my_scene)
{
	if (isDead) return;
	isDead = true;
	my_scene->updateMoney(reward);
	curr->retain();
	my_scene->removeChild(curr);//������ϰ�������ȫ���Դ�map��ɾ����
	if (this == my_scene->tar_o) {
		my_scene->isTarget = 0;
		my_scene->tar_o = nullptr;
		my_scene->lock->removeFromParent();
	}
	my_scene->map_data[x][y].flag = 0;
	if (size == 2) my_scene->map_data[x + 1][y].flag = 0;
	else if (size == 4) my_scene->map_data[x + 1][y].flag = my_scene->map_data[x][y + 1].flag = my_scene->map_data[x + 1][y + 1].flag = 0;

	//�ϰ��ﱬը�Ķ���
	 //  ����һ���µ���ʱ�������ڲ�����������
	auto deathSprite = cocos2d::Sprite::create();

	my_scene->addChild(deathSprite);
	deathSprite->setPosition(getPosition());  //λ�����ϰ�����ͬ
	deathSprite->setScale(1.5f);

	cocos2d::Vector<cocos2d::SpriteFrame*> frames;
	for (int i = 1; i <= 4; ++i) {
		std::string frameName = "Obstacles/obdead" + std::to_string(i) + ".png";
		auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 163, 163)); // ����ÿ��ͼƬ�Ĵ�С�� 64x64

		if (frame) {
			frames.pushBack(frame);
		}
		else {
			CCLOG("Failed to load frame: %s", frameName.c_str());
		}
	}

	//  ���û�м��ص�����֡��ֱ�ӷ���
	if (frames.empty()) {
		CCLOG("No frames found for death animation, skipping.");
		return;
	}

	// 6. ����������ÿ֡���� 0.2 ��
	auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.1f);
	auto animate = cocos2d::Animate::create(animation);

	// 7. ������ɺ�ɾ����ʱ��������������
	auto onDeathComplete = cocos2d::CallFunc::create([deathSprite]() {
		CCLOG("Death animation completed, removing death obstacle.");
		deathSprite->removeFromParent();
		});

	// 8. ������������������������������ʱ����
	deathSprite->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
	Music::getInstance()->Object();
}

void Obstacle::updateHealthBar(){
	float percentage = (static_cast<float>(health) / maxHp) * 100;
	if (percentage < 0) percentage = 0;
	if (percentage > 100) percentage = 100;
	_HP->setPercent(percentage); // ����Ѫ���ٷֱ�
}