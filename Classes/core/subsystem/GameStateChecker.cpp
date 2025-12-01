#include "GameStateChecker.h"

#include "cocos2d.h"
#include "../EventBusProvider.h"
#include "../GameManager.h"
#include "MonsterManager.h"
#include "../../gameplay/events/GameFlowEvents.h"

USING_NS_CC;

GameStateChecker::GameStateChecker(GameManager* owner)
    : owner_(owner) {}

void GameStateChecker::Reset() {
    hasGameWon_ = false;
    hasGameLost_ = false;
}

bool GameStateChecker::CheckLose() {
    if (!owner_) {
        return false;
    }
    auto* carrot = owner_->getCarrot();
    if (!carrot) {
        return false;
    }
    if (carrot->getHP() <= 0) {
        CCLOG("LOSE THE GAME!");
        PublishGameLostEvent();
        return true;
    }
    return false;
}

bool GameStateChecker::CheckWin() {
    if (!owner_) {
        return false;
    }
    if (owner_->getCurrentWaveIndex() + 1 < owner_->getAllWaveNum()) return false;
    auto* carrot = owner_->getCarrot();
    if (!carrot || carrot->getHP() <= 0) return false;
    auto& monsters = owner_->GetMonsters();
    CCLOG("------------------------------------%d", static_cast<int>(monsters.size()));
    if (monsters.size() < static_cast<size_t>(owner_->getAllMonsterNum())) return false;
    for (auto* monster : monsters) {
        if (monster && monster->getHealth() > 0) return false;
    }

    CCLOG("WIN THE GAME!");
    PublishGameWonEvent();
    return true;
}

void GameStateChecker::PublishGameWonEvent() {
    if (hasGameWon_ || hasGameLost_) {
        return;
    }
    hasGameWon_ = true;
    carrot::gameplay::events::GameWonEvent evt{};
    evt.currentWave = owner_->getCurrentWaveNum();
    evt.totalWave = owner_->getAllWaveNum();
    evt.levelId = owner_->getLevelId();
    auto bus = carrot::core::EventBusProvider::Get();
    if (bus) {
        bus->Publish(carrot::gameplay::events::kGameWonEventId, evt);
    }
}

void GameStateChecker::PublishGameLostEvent() {
    if (hasGameLost_ || hasGameWon_) {
        return;
    }
    hasGameLost_ = true;
    carrot::gameplay::events::GameLostEvent evt{};
    evt.currentWave = owner_->getCurrentWaveNum();
    evt.totalWave = owner_->getAllWaveNum();
    evt.levelId = owner_->getLevelId();
    auto bus = carrot::core::EventBusProvider::Get();
    if (bus) {
        bus->Publish(carrot::gameplay::events::kGameLostEventId, evt);
    }
}

