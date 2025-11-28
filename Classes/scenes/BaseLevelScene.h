#pragma once
#ifndef __BASE_H__
#define __BASE_H__
#include "cocos2d.h"
#include<string>
#include<map>
#include<vector>
#include<memory>
#include"entities/Monster/Monster.h"
#include"entities/Tower/Tower.h"
#include"Obstacle.h"
#include"core/state/GameState.h"
#include"core/state/PausedState.h"
#include"core/state/MenuState.h"
#include"core/state/LevelSelectState.h"
#include "core/EventBusProvider.h"

USING_NS_CC;

#define Y 9
#define X 15

namespace carrot {
namespace core {
class Subscription;
}  // namespace core
}  // namespace carrot

//������ʾ��ͼ������Ϣ�Ľṹ��
struct Cell {
    char flag;
    int key;
};
namespace carrot {
namespace ui {
namespace widgets {
class MoneyHud;
}
}
}

class BaseLevelScene : public cocos2d::Scene {
private:
    cocos2d::Node* plantsLayer;                              //ֲ��ͼ��
    TMXLayer* plantableLayer = nullptr;                      // plantable ��
    static const std::vector<std::string> mapFiles;          // �洢��ͼ�ļ���·��
    int levelId;                                             //�ؿ����
    //��ǩ���
    carrot::ui::widgets::MoneyHud* moneyHud;                 //金钱HUD
    Label* _numberLabel;                                     // ��ʾ���ﲨ��
    Label* _curNumberLabel;                                  // ��ʾ��ǰ���ﲨ��
    //�ٶȿ������
    bool isDoubleSpeed = false;                              // ��ʼ��Ϊfalse����ʾ��Ϸ��ʼ���Ƕ�����״̬
    bool isGamePaused = false;
    bool hasGameOverTriggered = false;
    std::shared_ptr<carrot::core::Subscription> gameWonSubscription;
    std::shared_ptr<carrot::core::Subscription> gameLostSubscription;
    std::shared_ptr<carrot::core::Subscription> monsterDiedSubscription;
    std::shared_ptr<carrot::core::Subscription> spawnEffectSubscription;
    std::shared_ptr<carrot::core::Subscription> carrotShakeSubscription;
    std::shared_ptr<carrot::core::Subscription> monsterSpawnSubscription;


    //�������
    std::vector<Sprite*>remove_table;                        //removeʱ�ķ���
    Sprite* curr_cell;                                       //��ֲ�ķ���
    std::vector<std::string>buy_tower[2];                    //��������Ĺ���ͼ
    std::vector<int>index_table;                             //��¼�����õ������������
    int cell_flag;       
    Vec2 last_position;                                      //��¼��һ�ε�λ��

    Scheduler* scheduler = Director::getInstance()->getScheduler();
    float time_total=0;                                      //�����ܲ�����
public:
    std::map<int, Tower*> towers;                            //��¼���ϵ�������Ϣ
    std::map<int, Obstacle*> Obstacles;                      //�ϰ�������
    Monster* tar_m = nullptr;
    Obstacle* tar_o = nullptr;                               //ָ�򱻱�ǵĶ���
    char isTarget = 0;                                       //0����û�б���ǵģ�1�������ﱻ��ǣ�2�����ϰ��ﱻ���
    Cell map_data[X][Y];                                     //��ͼ����
    Sprite* lock;                                          //������־����Ϊͬһʱ��ֻ����һ���������������Խ������BaseLevelScene��
    int getMoney()const;
    virtual void update(float deltaTime) override;
    cocos2d::TMXTiledMap* tileMap = nullptr;                // ��ͼ����
    cocos2d::Size tileSize;                                 //ÿ����Ƭ�Ĵ�С

    //��ʼ������
    static cocos2d::Scene* createScene(int level);          // ��������ʱ����ؿ����
    virtual bool init();                                    // Ĭ�ϵ� init() ����
    void initUI();                                          //��ʼ��ui
    bool initWithLevel(int level);                          //��д��init����
    CREATE_FUNC(BaseLevelScene);
    void loadMap();                                         // ���ص�ͼ�ĺ���
    void addMouseListener();                                // ������������
    void InitMapData();                                     //��ʼ����ͼ��Ϣ
    void ProduceObstacles();                                //�����ϰ���
    //�������
    void handleMouseDown(EventMouse* event);                //�����ʱ���ж��Ƿ���ֲ
    void handlePlant(const Vec2& position);                 // ��ֲ�ж�
    void PlantMenuAppear(Vec2 mapPos);                      //������ֲ�˵�
    void PlantMenuGone(Vec2 position);                      //��ֲ�˵���ʧ
    void drawGrid();                                        //��������
    void UpMenuAppear(Vec2& position);                      //�����˵�����
    void UpMenuGone(Vec2& position);                        //�����˵���ʧ
    void saveTowerData();
    bool loadTowerData(const std::string& filename);
    Vec2 gridToScreenCenter(const Vec2& gridPoint);         //����������ת����Ļ����Ĺ��ߺ���

    //��ť
    void updateMoney(int add);                                   //����money��addΪ�仯��
    void doublespeed(Ref* pSender);
    void pause_all(Ref* pSender);
    void menu_all(Ref* pSender);
    void updateCurrentWaveLabe();
    void wenhao(Ref* pSender);
    void CountDown(std::function<void()> onComplete);            //����ʱ
    void gameover(bool is_win,int currentWaveNum,int allWaveNum);//������Ϸ
    void Jineng1(Ref* pSender);                                  //�ܲ�Ѫ����1
    void Jineng2(Ref* pSender);                                  //����������ֹͣ
    void Jineng3(Ref* pSender);                                  //ը�����������й���
    void Jineng4(Ref* pSender);                                  //ֲ�﹥�ټӱ�
    void Jineng5(Ref* pSender);                                  //���޼���
    void Jineng6(Ref* pSender);                                  //�ܲ�޵�״̬
    void guaisou_jiansu(float guai_jiansu);

    void saveGameState();                                        //ͨ������浵
    Monster* IsTargetMonster(const Vec2& pos);                   //����Ƿ��й��ﱻ����
    void transitionToMenuState();
    void transitionToLevelSelectState();
    void registerOutcomeListeners();
    void registerMonsterListeners();
    void registerSpawnEffectListeners();
    void registerCarrotShakeListeners();
    void registerMonsterSpawnListeners();
   
};
#endif