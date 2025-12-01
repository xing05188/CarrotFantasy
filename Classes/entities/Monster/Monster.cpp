#include"Monster.h"
#include"MonsterFactory.h"

#include"music.h"
#include"BaseLevelScene.h"
#include"GameManager.h"
extern GameManager* manager;
extern float tower_jiasu;
int DeadCount=0;


//静态创建怪物对象
Monster* Monster::create(const std::string& monsterName, const std::vector<Vec2>& path, int startIndex, bool pause) {
    // 使用工厂模式创建怪物
    auto factory = MonsterFactoryProvider::getFactory(monsterName);
    if (!factory) {
        CCLOG("Failed to get factory for monster: %s", monsterName.c_str());
        return nullptr;
    }
    
    Monster* monster = factory->createMonster(monsterName, path, startIndex, pause);
    delete factory; // 释放工厂对象
    
    if (!monster) {
        CCLOG("Failed to create monster: %s", monsterName.c_str());
        return nullptr;
    }
    
    return monster;
}

bool Monster::initializeMonsterWithHealthBar(Monster* monster, 
                                                       const std::string& monsterName, 
                                                       const std::vector<Vec2>& path, 
                                                       int startIndex, 
                                                       bool pause) {
    // 初始化怪物
    if (monster->initWithPath(monsterName, path, startIndex, pause)) {
        monster->autorelease();
        
        // 创建血条
        monster->_HP = cocos2d::ui::LoadingBar::create("Monsters/HPbar.png");
        monster->_HP->setPercent(100); // 初始血量为满血
        monster->_HP->setPosition(Vec2(monster->getContentSize().width / 2, monster->getContentSize().height * 1.3f));
        monster->addChild(monster->_HP, 10);
        
        // 创建血条背景
        monster->hpback = Sprite::create("Monsters/HPBackground.png");
        monster->hpback->setPosition(Vec2(monster->getContentSize().width / 2, monster->getContentSize().height * 1.3f));
        monster->addChild(monster->hpback, 9);
        
        monster->_HP->setVisible(false);
        monster->hpback->setVisible(false);
        
        return true;
    }
    
    return false;
}

// ��ʼ������
bool Monster::initWithPath(const std::string& monsterName, const std::vector<Vec2>& path, int startIndex,bool pause) {
    // ���ؾ���֡��Դ
    std::string plistPath = "Monsters/" + monsterName + ".plist";
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath);
    // ��龫��֡�Ƿ����
    std::string frameName = monsterName + "_0.png";
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName)) {
        CCLOG("Failed to load SpriteFrame '%s'.", frameName.c_str());
        return false;
    }
    // ��ʼ������
    if (!Sprite::initWithSpriteFrameName(frameName)) {
        CCLOG("Failed to initialize sprite with frame '%s'.", frameName.c_str());
        return false;
    }
    // ���ع�������
    MonsterConfig config = MonsterConfigManager::getMonsterConfigByName(monsterName);
    this->startPosIndex=startIndex;
    this->maxHp=this->health=config.health;
    this->speed=config.speed;
    this->damage=config.damage;
    this->reward = config.reward;
    this->name= monsterName;
    this->pause=pause;
    CCLOG("Monster Config - Sprite Frame: %s, Health: %d, Speed: %.2f, Damage: %d, Reward: %d",
        config.spriteFrameName.c_str(), config.health, config.speed, config.damage, config.reward);
    // ���ع������߶���
    auto walkFrames = cocos2d::Vector<SpriteFrame*>();
    walkFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(monsterName + "_0.png"));
    walkFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(monsterName + "_1.png"));
    auto walkAnimation = Animation::createWithSpriteFrames(walkFrames, 0.5f);  // ��֡�������ٶȿ��Ե���
    auto walkAnimate = Animate::create(walkAnimation);
    auto repeatWalk = RepeatForever::create(walkAnimate);
    this->runAction(repeatWalk); // �����������߶���
    // ���ó�ʼλ��
    if (!path.empty()) {
        moveAlongPath(path); // ��������·���ƶ��߼�
    }

    return true;
}
//���޵��ƶ��߼�
void Monster::moveAlongPath(const std::vector<Vec2>& path) {
    if (path.empty()) {
        CCLOG("Path is empty, cannot move the monster.");
        return;
    }
    // ȷ��������·������ʼ��
    this->setPosition(path[startPosIndex]);
    // ��¼�յ�Ԫ��
    endPos = path.back();
    // ����һ������洢���еĶ���
    cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;
    // ����·���㣬�����ƶ�����
    float distance;
    float moveTime;
    for (size_t i = startPosIndex+1; i < path.size(); ++i) {
        // ��������֮��ľ��벢ȷ���ƶ�ʱ��
        distance = path[i - 1].distance(path[i]);
        moveTime = distance / speed;  // �����ٶȼ����ƶ�ʱ��

        // �����ƶ�����
        auto moveTo = cocos2d::MoveTo::create(moveTime, path[i]);
        //��ÿ��MoveTo�����һ���ص�������pathIndex
        auto updatePathIndex = cocos2d::CallFunc::create([this, i]() {
            this->PathIndex = i; // ���¹���� pathIndex ��Ա����
            CCLOG("Monster %p pathIndex updated to %d", this, i);
            });
        // ��ÿ���ƶ��������ӵ�����������
        actions.pushBack(moveTo);
        actions.pushBack(updatePathIndex); // ���ƶ�����󣬸��� pathIndex
    }
    // ��·�����ʱ�����¼�
    auto onPathComplete = cocos2d::CallFunc::create([this]() {
        CCLOG("Monster %p has completed its path.", this);

        // �����¼�
        cocos2d::EventCustom event("monster_path_complete");
        // �����޶���ָ�봫�ݸ��¼�
        event.setUserData(this); 

        // ��ȡ�¼��ַ������ַ��¼�
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->dispatchEvent(&event);
        });
    actions.pushBack(onPathComplete); // ������������
    auto sequence = cocos2d::Sequence::create(actions);
    if (pause) {
        auto delay = DelayTime::create(4.0f); // ����һ���ӳ�3��Ķ���
        auto delayedSequence = Sequence::create(delay, sequence, nullptr);  // ���ӳٶ��������еĶ������н������
        speedaction = Speed::create(delayedSequence, beishu); // ִ���µ����ж��������ӳ�3�룬��ִ��ԭ����sequence��
    }
    else
    {
        speedaction = Speed::create(sequence, beishu);
    }
    this->runAction(speedaction);
}
//�ù�������
void Monster::toDie(BaseLevelScene*my_scene)
{
    if(isDead)
     return;
    DeadCount++;
    CCLOG("Monster %p is dying.", this);
    my_scene->updateMoney(reward);
    // 1. ������������ع���
    this->isDead = true;
    // 2. ����һ���µ���ʱ�������ڲ�����������
    auto deathSprite = cocos2d::Sprite::create();
    if (!deathSprite) {
        CCLOG("Failed to create death sprite.");
        return;
    }
    // 3. �����������������ӵ��� Monster ��ͬ�ĸ��ڵ���
    this->getParent()->addChild(deathSprite);
    deathSprite->setPosition(this->getPosition());  // ����������������λ���������ͬ
    deathSprite->setScale(1.5f);
    // 4. �������������� 4 ��ͼƬ
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    for (int i = 1; i <= 4; ++i) {
        std::string frameName = "Monsters/dead_" + std::to_string(i) + ".png";
        auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 64, 64)); // ����ÿ��ͼƬ�Ĵ�С�� 64x64
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
    auto onDeathComplete = cocos2d::CallFunc::create([deathSprite]() {
        CCLOG("Death animation completed, removing death sprite.");
        deathSprite->removeFromParent();
        });
    // 8. ������������������������������ʱ����
    deathSprite->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
    Music::getInstance()->normalSound();
    // 9. ɾ�����ﱾ��
    this->retain();
    my_scene->removeChild(this);
    if (my_scene->tar_m == this) {//���������ֱ���������ô�����������������
        my_scene->isTarget = 0;
        my_scene->tar_m = nullptr;
    }
}
//����Ѫ���ɼ�
void Monster::setHpVisible(bool isVisible) 
{ 
    _HP->setVisible(isVisible); 
     hpback->setVisible(isVisible); 
     ishpvs = true; 
}
//����Ѫ��
void  Monster::updateHealthBar() {
    float percentage = (static_cast<float>(health) / maxHp) * 100;
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;
    _HP->setPercent(percentage); // ����Ѫ���ٷֱ�
}
//��������
void  Monster::getHurt(int value)
{
    health-=value;
	setHpVisible(true);
	updateHealthBar();
}
void showBossSkill(const std::string& skillText);
//BossYellow�ļ���:��������
void BossYellow::SpecialAttack() {
    //boss���ܣ������Ĺ��ټ���
    CCLOG("BossYellow's Attack!");
    showBossSkill("SpecialAttack:Slow Speed!");
            Music::getInstance()->duanSound();
            tower_jiasu = 0.3;
            auto delayaction = Sequence::create(DelayTime::create(20.0f), CallFunc::create([=] {
                tower_jiasu = 1;
                }), nullptr);
            this->runAction(delayaction);
    }

#define CELL_SIZE 64
void BossSheep::SpecialAttack()
{
    //boss���ܣ������һ������
    CCLOG("BossSheep's Attack!");
    showBossSkill("SpecialAttack:Destroy!");
            Music::getInstance()->duanSound();
            auto bong = Sprite::create();
            if (!bong) {
                CCLOG("Failed to create bong sprite.");
                return;
            }
            bong->setPosition(480, 320);
            bong->setScale(2);
            this->addChild(bong);
            cocos2d::Vector<cocos2d::SpriteFrame*> frames;
            for (int i = 0; i <= 3; ++i) {
                std::string frameName = "Carrot/bong/bong_" + std::to_string(i) + ".png";
                auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 164, 160)); // ����ÿ��ͼƬ�Ĵ�С�� 64x64
                if (frame) {
                    frames.pushBack(frame);
                }
                else {
                    CCLOG("Failed to load frame: %s", frameName.c_str());
                }
            }
            if (frames.empty()) {
                CCLOG("No frames found for bong, skipping.");
                return;
            }
            auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
            auto animate = cocos2d::Animate::create(animation);
            auto onbong = cocos2d::CallFunc::create([bong]() {
                CCLOG("bong.");
                bong->removeFromParent();
                });
            Music::getInstance()->bongSound();
            bong->runAction(cocos2d::Sequence::create(animate, onbong, nullptr));
            int num = 0;
            for (auto it = manager->getScene()->towers.begin(); it != manager->getScene()->towers.end();) {
                if (num % 2 == 0) {
                    manager->getScene()->removeChild(it->second->sprite_mark);
                    manager->getScene()->removeChild(it->second->sp_base);
                    it->second->sprite_mark->release();
                    it->second->sp_base->release();
                    auto tool = ++it;
                    it--;
                    manager->getScene()->map_data[int(it->second->pos.x / CELL_SIZE)][int(it->second->pos.y / CELL_SIZE)].flag=0;
                    manager->getScene()->towers.erase(it);
                    it = tool;
                }
                else
                    it++;
                num++;
            }
}

//������ʾBoss����
void showBossSkill(const std::string& skillText) {
    // ����һ�� Label ����ʾ��������
    auto skillLabel = Label::createWithTTF(skillText, "fonts/Marker Felt.ttf", 48); // ʹ�� TTF ����
    if (!skillLabel) return;

    // ���� Label λ��Ϊ��Ļ����
    skillLabel->setPosition(Director::getInstance()->getVisibleSize() / 2);
    skillLabel->setTextColor(Color4B::RED); // ����������ɫΪ��ɫ

    // ���� Label ����ǰ��
    manager->getScene()->addChild(skillLabel, 100); // ���ýϸߵ� z-order ȷ����ʾ����ǰ��

    // ���붯��
    auto fadeIn = FadeIn::create(0.5f);

    // ͣ��ʱ��
    auto delay = DelayTime::create(2.0f);

    // ��������
    auto fadeOut = FadeOut::create(0.5f);

    // ������ɺ��Ƴ� Label
    auto remove = CallFunc::create([=]() {
        manager->getScene()->removeChild(skillLabel);
        });

    // ��϶���
    auto sequence = Sequence::create(fadeIn, delay, fadeOut, remove, nullptr);

    // ���ж���
    skillLabel->runAction(sequence);
}