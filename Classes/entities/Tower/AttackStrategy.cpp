/**
 * Refactored with Strategy Pattern
 * 使用策略模式重构
 * 
 * Design Pattern: Strategy Pattern
 * 设计模式：策略模式
 * 
 * Benefits:
 * 1. Allows different attack behaviors to be selected at runtime
 * 2. Encapsulates attack algorithms in separate classes, making them interchangeable
 * 3. Eliminates conditional statements for different attack types
 * 4. Makes it easy to add new attack strategies without modifying existing code
 * 
 * 好处：
 * 1. 允许在运行时选择不同的攻击行为
 * 2. 将攻击算法封装在单独的类中，使它们可以互换
 * 3. 消除了针对不同攻击类型的条件语句
 * 4. 无需修改现有代码即可轻松添加新的攻击策略
 * 
 * Problems Solved:
 * 1. Avoids code duplication when implementing similar attack behaviors
 * 2. Provides a clean way to switch between different attack algorithms
 * 3. Makes the code more maintainable by separating attack logic from the tower class
 * 4. Enables towers to change their attack behavior dynamically during gameplay
 * 
 * 解决的问题：
 * 1. 避免在实现类似攻击行为时的代码重复
 * 2. 提供了一种在不同攻击算法之间切换的清晰方式
 * 3. 通过将攻击逻辑与塔类分离，使代码更易于维护
 * 4. 使塔能够在游戏过程中动态改变其攻击行为
 */
 // Refactored with Strategy Pattern
 // 使用策略模式重构
#include "AttackStrategy.h"
#include "Tower.h"
#include "BaseLevelScene.h"
#include "Projectile.h"

// 静态常量定义 - 定义各种攻击策略的速度
//const float BottleAttackStrategy::speed = 800; // 瓶子塔炮弹飞行速度
//const float ShitAttackStrategy::speed = 500;   // 粪便塔炮弹飞行速度
//const float FanAttackStrategy::speed = 400;    // 风扇塔四叶草飞行速度

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

        //////////////////////////////////////////////////////////////////////////////////////////////
            bottle->tower_angle = ang;  // 更新塔的当前角度
            
            Music::getInstance()->bottleSound(); // 播放攻击音效
            auto projPtr = my_scene->bottleProjectilePool->borrowObject(); // 保存所有权
            projPtr->pool = my_scene->bottleProjectilePool;
            // 根据塔的等级创建对应的炮弹
            projPtr->setIndex(bottle->GetGrade());
            // 把所有权移入 attack（之后 projPtr 为空）
            projPtr->attack(std::move(projPtr), bottle, my_scene, jiasu,sp);

        //////////////////////////////////////////////////////////////////////////////////////////////
        });

        // 创建旋转和攻击的序列动作
        auto sequence2 = Sequence::create(action1, callback2, nullptr);
        bottle->sprite_mark->runAction(sequence2);
        return true; // 成功攻击目标
       
    }
    return false; // 目标不在攻击范围内
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

    //////////////////////////////////////////////////////////////////////////
    // 创建太阳光环效果
    auto projPtr = my_scene->sunProjectilePool->borrowObject(); // 获取 unique_ptr
    projPtr->pool = my_scene->sunProjectilePool;
    projPtr->setSprite();
    // 把所有权移入 attack（之后 projPtr 为空）
    projPtr->attack(std::move(projPtr), sun, my_scene, monsters, jiasu);
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
                /////////////////////////////////////////////////////////////
                // 创建激光束的回调函数
                auto projPtr = my_scene->planeProjectilePool->borrowObject(); // 获取 unique_ptr
                projPtr->setIndex(plane->GetGrade());
                projPtr->pool = my_scene->planeProjectilePool;

                // 把所有权移入 attack（之后 projPtr 为空）
                projPtr->attack(std::move(projPtr), plane, my_scene, monsters, jiasu);
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
            
            auto projPtr = my_scene->shitProjectilePool->borrowObject(); // 保存所有权
            projPtr->pool = my_scene->shitProjectilePool;
            // 根据塔的等级创建对应的炮弹
            projPtr->setIndex(shit->GetGrade());
            // 把所有权移入 attack（之后 projPtr 为空）
            projPtr->attack(std::move(projPtr), shit, my_scene, jiasu,sp);
        });
        
        // 立即执行攻击回调
        callback2->execute();
        return true; // 成功攻击目标
    }
    return false; // 目标不在攻击范围内
}






////////////////////////////////////////////////////////////////////////////////////////////
// Fan攻击策略实现
void FanAttackStrategy::attack(Tower *tower, BaseLevelScene *scene,
                              std::vector<Monster *> &monsters, char isTarget,
                              Monster *tar_m, Obstacle *tar_o, float jiasu)
{
    // 类型转换，确保传入的是Fan类型的塔
    Fan *fan = dynamic_cast<Fan *>(tower);
    if (!fan)
        return;

    // 重置攻击间隔
    fan->interval = 0;

    // 优先攻击指定目标
    if (isTarget == 1)
    { // 指定怪物目标
        if (AttackSprite(fan, tar_m, scene, jiasu, monsters))
            return;
    }
    if (isTarget == 2)
    { // 指定障碍物目标
        if (AttackSprite(fan, tar_o, scene, jiasu, monsters))
            return;
    }

    // 遍历所有怪物，寻找攻击范围内的目标
    for (auto it = monsters.begin(); it != monsters.end(); it++)
    {
        if ((*it)->getHealth() <= 0)
            continue; // 跳过已死亡的怪物
        if (AttackSprite(fan, (*it), scene, jiasu, monsters))
            break; // 找到目标并攻击后退出循环
    }
}

// 风扇塔攻击精灵的模板函数 - 支持攻击怪物和障碍物
template <class T>
bool FanAttackStrategy::AttackSprite(Fan *fan, T *sp, BaseLevelScene *my_scene, float jiasu,std::vector<Monster *> &monsters)
{
    Vec2 m_pos = sp->getPosition();
    double distance = sqrt((m_pos.x - fan->pos.x) * (m_pos.x - fan->pos.x) +
                           (m_pos.y - fan->pos.y) * (m_pos.y - fan->pos.y));

    if (distance < fan->GetRange() / 2)
    {
        auto callback2 = CallFunc::create([fan, m_pos, my_scene, jiasu, &monsters, this]() {
            Vec2 direction = m_pos - fan->pos;
            direction.normalize();

            //////////////////////////////////////////////////////////////////////////////////
            auto projPtr = my_scene->fanProjectilePool->borrowObject(); // 保存所有权
            projPtr->pool = my_scene->fanProjectilePool;
            // 根据塔的等级创建对应的炮弹
            projPtr->setIndex(fan->GetGrade());
            // 把所有权移入 attack（之后 projPtr 为空）
            projPtr->attack(std::move(projPtr), fan, my_scene, jiasu,monsters,direction);

            Music::getInstance()->fanSound(); // 播放攻击音效

            
        });

        callback2->execute();
        return true;
    }
    return false;
}














// MagicAttackStrategy实现
void MagicAttackStrategy::attack(Tower *tower, BaseLevelScene *scene,
                                std::vector<Monster *> &monsters, char isTarget,
                                Monster *tar_m, Obstacle *tar_o, float jiasu)
{
    // 类型转换，确保传入的是MagicTower类型的塔
    MagicTower *magic = dynamic_cast<MagicTower *>(tower);
    if (!magic)
        return;

    // 重置攻击间隔
    magic->interval = 0;

    // 优先攻击指定目标
    if (isTarget == 1 && tar_m)
    {
        // 检查目标是否在攻击范围内
        Vec2 m_pos = tar_m->getPosition();
        float distance = sqrt((m_pos.x - magic->pos.x) * (m_pos.x - magic->pos.x) +
                              (m_pos.y - magic->pos.y) * (m_pos.y - magic->pos.y));
        if (distance < magic->GetRange() / 2 && tar_m->getHealth() > 0)
        {
            MagicBeamAttack(magic, scene, monsters, tar_m, jiasu);
            return;
        }
    }

    // 优先攻击指定障碍物目标（支持魔法塔对障碍物的攻击）
    if (isTarget == 2 && tar_o)
    {
        Vec2 o_pos = tar_o->getPosition();
        float distance = sqrt((o_pos.x - magic->pos.x) * (o_pos.x - magic->pos.x) +
                              (o_pos.y - magic->pos.y) * (o_pos.y - magic->pos.y));
        if (distance < magic->GetRange() / 2 && tar_o->getHealth() > 0)
        {
            MagicBeamAttack(magic, scene, monsters, tar_o, jiasu);
            return;
        }
    }

    // 如果已经锁定目标，检查目标是否仍在范围内且存活
    if (magic->lockedTarget)
    {
        Vec2 m_pos = magic->lockedTarget->getPosition();
        float distance = sqrt((m_pos.x - magic->pos.x) * (m_pos.x - magic->pos.x) +
                              (m_pos.y - magic->pos.y) * (m_pos.y - magic->pos.y));
        
                              // 如果目标是障碍物且 isTarget 不是 2（表示取消了障碍物追踪），清除锁定
        Obstacle* lockedObstacle = dynamic_cast<Obstacle*>(magic->lockedTarget);
        if (lockedObstacle && isTarget != 2) {
            magic->clearBeam();
        }
        // 如果目标死亡或离开范围，清除锁定
        else if (magic->lockedTarget->getHealth() <= 0 || distance >= magic->GetRange() / 2)
        {
            magic->lockedTarget = nullptr;
            if (magic->curr_beam)
            {
                magic->curr_beam->removeFromParent();
                magic->curr_beam = nullptr;
            }
        }
        // 否则继续攻击当前目标（传入锁定的怪物指针）
        else
        {
            MagicBeamAttack(magic, scene, monsters, magic->lockedTarget, jiasu);
            return;
        }
    }

    // 寻找新的目标，优先攻击最先进入射程的怪物
    for (auto it = monsters.begin(); it != monsters.end(); it++)
    {
        if ((*it)->getHealth() <= 0)
            continue; // 跳过已死亡的怪物
            
        Vec2 m_pos = (*it)->getPosition();
        float distance = sqrt((m_pos.x - magic->pos.x) * (m_pos.x - magic->pos.x) +
                              (m_pos.y - magic->pos.y) * (m_pos.y - magic->pos.y));
        
        // 找到范围内的目标
        if (distance < magic->GetRange() / 2)
        {
            magic->lockedTarget = (*it); // 锁定目标
            MagicBeamAttack(magic, scene, monsters, magic->lockedTarget, jiasu);
            return; // 找到目标后退出
        }
    }

    if (isTarget == 0) {
        magic->clearBeam();
        return;
    }
}

// 魔法塔执行光束攻击
template <class T>
void MagicAttackStrategy::MagicBeamAttack(MagicTower *magic, BaseLevelScene *my_scene, 
                                         std::vector<Monster *> &monsters, T *sp, float jiasu)
{
    // 检查魔法塔和目标是否存在
    if (!magic || !sp)
        return;

    // 如果目标是怪物，则记录为锁定目标；否则保持 lockedTarget 为 nullptr
    Monster *maybeMonster = dynamic_cast<Monster *>(sp);
    if (maybeMonster)
    {
        magic->lockedTarget = maybeMonster;
    }

    // 如果没有光束，创建新光束（基于传入目标）
    if (!magic->curr_beam)
    {
        CreateMagicBeam(magic, my_scene, sp);
    }
    else
    {
        // 更新光束位置和方向（使用传入目标）
        UpdateBeamPosition(magic, sp);
    }

    // 处理持续伤害（针对传入目标）
    float currentTime = Director::getInstance()->getTotalFrames() / 60.0f; // 假设60FPS
    ProcessBeamDamage(magic, my_scene, sp, currentTime);
}

// 创建魔法光束
template <class T>
void MagicAttackStrategy::CreateMagicBeam(MagicTower *magic, BaseLevelScene *my_scene, T *target)
{
    if (!magic || !target)
        return;

    // 根据塔的等级创建对应的光束
    magic->curr_beam = Sprite::create(magic->magic_beam[magic->GetGrade()]);
    magic->curr_beam->setAnchorPoint(Vec2(0.5, 0)); // 设置锚点为底部中心
    magic->curr_beam->setColor(Color3B(128, 0, 255)); // 设置为紫色

    // 计算光束的初始位置和方向
    Vec2 targetPos = target->getPosition();
    Vec2 direction = targetPos - magic->pos;
    float distance = direction.length();
    float angle = 90.0f-CC_RADIANS_TO_DEGREES(direction.getAngle());

    // 设置光束位置和旋转
    magic->curr_beam->setPosition(magic->pos);
    magic->curr_beam->setRotation(angle);

    // 设置光束长度，使其刚好到达目标
    float beamHeight = magic->curr_beam->getContentSize().height;
    if (beamHeight > 0)
        magic->curr_beam->setScaleY(distance / beamHeight);
    magic->curr_beam->setScaleX(1.0f); // 设置光束宽度

    my_scene->addChild(magic->curr_beam);

    // 播放魔法攻击音效
    Music::getInstance()->magicSound();
}

// 更新光束位置和方向
template <class T>
void MagicAttackStrategy::UpdateBeamPosition(MagicTower *magic, T *target)
{
    // 检查魔法塔和光束是否存在
    if (!magic || !magic->curr_beam || !target)
        return;

    // 计算新的方向和角度
    Vec2 targetPos = target->getPosition();
    Vec2 direction = targetPos - magic->pos;
    float distance = direction.length();
    float angle = 90.0f-CC_RADIANS_TO_DEGREES(direction.getAngle());

    // 更新光束旋转和长度
    magic->curr_beam->setRotation(angle);
    float beamHeight = magic->curr_beam->getContentSize().height;
    if (beamHeight > 0)
        magic->curr_beam->setScaleY(distance / beamHeight);
}

// 处理光束对目标的持续伤害
template <class T>
void MagicAttackStrategy::ProcessBeamDamage(MagicTower *magic, BaseLevelScene *my_scene, T *target, float currentTime)
{
    // 检查是否到了造成伤害的时间
    if (currentTime - magic->lastDamageTime >= magic->beamDamageInterval)
    {
        if (!target)
        {
            // 无目标，移除光束
            magic->lockedTarget = nullptr;
            if (magic->curr_beam)
            {
                magic->curr_beam->removeFromParent();
                magic->curr_beam = nullptr;
            }
            return;
        }

        // 只要目标存活则造成伤害
        if (target->getHealth() > 0)
        {
            DemageSprite(target, magic->GetDemage());

            // 创建伤害特效
            auto effect = Sprite::create("Towers/texiao_9.png");
            if (effect)
            {
                effect->setPosition(target->getPosition());
                my_scene->addChild(effect);

                // 闪烁动画
                auto fadeOut = FadeOut::create(0.3f);
                auto remove = CallFunc::create([effect]() { effect->removeFromParent(); });
                effect->runAction(Sequence::create(fadeOut, remove, nullptr));
            }

            // 如果目标死亡，尝试调用 toDie 并清理光束/锁定（如果是怪物）
            if (target->getHealth() <= 0)
            {
                target->toDie(my_scene);
                // 如果目标是怪物并且等于 lockedTarget，需要清除
                Monster *m = dynamic_cast<Monster *>(target);
                if (m && magic->lockedTarget == m)
                {
                    magic->lockedTarget = nullptr;
                }
                if (magic->curr_beam)
                {
                    magic->curr_beam->removeFromParent();
                    magic->curr_beam = nullptr;
                }
            }
        }
        else
        {
            // 目标无效，移除光束
            Monster *m = dynamic_cast<Monster *>(target);
            if (m && magic->lockedTarget == m)
                magic->lockedTarget = nullptr;
            if (magic->curr_beam)
            {
                magic->curr_beam->removeFromParent();
                magic->curr_beam = nullptr;
            }
        }

        // 更新上次伤害时间
        magic->lastDamageTime = currentTime;
    }
}