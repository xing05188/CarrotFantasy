/**
 * Refactored with Provider Pattern
 * 使用提供者模式重构
 * 
 * Design Pattern: Provider Pattern
 * 设计模式：提供者模式
 * 
 * Benefits:
 * 1. Provides a centralized access point to game flow information
 * 2. Decouples flow consumers from flow implementation details
 * 3. Enables easier testing by allowing mock providers to be injected
 * 4. Facilitates consistent flow control across different parts of the application
 * 
 * 好处：
 * 1. 提供游戏流程信息的集中访问点
 * 2. 将流程消费者与流程实现细节解耦
 * 3. 通过允许注入模拟提供者来简化测试
 * 4. 促进应用程序不同部分之间的一致流程控制
 * 
 * Problems Solved:
 * 1. Eliminates the need for multiple components to directly access the flow manager
 * 2. Reduces coupling between components and the flow management system
 * 3. Provides a consistent interface for accessing flow information
 * 4. Makes it easier to change the underlying flow implementation without affecting consumers
 * 
 * 解决的问题：
 * 1. 消除了多个组件直接访问流程管理器的需要
 * 2. 减少了组件与流程管理系统之间的耦合
 * 3. 为访问流程信息提供了一致的接口
 * 4. 使更改底层流程实现而不影响消费者变得更容易
 */
 // Refactored with Provider Pattern
 // 使用提供者模式重构
#include "GameFlowProvider.h"

#include "GameFlowController.h"

namespace carrot {
namespace core {
namespace state {

void GameFlowProvider::Set(const std::shared_ptr<GameFlowController>& controller) {
    std::lock_guard<std::mutex> guard(Mutex());
    InstanceRef() = controller;
}

std::shared_ptr<GameFlowController> GameFlowProvider::Get() {
    std::lock_guard<std::mutex> guard(Mutex());
    return InstanceRef();
}

std::shared_ptr<GameFlowController>& GameFlowProvider::InstanceRef() {
    static std::shared_ptr<GameFlowController> instance;
    return instance;
}

std::mutex& GameFlowProvider::Mutex() {
    static std::mutex mutex;
    return mutex;
}

}  // namespace state
}  // namespace core
}  // namespace carrot


