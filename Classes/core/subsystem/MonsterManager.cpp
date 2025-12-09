/**
 * Refactored with Manager Pattern
 * 使用管理者模式重构
 * 
 * Design Pattern: Manager Pattern
 * 设计模式：管理者模式
 * 
 * Benefits:
 * 1. Centralizes the management of monster entities in the game
 * 2. Provides a single point of control for monster lifecycle and behavior
 * 3. Encapsulates complex monster coordination logic
 * 4. Simplifies monster-related operations by providing a unified interface
 * 
 * 好处：
 * 1. 集中管理游戏中的怪物实体
 * 2. 为怪物生命周期和行为提供单一控制点
 * 3. 封装复杂的怪物协调逻辑
 * 4. 通过提供统一接口简化怪物相关操作
 * 
 * Problems Solved:
 * 1. Eliminates scattered monster management logic across multiple components
 * 2. Reduces coupling between monster-related systems by centralizing their coordination
 * 3. Provides a consistent way to manage monster state transitions
 * 4. Makes it easier to implement monster-specific features like spawning and pathfinding
 * 
 * 解决的问题：
 * 1. 消除了分散在多个组件中的怪物管理逻辑
 * 2. 通过集中协调减少怪物相关系统之间的耦合
 * 3. 提供了一种管理怪物状态转换的一致方法
 * 4. 使实现怪物特定功能如生成和寻路变得更容易
 */
 // Refactored with Manager Pattern
 // 使用管理者模式重构
#include "MonsterManager.h"

#include "Music.h"
#include "../EventBusProvider.h"
#include "../GameManager.h"
#include "../../gameplay/events/MonsterEvents.h"
#include "../json/document.h"
#include "../json/rapidjson.h"

USING_NS_CC;

MonsterManager::MonsterManager(GameManager* owner)
    : owner_(owner),
      monsters_(MAX_MONSTER_NUM) {
}

void MonsterManager::ResetForLevel() {
    monsters_.clear();
    waveConfigs_.clear();
    waveIndex_ = 0;
    ClearMonsters();
    ResetAllMonsterNum();
    allWaveNum_ = 0;
}

void MonsterManager::LoadMonsterResources() {
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

void MonsterManager::LoadMonsterWaveConfig(const std::string& filename, const std::string& levelName) {
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
            waveConfigs_.push_back(waveConfig);
            allWaveNum_ = static_cast<int>(waveConfigs_.size());
            CCLOG("Wave %d - Monster: %s, Count: %d, Spawn Interval: %.2f - %.2f",
                static_cast<int>(waveConfigs_.size()),
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
    for (const auto& waveConfig : waveConfigs_) {
        allMonsterNum_ += waveConfig.count;
    }
    CCLOG("All Num %d ", allMonsterNum_);
}

void MonsterManager::ProduceMonsters(const std::string& monsterName, int startIndex, int health, bool pause) {
    Music::getInstance()->born_music();
    PublishSpawnEffectIfNeeded(startIndex);
    const auto& screenPath = owner_->GetScreenPath();
    auto monster = Monster::create(monsterName, screenPath, startIndex, pause);
    if (!monster) {
        CCLOG("Failed to create monster.");
        return;
    }
    if (monster->getParent()) {
        auto parent = monster->getParent();
        const char* pname = parent->getName().c_str();
        CCLOG("MonsterManager::ProduceMonsters: Created monster already has parent! Monster=%p, parent=%p, parentName=%s", monster, parent, pname);
    } else {
        CCLOG("MonsterManager::ProduceMonsters: Created monster has no parent yet. Monster=%p", monster);
    }
    monsters_.push_back(monster);
    auto bus = carrot::core::EventBusProvider::Get();
    if (bus) {
        carrot::gameplay::events::MonsterSpawnedEvent evt{};
        evt.monster = monster;
        bus->Publish(carrot::gameplay::events::kMonsterSpawnedEventId, evt);
    }
    AttachMonsterToSceneIfNeeded(monster);
    monster->setPause(pause);
    CCLOG("MONSTER PAUSE  %d", monster->getPause());
    if (health != -1) {
        monster->setHealth(health);
    }
    if (monsterName.find("Boss") == 0) {
        monster->SpecialAttack();
    }
}

void MonsterManager::PublishSpawnEffectIfNeeded(int startIndex) {
    if (!owner_ || startIndex != 0) {
        return;
    }
    const auto& screenPath = owner_->GetScreenPath();
    if (screenPath.empty()) {
        CCLOG("MonsterManager::PublishSpawnEffectIfNeeded: screenPath is empty.");
        return;
    }
    owner_->playSpawnEffect(screenPath[0]);
}

void MonsterManager::AttachMonsterToSceneIfNeeded(Monster* monster) {
    if (!monster) {
        return;
    }
    if (monster->getParent() == nullptr) {
        if (owner_->currentScene) {
            CCLOG("MonsterManager::AttachMonsterToSceneIfNeeded: No subscriber added monster, attaching to currentScene. Monster=%p", monster);
            owner_->currentScene->addChild(monster);
        } else {
            CCLOG("MonsterManager::AttachMonsterToSceneIfNeeded: currentScene is null, cannot attach monster. Monster=%p", monster);
        }
    } else if (monster->getParent() != owner_->currentScene) {
        auto oldParent = monster->getParent();
        CCLOG("MonsterManager::AttachMonsterToSceneIfNeeded: Monster has parent %p (name=%s), reparenting to currentScene %p. Monster=%p",
            oldParent, oldParent->getName().c_str(), owner_->currentScene, monster);
        oldParent->removeChild(monster);
        if (owner_->currentScene) {
            owner_->currentScene->addChild(monster);
        }
    }
}

void MonsterManager::ProduceMonsterWave(const WaveConfig& waveConfig) {
    float delay = 0;
    CCLOG("%d   %d", waveIndex_, allWaveNum_);
    for (int i = 0; i < waveConfig.count; ++i) {
        delay += cocos2d::RandomHelper::random_real(waveConfig.spawnInterval[0], waveConfig.spawnInterval[1]);

        cocos2d::Director::getInstance()->getScheduler()->schedule([=](float) {
            ProduceMonsters(waveConfig.monsterName, 0, -1, false);
            }, owner_, 0, 0, delay, false, "produceMonster" + std::to_string(i));
    }
}

void MonsterManager::StartMonsterWaves() {
    CCLOG("Starting wave %d", waveIndex_);
    ProduceMonsterWave(waveConfigs_[waveIndex_]);
    cocos2d::Director::getInstance()->getScheduler()->schedule([this](float) {
        if (waveIndex_ >= static_cast<int>(waveConfigs_.size()) - 1) {
            CCLOG("All waves are complete.");
            cocos2d::Director::getInstance()->getScheduler()->unschedule("startWave", owner_);
            return;
        }
        ++waveIndex_;
        CCLOG("Starting wave %d", waveIndex_);
        ProduceMonsterWave(waveConfigs_[waveIndex_]);

        }, owner_, 15.0f, false, "startWave");
}

void MonsterManager::ApplyMonsterSpeed(float speedFactor) {
    for (auto* monster : monsters_) {
        if (!monster) continue;
        if (monster->getHealth() <= 0) continue;
        if (monster->speedaction) {
            monster->speedaction->setSpeed(speedFactor);
        }
    }
}

void MonsterManager::KillAllMonsters() {
    auto bus = carrot::core::EventBusProvider::Get();
    for (auto* monster : monsters_) {
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

void MonsterManager::ClearMonsters() {
    for (auto monster : monsters_) {
        if (monster->getParent()) {
            monster->getParent()->removeChild(monster);
        }
        monster->stopAllActions();
        monster->release();
    }
    monsters_.clear();
}

std::vector<Monster*>& MonsterManager::GetMonsters() {
    return monsters_;
}

int MonsterManager::GetAllMonsterNum() const {
    return allMonsterNum_;
}

void MonsterManager::IncrementAllMonsterNum(int delta) {
    allMonsterNum_ += delta;
}

void MonsterManager::ResetAllMonsterNum() {
    allMonsterNum_ = 0;
}

int MonsterManager::GetAllWaveNum() const {
    return allWaveNum_;
}

void MonsterManager::SetAllWaveNum(int value) {
    allWaveNum_ = value;
}

int MonsterManager::GetCurrentWaveIndex() const {
    return waveIndex_;
}

void MonsterManager::SetCurrentWaveIndex(int value) {
    waveIndex_ = value;
}

void MonsterManager::IncrementWaveIndex() {
    ++waveIndex_;
}

std::vector<WaveConfig>& MonsterManager::GetWaveConfigs() {
    return waveConfigs_;
}

