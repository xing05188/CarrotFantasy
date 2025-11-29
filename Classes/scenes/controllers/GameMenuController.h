#pragma once
#ifndef __GAME_MENU_CONTROLLER_H__
#define __GAME_MENU_CONTROLLER_H__

#include "cocos2d.h"
#include <functional>
#include <string>

USING_NS_CC;

// 前向声明
class BaseLevelScene;
class GameManager;

/**
 * 游戏菜单控制器
 * 负责处理游戏内菜单相关的逻辑，从 BaseLevelScene 中拆分出来
 */
class GameMenuController {
public:
    // 菜单回调接口，用于访问 BaseLevelScene 的方法
    struct MenuCallbacks {
        std::function<void(Node*)> addChildToScene;           // 向场景添加节点
        std::function<void(const std::string&)> removeChildByName;  // 通过名称移除子节点
        std::function<void(Node*)> removeChild;               // 移除子节点
        std::function<void()> pauseDirector;                  // 暂停 Director
        std::function<void()> resumeDirector;                 // 恢复 Director
        std::function<void(Scene*)> replaceScene;             // 替换场景
        std::function<void()> saveGameState;                  // 保存游戏状态
        std::function<void()> saveTowerData;                  // 保存塔数据
        std::function<void()> transitionToLevelSelectState;   // 切换到关卡选择状态
        std::function<Scene*(int)> createLevelScene;          // 创建关卡场景
        std::function<int()> getLevelId;                      // 获取关卡ID
        std::function<bool()> isGamePaused;                   // 获取暂停状态
        std::function<void()> stopAllSchedulers;              // 停止所有调度器
        std::function<void()> removeListener;                 // 移除监听器
        std::function<void(const std::string&)> saveMonstersData;  // 保存怪物数据
        std::function<void(float)> setTimeScale;              // 设置时间缩放
    };

    GameMenuController();
    ~GameMenuController();

    /**
     * 初始化菜单控制器
     * @param callbacks 回调接口，用于访问场景相关方法
     */
    void init(const MenuCallbacks& callbacks);

    // 菜单方法
    void showGameMenu(Ref* pSender);      // 显示游戏菜单（继续、重启、选择关卡）
    void showHelpMenu(Ref* pSender);      // 显示帮助/说明界面

private:
    MenuCallbacks callbacks_;
    bool initialized_;

    // 创建帮助界面的 PageView
    void createHelpPageView(Node* parentLayer, const Size& screenSize);
};

#endif // __GAME_MENU_CONTROLLER_H__

