/**
 * Refactored with Provider Pattern
 * 使用提供者模式重构
 * 
 * Design Pattern: Provider Pattern
 * 设计模式：提供者模式
 * 
 * Benefits:
 * 1. Provides a centralized access point to event bus functionality
 * 2. Decouples event consumers from event bus implementation details
 * 3. Enables easier testing by allowing mock providers to be injected
 * 4. Facilitates consistent event handling across different parts of the application
 * 
 * 好处：
 * 1. 提供事件总线功能的集中访问点
 * 2. 将事件消费者与事件总线实现细节解耦
 * 3. 通过允许注入模拟提供者来简化测试
 * 4. 促进应用程序不同部分之间的一致事件处理
 * 
 * Problems Solved:
 * 1. Eliminates the need for multiple components to directly access the event bus
 * 2. Reduces coupling between components and the event management system
 * 3. Provides a consistent interface for event subscription and publishing
 * 4. Makes it easier to change the underlying event bus implementation without affecting consumers
 * 
 * 解决的问题：
 * 1. 消除了多个组件直接访问事件总线的需要
 * 2. 减少了组件与事件管理系统之间的耦合
 * 3. 为事件订阅和发布提供了一致的接口
 * 4. 使更改底层事件总线实现而不影响消费者变得更容易
 */
 // Refactored with Provider Pattern
 // 使用提供者模式重构
#pragma once

#include "EventBus.h"

#include <memory>
#include <mutex>

namespace carrot {
namespace core {

class EventBusProvider {
public:
    static void Set(const std::shared_ptr<IEventBus>& bus) {
        std::lock_guard<std::mutex> guard(Mutex());
        InstanceRef() = bus;
    }

    static std::shared_ptr<IEventBus> Get() {
        std::lock_guard<std::mutex> guard(Mutex());
        std::shared_ptr<IEventBus>& instance = InstanceRef();
        if (!instance) {
            instance = CreateDefaultUnlocked();
        }
        return instance;
    }

private:
    static std::shared_ptr<IEventBus>& InstanceRef() {
        static std::shared_ptr<IEventBus> instance;
        return instance;
    }

    static std::mutex& Mutex() {
        static std::mutex mutex;
        return mutex;
    }

    static std::shared_ptr<IEventBus> CreateDefaultUnlocked() {
        return std::shared_ptr<IEventBus>(new SimpleEventBus());
    }
};

}  // namespace core
}  // namespace carrot

