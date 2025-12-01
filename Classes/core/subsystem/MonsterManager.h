#pragma once

#include <string>
#include <vector>

#include "cocos2d.h"
#include "entities/Monster/Monster.h"
#include "MonsterTypes.h"

class GameManager;

class MonsterManager {
public:
    explicit MonsterManager(GameManager* owner);

    void ResetForLevel();
    void LoadMonsterResources();
    void LoadMonsterWaveConfig(const std::string& filename, const std::string& levelName);
    void StartMonsterWaves();
    void ApplyMonsterSpeed(float speedFactor);
    void KillAllMonsters();
    void ClearMonsters();

    std::vector<Monster*>& GetMonsters();
    int GetAllMonsterNum() const;
    void IncrementAllMonsterNum(int delta);
    void ResetAllMonsterNum();
    int GetAllWaveNum() const;
    void SetAllWaveNum(int value);
    int GetCurrentWaveIndex() const;
    void SetCurrentWaveIndex(int value);
    void IncrementWaveIndex();
    std::vector<WaveConfig>& GetWaveConfigs();

    void ProduceMonsters(const std::string& monsterName, int startIndex, int health, bool pause);
    void ProduceMonsterWave(const WaveConfig& waveConfig);

private:
    void PublishSpawnEffectIfNeeded(int startIndex);
    void AttachMonsterToSceneIfNeeded(Monster* monster);

    GameManager* owner_;
    std::vector<Monster*> monsters_;
    std::vector<WaveConfig> waveConfigs_;
    int waveIndex_ = 0;
    int allWaveNum_ = 0;
    int allMonsterNum_ = 0;
};

