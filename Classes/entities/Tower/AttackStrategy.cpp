#include "AttackStrategy.h"
#include "Tower.h"
#include "BaseLevelScene.h"

// 静态常量定义 - 定义各种攻击策略的速度
const float BottleAttackStrategy::speed = 800; // 瓶子塔炮弹飞行速度
const float ShitAttackStrategy::speed = 500;   // 粪便塔炮弹飞行速度

// Bottle攻击策略实现
void BottleAttackStrategy::attack(Tower *tower, BaseLevelScene *scene,
                                  std::vector<Monster *> &monsters, char isTarget,
                                  Monster *tar_m, Obstacle *tar_o, float jiasu)
{
    // 类型转换，确保传入的是Bottle类型的塔
    Bottle *bottle = dynamic_cast<Bottle *>(tower);
    if (!bottle)
        return;

    // 重置攻击间隔
    bottle->interval = 0;

    // 优先攻击指定目标
    if (isTarget == 1)
    { // 指定怪物目标
        if (AttackSprite(bottle, tar_m, scene, jiasu))
            return;
    }
    if (isTarget == 2)
    { // 指定障碍物目标
        if (AttackSprite(bottle, tar_o, scene, jiasu))
            return;
    }

    // 遍历所有怪物，寻找攻击范围内的目标
    for (auto it = monsters.begin(); it != monsters.end(); it++)
    {
        if ((*it)->getHealth() <= 0)
            continue; // 跳过已死亡的怪物
        if (AttackSprite(bottle, (*it), scene, jiasu))
            break; // 找到目标并攻击后退出循环
    }
}

// 瓶子塔攻击精灵的模板函数 - 支持攻击怪物和障碍物
template <class T>
bool BottleAttackStrategy::AttackSprite(Bottle *bottle, T *sp, BaseLevelScene *my_scene, float jiasu)
{
    // 计算目标与塔之间的距离
    Vec2 m_pos = sp->getPosition();
    double distance = sqrt((m_pos.x - bottle->pos.x) * (m_pos.x - bottle->pos.x) +
                           (m_pos.y - bottle->pos.y) * (m_pos.y - bottle->pos.y));

    // 检查目标是否在攻击范围内
    if (distance < bottle->GetRange() / 2)
    {
        // 计算攻击方向和角度
        Vec2 direction = m_pos - bottle->pos;
        direction.normalize();
        auto ang = direction.getAngle();
        ang = m_pos.y < bottle->pos.y ? (ang < 0 ? fabs(ang / PI) * 180 : ang / PI * 180) : (ang > 0 ? -ang / PI * 180 : ang / PI * 180);

        // 计算旋转到目标角度所需的时间
        auto rotationDuration = static_cast<float>(fabs(ang / (360.0 * jiasu)));
        auto action1 = RotateTo::create(rotationDuration, static_cast<float>(ang));

        // 旋转完成后执行攻击的回调函数
        auto callback2 = CallFunc::create([bottle, my_scene, sp, m_pos, ang, jiasu, this]()
                                          {
            bottle->tower_angle = ang;  // 更新塔的当前角度
            ShellProduct(bottle, my_scene);  // 创建炮弹

            // 炮弹移动的回调函数
            auto callback3 = CallFunc::create([sp, bottle, jiasu]() {
                auto m_pos = sp->getPosition();
                auto s_pos = bottle->curr_shell->getPosition();
                auto move = m_pos - s_pos;
                move.normalize();
                move = move * 10;  // 设置移动速度
                auto action3 = MoveBy::create(10.0f / (speed * jiasu), move);
                bottle->curr_shell->runAction(action3);
            });
            
            // 炮弹命中检测的回调函数
            auto callback4 = CallFunc::create([my_scene, sp, bottle, this]() {
                // 检测炮弹与目标的距离，小于10像素视为命中
                if ((sp->getPosition() - bottle->curr_shell->getPosition()).distance(Vec2(0, 0)) < 10.f) {
                    bottle->curr_shell->stopAllActions();
                    ShellDemage(bottle, my_scene, sp);  // 处理伤害和特效
                }
            });
            
            // 设置炮弹飞行时间
            bottle->de_time = 10.0f / (speed * jiasu);
            // 创建炮弹移动和检测的序列动作
            auto sequence3 = Sequence::create(callback3, DelayTime::create(bottle->de_time), callback4, nullptr);
            auto repeat = RepeatForever::create(sequence3);
            bottle->curr_shell->runAction(repeat); });

        // 创建旋转和攻击的序列动作
        auto sequence2 = Sequence::create(action1, callback2, nullptr);
        bottle->sprite_mark->runAction(sequence2);
        return true; // 成功攻击目标
    }
    return false; // 目标不在攻击范围内
}

// 创建瓶子塔的炮弹
void BottleAttackStrategy::ShellProduct(Bottle *bottle, Scene *my_scene)
{
    // 根据塔的等级创建对应的炮弹
    auto shell = Sprite::create(bottle->bottle_shell[bottle->GetGrade()]);
    Vec2 start_pos;
    start_pos.x = bottle->pos.x;
    start_pos.y = bottle->pos.y;
    shell->setPosition(start_pos);
    shell->setScale(0.6); // 设置炮弹缩放比例
    my_scene->addChild(shell);
    bottle->curr_shell = shell;          // 保存当前炮弹引用
    Music::getInstance()->bottleSound(); // 播放攻击音效
}

// 瓶子塔炮弹造成伤害的模板函数 - 支持对怪物和障碍物造成伤害
template <class T>
void BottleAttackStrategy::ShellDemage(Bottle *bottle, BaseLevelScene *my_scene, T *sp)
{
    // 移除炮弹
    bottle->curr_shell->removeFromParent();

    // 创建爆炸特效精灵
    auto c = cocos2d::Sprite::create();
    if (!c)
    {
        CCLOG("Failed to create c sprite.");
        return;
    }
    my_scene->addChild(c);
    c->setPosition(sp->getPosition());
    c->setScale(1.4f);

    // 加载爆炸动画帧
    cocos2d::Vector<cocos2d::SpriteFrame *> frames;
    for (int i = 0; i <= 1; ++i)
    {
        std::string frameName = "Towers/texiao_" + std::to_string(i) + ".png";
        auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 44, 45));
        if (frame)
        {
            frames.pushBack(frame);
        }
    }

    // 检查是否成功加载动画帧
    if (frames.empty())
    {
        CCLOG("No frames found for death animation, skipping.");
        return;
    }

    // 创建并播放爆炸动画
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = cocos2d::Animate::create(animation);
    auto onDeathComplete = cocos2d::CallFunc::create([c]()
                                                     {
                                                         c->removeFromParent(); // 动画完成后移除特效精灵
                                                     });

    c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));

    // 对目标造成伤害
    DemageSprite(sp, bottle->GetDemage());
    // 检查目标是否死亡
    if (sp->getHealth() <= 0)
    {
        (sp)->toDie(my_scene);
    }
}

// Sun攻击策略实现
void SunAttackStrategy::attack(Tower *tower, BaseLevelScene *scene,
                               std::vector<Monster *> &monsters, char isTarget,
                               Monster *tar_m, Obstacle *tar_o, float jiasu)
{
    // 类型转换，确保传入的是Sun类型的塔
    Sun *sun = dynamic_cast<Sun *>(tower);
    if (!sun)
        return;

    // 重置攻击间隔
    sun->interval = 0;

    // 优先攻击指定障碍物目标
    if (isTarget == 2)
    {
        Vec2 o_pos = tar_o->getPosition();
        float distance = sqrt((o_pos.x - sun->pos.x) * (o_pos.x - sun->pos.x) +
                              (o_pos.y - sun->pos.y) * (o_pos.y - sun->pos.y));
        if (distance < sun->GetRange() / 2)
        {
            SunAttack(sun, scene, monsters, jiasu);
            return;
        }
    }

    // 遍历所有怪物，寻找攻击范围内的目标
    for (auto it = monsters.begin(); it != monsters.end(); it++)
    {
        if ((*it)->getHealth() <= 0)
            continue; // 跳过已死亡的怪物
        Vec2 m_pos = (*it)->getPosition();
        float distance = sqrt((m_pos.x - sun->pos.x) * (m_pos.x - sun->pos.x) +
                              (m_pos.y - sun->pos.y) * (m_pos.y - sun->pos.y));
        if (distance < sun->GetRange() / 2)
        {
            SunAttack(sun, scene, monsters, jiasu);
            return; // 找到目标并攻击后退出函数
        }
    }
}

// 太阳塔执行范围攻击
void SunAttackStrategy::SunAttack(Sun *sun, BaseLevelScene *my_scene, std::vector<Monster *> &monsters, float jiasu)
{
    // 播放攻击音效
    Music::getInstance()->sunSound();

    // 创建太阳光环效果
    sun->curr_halo = Sprite::create("Towers/sunhalo.png");
    sun->curr_halo->setPosition(sun->pos);
    // 根据攻击范围调整光环大小
    sun->curr_halo->setScale(0.8 * sun->GetRange() / sun->curr_halo->getContentSize().width);
    my_scene->addChild(sun->curr_halo);

    // 光环放大动画
    auto scale_up = ScaleBy::create(0.3 / jiasu, 1.25);

    // 光环放大完成后执行伤害的回调函数
    auto callback = CallFunc::create([sun, my_scene, &monsters]()
                                     {
        // 对范围内的障碍物造成伤害
        for (auto it = my_scene->Obstacles.begin(); it != my_scene->Obstacles.end(); it++) {
            Vec2 o_pos = (*it).second->getPosition();
            float distance = sqrt((o_pos.x - sun->pos.x) * (o_pos.x - sun->pos.x) + 
                                 (o_pos.y - sun->pos.y) * (o_pos.y - sun->pos.y));
            if (distance < sun->GetRange() / 2 && (*it).second->getHealth() > 0) {
                // 创建伤害特效
                auto c = cocos2d::Sprite::create();
                my_scene->addChild(c);
                c->setPosition(o_pos);
                c->setScale(1.6f);
                
                // 加载特效动画帧
                cocos2d::Vector<cocos2d::SpriteFrame*> frames;
                std::string frameName = "Towers/texiao_2.png";
                auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 26, 15));
                if (frame) {
                    frames.pushBack(frame);
                }
                
                // 创建并播放特效动画
                auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
                auto animate = cocos2d::Animate::create(animation);
                auto onDeathComplete = cocos2d::CallFunc::create([c]() {
                    c->removeFromParent();  // 动画完成后移除特效精灵
                });
                c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
                
                // 对障碍物造成伤害
                DemageSprite((*it).second, sun->GetDemage());
                if ((*it).second->getHealth() <= 0) (*it).second->toDie(my_scene);
            }
        }
        
        // 对范围内的怪物造成伤害
        for (auto it = monsters.begin(); it != monsters.end(); it++) {
            Vec2 m_pos = (*it)->getPosition();
            float distance = sqrt((m_pos.x - sun->pos.x) * (m_pos.x - sun->pos.x) + 
                                 (m_pos.y - sun->pos.y) * (m_pos.y - sun->pos.y));
            if (distance < sun->GetRange() / 2 && (*it)->getHealth() > 0) {
                // 创建伤害特效
                auto c = cocos2d::Sprite::create();
                my_scene->addChild(c);
                c->setPosition(m_pos);
                c->setScale(1.8f);
                
                // 加载特效动画帧
                cocos2d::Vector<cocos2d::SpriteFrame*> frames;
                std::string frameName = "Towers/texiao_2.png";
                auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 26, 15)); 
                if (frame) {
                    frames.pushBack(frame);
                }
                
                // 创建并播放特效动画
                auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
                auto animate = cocos2d::Animate::create(animation);
                auto onDeathComplete = cocos2d::CallFunc::create([c]() {
                    c->removeFromParent();  // 动画完成后移除特效精灵
                });
                c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
                
                // 对怪物造成伤害
                DemageSprite((*it), sun->GetDemage());
                if ((*it)->getHealth() <= 0) (*it)->toDie(my_scene);
            }
        }
        
        // 移除光环效果
        sun->curr_halo->removeFromParent(); });

    // 创建并运行动作序列
    auto seq = Sequence::create(scale_up, callback, nullptr);
    sun->curr_halo->runAction(seq);
}

// Plane攻击策略实现
void PlaneAttackStrategy::attack(Tower *tower, BaseLevelScene *scene,
                                 std::vector<Monster *> &monsters, char isTarget,
                                 Monster *tar_m, Obstacle *tar_o, float jiasu)
{
    // 类型转换，确保传入的是MyPlane类型的塔
    MyPlane *plane = dynamic_cast<MyPlane *>(tower);
    if (!plane)
        return;

    // 重置攻击间隔
    plane->interval = 0;

    // 优先攻击指定目标
    if (isTarget == 1)
    { // 指定怪物目标
        if (AttackSprite(plane, tar_m, scene, monsters, jiasu))
            return;
    }
    if (isTarget == 2)
    { // 指定障碍物目标
        if (AttackSprite(plane, tar_o, scene, monsters, jiasu))
            return;
    }

    // 遍历所有怪物，寻找攻击范围内的目标
    for (auto it = monsters.begin(); it != monsters.end(); it++)
    {
        if ((*it)->getHealth() <= 0)
            continue; // 跳过已死亡的怪物
        if (AttackSprite(plane, (*it), scene, monsters, jiasu))
            break; // 找到目标并攻击后退出循环
    }
}

// 飞机塔攻击精灵的模板函数 - 支持攻击怪物和障碍物
template <class T>
bool PlaneAttackStrategy::AttackSprite(MyPlane *plane, T *sp, BaseLevelScene *my_scene,
                                       std::vector<Monster *> &monsters, float jiasu)
{
    // 计算目标与塔之间的距离
    Vec2 m_pos = sp->getPosition();
    double distance = sqrt((m_pos.x - plane->pos.x) * (m_pos.x - plane->pos.x) +
                           (m_pos.y - plane->pos.y) * (m_pos.y - plane->pos.y));

    // 检查目标是否在攻击范围内
    if (distance > plane->GetRange() / 2)
        return false;

    // 开始旋转瞄准的回调函数
    auto callback1 = CallFunc::create([plane, m_pos, sp, my_scene, &monsters, jiasu, this]()
                                      {
        plane->de_ang = 0;  // 重置旋转角度
        Vec2 tool = m_pos - plane->pos;
        auto ang = CC_RADIANS_TO_DEGREES((tool).getAngle());
        auto m_ang = ang >= 0 ? ang : 360 + ang;
        
        // 确定旋转方向（顺时针或逆时针）
        int Clockwise = m_ang - plane->tower_angle > 0 ? 
                       (m_ang - plane->tower_angle > 180 ? 1 : -1) : 
                       (plane->tower_angle - m_ang > 180 ? -1 : 1);

        // 旋转回调函数
        auto TurnCallback = CallFunc::create([plane, sp, Clockwise, my_scene, &monsters, jiasu, this]() {
            // 更新飞机塔的旋转角度
            plane->sprite_mark->setRotation(-(plane->tower_angle - 90));
            Vec2 m_pos = sp->getPosition();
            Vec2 tool = m_pos - plane->pos;
            auto ang = CC_RADIANS_TO_DEGREES((tool).getAngle());
            auto m_ang = ang >= 0 ? ang : 360 + ang;
            
            // 检查是否已经瞄准目标
            if (fabs(plane->tower_angle - m_ang) < 5) {
                plane->sprite_mark->stopAllActions();
                
                // 创建激光束的回调函数
                auto callback2 = CallFunc::create([plane, my_scene]() {
                    // 根据塔的等级创建对应的激光束
                    plane->curr_ray = Sprite::create(plane->ray[plane->GetGrade()]);
                    plane->curr_ray->setAnchorPoint(Vec2(0.5, 0));
                    plane->curr_ray->setScale(CELL_SIZE / plane->curr_ray->getContentSize().width);
                    
                    // 计算激光束的位置和角度
                    Vec2 ray_pos;
                    ray_pos.x = plane->pos.x + 0.5 * plane->sprite_mark->getContentSize().height * 
                               cos(CC_DEGREES_TO_RADIANS(plane->tower_angle));
                    ray_pos.y = plane->pos.y + 0.5 * plane->sprite_mark->getContentSize().height * 
                               sin(CC_DEGREES_TO_RADIANS(plane->tower_angle));
                    plane->curr_ray->setPosition(ray_pos);
                    plane->curr_ray->setRotation(90 - plane->tower_angle);
                    my_scene->addChild(plane->curr_ray);
                });
                
                // 激光伤害的回调函数
                auto callback3 = CallFunc::create([plane, my_scene, &monsters, this]() {
                    PlaneDemage(plane, my_scene, monsters);  // 处理激光伤害
                    plane->curr_ray->removeFromParent();    // 移除激光束
                });
                
                // 创建并运行动作序列
                auto seq = Sequence::create(callback2, DelayTime::create(0.3f / jiasu), callback3, nullptr);
                plane->sprite_mark->runAction(seq);
                return;
            }
            
            // 更新飞机塔的角度
            plane->tower_angle = plane->tower_angle - Clockwise * 5 < 0 ? 
                                360 + plane->tower_angle - Clockwise * 5 : 
                                (plane->tower_angle - Clockwise * 5 >= 360 ? 
                                 plane->tower_angle - Clockwise * 5 - 360 : 
                                 plane->tower_angle - Clockwise * 5);
            
            // 创建小幅旋转动作
            auto TurnLittle = RotateBy::create(5.0f / (360.0f * jiasu), static_cast<float>(Clockwise * 5));
            plane->sprite_mark->runAction(TurnLittle);
            plane->de_ang += 5;  // 累计旋转角度
        });
        
        // 创建旋转动作序列
        auto turn_sequence = Sequence::create(TurnCallback, DelayTime::create(5.0f / (360.0f * jiasu)), nullptr);
        auto action1 = RepeatForever::create(turn_sequence);
        plane->sprite_mark->runAction(action1); });

    // 执行瞄准回调
    plane->sprite_mark->runAction(callback1);
    Music::getInstance()->planeSound(); // 播放攻击音效
    return true;
}

// 飞机塔激光伤害处理
void PlaneAttackStrategy::PlaneDemage(MyPlane *plane, BaseLevelScene *my_scene, std::vector<Monster *> &monsters)
{
    // 对范围内的障碍物造成伤害
    for (auto it = my_scene->Obstacles.begin(); it != my_scene->Obstacles.end(); it++)
    {
        // 检测障碍物是否与激光束碰撞
        if (isColliding(it->second->curr, plane->curr_ray) && (*it).second->getHealth() > 0)
        {
            // 创建伤害特效
            auto c = cocos2d::Sprite::create();
            my_scene->addChild(c);
            c->setPosition((*it).second->getPosition());
            c->setScale(1.4f);

            // 加载特效动画帧
            cocos2d::Vector<cocos2d::SpriteFrame *> frames;
            for (int i = 3; i <= 4; ++i)
            {
                std::string frameName = "Towers/texiao_" + std::to_string(i) + ".png";
                auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 44, 47));
                if (frame)
                {
                    frames.pushBack(frame);
                }
            }

            // 创建并播放特效动画
            auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
            auto animate = cocos2d::Animate::create(animation);
            auto onDeathComplete = cocos2d::CallFunc::create([c]()
                                                             {
                                                                 c->removeFromParent(); // 动画完成后移除特效精灵
                                                             });
            c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));

            // 对障碍物造成伤害
            DemageSprite((*it).second, plane->GetDemage());
            if ((*it).second->getHealth() <= 0)
                (*it).second->toDie(my_scene);
        }
    }

    // 对范围内的怪物造成伤害
    for (auto it = monsters.begin(); it != monsters.end(); it++)
    {
        // 检测怪物是否与激光束碰撞
        if (isColliding(*it, plane->curr_ray) && (*it)->getHealth() > 0)
        {
            // 创建伤害特效
            auto c = cocos2d::Sprite::create();
            my_scene->addChild(c);
            c->setPosition((*it)->getPosition());
            c->setScale(1.4f);

            // 加载特效动画帧
            cocos2d::Vector<cocos2d::SpriteFrame *> frames;
            for (int i = 3; i <= 4; ++i)
            {
                std::string frameName = "Towers/texiao_" + std::to_string(i) + ".png";
                auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 44, 47));
                if (frame)
                {
                    frames.pushBack(frame);
                }
            }

            // 创建并播放特效动画
            auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
            auto animate = cocos2d::Animate::create(animation);
            auto onDeathComplete = cocos2d::CallFunc::create([c]()
                                                             {
                                                                 c->removeFromParent(); // 动画完成后移除特效精灵
                                                             });
            c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));

            // 对怪物造成伤害
            DemageSprite((*it), plane->GetDemage());
            if ((*it)->getHealth() <= 0)
                (*it)->toDie(my_scene);
        }
    }
}

// Shit攻击策略实现
void ShitAttackStrategy::attack(Tower *tower, BaseLevelScene *scene,
                                std::vector<Monster *> &monsters, char isTarget,
                                Monster *tar_m, Obstacle *tar_o, float jiasu)
{
    // 类型转换，确保传入的是Shit类型的塔
    Shit *shit = dynamic_cast<Shit *>(tower);
    if (!shit)
        return;

    // 重置攻击间隔
    shit->interval = 0;

    // 优先攻击指定目标
    if (isTarget == 1)
    { // 指定怪物目标
        if (AttackSprite(shit, tar_m, scene, jiasu))
            return;
    }
    if (isTarget == 2)
    { // 指定障碍物目标
        if (AttackSprite(shit, tar_o, scene, jiasu))
            return;
    }

    // 遍历所有怪物，寻找攻击范围内的目标
    for (auto it = monsters.begin(); it != monsters.end(); it++)
    {
        if ((*it)->getHealth() <= 0)
            continue; // 跳过已死亡的怪物
        if (AttackSprite(shit, (*it), scene, jiasu))
            break; // 找到目标并攻击后退出循环
    }
}

// 粪便塔攻击精灵的模板函数 - 支持攻击怪物和障碍物
template <class T>
bool ShitAttackStrategy::AttackSprite(Shit *shit, T *sp, BaseLevelScene *my_scene, float jiasu)
{
    // 计算目标与塔之间的距离
    Vec2 m_pos = sp->getPosition();
    double distance = sqrt((m_pos.x - shit->pos.x) * (m_pos.x - shit->pos.x) +
                           (m_pos.y - shit->pos.y) * (m_pos.y - shit->pos.y));

    // 检查目标是否在攻击范围内
    if (distance < shit->GetRange() / 2)
    {
        // 攻击回调函数
        auto callback2 = CallFunc::create([shit, my_scene, sp, jiasu, this]()
                                          {
            ShellProduct(shit, my_scene);  // 创建炮弹

            // 炮弹移动的回调函数
            auto callback3 = CallFunc::create([sp, shit, jiasu]() {
                auto m_pos = sp->getPosition();
                auto s_pos = shit->curr_shell->getPosition();
                auto move = m_pos - s_pos;
                move.normalize();
                move = move * 10;  // 设置移动速度
                auto action3 = MoveBy::create(10 / (speed * jiasu), move);
                shit->curr_shell->runAction(action3);
            });
            
            // 炮弹命中检测的回调函数
            auto callback4 = CallFunc::create([my_scene, sp, shit, this]() {
                // 检测炮弹与目标的距离，小于10像素视为命中
                if ((sp->getPosition() - shit->curr_shell->getPosition()).distance(Vec2(0, 0)) < 10.f) {
                    shit->curr_shell->stopAllActions();
                    ShellDemage(shit, my_scene, sp);  // 处理伤害和特效
                }
            });
            
            // 设置炮弹飞行时间
            shit->de_time = 10 / (speed * jiasu);
            // 创建炮弹移动和检测的序列动作
            auto sequence3 = Sequence::create(callback3, DelayTime::create(shit->de_time), callback4, nullptr);
            auto repeat = RepeatForever::create(sequence3);
            shit->curr_shell->runAction(repeat); });

        // 立即执行攻击回调
        callback2->execute();
        return true; // 成功攻击目标
    }
    return false; // 目标不在攻击范围内
}

// 创建粪便塔的炮弹
void ShitAttackStrategy::ShellProduct(Shit *shit, Scene *my_scene)
{
    // 根据塔的等级创建对应的炮弹
    auto shell = Sprite::create(shit->shit_shell[shit->GetGrade()]);
    Vec2 start_pos;
    start_pos.x = shit->pos.x;
    start_pos.y = shit->pos.y;
    shell->setPosition(start_pos);
    shell->setScale(1.0); // 设置炮弹缩放比例
    my_scene->addChild(shell);
    shit->curr_shell = shell;          // 保存当前炮弹引用
    Music::getInstance()->ShitSound(); // 播放攻击音效
}

// 粪便塔炮弹造成伤害的模板函数 - 支持对怪物和障碍物造成伤害
template <class T>
void ShitAttackStrategy::ShellDemage(Shit *shit, BaseLevelScene *my_scene, T *sp)
{
    // 移除炮弹
    shit->curr_shell->removeFromParent();
    // 对目标造成伤害
    DemageSprite(sp, shit->GetDemage());
    // 检查目标是否死亡
    if (sp->getHealth() <= 0)
    {
        (sp)->toDie(my_scene);
    }
}

// 特化版本的ShellDemage for Monster - 对怪物造成减速效果
template <>
void ShitAttackStrategy::ShellDemage(Shit *shit, BaseLevelScene *my_scene, Monster *sp)
{
    // 移除炮弹
    shit->curr_shell->removeFromParent();

    // 创建减速特效精灵
    auto c = cocos2d::Sprite::create();
    if (!c)
    {
        CCLOG("Failed to create c sprite.");
        return;
    }
    sp->addChild(c);
    c->setPosition(40, 0); // 设置特效位置
    c->setScale(1.4f);

    // 加载减速特效动画帧
    cocos2d::Vector<cocos2d::SpriteFrame *> frames;
    for (int i = 5; i <= 6; ++i)
    {
        std::string frameName = "Towers/texiao_" + std::to_string(i) + ".png";
        auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 55, 18));
        if (frame)
        {
            frames.pushBack(frame);
        }
    }

    // 检查是否成功加载动画帧
    if (frames.empty())
    {
        CCLOG("No frames found for death animation, skipping.");
        return;
    }

    // 创建并播放减速特效动画
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 2.0f);
    auto animate = cocos2d::Animate::create(animation);
    auto onDeathComplete = cocos2d::CallFunc::create([c]()
                                                     {
                                                         c->removeFromParent(); // 动画完成后移除特效精灵
                                                     });
    c->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));

    // 对怪物造成伤害
    DemageSprite(sp, shit->GetDemage());

    // 如果怪物未死亡，则减速效果
    if (sp->getHealth() > 0)
        sp->speedaction->setSpeed(0.5f); // 减速至原速度的50%

    // 创建延迟动作，4秒后恢复速度
    auto delayaction = Sequence::create(
        DelayTime::create(4.0f),
        CallFunc::create([=]
                         { sp->speedaction->setSpeed(1.0f); }), // 恢复正常速度
        nullptr);
    my_scene->runAction(delayaction);

    // 检查怪物是否死亡
    if (sp->getHealth() <= 0)
    {
        (sp)->toDie(my_scene);
    }
}

// 碰撞检测函数 - 检测两个精灵是否相交
bool isColliding(Sprite *spriteA, Sprite *spriteB)
{
    return spriteA->getBoundingBox().intersectsRect(spriteB->getBoundingBox());
}

// 通用伤害函数模板 - 对目标造成指定伤害
template <class T>
void DemageSprite(T *sp, int demage)
{
    sp->getHurt(demage); // 调用目标的受伤方法
}