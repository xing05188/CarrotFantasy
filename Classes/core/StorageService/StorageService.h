/**
 * Refactored with Provider Pattern
 * 使用提供者模式重构
 * 
 * Design Pattern: Provider Pattern
 * 设计模式：提供者模式
 * 
 * Benefits:
 * 1. Provides a centralized access point to storage functionality
 * 2. Decouples storage consumers from storage implementation details
 * 3. Enables easier testing by allowing mock providers to be injected
 * 4. Facilitates consistent data persistence across different parts of the application
 * 
 * 好处：
 * 1. 提供存储功能的集中访问点
 * 2. 将存储消费者与存储实现细节解耦
 * 3. 通过允许注入模拟提供者来简化测试
 * 4. 促进应用程序不同部分之间的一致数据持久化
 * 
 * Problems Solved:
 * 1. Eliminates the need for multiple components to directly access storage mechanisms
 * 2. Reduces coupling between components and the storage management system
 * 3. Provides a consistent interface for data storage and retrieval
 * 4. Makes it easier to change the underlying storage implementation without affecting consumers
 * 
 * 解决的问题：
 * 1. 消除了多个组件直接访问存储机制的需要
 * 2. 减少了组件与存储管理系统之间的耦合
 * 3. 为数据存储和检索提供了一致的接口
 * 4. 使更改底层存储实现而不影响消费者变得更容易
 */
 // Refactored with Provider Pattern
 // 使用提供者模式重构
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

