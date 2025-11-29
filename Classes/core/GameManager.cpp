#include"GameManager.h"
#include<vector>
#include "json/document.h"
#include "json/rapidjson.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include<fstream>
#include <sstream>
#include"Music.h"
#include "EventBusProvider.h"
#include "../gameplay/events/MoneyEvents.h"
#include "../gameplay/events/GameFlowEvents.h"
#include "../gameplay/events/MonsterEvents.h"
#include "../gameplay/events/CarrotEvents.h"
#include "../entities/Tower/TowerFactory.h"
#include "../entities/Obstacle.h"

USING_NS_CC;
extern int DeadCount;

// 单例实例指针（全局唯一 GameManager）
GameManager* GameManager::instance = nullptr;

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
    CCLOG("monsters size %d", monsters.size());
    CCLOG("monsters DEAD NUM %d", DeadCount);
}
// 判断失败条件并仅发布一次游戏失败事件，供 UI 监听
bool GameManager::CheckLose()
{
    if (carrot->getHP() <= 0)
    {
        CCLOG("LOSE THE GAME!");
        PublishGameLostEvent();
        return true;
    }
    return false;
}
// 判断胜利条件（全部波次完成且怪物清空）并发布胜利事件
bool GameManager::CheckWin()
{
    if (waveIndex + 1 < AllWaveNum) return false;
    if (carrot->getHP() <= 0) return false;
    CCLOG("------------------------------------%d", monsters.size());
    if (monsters.size() < static_cast<size_t>(AllMonsterNum)) return false;
    for (auto it = monsters.begin(); it != monsters.end(); it++) {
        if ((*it)->getHealth() > 0) return false;
    }

    CCLOG("WIN THE GAME!");
    PublishGameWonEvent();
    return true;
}
void GameManager::ApplyMonsterSpeed(float speedFactor) {
    for (auto* monster : monsters) {
        if (!monster) continue;
        if (monster->getHealth() <= 0) continue;
        if (monster->speedaction) {
            monster->speedaction->setSpeed(speedFactor);
        }
    }
}

void GameManager::KillAllMonsters() {
    auto bus = carrot::core::EventBusProvider::Get();
    for (auto* monster : monsters) {
        if (!monster) continue;
        if (monster->getHealth() <= 0) continue;
        monster->setHealth(0);
        if (bus) {
            carrot::gameplay::events::MonsterDiedEvent evt{};
            evt.monster = monster;
            bus->Publish(carrot::gameplay::events::kMonsterDiedEventId, evt);
        }
    }
}

// 初始化关卡的路径、波次、金币与胜负标记，并加载相关资源
void GameManager::initLevel(int level,bool initMode)
{
    levelId=level;
    path.clear();
    screenPath.clear();
    monsters.clear();
    waveConfigs.clear();
    waveIndex=0;
    ClearMonsters();
    AllMonsterNum=0;
    DeadCount=0;
    SetMoney(kDefaultStartingMoney, false);
    hasGameWon = false;
    hasGameLost = false;
    initPath();
    loadMonsterResources();
    initCarrot();
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
    if (pathsCache.find(levelId) == pathsCache.end()) {
        if (!loadPathForLevel(levelId, "paths.json")) {
            CCLOG("Failed to load path for level %d.", levelId);
            return;
        }
    }
    path = pathsCache[levelId];
    CCLOG("Path for level %d:", levelId);
    for (const auto& point : path) {
        CCLOG("Point: (%f, %f)", point.x, point.y);
    }
    CCLOG("%f", currentScene->tileMap->getMapSize().height);
    for (const auto& gridPoint : path) {
        screenPath.push_back(gridToScreenCenter(gridPoint));
        float x = gridToScreenCenter(gridPoint).x;
        float y = gridToScreenCenter(gridPoint).y;
        CCLOG("ScreenPoint: (%f, %f)", x, y);
    }
}
// 从 JSON 配置中读取某一关卡的路径数据，并缓存到 pathsCache / ScreenPaths
bool GameManager::loadPathForLevel(int levelId, const std::string& filePath)
{
    if (pathsCache.find(levelId) != pathsCache.end()) {
        return true;
    }

    std::string fileContent = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
    if (fileContent.empty()) {
        CCLOG("Failed to load JSON file: %s", filePath.c_str());
        return false;
    }

    rapidjson::Document document;
    document.Parse(fileContent.c_str());

    if (document.HasParseError() || !document.IsObject()) {
        CCLOG("Failed to parse JSON or invalid format: %s", filePath.c_str());
        return false;
    }

    for (auto& level : document.GetObject()) {
        int levelIdInFile = std::stoi(level.name.GetString());
        if (levelIdInFile == levelId) {

            if (!level.value.IsArray()) {
                CCLOG("Path data for level %d is not an array.", levelId);
                return false;
            }

            const auto& points = level.value.GetArray();
            std::vector<cocos2d::Vec2> path;

            for (rapidjson::Value::ConstValueIterator it = points.Begin(); it != points.End(); ++it) {
                if (!it->IsArray() || it->Size() != 2) {
                    CCLOG("Invalid point format in level %d.", levelId);
                    continue;
                }

                float x = (*it)[0].GetFloat();
                float y = (*it)[1].GetFloat();
                path.emplace_back(x, y);
            }

            if (path.empty()) {
                CCLOG("No valid points found for level %d.", levelId);
                return false;
            }

            pathsCache[levelId] = path;
            for (const auto& point : pathsCache[levelId]) {
                CCLOG("Grid Point: (%f, %f)", point.x, point.y);
                Vec2 screenCenter = gridToScreenCenter(point);
                ScreenPaths[levelId].emplace_back(screenCenter);
                CCLOG("Center Screen Point: (%f, %f)", screenCenter.x, screenCenter.y);
            }
            return true;
        }
    }

    CCLOG("Path for level %d not found in file: %s", levelId, filePath.c_str());
    return false;
}
// 预加载所有怪物相关的帧动画资源，避免战斗过程中卡顿
void GameManager::loadMonsterResources() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/pig.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/yellowbat.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/blue.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/tuzi.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/pink.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/boss.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/sheep.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/biao.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/star.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/bubble.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/fuck.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/xin.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/yuxin.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/BossYellow.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/BossSheep.plist");
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig_0.png")) {
        CCLOG("Failed to load SpriteFrame 'pig_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig_1.png")) {
        CCLOG("Failed to load SpriteFrame 'pig_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("yellowbat_0.png")) {
        CCLOG("Failed to load SpriteFrame 'yellowbat_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("yellowbat_1.png")) {
        CCLOG("Failed to load SpriteFrame 'yellowbat_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("blue_0.png")) {
        CCLOG("Failed to load SpriteFrame 'blue_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("blue_1.png")) {
        CCLOG("Failed to load SpriteFrame 'blue_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("tuzi_0.png")) {
        CCLOG("Failed to load SpriteFrame 'tuzi_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("tuzi_1.png")) {
        CCLOG("Failed to load SpriteFrame 'tuzi_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pink_0.png")) {
        CCLOG("Failed to load SpriteFrame 'pink_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pink_1.png")) {
        CCLOG("Failed to load SpriteFrame 'pink_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("boss_0.png")) {
        CCLOG("Failed to load SpriteFrame 'boss_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("boss_1.png")) {
        CCLOG("Failed to load SpriteFrame 'boss_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("sheep_0.png")) {
        CCLOG("Failed to load SpriteFrame 'sheep_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("sheep_1.png")) {
        CCLOG("Failed to load SpriteFrame 'sheep_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("fuck_0.png")) {
        CCLOG("Failed to load SpriteFrame 'fuck_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("fuck_1.png")) {
        CCLOG("Failed to load SpriteFrame 'fuck_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("bubble_0.png")) {
        CCLOG("Failed to load SpriteFrame 'bubble_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("bubble_1.png")) {
        CCLOG("Failed to load SpriteFrame 'bubble_1.png'.");
    }

    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("star_0.png")) {
        CCLOG("Failed to load SpriteFrame 'star_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("star_1.png")) {
        CCLOG("Failed to load SpriteFrame 'star_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("biao_0.png")) {
        CCLOG("Failed to load SpriteFrame 'biao_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("biao_1.png")) {
        CCLOG("Failed to load SpriteFrame 'biao_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("yuxin_0.png")) {
        CCLOG("Failed to load SpriteFrame 'yuxin_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("yuxin_1.png")) {
        CCLOG("Failed to load SpriteFrame 'yuxin_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("xin_0.png")) {
        CCLOG("Failed to load SpriteFrame 'xin_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("xin_1.png")) {
        CCLOG("Failed to load SpriteFrame 'xin_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("BossYellow_0.png")) {
        CCLOG("Failed to load SpriteFrame 'BossYellow_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("BossYellow_1.png")) {
        CCLOG("Failed to load SpriteFrame 'BossYellow_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("BossSheep_0.png")) {
        CCLOG("Failed to load SpriteFrame 'BossSheep_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("BossSheep_1.png")) {
        CCLOG("Failed to load SpriteFrame 'BossSheep_1.png'.");
    }
}
// 创建单个怪物并加入场景，可用于正常刷怪或读档恢复
void GameManager::produceMonsters(const std::string monsterName, const int startIndex, int health, bool pause) {
    Music::getInstance()->born_music();
    if (startIndex == 0)
        playSpawnEffect(screenPath[0]);
    auto Monster = Monster::create(monsterName, screenPath, startIndex, pause);
    if (!Monster) {
        CCLOG("Failed to create monster.");
        return;
    }
    monsters.push_back(Monster);
    CCLOG("");
    // 由事件通知场景把怪物节点挂到合适的位置/层级上
    auto bus = carrot::core::EventBusProvider::Get();
    if (bus) {
        carrot::gameplay::events::MonsterSpawnedEvent evt{};
        evt.monster = Monster;
        bus->Publish(carrot::gameplay::events::kMonsterSpawnedEventId, evt);
    }
    Monster->setPause(pause);
    CCLOG("MONSTER PAUSE  %d", Monster->getPause());
    if (health != -1)
    {
        Monster->setHealth(health);
    }
    if(monsterName.find("Boss") == 0)
    {
      
    Monster->SpecialAttack();
    }
}
// 从 JSON 文件中读取某个关卡的全部波次配置
void GameManager::loadMonsterWaveConfig(const std::string& filename, const std::string& levelName) {
    std::string path = FileUtils::getInstance()->fullPathForFilename(filename);
    std::string fileContent = FileUtils::getInstance()->getStringFromFile(path);

    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());

    if (doc.HasParseError()) {
        CCLOG("Error parsing JSON file: %s", filename.c_str());
        return;
    }
    if (doc.HasMember(levelName.c_str())) {
        const rapidjson::Value& waves = doc[levelName.c_str()];
        for (rapidjson::SizeType i = 0; i < waves.Size(); ++i) {
            const rapidjson::Value& wave = waves[i];
            WaveConfig waveConfig;
            waveConfig.wave = wave["wave"].GetInt();
            waveConfig.monsterName = wave["monsterName"].GetString();

            waveConfig.count = wave["count"].GetInt();

            const rapidjson::Value& spawnInterval = wave["spawnInterval"];
            waveConfig.spawnInterval[0] = spawnInterval[0].GetFloat();
            waveConfig.spawnInterval[1] = spawnInterval[1].GetFloat();
            waveConfigs.push_back(waveConfig);
            AllWaveNum = waveConfigs.size();
            CCLOG("Wave %d - Monster: %s, Count: %d, Spawn Interval: %.2f - %.2f",
                waveConfigs.size(),
                waveConfig.monsterName.c_str(),
                waveConfig.count,
                waveConfig.spawnInterval[0],
                waveConfig.spawnInterval[1]
            );
        }
    }
    else {
        CCLOG("No such level: %s", levelName.c_str());
    }
    for (auto waveConfig : waveConfigs)
    {
        AllMonsterNum += waveConfig.count;
    }
    CCLOG("All Num %d ", AllMonsterNum);
}
// 按照一条波次配置，利用调度器在一段时间内依次生成该波全部怪物
void GameManager::produceMonsterWave(const WaveConfig& waveConfig) {
    float delay = 0;
    CCLOG("%d   %d", waveIndex,AllWaveNum);
    for (int i = 0; i < waveConfig.count; ++i) {
        delay += cocos2d::RandomHelper::random_real(waveConfig.spawnInterval[0], waveConfig.spawnInterval[1]);

        cocos2d::Director::getInstance()->getScheduler()->schedule([=](float) {
            produceMonsters(waveConfig.monsterName, 0, -1, false);
            }, this, 0, 0, delay, false, "produceMonster" + std::to_string(i));
    }
}
// 入口：开始整局战斗的刷怪流程，会按固定间隔推进到下一波
void GameManager::startMonsterWaves() {
    CCLOG("Starting wave %d", waveIndex);
    produceMonsterWave(waveConfigs[waveIndex]);
    cocos2d::Director::getInstance()->getScheduler()->schedule([this](float) {
        if (waveIndex >= static_cast<int>(waveConfigs.size()) - 1) {
            CCLOG("All waves are complete.");
            cocos2d::Director::getInstance()->getScheduler()->unschedule("startWave", this);
            return;
        }
        ++waveIndex;
        CCLOG("Starting wave %d", waveIndex);
        produceMonsterWave(waveConfigs[waveIndex]);

        }, this, 15.0f, false, "startWave");
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
        if (monster->getHealth() > 0)
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
    for (auto monster : monsters) {
        if (monster->getParent()) {
            monster->getParent()->removeChild(monster);
        }
        monster->stopAllActions();
        monster->release();
    }
    monsters.clear();
}

bool GameManager::loadGameData(const std::string& fileName) {
    waveConfigs.clear();
    WaveConfig waveConfig;
    std::string filePath = cocos2d::FileUtils::getInstance()->getWritablePath() + fileName;
    CCLOG("Loading save file: %s", filePath.c_str());

    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        CCLOG("Cannot open save file: %s", filePath.c_str());
        return false;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string fileContent = buffer.str();
    ifs.close();
    CCLOG("File content: %s", fileContent.c_str());

    rapidjson::Document document;
    if (document.Parse(fileContent.c_str()).HasParseError()) {
        CCLOG("JSON parse error");
        return false;
    }

    if (document.HasMember("livemonsters") && document["livemonsters"].IsArray()) {
        const rapidjson::Value& livingMonsters = document["livemonsters"];
        for (rapidjson::SizeType i = 0; i < livingMonsters.Size(); ++i) {
            const rapidjson::Value& monsterData = livingMonsters[i];
            if (monsterData.IsObject()) {
                std::string monsterName = monsterData["monsterName"].GetString();
                int pathIndex = monsterData["pathIndex"].GetInt();
                int health = monsterData["health"].GetInt();
                produceMonsters(monsterName, pathIndex,health,true);
                CCLOG("readMonsters: name=%s, pathIndex=%d, health=%d", monsterName.c_str(), pathIndex, health);
                AllMonsterNum++; 
            }
        }
    }

    if (document.HasMember("currentWave") && document["currentWave"].IsObject()) {
        const rapidjson::Value& currentWave = document["currentWave"];
        std::string monsterName = currentWave["monsterName"].GetString();
        int count = currentWave["count"].GetInt();
            waveConfig.monsterName = monsterName;
            waveConfig.count = count;
            AllMonsterNum+=count;
            waveConfigs.push_back(waveConfig);
            CCLOG("currentWave: name=%s, count=%d", monsterName.c_str(), count);
    }

    if (document.HasMember("waveIndex") && document["waveIndex"].IsInt()) {
        int waveIndex = document["waveIndex"].GetInt();
        CCLOG("currentIndex: %d", waveIndex);
        this->waveIndex = waveIndex;
    }
    AllWaveNum= waveIndex+1;
    if (document.HasMember("upcomingWaves") && document["upcomingWaves"].IsArray()) {
        const rapidjson::Value& upcomingWaves = document["upcomingWaves"];
        for (rapidjson::SizeType i = 0; i < upcomingWaves.Size(); ++i) {
            const rapidjson::Value& waveData = upcomingWaves[i];
            if (waveData.IsObject()) {
                std::string monsterName = waveData["monsterName"].GetString();
                int count = waveData["count"].GetInt();
                waveConfig.monsterName = monsterName;
                waveConfig.count = count;
               AllMonsterNum += count;
                AllWaveNum++;
                waveConfigs.push_back(waveConfig);
                CCLOG("upComingWave: name=%s, count=%d", monsterName.c_str(), count);
            }
        }
    }
    
    CCLOG("Read All Data!");
    return true;
}
void GameManager::saveMonstersDataToJson(const std::string& fileName) {
    rapidjson::Document document;
    document.SetObject();
    int currentWaveIndex = getCurrentWaveIndex();
    rapidjson::Value livingMonsters(rapidjson::kArrayType);
    for (auto* monster : monsters) {
        if (monster->checkLive()) {
            rapidjson::Value monsterData(rapidjson::kObjectType);
            // monsterName
            monsterData.AddMember("monsterName", rapidjson::Value(monster->getMonsterName().c_str(), document.GetAllocator()), document.GetAllocator());
            // pathIndex
            monsterData.AddMember("pathIndex", monster->getPathIndex(), document.GetAllocator());
            // health
            monsterData.AddMember("health", monster->getHealth(), document.GetAllocator());
            livingMonsters.PushBack(monsterData, document.GetAllocator());
        }
    }
    document.AddMember("livemonsters", livingMonsters, document.GetAllocator());
    int totalMonstersCount = 0;
    for (int i = 0; i <= currentWaveIndex; ++i) {
        totalMonstersCount += waveConfigs[i].count;
    }
    int remainingCount = totalMonstersCount - monsters.size();
    rapidjson::Value currentWave(rapidjson::kObjectType);
    currentWave.AddMember("monsterName", rapidjson::Value(waveConfigs[currentWaveIndex].monsterName.c_str(), document.GetAllocator()), document.GetAllocator());
    currentWave.AddMember("count", remainingCount, document.GetAllocator());
    document.AddMember("currentWave", currentWave, document.GetAllocator());
    document.AddMember("waveIndex", currentWaveIndex, document.GetAllocator());
    rapidjson::Value upcomingWaves(rapidjson::kArrayType);
    for (size_t i = currentWaveIndex + 1; i < waveConfigs.size(); ++i) {
        rapidjson::Value waveData(rapidjson::kObjectType);
        waveData.AddMember("monsterName", rapidjson::Value(waveConfigs[i].monsterName.c_str(), document.GetAllocator()), document.GetAllocator());
        waveData.AddMember("count", waveConfigs[i].count, document.GetAllocator());
        upcomingWaves.PushBack(waveData, document.GetAllocator());
    }
    document.AddMember("upcomingWaves", upcomingWaves, document.GetAllocator());
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string filePath = writablePath + fileName;
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    std::ofstream ofs(filePath);
    if (ofs.is_open()) {
        ofs << buffer.GetString();
        ofs.close();
        CCLOG("Save success: %s", filePath.c_str());
    }
    else {
        CCLOG("Save failed: %s", filePath.c_str());
    }
}
void GameManager::doudong() {
    if (!carrot) {
        return;
    }
    if (carrot->getHP() == carrot->getMaxHP()) {
        auto bus = carrot::core::EventBusProvider::Get();
        if (!bus) {
            CCLOG("GameManager: EventBus not available, carrot shake event not published");
            return;
        }
        carrot::gameplay::events::CarrotShakeRequestedEvent evt{};
        // 使用当前关卡目标点作为抖动特效位置
        evt.x = dst1[levelId - 1].x;
        evt.y = dst1[levelId - 1].y;
        bus->Publish(carrot::gameplay::events::kCarrotShakeRequestedEventId, evt);
    }
}
void GameManager::initCarrot() {
    carrot = Carrot::create(10, dst1[levelId - 1], dst2[levelId - 1]);
    currentScene->addChild(carrot, 1);
    CCLOG("CARROT READY!");
}

// 所有加减金币操作从这里入口，再统一走 SetMoney
void GameManager::ChangeMoney(int delta) {
    SetMoney(money + delta);
}

// 设置金币并按需通知监听者，保持 HUD 等同步
void GameManager::SetMoney(int value, bool publishEvent) {
    int delta = value - money;
    money = value;
    if (publishEvent) {
        PublishMoneyChangedEvent(delta);
    }
}

// 发布金币变化事件，UI 与其他系统可通过事件解耦
void GameManager::PublishMoneyChangedEvent(int delta) {
    carrot::gameplay::events::MoneyChangedEvent evt{};
    evt.delta = delta;
    evt.current = money;
    auto bus = carrot::core::EventBusProvider::Get();
    if (bus) {
        bus->Publish(carrot::gameplay::events::kMoneyChangedEventId, evt);
    }
}

// 胜利事件只发送一次，Scene/UI 通过订阅获知
void GameManager::PublishGameWonEvent() {
    if (hasGameWon || hasGameLost) {
        return;
    }
    hasGameWon = true;
    carrot::gameplay::events::GameWonEvent evt{};
    evt.currentWave = getCurrentWaveNum();
    evt.totalWave = getAllWaveNum();
    evt.levelId = levelId;
    auto bus = carrot::core::EventBusProvider::Get();
    if (bus) {
        bus->Publish(carrot::gameplay::events::kGameWonEventId, evt);
    }
}

// 失败事件也只发送一次，避免 Scene/UI 轮询
void GameManager::PublishGameLostEvent() {
    if (hasGameLost || hasGameWon) {
        return;
    }
    hasGameLost = true;
    carrot::gameplay::events::GameLostEvent evt{};
    evt.currentWave = getCurrentWaveNum();
    evt.totalWave = getAllWaveNum();
    evt.levelId = levelId;
    auto bus = carrot::core::EventBusProvider::Get();
    if (bus) {
        bus->Publish(carrot::gameplay::events::kGameLostEventId, evt);
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
    carrot->getRecover();
}

void GameManager::Jineng6()
{
    carrot->enterInvincibleState();
}
void GameManager::stopAllSchedulers() {
    CCLOG("Stopping all schedulers for GameManager.");
    cocos2d::Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
}
Vec2 GameManager::gridToScreenCenter(const Vec2& gridPoint) {
    float mapHeight = currentScene->tileMap->getMapSize().height;
    float screenX = gridPoint.x * (currentScene->tileSize.height) + (currentScene->tileSize.width) / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * (currentScene->tileSize.height) + (currentScene->tileSize.height) / 2;
    return Vec2(screenX, screenY);
}

// 存档/读档功能实现
void GameManager::saveGameState() {
    extern bool level_is_win[3];
    
    rapidjson::Document document;
    document.SetObject();

    rapidjson::Value GameState(rapidjson::kArrayType);

    for (auto level : level_is_win) {
        GameState.PushBack(rapidjson::Value(level), document.GetAllocator());
    }

    document.AddMember("levels", GameState, document.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string filePath = writablePath + "level_state.json";

    std::ofstream ofs(filePath);
    if (ofs.is_open()) {
        ofs << buffer.GetString();
        ofs.close();
        CCLOG("Save success: %s", filePath.c_str());
    }
    else {
        CCLOG("Save failed: %s", filePath.c_str());
    }
}

void GameManager::saveTowerData() {
    if (!currentScene) {
        CCLOG("GameManager: currentScene is null, cannot save tower data");
        return;
    }

    const int CELL_SIZE = 64;
    const int X_SIZE = 15;
    const int Y_SIZE = 9;

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Value towerArray(rapidjson::kArrayType);
    
    for (int i = 0; i < X_SIZE; i++) {
        rapidjson::Value rowArray(rapidjson::kArrayType);
        for (int j = 0; j < Y_SIZE; j++) {
            rapidjson::Value towerObj(rapidjson::kObjectType);
            towerObj.AddMember("flag", currentScene->map_data[i][j].flag, document.GetAllocator());
            if (currentScene->map_data[i][j].flag == 1) {
                auto it = currentScene->towers.find(currentScene->map_data[i][j].key);
                if (it != currentScene->towers.end()) {
                    towerObj.AddMember("index", it->second->GetIndex(), document.GetAllocator());
                    towerObj.AddMember("data", it->second->GetGrade(), document.GetAllocator());
                } else {
                    towerObj.AddMember("index", 0, document.GetAllocator());
                    towerObj.AddMember("data", 0, document.GetAllocator());
                }
            }
            else if (currentScene->map_data[i][j].flag == 2) {
                auto it = currentScene->Obstacles.find(currentScene->map_data[i][j].key);
                if (it != currentScene->Obstacles.end()) {
                    towerObj.AddMember("index", it->second->GetIndex(), document.GetAllocator());
                    towerObj.AddMember("data", it->second->getHealth(), document.GetAllocator());
                } else {
                    towerObj.AddMember("index", 0, document.GetAllocator());
                    towerObj.AddMember("data", 0, document.GetAllocator());
                }
            }
            else {
                towerObj.AddMember("index", 0, document.GetAllocator());
                towerObj.AddMember("data", 0, document.GetAllocator());
            }
            rowArray.PushBack(towerObj, document.GetAllocator());
        }
        towerArray.PushBack(rowArray, document.GetAllocator());
    }
    
    document.AddMember("towers", towerArray, document.GetAllocator());
    document.AddMember("money", GetMoney(), document.GetAllocator());
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string filePath = writablePath + "level" + std::to_string(levelId) + "_tower.json";

    std::ofstream ofs(filePath);
    if (ofs.is_open()) {
        ofs << buffer.GetString();
        ofs.close();
        CCLOG("Save success: %s", filePath.c_str());
    }
    else {
        CCLOG("Save failed: %s", filePath.c_str());
    }
}

bool GameManager::loadTowerData(const std::string& filename) {
    if (!currentScene) {
        CCLOG("GameManager: currentScene is null, cannot load tower data");
        return false;
    }

    const int CELL_SIZE = 64;
    const int X_SIZE = 15;
    const int Y_SIZE = 9;

    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string path = writablePath + filename;
    std::string fileContent = FileUtils::getInstance()->getStringFromFile(path);
    
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    
    if (doc.HasParseError()) {
        CCLOG("Error parsing JSON file: %s", filename.c_str());
        return false;
    }
    
    if (doc.HasMember("towers") && doc["towers"].IsArray()) {
        const rapidjson::Value& towersArray = doc["towers"];
        if (towersArray.IsArray()) {
            for (rapidjson::SizeType i = 0; i < towersArray.Size(); ++i) {
                const rapidjson::Value& row = towersArray[i];
                if (row.IsArray()) {
                    for (rapidjson::SizeType j = 0; j < row.Size(); ++j) {
                        const rapidjson::Value& towerObj = row[j];
                        if (towerObj.IsObject()) {
                            int flag = towerObj["flag"].GetInt();
                            int index = towerObj["index"].GetInt();
                            int data = towerObj["data"].GetInt();
                            
                            if (flag == 1) {
                                Vec2 pos = Vec2((i + 0.5f) * CELL_SIZE, (j + 0.5f) * CELL_SIZE);
                                auto tower = TowerFactoryProvider::createTower(index, data);
                                if (tower) {
                                    tower->build(currentScene, pos);
                                    currentScene->towers[currentScene->map_data[i][j].key] = tower;
                                }
                            }
                            else if (flag == 2 && currentScene->map_data[i][j].flag == 3) {
                                auto obb = new Obstacle(index);
                                obb->Produce(currentScene, i, j);
                                obb->setHealth(data);
                                obb->updateHealthBar();
                                currentScene->Obstacles[currentScene->map_data[i][j].key] = obb;
                                if (obb->GetSize() == 2) {
                                    currentScene->map_data[i + 1][j].flag = 2;
                                    currentScene->Obstacles[currentScene->map_data[i + 1][j].key] = obb;
                                }
                                else if (obb->GetSize() == 4) {
                                    currentScene->map_data[i][j + 1].flag = currentScene->map_data[i + 1][j].flag = currentScene->map_data[i + 1][j + 1].flag = 2;
                                    currentScene->Obstacles[currentScene->map_data[i + 1][j].key] = currentScene->Obstacles[currentScene->map_data[i][j + 1].key] = currentScene->Obstacles[currentScene->map_data[i + 1][j + 1].key] = obb;
                                }
                            }
                            currentScene->map_data[i][j].flag = flag;
                        }
                    }
                }
            }
        }
        else {
            CCLOG("Towers data is not an array in level");
            return false;
        }
    }
    
    if (doc.HasMember("money") && doc["money"].IsInt()) {
        int savedMoney = doc["money"].GetInt();
        SetMoney(savedMoney, false);
        CCLOG("INIT_MONEY:currentIndex: %d", GetMoney());
    }
    else {
        CCLOG("No money data in file: %s", filename.c_str());
        return false;
    }
    
    return true;
}