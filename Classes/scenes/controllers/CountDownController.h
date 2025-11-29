#pragma once
#ifndef __COUNT_DOWN_CONTROLLER_H__
#define __COUNT_DOWN_CONTROLLER_H__

#include "cocos2d.h"
#include <functional>

USING_NS_CC;

/**
 * 倒计时控制器
 * 负责处理游戏开始倒计时相关的逻辑，从 BaseLevelScene 中拆分出来
 */
class CountDownController {
public:
    // 倒计时回调接口，用于访问 BaseLevelScene 的方法
    struct CountDownCallbacks {
        std::function<void(Node*)> addChildToScene;       // 向场景添加节点
        std::function<void(Node*)> removeChild;           // 移除子节点
        std::function<void(Action*)> runActionOnScene;     // 在场景上运行动作
    };

    CountDownController();
    ~CountDownController();

    /**
     * 初始化倒计时控制器
     * @param callbacks 回调接口，用于访问场景相关方法
     */
    void init(const CountDownCallbacks& callbacks);

    /**
     * 开始倒计时（3、2、1、GO）
     * @param onComplete 倒计时完成后的回调
     */
    void startCountDown(std::function<void()> onComplete);

private:
    CountDownCallbacks callbacks_;
    bool initialized_;
};

#endif // __COUNT_DOWN_CONTROLLER_H__

