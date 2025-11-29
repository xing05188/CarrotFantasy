#include"Obstacle.h"
#include"BaseLevelScene.h"//��ΪBaseLevelScene���õ���Obstacle,Ϊ��ֹѭ��Ƕ�ף�����include�����������.h��
#include"music.h"
#define CELL_SIZE 64

// 障碍物数据：{图片路径, 尺寸, 奖励金币, 生命值}
ObData Obstacle::Ob[10] = { 
    {"Obstacles/obstacle1.png", 1, 50, 300},
    {"Obstacles/obstacle2.png", 1, 50, 300},
    {"Obstacles/obstacle3.png", 1, 100, 400},
    {"Obstacles/obstacle4.png", 1, 100, 400},
    {"Obstacles/obstacle5.png", 1, 100, 400},
    {"Obstacles/obstacle6.png", 2, 150, 500},
    {"Obstacles/obstacle7.png", 4, 200, 600},
    {"Obstacles/obstacle8.png", 4, 200, 600},
    {"Obstacles/obstacle9.png", 4, 200, 600},
    {"Obstacles/obstacle10.png", 4, 200, 600}
};

void Obstacle::Produce(BaseLevelScene* my_scene, int x_, int y_) {
	curr = Sprite::create(Ob[index].road_path); // 创建图片
	x = x_; y = y_;
	float scale;
	Vec2 pos;
	if (size == 4) { // 根据size不同，设置位置和大小
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
	my_scene->addChild(curr); // 添加到场景
	
	// 创建护盾精灵（如果有护盾）
	if (maxShield > 0) {
		shieldSprite = Sprite::create("Obstacles/dun.png"); // 护盾图片
		shieldSprite->setPosition(Vec2(curr->getContentSize().width / 2 , curr->getContentSize().height / 2 ));
		shieldSprite->setScale(scale);
		curr->addChild(shieldSprite, 8); // 护盾在障碍物上方

		_ShieldBar = cocos2d::ui::LoadingBar::create("Obstacles/HPbar.png"); // 护盾条图片
		_ShieldBar->setPercent(100); // 初始护盾为满值
		_ShieldBar->setPosition(Vec2(curr->getContentSize().width / 2, curr->getContentSize().height * 1.1f));
		curr->addChild(_ShieldBar, 10);
		
		// 添加护盾条背景
		shieldback = Sprite::create("Monsters/HPBackground.png"); // 护盾条背景图片
		shieldback->setPosition(Vec2(curr->getContentSize().width / 2, curr->getContentSize().height * 1.1f));
		curr->addChild(shieldback, 9);
		
		setShieldVisible(false); // 初始护盾血条不可见
	}
	
	_HP = cocos2d::ui::LoadingBar::create("Monsters/HPbar.png");
	_HP->setPercent(100); // 初始血量为满血
	_HP->setPosition(Vec2(curr->getContentSize().width / 2, curr->getContentSize().height * 1.1f));
	curr->addChild(_HP, 10);
	
	// 添加血条背景
	hpback = Sprite::create("Monsters/HPBackground.png");
	hpback->setPosition(Vec2(curr->getContentSize().width / 2, curr->getContentSize().height * 1.1f));
	curr->addChild(hpback, 9);
	setHpVisible(false); // 初始血条不可见
}


void Obstacle::toDie(BaseLevelScene* my_scene)
{
	if (isDead) return;
	isDead = true;
	my_scene->updateMoney(reward);
	curr->retain();
	my_scene->removeChild(curr); // 从场景中删除障碍物，并从map中删除
	if (this == my_scene->tar_o) {
		my_scene->isTarget = 0;
		my_scene->tar_o = nullptr;
		my_scene->lock->removeFromParent();
	}
	my_scene->map_data[x][y].flag = 0;
	if (size == 2) my_scene->map_data[x + 1][y].flag = 0;
	else if (size == 4) my_scene->map_data[x + 1][y].flag = my_scene->map_data[x][y + 1].flag = my_scene->map_data[x + 1][y + 1].flag = 0;

	// 障碍物爆炸的动画
	// 创建一个新的临时精灵用于播放死亡动画
	auto deathSprite = cocos2d::Sprite::create();

	my_scene->addChild(deathSprite);
	deathSprite->setPosition(getPosition()); // 位置与障碍物相同
	deathSprite->setScale(1.5f);

	cocos2d::Vector<cocos2d::SpriteFrame*> frames;
	for (int i = 1; i <= 4; ++i) {
		std::string frameName = "Obstacles/obdead" + std::to_string(i) + ".png";
		auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 163, 163)); // 设置每张图片的大小（假设为 64x64）

		if (frame) {
			frames.pushBack(frame);
		}
		else {
			CCLOG("Failed to load frame: %s", frameName.c_str());
		}
	}

	// 如果没有加载到任何帧，直接返回
	if (frames.empty()) {
		CCLOG("No frames found for death animation, skipping.");
		return;
	}

	// 创建动画，每帧持续 0.2 秒
	auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.1f);
	auto animate = cocos2d::Animate::create(animation);

	// 动画完成后删除临时精灵
	auto onDeathComplete = cocos2d::CallFunc::create([deathSprite]() {
		CCLOG("Death animation completed, removing death obstacle.");
		deathSprite->removeFromParent();
		});

	// 运行动画序列，动画结束后删除临时精灵
	deathSprite->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
	Music::getInstance()->Object();
}

void Obstacle::updateHealthBar(){
	if (health <= 0) {
		_HP->setPercent(0);
	}
	else {
		float percentage = (float)health / maxHp * 100;
		_HP->setPercent(percentage);
	}
	// 更新血条百分比
}

void Obstacle::updateShieldBar() {
	if (shield <= 0) {
		_ShieldBar->setPercent(0);
	}
	else {
		float percentage = (float)shield / maxShield * 100;
		_ShieldBar->setPercent(percentage);
	}
	// 更新护盾条百分比
}

void Obstacle::getHurt(int value) {
	// 如果有护盾，先扣除护盾值
	if (shield > 0) {
		if (shield >= value) {
			setShieldVisible(true);
			shield -= value;
			updateShieldBar();
			return; // 护盾足够吸收所有伤害
		} else {
			// 护盾不足，扣除剩余护盾，剩余伤害扣血
			int remainingDamage = value - shield;
			shield = 0;
			updateShieldBar();
			
			// 护盾被打破，隐藏护盾精灵和护盾条，显示血条
			shieldSprite->setVisible(false);
			setShieldVisible(false);
			
			// 显示血条
			setHpVisible(true);
			
			// 扣除剩余血量
			health -= remainingDamage;
			updateHealthBar();
		}
	} else {
		// 没有护盾，直接扣血
		setHpVisible(true);
		health -= value;
		updateHealthBar();
	}
}