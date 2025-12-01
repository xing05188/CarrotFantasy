#include "GameSaveLoader.h"

#include <fstream>
#include <sstream>

#include "cocos2d.h"
#include "../GameManager.h"
#include "MonsterManager.h"
#include "../StorageService/StorageService.h"
#include "../json/document.h"
#include "../json/stringbuffer.h"
#include "../json/writer.h"

USING_NS_CC;

GameSaveLoader::GameSaveLoader(GameManager* owner)
    : owner_(owner) {}

void GameSaveLoader::SaveGameState() {
    StorageService::getInstance()->saveGameState();
}

void GameSaveLoader::SaveTowerData() {
    if (!owner_ || !owner_->currentScene) {
        CCLOG("GameSaveLoader::SaveTowerData: currentScene is null");
        return;
    }
    StorageService::getInstance()->saveTowerData(owner_->currentScene, owner_->getLevelId(), owner_->GetMoney());
}

bool GameSaveLoader::LoadTowerData(const std::string& filename) {
    if (!owner_ || !owner_->currentScene) {
        CCLOG("GameSaveLoader::LoadTowerData: currentScene is null");
        return false;
    }
    CCLOG("GameSaveLoader::LoadTowerData: Loading file %s, current money: %d", filename.c_str(), owner_->GetMoney());
    bool result = StorageService::getInstance()->loadTowerData(owner_->currentScene, filename,
        [this](int money) {
            if (owner_) {
                CCLOG("GameSaveLoader::LoadTowerData callback: Setting money to %d", money);
                owner_->SetMoney(money, true);
                CCLOG("GameSaveLoader::LoadTowerData callback: Money after SetMoney: %d", owner_->GetMoney());
            }
        });
    CCLOG("GameSaveLoader::LoadTowerData: Load result: %s, final money: %d",
        result ? "success" : "failed", owner_->GetMoney());
    return result;
}

bool GameSaveLoader::LoadGameData(const std::string& fileName) {
    if (!owner_) {
        return false;
    }
    auto* monsterManager = owner_->monsterManager.get();
    if (!monsterManager) {
        return false;
    }

    auto& waveConfigs = monsterManager->GetWaveConfigs();
    waveConfigs.clear();
    monsterManager->ResetAllMonsterNum();
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
                monsterManager->ProduceMonsters(monsterName, pathIndex, health, true);
                CCLOG("readMonsters: name=%s, pathIndex=%d, health=%d", monsterName.c_str(), pathIndex, health);
                monsterManager->IncrementAllMonsterNum(1);
            }
        }
    }

    if (document.HasMember("currentWave") && document["currentWave"].IsObject()) {
        const rapidjson::Value& currentWave = document["currentWave"];
        std::string monsterName = currentWave["monsterName"].GetString();
        int count = currentWave["count"].GetInt();
        waveConfig.monsterName = monsterName;
        waveConfig.count = count;
        monsterManager->IncrementAllMonsterNum(count);
        waveConfigs.push_back(waveConfig);
        CCLOG("currentWave: name=%s, count=%d", monsterName.c_str(), count);
    }

    if (document.HasMember("waveIndex") && document["waveIndex"].IsInt()) {
        int waveIndex = document["waveIndex"].GetInt();
        CCLOG("currentIndex: %d", waveIndex);
        monsterManager->SetCurrentWaveIndex(waveIndex);
    }
    monsterManager->SetAllWaveNum(monsterManager->GetCurrentWaveIndex() + 1);
    if (document.HasMember("upcomingWaves") && document["upcomingWaves"].IsArray()) {
        const rapidjson::Value& upcomingWaves = document["upcomingWaves"];
        for (rapidjson::SizeType i = 0; i < upcomingWaves.Size(); ++i) {
            const rapidjson::Value& waveData = upcomingWaves[i];
            if (waveData.IsObject()) {
                std::string monsterName = waveData["monsterName"].GetString();
                int count = waveData["count"].GetInt();
                waveConfig.monsterName = monsterName;
                waveConfig.count = count;
                monsterManager->IncrementAllMonsterNum(count);
                monsterManager->SetAllWaveNum(monsterManager->GetAllWaveNum() + 1);
                waveConfigs.push_back(waveConfig);
                CCLOG("upComingWave: name=%s, count=%d", monsterName.c_str(), count);
            }
        }
    }

    CCLOG("Read All Data!");
    return true;
}

void GameSaveLoader::SaveMonstersDataToJson(const std::string& fileName) {
    if (!owner_) {
        return;
    }
    auto* monsterManager = owner_->monsterManager.get();
    if (!monsterManager) {
        return;
    }

    rapidjson::Document document;
    document.SetObject();
    int currentWaveIndex = monsterManager->GetCurrentWaveIndex();
    rapidjson::Value livingMonsters(rapidjson::kArrayType);
    for (auto* monster : monsterManager->GetMonsters()) {
        if (monster && monster->checkLive()) {
            rapidjson::Value monsterData(rapidjson::kObjectType);
            monsterData.AddMember("monsterName", rapidjson::Value(monster->getMonsterName().c_str(), document.GetAllocator()), document.GetAllocator());
            monsterData.AddMember("pathIndex", monster->getPathIndex(), document.GetAllocator());
            monsterData.AddMember("health", monster->getHealth(), document.GetAllocator());
            livingMonsters.PushBack(monsterData, document.GetAllocator());
        }
    }
    document.AddMember("livemonsters", livingMonsters, document.GetAllocator());
    int totalMonstersCount = 0;
    auto& waveConfigs = monsterManager->GetWaveConfigs();
    for (int i = 0; i <= currentWaveIndex && i < static_cast<int>(waveConfigs.size()); ++i) {
        totalMonstersCount += waveConfigs[i].count;
    }
    int remainingCount = totalMonstersCount - static_cast<int>(monsterManager->GetMonsters().size());
    rapidjson::Value currentWave(rapidjson::kObjectType);
    if (!waveConfigs.empty() && currentWaveIndex < static_cast<int>(waveConfigs.size())) {
        currentWave.AddMember("monsterName", rapidjson::Value(waveConfigs[currentWaveIndex].monsterName.c_str(), document.GetAllocator()), document.GetAllocator());
        currentWave.AddMember("count", remainingCount, document.GetAllocator());
    } else {
        currentWave.AddMember("monsterName", "", document.GetAllocator());
        currentWave.AddMember("count", 0, document.GetAllocator());
    }
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

