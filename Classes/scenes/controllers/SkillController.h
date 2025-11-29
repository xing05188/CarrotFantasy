#pragma once
#ifndef __SKILL_CONTROLLER_H__
#define __SKILL_CONTROLLER_H__

#include "cocos2d.h"
#include <functional>

USING_NS_CC;

// 前向声明
class BaseLevelScene;
class GameManager;

/**
 * 技能控制器
 * 负责处理所有技能相关的逻辑，从 BaseLevelScene 中拆分出来
 */
class SkillController {
public:
    // 技能回调接口，用于访问 BaseLevelScene 的方法
    struct SkillCallbacks {
        std::function<int()> getMoney;                    // 获取当前金币
        std::function<void(int)> updateMoney;             // 更新金币
        std::function<void(float)> applyMonsterSpeed;     // 应用怪物速度倍率
        std::function<void(Node*)> addChildToScene;       // 向场景添加节点
        std::function<void(Action*)> runActionOnScene;    // 在场景上运行动作
    };

    SkillController();
    ~SkillController();

    /**
     * 初始化技能控制器
     * @param callbacks 回调接口，用于访问场景相关方法
     * @param gameManager GameManager 实例
     */
    void init(const SkillCallbacks& callbacks, GameManager* gameManager);

    // 技能方法
    void executeSkill1(Ref* pSender);  // 萝卜回血技能1
    void executeSkill2(Ref* pSender);  // 怪物减速停止
    void executeSkill3(Ref* pSender);  // 爆炸杀死所有怪物
    void executeSkill4(Ref* pSender);  // 植物攻速加倍
    void executeSkill5(Ref* pSender);  // 怪物减速
    void executeSkill6(Ref* pSender);  // 萝卜无敌状态

private:
    SkillCallbacks callbacks_;
    GameManager* gameManager_;
    bool initialized_;

    // 应用怪物速度倍率（内部辅助方法）
    void applyMonsterSpeedInternal(float speedMultiplier);
};

#endif // __SKILL_CONTROLLER_H__

