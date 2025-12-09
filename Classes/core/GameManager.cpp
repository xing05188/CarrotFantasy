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