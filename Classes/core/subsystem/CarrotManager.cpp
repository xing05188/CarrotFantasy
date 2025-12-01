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

