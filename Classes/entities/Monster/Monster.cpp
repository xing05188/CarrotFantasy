#include"Monster.h"
#include"MonsterFactory.h"
#include"SkillDecorator.h"

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

// 初始化怪物
bool Monster::initWithPath(const std::string& monsterName, const std::vector<Vec2>& path, int startIndex,bool pause) {
    // 加载怪物帧动画资源
    std::string plistPath = "Monsters/" + monsterName + ".plist";
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath);
    // 检查精灵帧是否存在
    std::string frameName = monsterName + "_0.png";
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName)) {
        CCLOG("Failed to load SpriteFrame '%s'.", frameName.c_str());
        return false;
    }
    // 初始化精灵
    if (!Sprite::initWithSpriteFrameName(frameName)) {
        CCLOG("Failed to initialize sprite with frame '%s'.", frameName.c_str());
        return false;
    }
    // 加载怪物配置
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
    // 加载怪物行走动画
    auto walkFrames = cocos2d::Vector<SpriteFrame*>();
    walkFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(monsterName + "_0.png"));
    walkFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(monsterName + "_1.png"));
    auto walkAnimation = Animation::createWithSpriteFrames(walkFrames, 0.5f);  // 帧动画播放速度可以调整
    auto walkAnimate = Animate::create(walkAnimation);
    auto repeatWalk = RepeatForever::create(walkAnimate);
    this->runAction(repeatWalk); // 运行怪物行走动画
    // 设置初始位置
    if (!path.empty()) {
        moveAlongPath(path); // 调用怪物路径移动逻辑
    }

    return true;
}
//怪物的移动逻辑
void Monster::moveAlongPath(const std::vector<Vec2>& path) {
    if (path.empty()) {
        CCLOG("Path is empty, cannot move the monster.");
        return;
    }
    // 确定怪物路径的起始点
    this->setPosition(path[startPosIndex]);
    // 记录终点元素
    endPos = path.back();
    // 创建一个数组存储所有的动作
    cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;
    // 遍历路径点，创建移动动作
    float distance;
    float moveTime;
    for (size_t i = startPosIndex+1; i < path.size(); ++i) {
        // 计算两点之间的距离并确定移动时间
        distance = path[i - 1].distance(path[i]);
        moveTime = distance / speed;  // 根据速度计算移动时间

        // 创建移动动作
        auto moveTo = cocos2d::MoveTo::create(moveTime, path[i]);
        //为每个MoveTo动作添加一个回调函数来更新pathIndex
        auto updatePathIndex = cocos2d::CallFunc::create([this, i]() {
            this->PathIndex = i; // 更新怪物的 pathIndex 成员变量
            CCLOG("Monster %p pathIndex updated to %d", this, i);
            });
        // 将每个移动动作添加到动作数组中
        actions.pushBack(moveTo);
        actions.pushBack(updatePathIndex); // 在移动动作后，更新 pathIndex
    }
    // 当路径完成时的回调
    auto onPathComplete = cocos2d::CallFunc::create([this]() {
        CCLOG("Monster %p has completed its path.", this);

        // 创建事件
        cocos2d::EventCustom event("monster_path_complete");
        // 将怪物对象指针传递给事件
        event.setUserData(this); 

        // 获取事件分发器并分发事件
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->dispatchEvent(&event);
        });
    actions.pushBack(onPathComplete); // 添加完成回调
    auto sequence = cocos2d::Sequence::create(actions);
    if (pause) {
        auto delay = DelayTime::create(4.0f); // 创建一个延迟4秒的动作
        auto delayedSequence = Sequence::create(delay, sequence, nullptr);  // 将延迟动作与序列动作连接起来
        speedaction = Speed::create(delayedSequence, beishu); // 执行新的序列动作：延迟3秒，再执行原来的sequence
    }
    else
    {
        speedaction = Speed::create(sequence, beishu);
    }
    this->runAction(speedaction);
}
//怪物死亡
Monster::~Monster()
{
    // 释放 Boss 技能装饰器
    CC_SAFE_DELETE(bossSkill);
}

void Monster::toDie(BaseLevelScene*my_scene)
{
    if(isDead)
     return;
    DeadCount++;
    CCLOG("Monster %p is dying.", this);
    my_scene->updateMoney(reward);
    // 1. 停止怪物当前所有动作
    this->isDead = true;
    // 2. 创建一个新的临时精灵用于播放死亡动画
    auto deathSprite = cocos2d::Sprite::create();
    if (!deathSprite) {
        CCLOG("Failed to create death sprite.");
        return;
    }
    // 3. 将死亡精灵添加到与 Monster 相同的父节点中
    this->getParent()->addChild(deathSprite);
    deathSprite->setPosition(this->getPosition());  // 设置死亡精灵位置与怪物相同
    deathSprite->setScale(1.5f);
    // 4. 加载死亡动画的4张图片
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    for (int i = 1; i <= 4; ++i) {
        std::string frameName = "Monsters/dead_" + std::to_string(i) + ".png";
        auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 64, 64)); // 假设每张图片的大小是 64x64
        if (frame) {
            frames.pushBack(frame);
        }
        else {
            CCLOG("Failed to load frame: %s", frameName.c_str());
        }
    }
    // 5. 如果没有加载到任何帧，直接返回
    if (frames.empty()) {
        CCLOG("No frames found for death animation, skipping.");
        return;
    }
    // 6. 创建动画，每帧持续 0.2 秒
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = cocos2d::Animate::create(animation);
    // 7. 动画完成后删除临时精灵和死亡精灵
    auto onDeathComplete = cocos2d::CallFunc::create([deathSprite]() {
        CCLOG("Death animation completed, removing death sprite.");
        deathSprite->removeFromParent();
        });
    // 8. 运行死亡动画序列，完成后删除临时精灵
    deathSprite->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
    Music::getInstance()->normalSound();
    // 9. 删除怪物本身
    this->retain();
    my_scene->removeChild(this);
    if (my_scene->tar_m == this) {//如果当前怪物是目标怪物，那么清空目标怪物
        my_scene->isTarget = 0;
        my_scene->tar_m = nullptr;
    }
}
//设置血条可见
void Monster::setHpVisible(bool isVisible) 
{ 
    _HP->setVisible(isVisible); 
     hpback->setVisible(isVisible); 
     ishpvs = true; 
}
//更新血条
void  Monster::updateHealthBar() {
    float percentage = (static_cast<float>(health) / maxHp) * 100;
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;
    _HP->setPercent(percentage); // 更新血条百分比
}
//受到伤害
void  Monster::getHurt(int value)
{
    health-=value;
	setHpVisible(true);
	updateHealthBar();
}
// 默认的怪物特殊攻击：由装饰器执行
void Monster::SpecialAttack()
{
    if (bossSkill)
    {
        bossSkill->use(this);
    }
}

// BossYellow / BossSheep 仅作为 Monster 的具体类型，由装饰器完成真正逻辑
void BossYellow::SpecialAttack()
{
    Monster::SpecialAttack();
}

void BossSheep::SpecialAttack()
{
    Monster::SpecialAttack();
}