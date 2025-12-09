// Refactored with Manager Pattern
// 使用管理者模式重构
#pragma once

#include <string>

class GameManager;

class GameSaveLoader {
public:
    explicit GameSaveLoader(GameManager* owner);

    void SaveGameState();
    void SaveTowerData();
    bool LoadTowerData(const std::string& filename);
    bool LoadGameData(const std::string& fileName);
    void SaveMonstersDataToJson(const std::string& fileName);

private:
    GameManager* owner_;
};

