/**
 * Subsystem Component of GameManager Facade
 * GameManager外观模式的子系统组件
 * 
 * Design Pattern: Facade Pattern (Subsystem Component)
 * 设计模式：外观模式（子系统组件）
 * 
 * Benefits:
 * 好处：
 * - Encapsulates all monster-related functionality within a dedicated subsystem
 *   将所有怪物相关功能封装在专门的子系统中
 * - Provides a clean interface for the GameManager facade to interact with monster operations
 *   为GameManager外观提供与怪物操作交互的清晰接口
 * - Enables independent evolution of monster management without affecting other subsystems
 *   使怪物管理能够独立演进，而不影响其他子系统
 * - Simplifies monster-related operations by abstracting complex implementation details
 *   通过抽象复杂的实现细节来简化怪物相关操作
 * - Promotes single responsibility principle by focusing solely on monster management
 *   通过专注于怪物管理来促进单一职责原则
 * 
 * Problems Solved:
 * 解决的问题：
 * - Isolates monster management logic from other game systems
 *   将怪物管理逻辑与其他游戏系统隔离
 * - Prevents monster-related code from being scattered across multiple components
 *   防止怪物相关代码分散在多个组件中
 * - Provides a centralized point for monster state management and lifecycle control
 *   为怪物状态管理和生命周期控制提供集中点
 * - Enables easier testing and debugging of monster-specific functionality
 *   使怪物特定功能的测试和调试更容易
 * - Reduces coupling between monster operations and other game systems
 *   减少怪物操作与其他游戏系统之间的耦合
 * 
 * Role in Facade Pattern:
 * 在外观模式中的角色：
 * - Acts as a subsystem component that is managed by the GameManager facade
 *   作为由GameManager外观管理的子系统组件
 * - Implements specific monster-related functionality on behalf of the facade
 *   代表外观实现特定的怪物相关功能
 * - Hides the complexity of monster management from the facade and clients
 *   向外观和客户端隐藏怪物管理的复杂性
 * - Provides specialized services that the facade can delegate to
 *   提供外观可以委托的专门服务
 */
 // Refactored with Manager Pattern
 // 使用管理者模式重构
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

