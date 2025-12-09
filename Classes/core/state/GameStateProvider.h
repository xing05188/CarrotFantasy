/**
 * Refactored with Provider Pattern
 * 使用提供者模式重构
 * 
 * Design Pattern: Provider Pattern
 * 设计模式：提供者模式
 * 
 * Benefits:
 * 1. Provides a centralized access point to game state information
 * 2. Decouples state consumers from state implementation details
 * 3. Enables easier testing by allowing mock providers to be injected
 * 4. Facilitates consistent state access across different parts of the application
 * 
 * 好处：
 * 1. 提供游戏状态信息的集中访问点
 * 2. 将状态消费者与状态实现细节解耦
 * 3. 通过允许注入模拟提供者来简化测试
 * 4. 促进应用程序不同部分之间的一致状态访问
 * 
 * Problems Solved:
 * 1. Eliminates the need for multiple components to directly access the state manager
 * 2. Reduces coupling between components and the state management system
 * 3. Provides a consistent interface for accessing state information
 * 4. Makes it easier to change the underlying state implementation without affecting consumers
 * 
 * 解决的问题：
 * 1. 消除了多个组件直接访问状态管理器的需要
 * 2. 减少了组件与状态管理系统之间的耦合
 * 3. 为访问状态信息提供了一致的接口
 * 4. 使更改底层状态实现而不影响消费者变得更容易
 */
 // Refactored with Provider Pattern
 // 使用提供者模式重构
#pragma once

#include "GameState.h"

#include <memory>
#include <mutex>

namespace carrot {
namespace core {
namespace state {

class GameStateProvider {
public:
    static void Set(const std::shared_ptr<GameStateContext>& context) {
        std::lock_guard<std::mutex> guard(Mutex());
        InstanceRef() = context;
    }

    static std::shared_ptr<GameStateContext> Get() {
        std::lock_guard<std::mutex> guard(Mutex());
        std::shared_ptr<GameStateContext>& instance = InstanceRef();
        if (!instance) {
            instance = std::make_shared<GameStateContext>();
        }
        return instance;
    }

private:
    static std::shared_ptr<GameStateContext>& InstanceRef() {
        static std::shared_ptr<GameStateContext> instance;
        return instance;
    }

    static std::mutex& Mutex() {
        static std::mutex mutex;
        return mutex;
    }
};

}  // namespace state
}  // namespace core
}  // namespace carrot


