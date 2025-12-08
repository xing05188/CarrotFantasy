#include "Projectile.h"
#include "Tower.h"
#include "BaseLevelScene.h"

// 静态成员定义
std::string BottleProjectile::bottle_shell[3] = { "Towers/THuo/shell1-1.png","Towers/THuo/shell1-2.png" ,"Towers/THuo/shell1-3.png" }; // 瓶子塔炮弹
std::string PlaneProjectile::ray[3] = { "Towers/TPlane/ray0.png","Towers/TPlane/ray1.png","Towers/TPlane/ray2.png" }; // 飞机塔光线
std::string ShitProjectile::shit_shell[3] = { "Towers/TShit/shitshell1.png","Towers/TShit/shitshell2.png","Towers/TShit/shitshell3.png" }; // 粪便塔炮弹
std::string FanProjectile::clover[3] = { "Towers/TFan/windmill0.png","Towers/TFan/windmill1.png","Towers/TFan/windmill2.png" }; // 风扇塔四叶草


std::string SunProjectile::sun_halo = "Towers/sunhalo.png"; // 太阳塔光环

const float BottleProjectile::speed = 800; // 瓶子塔炮弹飞行速度

const float ShitProjectile::speed = 500;   // 粪便塔炮弹飞行速度

const float FanProjectile::speed = 400;    // 风扇塔四叶草飞行速度


//瓶子炮抛射物处理
template <class T>
void BottleProjectile::attack(std::unique_ptr<BottleProjectile> self,Bottle *bottle, BaseLevelScene *my_scene, float jiasu,T*sp)
{
    // 把 unique_ptr 包装到可拷贝的 holder 中
    auto pool = self ? self->pool : nullptr;
    auto holder = std::make_shared<std::unique_ptr<BottleProjectile>>(std::move(self)); // holder 持有 unique_ptr
    
    BottleProjectile*obj= holder->get();
    obj->shell_sprite = Sprite::create(obj->shell_path);

    Vec2 start_pos;
    start_pos.x = bottle->pos.x;
    start_pos.y = bottle->pos.y;
    obj->shell_sprite->setPosition(start_pos);
    obj->shell_sprite->setScale(0.6); // 设置炮弹缩放比例
    my_scene->addChild(obj->shell_sprite);


    // 炮弹移动的回调函数
    auto callback3 = CallFunc::create([sp, bottle, jiasu,holder]() {
                BottleProjectile*obj= holder->get();
                if(!obj)return;
                auto m_pos = sp->getPosition();
                auto s_pos = obj->shell_sprite->getPosition();
                auto move = m_pos - s_pos;
                move.normalize();
                move = move * 10;  // 设置移动速度
                auto action3 = MoveBy::create(10.0f / (speed * jiasu), move);
                
                obj->shell_sprite->runAction(action3);
            });
            
            // 炮弹命中检测的回调函数
    auto callback4 = CallFunc::create([my_scene, sp, bottle, this,holder,pool]() mutable{
                if (!holder || !holder->get()) {
                    return;
                }        
                BottleProjectile*obj= holder->get();
                    // 检查精灵是否存在
                if (!obj) {
                    return;
                }

                // 检测炮弹与目标的距离，小于10像素视为命中
                if ((sp->getPosition() - obj->shell_sprite->getPosition()).distance(Vec2(0, 0)) < 10.f) {
                    obj->shell_sprite->stopAllActions();
                    
                    bottleDemage(bottle, my_scene, sp);  // 处理伤害和特效
                    
                    obj->shell_sprite->removeFromParent(); // 移除炮弹

                    
                    if (pool && holder&&holder->get()) {
                        pool->returnObject(std::move(*holder));
                        holder.reset();  // 清空 holder，后续检查 holder->get() 会返回 nullptr
                    }
                }
            });
            
    // 设置炮弹飞行时间
    float fly_time = 10.0f / (speed * jiasu);
    // 创建炮弹移动和检测的序列动作
    auto sequence3 = Sequence::create(callback3, DelayTime::create(fly_time), callback4, nullptr);
    auto repeat = RepeatForever::create(sequence3);
    obj->shell_sprite->runAction(repeat); 
}

template <class T>
void BottleProjectile::bottleDemage(Bottle *bottle, BaseLevelScene *my_scene, T *sp){



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



//太阳抛射物处理
void SunProjectile::attack(std::unique_ptr<SunProjectile> self,Sun *sun, BaseLevelScene *my_scene,std::vector<Monster *> &monsters, float jiasu)
{   
    // 把 unique_ptr 包装到可拷贝的 holder 中
    auto pool = self ? self->pool : nullptr;
    auto holder = std::make_shared<std::unique_ptr<SunProjectile>>(std::move(self)); // holder 持有 unique_ptr
    SunProjectile*obj= holder->get();

    obj->halo_sprite->setPosition(sun->pos);
    // 根据攻击范围调整光环大小
    obj->halo_sprite->setScale(0.8 * sun->GetRange() / obj->halo_sprite->getContentSize().width);
    my_scene->addChild(obj->halo_sprite);

    // 光环放大动画
    auto scale_up = ScaleBy::create(0.3 / jiasu, 1.25);

    // 光环放大完成后执行伤害的回调函数
    auto callback = CallFunc::create([sun, my_scene, &monsters,holder,pool]()
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
        SunProjectile*obj= holder->get();
        obj->halo_sprite->removeFromParent(); 
        pool->returnObject(std::move(*holder));
    });
    
    // 创建并运行动作序列
    auto seq = Sequence::create(scale_up, callback, nullptr);
    obj->halo_sprite->runAction(seq);

    
}




//飞机抛射物处理
void PlaneProjectile::attack(std::unique_ptr<PlaneProjectile> self,MyPlane *plane, BaseLevelScene *my_scene,std::vector<Monster *> &monsters, float jiasu)
{
   // 把 unique_ptr 包装到可拷贝的 holder 中
    auto pool = self ? self->pool : nullptr;
    auto holder = std::make_shared<std::unique_ptr<PlaneProjectile>>(std::move(self)); // holder 持有 unique_ptr


    auto callback2 = CallFunc::create([plane, my_scene,holder]() {
                    // 根据塔的等级创建对应的激光束
                    PlaneProjectile* obj = holder->get();
                    obj->ray_sprite= Sprite::create(obj->ray_path);
                    obj->ray_sprite->setAnchorPoint(Vec2(0.5, 0));
                    obj->ray_sprite->setScale(CELL_SIZE / obj->ray_sprite->getContentSize().width);
                    
                    // 计算激光束的位置和角度
                    Vec2 ray_pos;
                    ray_pos.x = plane->pos.x + 0.5 * plane->sprite_mark->getContentSize().height * 
                               cos(CC_DEGREES_TO_RADIANS(plane->tower_angle));
                    ray_pos.y = plane->pos.y + 0.5 * plane->sprite_mark->getContentSize().height * 
                               sin(CC_DEGREES_TO_RADIANS(plane->tower_angle));
                    obj->ray_sprite->setPosition(ray_pos);
                    obj->ray_sprite->setRotation(90 - plane->tower_angle);
                    my_scene->addChild(obj->ray_sprite);
                });
                
                // 激光伤害的回调函数
                auto callback3 = CallFunc::create([plane, my_scene, &monsters,holder, pool,this]() {
                    //问题出在这一步
                    PlaneProjectile* obj = holder->get();
                    planeDemage(plane, my_scene, monsters,obj->ray_sprite);  // 处理激光伤害

                    obj->ray_sprite->removeFromParent();

                    pool->returnObject(std::move(*holder));
        
                });
                /////////////////////////////////////////////////////////////////////////////
                // 创建并运行动作序列
                PlaneProjectile* obj = holder->get();
                callback2->execute();
                auto seq = Sequence::create(DelayTime::create(0.3f / jiasu), callback3, nullptr);
                if (obj->ray_sprite){obj->ray_sprite->runAction(seq);}
                
}


void PlaneProjectile::planeDemage(MyPlane *plane, BaseLevelScene *my_scene, std::vector<Monster *> &monsters,Sprite*ray_sprite){
    // 对范围内的障碍物造成伤害
    /*
    
    */
    for (auto it = my_scene->Obstacles.begin(); it != my_scene->Obstacles.end(); it++)
    {
        // 检测障碍物是否与激光束碰撞
        if (isColliding__(it->second->curr,ray_sprite) && (*it).second->getHealth() > 0)
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
        if (isColliding__(*it, ray_sprite) && (*it)->getHealth() > 0)
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


//大便抛射物处理
template <class T>
void ShitProjectile::attack(std::unique_ptr<ShitProjectile> self,Shit *shit, BaseLevelScene *my_scene, float jiasu,T*sp)
{
    // 把 unique_ptr 包装到可拷贝的 holder 中
    auto pool = self ? self->pool : nullptr;
    auto holder = std::make_shared<std::unique_ptr<ShitProjectile>>(std::move(self)); // holder 持有 unique_ptr
    ShitProjectile* obj=holder->get();
    obj->shit_sprite=Sprite::create(obj->shit_path);
    Vec2 start_pos;
    start_pos.x = shit->pos.x;
    start_pos.y = shit->pos.y;
    obj->shit_sprite->setPosition(start_pos);
    obj->shit_sprite->setScale(1.0); // 设置炮弹缩放比例
    my_scene->addChild(obj->shit_sprite);
 
    Music::getInstance()->ShitSound(); // 播放攻击音效


                // 炮弹移动的回调函数
    auto callback3 = CallFunc::create([sp, shit, jiasu,holder]() {
                ShitProjectile* obj=holder->get();
                if(!obj) return;
                auto m_pos = sp->getPosition();
                auto s_pos = obj->shit_sprite->getPosition();
                auto move = m_pos - s_pos;
                move.normalize();
                move = move * 10;  // 设置移动速度
                auto action3 = MoveBy::create(10 / (speed * jiasu), move);
                obj->shit_sprite->runAction(action3);
            });
            
            // 炮弹命中检测的回调函数
            auto callback4 = CallFunc::create([my_scene, sp, shit, this,holder,pool]() {
                ShitProjectile* obj=holder->get();
                if(!obj) return;
                // 检测炮弹与目标的距离，小于10像素视为命中
                if ((sp->getPosition() - obj->shit_sprite->getPosition()).distance(Vec2(0, 0)) < 10.f) {
                    obj->shit_sprite->stopAllActions();
                    shitDemage(shit, my_scene, sp, obj->shit_sprite);  // 处理伤害和特效
                    obj->shit_sprite->removeFromParent();

                    if(pool && holder&&holder->get()){
                        pool->returnObject(std::move(*holder));
                    }
                }
            });
            
            // 设置炮弹飞行时间
            float fly_time= 10 / (speed * jiasu);
            // 创建炮弹移动和检测的序列动作
            auto sequence3 = Sequence::create(callback3, DelayTime::create(fly_time), callback4, nullptr);
            auto repeat = RepeatForever::create(sequence3);
            obj->shit_sprite->runAction(repeat);
}


template <class T>
void ShitProjectile::shitDemage(Shit *shit, BaseLevelScene *my_scene, T*sp, Sprite* shit_sprite)
{
    // 对目标造成伤害
    DemageSprite(sp, shit->GetDemage());
    // 检查目标是否死亡
    if (sp->getHealth() <= 0)
    {
        (sp)->toDie(my_scene);
    }
}


template<>
void ShitProjectile::shitDemage(Shit *shit, BaseLevelScene *my_scene, Monster*sp, Sprite* shit_sprite)
{
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




//风扇塔抛射物处理
void FanProjectile::attack(std::unique_ptr<FanProjectile> self,Fan *fan, BaseLevelScene *my_scene,float jiasu,std::vector<Monster *> &monsters,Vec2&direction)
{
     // 把 unique_ptr 包装到可拷贝的 holder 中
    auto pool = self ? self->pool : nullptr;
    auto holder = std::make_shared<std::unique_ptr<FanProjectile>>(std::move(self)); // holder 持有 unique_ptr
    FanProjectile*obj=holder->get();
    obj->clover_sprite=Sprite::create(obj->clover_path);

    Vec2 start_pos;
    start_pos.x = fan->pos.x;
    start_pos.y = fan->pos.y;
    obj->clover_sprite->setPosition(start_pos);
    obj->clover_sprite->setScale(0.8); // 设置四叶草缩放比例
    my_scene->addChild(obj->clover_sprite);

   
    float maxDistance = 1000.0f;
    float flightTime = maxDistance / (speed * jiasu);
            
    auto moveAction = MoveBy::create(flightTime, direction * maxDistance);
    auto rotateAction = RotateBy::create(flightTime, 360.0f * flightTime);
    auto spawnAction = Spawn::create(moveAction, rotateAction, nullptr);
            
            // 飞行结束后自动移除
    auto removeAction = CallFunc::create([holder]() {
                auto obj = holder->get();
                if(!obj)return;
                if (obj->clover_sprite) {
                    obj->clover_sprite->removeFromParent();
                    obj->clover_sprite = nullptr;
                }
            });
            
            auto sequence = Sequence::create(spawnAction, removeAction, nullptr);
            obj->clover_sprite->runAction(sequence);
            
            // 修改：创建持续检测的回调函数
    auto checkCollision = CallFunc::create([fan, my_scene, monsters,holder, this,pool,start_pos,maxDistance]() {
                FanProjectile* obj=holder->get();
                if(!obj)return;
                if (!obj->clover_sprite) return;
                
                // 检测与所有怪物的碰撞
                for (auto monster : monsters) {
                    if (monster->getHealth() > 0) {
                        float distance = (monster->getPosition() - obj->clover_sprite->getPosition()).length();
                        if (distance < 40.f) { // 碰撞阈值
                            cloverDemage(fan, my_scene, monster);
                        }
                    }
                }
                
                // 检测与所有障碍物的碰撞
                for (auto obstacle : my_scene->Obstacles) {
                    if (obstacle.second->getHealth() > 0) {
                        float distance = (obstacle.second->getPosition() - obj->clover_sprite->getPosition()).length();
                        if (distance < 40.f) {
                            cloverDemage(fan, my_scene, obstacle.second);
                        }
                    }
                }

                if ((obj->clover_sprite->getPosition() - start_pos).length() >= maxDistance-40.f) {
                    // 达到最大飞行距离，移除四叶草
                    obj->clover_sprite->stopAllActions();
                    obj->clover_sprite->removeFromParent();
                    if(pool && holder&&holder->get()){
                        pool->returnObject(std::move(*holder));
                    }
                }
            });
            
            // 修改：创建重复检测的动作序列
            auto collisionSequence = Sequence::create(
                checkCollision,
                DelayTime::create(0.1f), // 每0.1秒检测一次
                nullptr
            );
            auto repeatCollision = Repeat::create(collisionSequence, static_cast<int>(flightTime / 0.1f));
            obj->clover_sprite->runAction(repeatCollision);

}

template<class T>
void FanProjectile::cloverDemage(Fan *fan, BaseLevelScene *my_scene, T *sp)
{
    // 对目标造成伤害
    DemageSprite(sp, fan->GetDemage());
    
    // 创建击中特效精灵
    auto c = cocos2d::Sprite::create();
    if (!c)
    {
        CCLOG("Failed to create c sprite.");
        return;
    }
    my_scene->addChild(c);
    c->setPosition(sp->getPosition());
    c->setScale(1.2f);

    // 加载击中特效动画帧
    cocos2d::Vector<cocos2d::SpriteFrame *> frames;
    for (int i = 7; i <= 8; ++i)
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
        CCLOG("No frames found for hit animation, skipping.");
        return;
    }

    // 创建并播放击中动画
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = cocos2d::Animate::create(animation);
    auto onHitComplete = cocos2d::CallFunc::create([c]()
                                                     {
                                                         c->removeFromParent(); // 动画完成后移除特效精灵
                                                     });

    c->runAction(cocos2d::Sequence::create(animate, onHitComplete, nullptr));
    
    // 检查目标是否死亡
    if (sp->getHealth() <= 0)
    {
        (sp)->toDie(my_scene);
    }
}









bool isColliding__(Sprite *spriteA, Sprite *spriteB)
{
    return spriteA->getBoundingBox().intersectsRect(spriteB->getBoundingBox());
}


// 通用伤害函数模板 - 对目标造成指定伤害
template <class T>
void DemageSprite(T *sp, int demage)
{
    sp->getHurt(demage); // 调用目标的受伤方法
}


template void BottleProjectile::attack<Monster>(std::unique_ptr<BottleProjectile> self, Bottle* bottle, BaseLevelScene* my_scene, float jiasu, Monster* sp);
template void BottleProjectile::attack<Obstacle>(std::unique_ptr<BottleProjectile> self, Bottle* bottle, BaseLevelScene* my_scene, float jiasu, Obstacle* sp);

template void BottleProjectile::bottleDemage<Monster>(Bottle *bottle, BaseLevelScene *my_scene, Monster* sp);
template void BottleProjectile::bottleDemage<Obstacle>(Bottle *bottle, BaseLevelScene *my_scene, Obstacle* sp);


template void ShitProjectile::attack<Monster>(std::unique_ptr<ShitProjectile> self, Shit* shit, BaseLevelScene* my_scene, float jiasu, Monster* sp);
template void ShitProjectile::attack<Obstacle>(std::unique_ptr<ShitProjectile> self, Shit* shit, BaseLevelScene* my_scene, float jiasu, Obstacle* sp);

template void ShitProjectile::shitDemage<Monster>(Shit *shit, BaseLevelScene *my_scene, Monster* sp, Sprite* shit_sprite);
template void ShitProjectile::shitDemage<Obstacle>(Shit *shit, BaseLevelScene *my_scene, Obstacle* sp, Sprite* shit_sprite);


template void FanProjectile::cloverDemage<Monster>(Fan* fan, BaseLevelScene* my_scene, Monster* sp);
template void FanProjectile::cloverDemage<Obstacle>(Fan* fan, BaseLevelScene* my_scene, Obstacle* sp);