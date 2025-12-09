/**
 * Refactored with Facade Pattern
 * 使用外观模式重构
 * 
 * Design Pattern: Facade Pattern
 * 设计模式：外观模式
 * 
 * Benefits:
 * 好处：
 * - Provides a simplified, unified interface to the complex subsystem of game management
 *   为游戏管理复杂子系统提供简化、统一的接口
 * - Decouples clients from the internal implementation details of subsystems
 *   将客户端与子系统的内部实现细节解耦
 * - Promotes loose coupling between subsystems by centralizing their coordination
 *   通过集中协调子系统间的交互来促进松耦合
 * - Simplifies the overall architecture by providing a clear entry point to all game operations
 *   通过为所有游戏操作提供清晰的入口点来简化整体架构
 * - Enables easier maintenance and evolution of subsystems without affecting client code
 *   使子系统的维护和演进更容易，而不影响客户端代码
 * - Facilitates testing by providing a single point of control for mocking subsystems
 *   通过提供单一控制点来模拟子系统，便于测试
 * 
 * Problems Solved:
 * 解决的问题：
 * - Eliminates the need for clients to understand and interact with multiple complex subsystems directly
 *   消除了客户端需要直接理解和交互多个复杂子系统的需求
 * - Reduces the complexity of client code by providing a high-level interface to subsystem operations
 *   通过为子系统操作提供高级接口，降低了客户端代码的复杂性
 * - Prevents tight coupling between clients and individual subsystem components
 *   防止客户端与单个子系统组件之间的紧耦合
 * - Centralizes subsystem coordination logic that was previously scattered across multiple components
 *   将先前分散在多个组件中的子系统协调逻辑集中化
 * - Provides a clear separation of concerns between high-level game logic and low-level subsystem operations
 *   在高级游戏逻辑和低级子系统操作之间提供清晰的关注点分离
 * - Enables easier refactoring of subsystems without breaking client code
 *   使子系统的重构更容易，而不会破坏客户端代码
 * 
 * Subsystems Managed:
 * 管理的子系统：
 * - MonsterManager: Handles all monster-related operations (spawning, movement, lifecycle)
 *   MonsterManager：处理所有怪物相关操作（生成、移动、生命周期）
 * - CarrotManager: Manages carrot entities and their state
 *   CarrotManager：管理萝卜实体及其状态
 * - PathManager: Controls monster movement paths and navigation
 *   PathManager：控制怪物移动路径和导航
 * - GameSaveLoader: Handles game state persistence and loading
 *   GameSaveLoader：处理游戏状态持久化和加载
 * - GameStateChecker: Monitors and evaluates game win/lose conditions
 *   GameStateChecker：监控和评估游戏胜利/失败条件
 * - MoneySystem: Manages in-game currency and transactions
 *   MoneySystem：管理游戏内货币和交易
 */
 // Refactored with Manager Pattern
 // 使用管理者模式重构
#pragma once

#include "cocos2d.h"
#include "entities/Monster/Monster.h"
#include "subsystem/MonsterTypes.h"
#include "subsystem/MonsterManager.h"
#include "subsystem/PathManager.h"
#include "subsystem/CarrotManager.h"
#include "subsystem/GameSaveLoader.h"
#include "subsystem/GameStateChecker.h"
#include "subsystem/MoneySystem.h"
#include<vector>
#include<memory>
#include<string>
#include"BaseLevelScene.h"
#include<array>
#include"entities/Carrot/Carrot.h"
USING_NS_CC;

class GameManager {
private:
    friend class MonsterManager;
    friend class CarrotManager;
    friend class GameSaveLoader;
    friend class GameStateChecker;
    // Ŀ��λ�õ����飬����levelId��1��2��3�ĳ����ؿ�
    std::vector<cocos2d::Vec2> dst1 = { Vec2(804, 444), Vec2(826, 430), Vec2(831, 353) }; //萝卜位置
    std::vector<cocos2d::Vec2> dst2 = { Vec2(854, 444), Vec2(886, 430), Vec2(881, 353) }; //血条位置
    //���������
    static GameManager* instance;                                        // ����ָ��
    BaseLevelScene* currentScene;                                        // ��ǰ�󶨵ĳ���
    GameManager();                                   // ˽�л����캯������ֹ�ⲿʵ����                                   
    //����
    static constexpr int kDefaultStartingMoney = 1000;
    //·��
    //·��
    std::unique_ptr<PathManager> pathManager;
    //�ؿ�
    int levelId;                                             //�ؿ����
    //�¼�������
    cocos2d::EventListenerCustom* _listener;                 //���ڼ����޵����յ���¼�
    std::unique_ptr<MonsterManager> monsterManager;
    std::unique_ptr<CarrotManager> carrotManager;
    std::unique_ptr<GameSaveLoader> gameSaveLoader;
    std::unique_ptr<GameStateChecker> gameStateChecker;
    std::unique_ptr<MoneySystem> moneySystem;
public:
    void stopAllSchedulers();                                //ֹͣmanager�����е�����
    GameManager(const GameManager&) = delete;               
    GameManager& operator=(const GameManager&) = delete;     // ���ÿ����͸�ֵ
    static GameManager* getInstance(BaseLevelScene* scene = nullptr);                       // ������ȡ����
    void setScene(BaseLevelScene* scene);                    //���õ�ǰ����
    BaseLevelScene* getScene() const;                        // ��ȡ��ǰ����
    static void destroyInstance();                           //�ͷ�ʵ��
    //����
    void update(float deltaTime);
    bool CheckLose();                                        //�����״̬
    bool CheckWin();                                         //���Ӯ״̬
    //�ؿ�
    void initLevel(int level, bool ReadMode);                // ��ʼ���ؿ�����
    void initPath();                                         // ��ʼ��·��
    // �������
    void loadMonsterWaveConfig(const std::string& filename, const std::string& levelName);//���ع��޲�����
    void loadMonsterResources();                             //���ع�����Դ
    void produceMonsters(const std::string monsterName,
        const int startIndex,int health,bool pause=false);
                                                             //�������ֺͳ�ʼ���ݲ�������
    void produceMonsterWave(const WaveConfig& waveConfig);   //�������޲�
    void startMonsterWaves();                                //��ʼ���޲�
    void playSpawnEffect(const cocos2d::Vec2& spawnPosition);//���޳�����Ч
    void ClearMonsters();                                    //������й����ڴ�
    int getCurrentWaveIndex() const;       //��ȡ��ǰ���޲����
    int getAllWaveNum()const;          //��ȡ�ܲ���
    int getCurrentWaveNum()const;       //��ȡ�ֲ���
    int getAllMonsterNum()const;
    // 提供对怪物容器的只读引用，封装内部成员
    std::vector<Monster*>& GetMonsters();
    // 全局控制：给所有存活怪物应用一个速度倍率
    void ApplyMonsterSpeed(float speedFactor);
    // 技能 / 调试：立刻杀死所有存活怪物（用于清场炸弹）
    void KillAllMonsters();
  //�����浵���
    bool loadGameData(const std::string& fileName);          //���س�ʼ��Ϸ����
    bool loadPathForLevel(int levelId, const std::string& filePath);
                                                             //���ص�ͼ����
    void saveMonstersDataToJson(const std::string& fileName);//�洢��������
    Vec2 gridToScreenCenter(const Vec2& gridPoint);
    const std::vector<cocos2d::Vec2>& GetScreenPath() const;
    const std::vector<cocos2d::Vec2>& GetPath() const;
    int getLevelId() const;
    // 存档/读档功能（通过 StorageService 实现）
    void saveGameState();                                        //保存关卡状态
    void saveTowerData();                                        //保存塔和障碍物数据
    bool loadTowerData(const std::string& filename);             //加载塔和障碍物数据
 //ʱ�����
    void registerListener();                                 // ע���¼�������
    void removeListener() ;                                  // �Ƴ��¼�������
    void onMonsterPathComplete(cocos2d::EventCustom* event); // �¼��ص�
    void Jineng1();
    void Jineng6();
    Carrot* getCarrot() const;
    void doudong();
    int GetMoney() const;
    void ChangeMoney(int delta);
    void SetMoney(int value, bool publishEvent = true);
};