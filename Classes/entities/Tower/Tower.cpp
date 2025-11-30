#include"Tower.h"
#include"BaseLevelScene.h"
#include"music.h"
#include"TowerFactory.h"

// 塔的图片资源表 - 每种塔有3个等级的图片
std::string Tower::tower_table[TOWER_NUM][3] = { 
    {"Towers/THuo/huo0.png","Towers/THuo/huo1.png","Towers/THuo/huo2.png"},    // 火焰塔
    {"Towers/TSun/sun0.png","Towers/TSun/sun1.png","Towers/TSun/sun2.png"},    // 太阳塔
    {"Towers/TPlane/plane0.png","Towers/TPlane/plane1.png","Towers/TPlane/plane2.png"}, // 飞机塔
    {"Towers/TShit/shit0.png","Towers/TShit/shit1.png","Towers/TShit/shit2.png"},    // 粪便塔
    {"Towers/TFan/windmill0.png","Towers/TFan/windmill1.png","Towers/TFan/windmill2.png"},      // 风扇塔
    {"Towers/TMagic/magic0.png","Towers/TMagic/magic1.png","Towers/TMagic/magic2.png"}      // 魔法塔
};

// 塔基图片资源表
std::string Tower::base_table[TOWER_NUM] = { 
    "Towers/THuo/huobase.png", "Towers/TSun/sunbase.png", "Towers/TPlane/planebase.png", "Towers/TShit/shitbase.png", "Towers/TFan/windmillbase.png", "Towers/TMagic/magicbase.png" 
};

// 锚点表 - 第一个为基座锚点，第二个为塔身锚点
Vec2 Tower::anchorpoint_table[TOWER_NUM][2] = { 
    {Vec2(0.5,0.5),Vec2(0.4,0.46)},  // 火焰塔
    {Vec2(0.5,0.5),Vec2(0.5,0.5)},  // 太阳塔
    {Vec2(0.5,0.5),Vec2(0.5,0.5)},  // 飞机塔
    {Vec2(0.5,1.0),Vec2(0.5,0.5)},  // 粪便塔
    {Vec2(0.5,0.8),Vec2(0.5,0.3)},  // 风扇塔
    {Vec2(0.5,1.0),Vec2(0.5,0.4)}   // 魔法塔
};

// 建造成本表
int Tower::build_cost[TOWER_NUM] = { 100,180 ,220,120,160,160 };
// 攻击力表 - 每种塔3个等级的攻击力
int Tower::demage_table[TOWER_NUM][3] = { {20,35,50},{30,45,60},{50,80,110},{5,10,20},{25,45,65},{12,18,25} };
// 攻击范围表 - 每种塔3个等级的攻击范围
int Tower::range_table[TOWER_NUM][3] = { {200,250,300},{200,250,350},{250,375,400},{200,250,300},{220,280,340},{180,240,310} };

// 升级成本表 - 每种塔有2次升级机会
int Tower::up_cost[TOWER_NUM][2] = { {180,260},{260,320},{320,380},{220,260},{260,320},{180,260} };
// 出售价格表 - 每种塔3个等级的出售价格
int Tower::sell_money[TOWER_NUM][3] = { {80,224,432},{144,352,608},{176,432,736},{96,272,480},{144,352,608},{80,224,432} };

// 出售按钮图片映射表
std::map<int, std::string> Tower::sale_graph = { 
    {80,"Towers/sale_80.png"}, {96,"Towers/sale_96.png"},
    {144,"Towers/sale_144.png"}, {176,"Towers/sale_176.png"}, 
    {224,"Towers/sale_224.png"}, {272,"Towers/sale_272.png"}, 
    {352,"Towers/sale_352.png"}, {432,"Towers/sale_432.png"},
    {480,"Towers/sale_480.png"}, {608,"Towers/sale_608.png"},
    {736,"Towers/sale_736.png"} 
};

// 升级按钮图片映射表
std::map<int, std::string> Tower::up_graph = { 
    {180,"Towers/up_180.png"}, {220,"Towers/up_220.png"},
    {260,"Towers/up_260.png"}, {320,"Towers/up_320.png"},
    {380,"Towers/up_380.png"} 
};

// 不可升级按钮图片映射表
std::map<int, std::string> Tower::noup_graph = { 
    {180,"Towers/noup_180.png"}, {220,"Towers/noup_220.png"},
    {260,"Towers/noup_260.png"}, {320,"Towers/noup_320.png"},
    {380,"Towers/noup_380.png"} 
};

// UI相关静态精灵指针
Sprite* Tower::curr_sale;
Sprite* Tower::curr_up;
Sprite* Tower::curr_range;

// 攻击间隔表 - 每种塔的攻击间隔
float Tower::interval_table[TOWER_NUM] = { 0.8,1.5,2.5,1.0,1.6,0.1 };

// 子类资源表
std::string Bottle::bottle_shell[3] = { "Towers/THuo/shell1-1.png","Towers/THuo/shell1-2.png" ,"Towers/THuo/shell1-3.png" }; // 瓶子塔炮弹
std::string MyPlane::ray[3] = { "Towers/TPlane/ray0.png","Towers/TPlane/ray1.png","Towers/TPlane/ray2.png" }; // 飞机塔光线
std::string Shit::shit_shell[3] = { "Towers/TShit/shitshell1.png","Towers/TShit/shitshell2.png","Towers/TShit/shitshell3.png" }; // 粪便塔炮弹
std::string Fan::clover[3] = { "Towers/TFan/windmill0.png","Towers/TFan/windmill1.png","Towers/TFan/windmill2.png" }; // 风扇塔四叶草
std::string MagicTower::magic_beam[3] = { "Towers/TMagic/magicbeam.png","Towers/TMagic/magicbeam.png","Towers/TMagic/magicbeam.png" }; // 魔法塔光束

// 建造塔的方法 - 在指定位置创建塔的基座和塔身
void Tower::build(BaseLevelScene* my_scene, Vec2 position)
{
    // 创建塔基
    auto base = Sprite::create(base_table[index]);
    base->setAnchorPoint(anchorpoint_table[index][0]);
    base->setPosition(position);
    my_scene->addChild(base);
    sp_base = base;
    
    // 创建塔身
    auto tower = Sprite::create(tower_table[index][grade]);
    tower->setAnchorPoint(anchorpoint_table[index][1]);
    tower->setPosition(position);
    // 太阳塔需要特殊缩放
    if (index == 1) tower->setScale(0.8);
    my_scene->addChild(tower);
    sprite_mark = tower;
    
    // 记录位置和初始化攻击间隔
    pos = position;
    interval = 0;
}

// 销毁塔的方法 - 从场景中移除塔并返还金币
void Tower::destroy(BaseLevelScene* my_scene)
{
    // 从场景中移除塔的精灵
    my_scene->removeChild(sprite_mark);
    my_scene->removeChild(sp_base);
    // 释放精灵资源
    sprite_mark->release();
    sp_base->release();
    // 返还金币
    my_scene->updateMoney(sell_money[index][grade]);
    // 播放出售音效
    Music::getInstance()->TowerSell();
}

// 升级塔的方法 - 提升塔的等级并更新属性
void Tower::update(BaseLevelScene* my_scene, Vec2 position)
{
    // 检查是否可以升级
    if (grade >= 2 || my_scene->getMoney() < up_cost[index][grade]) return;
    
    // 扣除升级费用
    my_scene->updateMoney(-up_cost[index][grade]);
    
    // 移除旧塔身
    my_scene->removeChild(sprite_mark);
    sprite_mark->release();
    
    // 提升等级
    grade += 1;
    
    // 创建新塔身
    auto tower = Sprite::create(tower_table[index][grade]);
    if (index == 1) tower->setScale(0.8); // 太阳塔特殊缩放
    tower->setAnchorPoint(anchorpoint_table[index][1]);
    tower->setPosition(position);
    my_scene->addChild(tower);
    sprite_mark = tower;
    
    // 更新塔的属性
    range = range_table[index][grade];
    demage = demage_table[index][grade];
    
    // 播放升级音效
    Music::getInstance()->TowerUpdata();
}

// 显示升级菜单的方法 - 在塔周围显示升级和出售按钮
void Tower::UpMenuAppear(BaseLevelScene* my_scene, Vec2& position)
{
    // 根据状态创建升级按钮
    Sprite* up;
    if (grade < 2 && my_scene->getMoney() >= up_cost[index][grade]) 
        up = Sprite::create(up_graph[up_cost[index][grade]]); // 可升级
    else if (grade < 2) 
        up = Sprite::create(noup_graph[up_cost[index][grade]]); // 金币不足
    else 
        up = Sprite::create("Towers/upgrade_top.png"); // 已满级
    
    // 创建出售按钮和攻击范围显示
    auto sale = Sprite::create(sale_graph[sell_money[index][grade]]);
    auto range = Sprite::create("Towers/range.png");
    
    // 设置缩放比例
    range->setScale(range_table[index][grade] / range->getContentSize().width);
    up->setScale(CELL_SIZE / up->getContentSize().width);
    sale->setScale(CELL_SIZE / sale->getContentSize().width);
    
    // 设置位置
    range->setPosition(position);
    up->setPosition(Vec2(position.x, position.y + CELL_SIZE));
    sale->setPosition(Vec2(position.x, position.y - CELL_SIZE));
    
    // 添加到场景并保存引用
    my_scene->addChild(up);
    my_scene->addChild(sale);
    my_scene->addChild(range);
    curr_up = up; curr_sale = sale; curr_range = range;
}

// 隐藏升级菜单的方法 - 移除升级和出售按钮
void Tower::UpMenuGone(BaseLevelScene* my_scene)
{
    // 从场景中移除UI元素
    my_scene->removeChild(curr_up);
    my_scene->removeChild(curr_sale);
    my_scene->removeChild(curr_range);
    
    // 释放资源
    curr_up->release();
    curr_sale->release();
    curr_range->release();
}