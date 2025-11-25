# 核心 EventBus 说明

## 背景

- 旧逻辑里 UI、场景、玩法系统彼此直接持有指针，造成强耦合与生命周期难题。
- 新架构需要一个可在 UI、玩法、基础设施之间传递消息的轻量媒介，以便实现 HUD 更新、技能广播、波次事件等。
- 为适配课程要求的 Observer 模式、支持日志采集与 AI 调试，需要提供多订阅者广播能力。

## 模块位置

```
Classes/core/EventBus.h
Classes/core/EventBusProvider.h
Classes/gameplay/events/MoneyEvents.h
```

## 设计概览

- `carrot::core::Event`：所有事件的基类，具体事件通过继承携带数据。
- `IEventBus`：面向依赖倒置的接口，供 DI/工厂注入。
- `SimpleEventBus`：线程安全实现，使用 `std::mutex + copy-on-publish` 策略防止发布过程中修改集合导致崩溃。
- `Subscription`：返回给调用者的可释放句柄，析构时自动取消订阅，避免忘记移除回调的泄漏问题。

## 使用示例

```cpp
struct TowerBuiltEvent : public carrot::core::Event {
    int towerId;
    cocos2d::Vec2 position;
};

auto bus = std::make_shared<carrot::core::SimpleEventBus>();

auto subscription = bus->Subscribe("tower.built", [](const carrot::core::Event& base) {
    auto& evt = static_cast<const TowerBuiltEvent&>(base);
    CCLOG("Tower %d built at (%f, %f)", evt.towerId, evt.position.x, evt.position.y);
});

TowerBuiltEvent event{42, {10.f, 20.f}};
bus->Publish("tower.built", event);
```

## 首个落地事件：金币变更

- `BaseLevelScene::updateMoney` 仅负责更新数值并广播 `money.changed`。
- `BaseLevelScene::bindMoneyEvents` 在初始化 UI 时订阅 `money.changed`，回调里刷新 `moneyLable`，实现“事件→UI”链路。
- 事件定义 `MoneyChangedEvent` 位于 `src/gameplay/events/MoneyEvents.h`，包含 `delta`（本次变化量）与 `current`（最新余额）。
- `core/EventBusProvider` 暂时代管全局总线实例，确保旧代码无需依赖 DI 也能获取事件总线。
- HUD、经济/任务系统可通过 `Subscribe(kMoneyChangedEventId, handler)` 监听金币更新，后续可以逐步迁出对 `BaseLevelScene` 的直接依赖。

## 萝卜 HP / 死亡事件

- `Carrot` 在 `changeHP` 中更新血量后发布 `carrot.hp.changed` 事件（`CarrotHpChangedEvent`），携带 `current/delta/max` 三个指标。
- 当血量首次降到 0，会额外广播 `carrot.died` 事件，供 GameState/存档/统计订阅。
- 新的 `ui/widgets/CarrotHpHud` 组件订阅 `carrot.hp.changed`，使用 `HP: current/max` 文本展示当前状态，验证 HUD 模块拆分思路。

## 下一步建议

1. `core/ServiceLocator` 或 DI 容器负责提供单例 `IEventBus`。
2. 封装常用事件 ID（例如 `events::kTowerBuilt`）避免字符串拼写错误。
3. 为关键模块（HUD、Economy、Snapshots）编写事件契约文档，统一数据模型。

