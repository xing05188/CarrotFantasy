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