#include"GameManager.h"
#include<vector>
#include "json/document.h"
#include "json/rapidjson.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include"Music.h"
#include "EventBusProvider.h"
#include "../gameplay/events/MonsterEvents.h"
#include "../entities/Tower/TowerFactory.h"
#include "../entities/Obstacle/Obstacle.h"

USING_NS_CC;
extern int DeadCount;

// 单例实例指针（全局唯一 GameManager）
GameManager* GameManager::instance = nullptr;

GameManager::GameManager()
    : currentScene(nullptr),
      pathManager(std::make_unique<PathManager>()),
      monsterManager(std::make_unique<MonsterManager>(this)),
      carrotManager(std::make_unique<CarrotManager>(this)),
      gameSaveLoader(std::make_unique<GameSaveLoader>(this)),
      gameStateChecker(std::make_unique<GameStateChecker>(this)),
      moneySystem(std::make_unique<MoneySystem>(this)) {
    if (moneySystem) {
        moneySystem->SetMoney(kDefaultStartingMoney, false);
    }
}

// 获取 GameManager 单例；首次调用时创建实例，并可顺便绑定当前场景
GameManager* GameManager::getInstance(BaseLevelScene* scene) {
    if (!instance) {
        instance = new GameManager();
    }
    if (scene) {
        instance->setScene(scene);
    }
    return instance;
}
// 绑定当前正在使用的关卡场景（用于添加节点、取 tileMap 等）
void GameManager::setScene(BaseLevelScene* scene) {
    currentScene = scene;
}
// 获取当前绑定的场景指针
BaseLevelScene* GameManager::getScene() const {
    return currentScene;
}
// 释放单例，占用完毕后在退出游戏时可调用
void GameManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

// 预留的逐帧更新接口，目前仅打印调试信息
void GameManager::update(float deltaTime) {
    CCLOG("monsters size %d", static_cast<int>(monsterManager->GetMonsters().size()));
    CCLOG("monsters DEAD NUM %d", DeadCount);
}
// 判断失败条件并仅发布一次游戏失败事件，供 UI 监听
bool GameManager::CheckLose()
{
    return gameStateChecker ? gameStateChecker->CheckLose() : false;
}
// 判断胜利条件（全部波次完成且怪物清空）并发布胜利事件
bool GameManager::CheckWin()
{
    return gameStateChecker ? gameStateChecker->CheckWin() : false;
}
void GameManager::ApplyMonsterSpeed(float speedFactor) {
    monsterManager->ApplyMonsterSpeed(speedFactor);
}

void GameManager::KillAllMonsters() {
    monsterManager->KillAllMonsters();
}

// 初始化关卡的路径、波次、金币与胜负标记，并加载相关资源
void GameManager::initLevel(int level,bool initMode)
{
    levelId=level;
    pathManager->ResetCurrentPath();
    monsterManager->ResetForLevel();
    DeadCount=0;
    // 只有在非读档模式（新游戏）时才重置金钱，读档模式下金钱会从存档中恢复
    CCLOG("GameManager::initLevel: level=%d, initMode=%s, current money=%d", 
          level, initMode ? "true (load)" : "false (new)", GetMoney());
    if (!initMode) {
        CCLOG("GameManager::initLevel: Resetting money to default: %d", kDefaultStartingMoney);
        SetMoney(kDefaultStartingMoney, false);
    } else {
        CCLOG("GameManager::initLevel: Load mode - keeping current money: %d (will be loaded from file)", GetMoney());
    }
    if (gameStateChecker) {
        gameStateChecker->Reset();
    }
    initPath();
    loadMonsterResources();
    carrotManager->InitCarrot();
     registerListener();
     
     if(!initMode)
     {
    loadMonsterWaveConfig("MonsterWaves.json", "level"+std::to_string(level));
     }
     else
     {
        if (!loadGameData("level" + std::to_string(levelId) + "Monster.json"))
        {
            loadMonsterWaveConfig("MonsterWaves.json", "level" + std::to_string(level));
        }
     }
}
void GameManager::initPath()
{
    pathManager->InitPath(levelId, currentScene);
}
// 从 JSON 配置中读取某一关卡的路径数据，并缓存到 pathsCache / ScreenPaths
bool GameManager::loadPathForLevel(int levelId, const std::string& filePath)
{
    return pathManager->LoadPathForLevel(levelId, filePath, currentScene);
}
// 预加载所有怪物相关的帧动画资源，避免战斗过程中卡顿
void GameManager::loadMonsterResources() {
    monsterManager->LoadMonsterResources();
}
// 创建单个怪物并加入场景，可用于正常刷怪或读档恢复
void GameManager::produceMonsters(const std::string monsterName, const int startIndex, int health, bool pause) {
    monsterManager->ProduceMonsters(monsterName, startIndex, health, pause);
}
// 从 JSON 文件中读取某个关卡的全部波次配置
void GameManager::loadMonsterWaveConfig(const std::string& filename, const std::string& levelName) {
    monsterManager->LoadMonsterWaveConfig(filename, levelName);
}
// 按照一条波次配置，利用调度器在一段时间内依次生成该波全部怪物
void GameManager::produceMonsterWave(const WaveConfig& waveConfig) {
    monsterManager->ProduceMonsterWave(waveConfig);
}
// 入口：开始整局战斗的刷怪流程，会按固定间隔推进到下一波
void GameManager::startMonsterWaves() {
    monsterManager->StartMonsterWaves();
}
void GameManager::playSpawnEffect(const cocos2d::Vec2& spawnPosition) {
    auto bus = carrot::core::EventBusProvider::Get();
    if (!bus) {
        CCLOG("GameManager: EventBus not available, spawn effect event not published");
        return;
    }
    carrot::gameplay::events::SpawnEffectRequestedEvent evt{};
    evt.x = spawnPosition.x;
    evt.y = spawnPosition.y;
    bus->Publish(carrot::gameplay::events::kSpawnEffectRequestedEventId, evt);
}
// 处理“怪物走到终点”的自定义事件：对萝卜扣血并销毁怪物
void GameManager::onMonsterPathComplete(cocos2d::EventCustom* event)
{
    Monster* monster = static_cast<Monster*>(event->getUserData());

    if (monster) {
        CCLOG("Monster has completed the path. Perform further actions here.");
        auto* carrot = getCarrot();
        if (monster->getHealth() > 0 && carrot)
        {
            monster->setHealth(0);
            carrot->getDamage(monster->getDamage());
            CCLOG("carrot'HP    %d", carrot->getHP());
        }
        auto bus = carrot::core::EventBusProvider::Get();
        if (bus) {
            carrot::gameplay::events::MonsterDiedEvent evt{};
            evt.monster = monster;
            bus->Publish(carrot::gameplay::events::kMonsterDiedEventId, evt);
        }
    }
}
// 清理当前所有怪物节点及其动作，用于重新开始或退出关卡
void GameManager::ClearMonsters()
{
    monsterManager->ClearMonsters();
}

bool GameManager::loadGameData(const std::string& fileName) {
    if (!gameSaveLoader) {
        return false;
    }
    return gameSaveLoader->LoadGameData(fileName);
}
void GameManager::saveMonstersDataToJson(const std::string& fileName) {
    if (gameSaveLoader) {
        gameSaveLoader->SaveMonstersDataToJson(fileName);
    }
}
void GameManager::doudong() {
    if (carrotManager) {
        carrotManager->Doudong();
    }
}
int GameManager::GetMoney() const {
    return moneySystem ? moneySystem->GetMoney() : 0;
}
// 所有加减金币操作从这里入口，再统一走 SetMoney
void GameManager::ChangeMoney(int delta) {
    if (moneySystem) {
        moneySystem->ChangeMoney(delta);
    }
}

// 设置金币并按需通知监听者，保持 HUD 等同步
void GameManager::SetMoney(int value, bool publishEvent) {
    if (moneySystem) {
        moneySystem->SetMoney(value, publishEvent);
    }
}

void GameManager::registerListener() {
    _listener = cocos2d::EventListenerCustom::create("monster_path_complete",
        CC_CALLBACK_1(GameManager::onMonsterPathComplete, this));
    currentScene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_listener, currentScene);
    CCLOG("REGISTER READY!");
}
void GameManager::removeListener() {
    if (_listener) {
        auto _eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
        _eventDispatcher->removeEventListener(_listener);
        _listener = nullptr;
    }
}

void GameManager::Jineng1()
{
    if (carrotManager) {
        carrotManager->Jineng1();
    }
}

void GameManager::Jineng6()
{
    if (carrotManager) {
        carrotManager->Jineng6();
    }
}
void GameManager::stopAllSchedulers() {
    CCLOG("Stopping all schedulers for GameManager.");
    cocos2d::Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
}
Vec2 GameManager::gridToScreenCenter(const Vec2& gridPoint) {
    return pathManager->GridToScreenCenter(gridPoint, currentScene);
}

// 存档/读档功能实现
void GameManager::saveGameState() {
    if (gameSaveLoader) {
        gameSaveLoader->SaveGameState();
    }
}

void GameManager::saveTowerData() {
    if (gameSaveLoader) {
        gameSaveLoader->SaveTowerData();
    }
}

bool GameManager::loadTowerData(const std::string& filename) {
    if (!gameSaveLoader) {
        return false;
    }
    return gameSaveLoader->LoadTowerData(filename);
}

int GameManager::getCurrentWaveIndex() const {
    return monsterManager->GetCurrentWaveIndex();
}

int GameManager::getAllWaveNum() const {
    return monsterManager->GetAllWaveNum();
}

int GameManager::getCurrentWaveNum() const {
    return monsterManager->GetCurrentWaveIndex();
}

int GameManager::getAllMonsterNum() const {
    return monsterManager->GetAllMonsterNum();
}

std::vector<Monster*>& GameManager::GetMonsters() {
    return monsterManager->GetMonsters();
}

const std::vector<Vec2>& GameManager::GetScreenPath() const {
    return pathManager->GetScreenPath();
}

const std::vector<Vec2>& GameManager::GetPath() const {
    return pathManager->GetPath();
}

int GameManager::getLevelId() const {
    return levelId;
}

Carrot* GameManager::getCarrot() const {
    return carrotManager ? carrotManager->GetCarrot() : nullptr;
}