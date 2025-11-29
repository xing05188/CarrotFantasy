#pragma once
#ifndef __GAME_SPEED_CONTROLLER_H__
#define __GAME_SPEED_CONTROLLER_H__

#include "cocos2d.h"

USING_NS_CC;

/**
 * 游戏速度控制器
 * 负责处理游戏速度切换相关的逻辑，从 BaseLevelScene 中拆分出来
 */
class GameSpeedController {
public:
    // 速度回调接口，用于访问 BaseLevelScene 的方法
    struct SpeedCallbacks {
        std::function<void(float)> applyMonsterSpeed;     // 应用怪物速度倍率
    };

    GameSpeedController();
    ~GameSpeedController();

    /**
     * 初始化速度控制器
     * @param callbacks 回调接口，用于访问场景相关方法
     */
    void init(const SpeedCallbacks& callbacks);

    /**
     * 切换游戏速度（1倍/2倍）
     * @param pSender 按钮对象
     */
    void toggleSpeed(Ref* pSender);

    /**
     * 获取当前速度状态
     * @return true 表示 2倍速，false 表示 1倍速
     */
    bool isDoubleSpeed() const { return isDoubleSpeed_; }

private:
    SpeedCallbacks callbacks_;
    bool initialized_;
    bool isDoubleSpeed_;  // 当前速度状态：false=1倍速, true=2倍速

    // 设置速度倍率（内部辅助方法）
    void setSpeedMultiplier(float multiplier, MenuItemImage* button);
};

#endif // __GAME_SPEED_CONTROLLER_H__

