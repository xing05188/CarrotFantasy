#pragma once

#include "cocos2d.h"
#include <string>
#include <fstream>

USING_NS_CC;

// 前向声明
class BaseLevelScene;
class GameManager;

/**
 * 存储服务类
 * 负责游戏数据的保存和加载
 */
class StorageService {
public:
    static StorageService* getInstance();
    static void destroyInstance();
    
    // 禁用拷贝和赋值
    StorageService(const StorageService&) = delete;
    StorageService& operator=(const StorageService&) = delete;
    
    /**
     * 保存关卡状态（关卡是否通关）
     * @return 是否保存成功
     */
    bool saveGameState();
    
    /**
     * 保存塔和障碍物数据
     * @param scene 当前场景
     * @param levelId 关卡ID
     * @param money 当前金钱
     * @return 是否保存成功
     */
    bool saveTowerData(BaseLevelScene* scene, int levelId, int money);
    
    /**
     * 加载塔和障碍物数据
     * @param scene 当前场景
     * @param filename 文件名
     * @param moneyCallback 设置金钱的回调函数 (money) -> void
     * @return 是否加载成功
     */
    bool loadTowerData(BaseLevelScene* scene, const std::string& filename, 
                       std::function<void(int)> moneyCallback);

private:
    StorageService() = default;
    ~StorageService() = default;
    
    static StorageService* instance;
    
    /**
     * 获取可写路径
     */
    std::string getWritablePath() const;
    
    /**
     * 写入JSON文件
     * @param filePath 文件路径
     * @param jsonContent JSON内容
     * @return 是否写入成功
     */
    bool writeJsonFile(const std::string& filePath, const std::string& jsonContent) const;
    
    /**
     * 读取JSON文件
     * @param filePath 文件路径
     * @return JSON内容，失败返回空字符串
     */
    std::string readJsonFile(const std::string& filePath) const;
};

