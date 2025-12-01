#include "MoneySystem.h"

#include "cocos2d.h"
#include "../EventBusProvider.h"
#include "../../gameplay/events/MoneyEvents.h"
#include "../GameManager.h"

USING_NS_CC;

MoneySystem::MoneySystem(GameManager* owner)
    : owner_(owner) {
}

int MoneySystem::GetMoney() const {
    return money_;
}

void MoneySystem::ChangeMoney(int delta) {
    SetMoney(money_ + delta);
}

void MoneySystem::SetMoney(int value, bool publishEvent) {
    int oldMoney = money_;
    int delta = value - money_;
    money_ = value;
    CCLOG("MoneySystem::SetMoney: %d -> %d (delta: %d, publishEvent: %s)",
        oldMoney, money_, delta, publishEvent ? "true" : "false");
    if (publishEvent) {
        PublishMoneyChangedEvent(delta);
    }
}

void MoneySystem::PublishMoneyChangedEvent(int delta) {
    carrot::gameplay::events::MoneyChangedEvent evt{};
    evt.delta = delta;
    evt.current = money_;
    auto bus = carrot::core::EventBusProvider::Get();
    if (bus) {
        bus->Publish(carrot::gameplay::events::kMoneyChangedEventId, evt);
    }
}

