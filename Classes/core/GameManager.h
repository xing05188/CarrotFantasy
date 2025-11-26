#pragma once

#include "cocos2d.h"
#include "Monster.h"
#include<vector>
#include<map>
#include<string>
#include"BaseLevelScene.h"
#include<array>
#include"Carrot.h"
USING_NS_CC;

//���޲�����Ϣ
struct WaveConfig {
    int wave;
    std::string monsterName;
    int count;
    std::array<float, 2> spawnInterval={1,2};  // ���ɼ���ķ�Χ [0.0, 2.0] ֮��
};
//һ�����Ĺ�����Ŀ
const int MAX_MONSTER_NUM=150;

class GameManager {
private:
    // Ŀ��λ�õ����飬����levelId��1��2��3�ĳ����ؿ�
    std::vector<cocos2d::Vec2> dst1 = { Vec2(804, 640 - 196), Vec2(826, 640 - 210), Vec2(831, 640 - 287) };
    std::vector<cocos2d::Vec2> dst2 = { Vec2(854, 640 - 196), Vec2(886, 640 - 210), Vec2(881, 640 - 287) };
    //���������
    static GameManager* instance;                                        // ����ָ��
    BaseLevelScene* currentScene;                                        // ��ǰ�󶨵ĳ���
    GameManager() : currentScene(nullptr), monsters(MAX_MONSTER_NUM) {}  // ˽�л����캯������ֹ�ⲿʵ����                                   
    //�ܲ����
    Carrot* carrot;                                          // Carrot����
    void initCarrot();                                       //ÿ�س�ʼ���ܲ�
    //����
    int AllMonsterNum=0;                                    // ��������
    std::vector<WaveConfig> waveConfigs;                    //�洢���޲�
    int waveIndex = 0;                                      //��ǰ����
    int AllWaveNum=0;
    static constexpr int kDefaultStartingMoney = 1000;
    int money = kDefaultStartingMoney;
    bool hasGameWon = false;
    bool hasGameLost = false;
    //·��
    std::map<int, std::vector<cocos2d::Vec2>>pathsCache;     //�洢�Ѿ����ع��Ĺؿ�������·��
    std::map<int, std::vector<cocos2d::Vec2>>ScreenPaths;    //�洢�Ѿ����ع��Ĺؿ�����Ļ·��
    std::vector<cocos2d::Vec2> path;                         //��ǰ��Ļ����·��
    std::vector<cocos2d::Vec2> screenPath;                   //��ǰ��Ļ·��
    //�ؿ�
    int levelId;                                             //�ؿ����
    //�¼�������
    cocos2d::EventListenerCustom* _listener;                 //���ڼ����޵����յ���¼�
    // �����б�（只允许通过接口对外暴露）
    std::vector<Monster*> monsters;
public:
    void stopAllSchedulers();                                //ֹͣmanager�����е�����
    GameManager(const GameManager&) = delete;               
    GameManager& operator=(const GameManager&) = delete;     // ���ÿ����͸�ֵ
    static GameManager* getInstance
    (BaseLevelScene* scene = nullptr);                       // ������ȡ����
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
    void loadMonsterWaveConfig
    (const std::string& filename, const std::string& levelName);//���ع��޲�����
    void loadMonsterResources();                             //���ع�����Դ
    void produceMonsters(const std::string monsterName,
        const int startIndex,int health,bool pause=false);
                                                             //�������ֺͳ�ʼ���ݲ�������
    void produceMonsterWave(const WaveConfig& waveConfig);   //�������޲�
    void startMonsterWaves();                                //��ʼ���޲�
    void playSpawnEffect(const cocos2d::Vec2& spawnPosition);//���޳�����Ч
    void ClearMonsters();                                    //������й����ڴ�
    int getCurrentWaveIndex() const{return waveIndex;}       //��ȡ��ǰ���޲����
    int getAllWaveNum()const { return AllWaveNum; }          //��ȡ�ܲ���
    int getCurrentWaveNum()const { return waveIndex; }       //��ȡ�ֲ���
    int getAllMonsterNum()const{return AllMonsterNum;}
    // 提供对怪物容器的只读引用，封装内部成员
    std::vector<Monster*>& GetMonsters() { return monsters; }
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
 //ʱ�����
    void registerListener();                                 // ע���¼�������
    void removeListener() ;                                  // �Ƴ��¼�������
    void onMonsterPathComplete(cocos2d::EventCustom* event); // �¼��ص�
    void Jineng1();
    Carrot* getCarrot() const { return carrot; }
    void doudong();
    int GetMoney() const { return money; }
    void ChangeMoney(int delta);
    void SetMoney(int value, bool publishEvent = true);
private:
    void PublishMoneyChangedEvent(int delta);
    void PublishGameWonEvent();
    void PublishGameLostEvent();
};