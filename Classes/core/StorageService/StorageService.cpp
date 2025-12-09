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
#include "StorageService.h"
#include "json/document.h"
#include "json/rapidjson.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "json/error/en.h"
#include "../../scenes/BaseLevelScene.h"
#include "../../entities/Tower/TowerFactory.h"
#include "../../entities/Obstacle/Obstacle.h"
#include <functional>

USING_NS_CC;

// 单例实例指针
StorageService* StorageService::instance = nullptr;
#define CELL_SIZE 64
#define X_SIZE 15
#define Y_SIZE 9

StorageService* StorageService::getInstance() {
    if (!instance) {
        instance = new StorageService();
    }
    return instance;
}

void StorageService::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

std::string StorageService::getWritablePath() const {
    return FileUtils::getInstance()->getWritablePath();
}

bool StorageService::writeJsonFile(const std::string& filePath, const std::string& jsonContent) const {
    std::ofstream ofs(filePath);
    if (ofs.is_open()) {
        ofs << jsonContent;
        ofs.close();
        CCLOG("Save success: %s", filePath.c_str());
        return true;
    }
    else {
        CCLOG("Save failed: %s", filePath.c_str());
        return false;
    }
}

std::string StorageService::readJsonFile(const std::string& filePath) const {
    return FileUtils::getInstance()->getStringFromFile(filePath);
}

bool StorageService::saveGameState() {
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

    std::string writablePath = getWritablePath();
    std::string filePath = writablePath + "level_state.json";

    return writeJsonFile(filePath, buffer.GetString());
}

bool StorageService::saveTowerData(BaseLevelScene* scene, int levelId, int money) {
    if (!scene) {
        CCLOG("StorageService: scene is null, cannot save tower data");
        return false;
    }
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Value towerArray(rapidjson::kArrayType);
    
    for (int i = 0; i < X_SIZE; i++) {
        rapidjson::Value rowArray(rapidjson::kArrayType);
        for (int j = 0; j < Y_SIZE; j++) {
            rapidjson::Value towerObj(rapidjson::kObjectType);
            towerObj.AddMember("flag", scene->map_data[i][j].flag, document.GetAllocator());
            if (scene->map_data[i][j].flag == 1) {
                auto it = scene->towers.find(scene->map_data[i][j].key);
                if (it != scene->towers.end()) {
                    towerObj.AddMember("index", it->second->GetIndex(), document.GetAllocator());
                    towerObj.AddMember("data", it->second->GetGrade(), document.GetAllocator());
                } else {
                    towerObj.AddMember("index", 0, document.GetAllocator());
                    towerObj.AddMember("data", 0, document.GetAllocator());
                }
            }
            else if (scene->map_data[i][j].flag == 2) {
                auto it = scene->Obstacles.find(scene->map_data[i][j].key);
                if (it != scene->Obstacles.end()) {
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
    document.AddMember("money", money, document.GetAllocator());
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string writablePath = getWritablePath();
    std::string filePath = writablePath + "level" + std::to_string(levelId) + "_tower.json";

    return writeJsonFile(filePath, buffer.GetString());
}

bool StorageService::loadTowerData(BaseLevelScene* scene, const std::string& filename, 
                                    std::function<void(int)> moneyCallback) {
    CCLOG("StorageService::loadTowerData: Called with filename=%s", filename.c_str());
    if (!scene) {
        CCLOG("StorageService::loadTowerData: ERROR - scene is null, cannot load tower data");
        return false;
    }
    std::string writablePath = getWritablePath();
    std::string path = writablePath + filename;
    std::string fileContent = readJsonFile(path);
    
    if (fileContent.empty()) {
        CCLOG("Error reading file: %s", filename.c_str());
        return false;
    }
    
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    
    if (doc.HasParseError()) {
        CCLOG("StorageService::loadTowerData: ERROR - Error parsing JSON file: %s, error code: %d, offset: %zu", 
              filename.c_str(), doc.GetParseError(), doc.GetErrorOffset());
        return false;
    }
    
    CCLOG("StorageService::loadTowerData: Successfully parsed JSON file: %s", filename.c_str());
    
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
                                    tower->build(scene, pos);
                                    scene->towers[scene->map_data[i][j].key] = tower;
                                }
                            }
                            else if (flag == 2 && scene->map_data[i][j].flag == 3) {
                                auto obb = new Obstacle(index);
                                obb->Produce(scene, i, j);
                                obb->setHealth(data);
                                obb->updateHealthBar();
                                scene->Obstacles[scene->map_data[i][j].key] = obb;
                                if (obb->GetSize() == 2) {
                                    scene->map_data[i + 1][j].flag = 2;
                                    scene->Obstacles[scene->map_data[i + 1][j].key] = obb;
                                }
                                else if (obb->GetSize() == 4) {
                                    scene->map_data[i][j + 1].flag = scene->map_data[i + 1][j].flag = scene->map_data[i + 1][j + 1].flag = 2;
                                    scene->Obstacles[scene->map_data[i + 1][j].key] = scene->Obstacles[scene->map_data[i][j + 1].key] = scene->Obstacles[scene->map_data[i + 1][j + 1].key] = obb;
                                }
                            }
                            scene->map_data[i][j].flag = flag;
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
        CCLOG("StorageService::loadTowerData: Found money in file: %d", savedMoney);
        if (moneyCallback) {
            CCLOG("StorageService::loadTowerData: Calling moneyCallback with value: %d", savedMoney);
            moneyCallback(savedMoney);
            CCLOG("StorageService::loadTowerData: moneyCallback completed");
        } else {
            CCLOG("StorageService::loadTowerData: ERROR - moneyCallback is null!");
        }
        CCLOG("INIT_MONEY:currentIndex: %d", savedMoney);
    }
    else {
        CCLOG("StorageService::loadTowerData: ERROR - No money data in file: %s", filename.c_str());
        return false;
    }
    
    return true;
}