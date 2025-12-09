/**
 * Refactored with Manager Pattern
 * 使用管理者模式重构
 * 
 * Design Pattern: Manager Pattern
 * 设计模式：管理者模式
 * 
 * Benefits:
 * 1. Centralizes the management of carrot entities in the game
 * 2. Provides a single point of control for carrot lifecycle and behavior
 * 3. Encapsulates complex carrot coordination logic
 * 4. Simplifies carrot-related operations by providing a unified interface
 * 
 * 好处：
 * 1. 集中管理游戏中的萝卜实体
 * 2. 为萝卜生命周期和行为提供单一控制点
 * 3. 封装复杂的萝卜协调逻辑
 * 4. 通过提供统一接口简化萝卜相关操作
 * 
 * Problems Solved:
 * 1. Eliminates scattered carrot management logic across multiple components
 * 2. Reduces coupling between carrot-related systems by centralizing their coordination
 * 3. Provides a consistent way to manage carrot state transitions
 * 4. Makes it easier to implement carrot-specific features like health management and upgrades
 * 
 * 解决的问题：
 * 1. 消除了分散在多个组件中的萝卜管理逻辑
 * 2. 通过集中协调减少萝卜相关系统之间的耦合
 * 3. 提供了一种管理萝卜状态转换的一致方法
 * 4. 使实现萝卜特定功能如健康管理和升级变得更容易
 */
 // Refactored with Manager Pattern
 // 使用管理者模式重构
#include "CarrotManager.h"

#include "cocos2d.h"
#include "entities/Carrot/Carrot.h"
#include "../EventBusProvider.h"
#include "../GameManager.h"
#include "../../gameplay/events/CarrotEvents.h"

USING_NS_CC;

CarrotManager::CarrotManager(GameManager* owner)
    : owner_(owner) {}

void CarrotManager::InitCarrot() {
    if (!owner_) {
        return;
    }
    int levelId = owner_->getLevelId();
    if (levelId <= 0) {
        CCLOG("CarrotManager::InitCarrot: Invalid level id");
        return;
    }
    const auto& dst1 = owner_->dst1;
    const auto& dst2 = owner_->dst2;
    if (static_cast<size_t>(levelId - 1) >= dst1.size() || static_cast<size_t>(levelId - 1) >= dst2.size()) {
        CCLOG("CarrotManager::InitCarrot: Level id %d out of destination range", levelId);
        return;
    }
    carrot_ = Carrot::create(10, dst1[levelId - 1], dst2[levelId - 1]);
    if (carrot_) {
        if (carrot_->getParent() == nullptr && owner_->currentScene) {
            owner_->currentScene->addChild(carrot_, 1);
        } else if (carrot_->getParent()) {
            CCLOG("CarrotManager::InitCarrot: carrot already has parent, skipping addChild.");
        }
    } else {
        CCLOG("CarrotManager::InitCarrot: Failed to create carrot.");
    }
    CCLOG("CARROT READY!");
}

void CarrotManager::Doudong() {
    if (!carrot_) {
        return;
    }
    if (carrot_->getHP() == carrot_->getMaxHP()) {
        auto bus = carrot::core::EventBusProvider::Get();
        if (!bus) {
            CCLOG("CarrotManager::Doudong: EventBus not available");
            return;
        }
        carrot::gameplay::events::CarrotShakeRequestedEvent evt{};
        int levelId = owner_->getLevelId();
        evt.x = owner_->dst1[levelId - 1].x;
        evt.y = owner_->dst1[levelId - 1].y;
        bus->Publish(carrot::gameplay::events::kCarrotShakeRequestedEventId, evt);
    }
}

Carrot* CarrotManager::GetCarrot() const {
    return carrot_;
}

void CarrotManager::Jineng1() {
    if (carrot_) {
        carrot_->getRecover();
    }
}

void CarrotManager::Jineng6() {
    if (carrot_) {
        carrot_->enterInvincibleState();
    }
}

