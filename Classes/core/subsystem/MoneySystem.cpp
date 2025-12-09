/**
 * Refactored with Manager Pattern
 * 使用管理者模式重构
 * 
 * Design Pattern: Manager Pattern
 * 设计模式：管理者模式
 * 
 * Benefits:
 * 1. Centralizes the management of game currency and transactions
 * 2. Provides a single point of control for money-related operations
 * 3. Encapsulates complex financial coordination logic
 * 4. Simplifies money-related operations by providing a unified interface
 * 
 * 好处：
 * 1. 集中管理游戏货币和交易
 * 2. 为金钱相关操作提供单一控制点
 * 3. 封装复杂的财务协调逻辑
 * 4. 通过提供统一接口简化金钱相关操作
 * 
 * Problems Solved:
 * 1. Eliminates scattered money management logic across multiple components
 * 2. Reduces coupling between money-related systems by centralizing their coordination
 * 3. Provides a consistent way to manage financial transactions
 * 4. Makes it easier to implement money-specific features like rewards and purchases
 * 
 * 解决的问题：
 * 1. 消除了分散在多个组件中的金钱管理逻辑
 * 2. 通过集中协调减少金钱相关系统之间的耦合
 * 3. 提供了一种管理财务交易的一致方法
 * 4. 使实现金钱特定功能如奖励和购买变得更容易
 */
 // Refactored with Manager Pattern
 // 使用管理者模式重构
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

