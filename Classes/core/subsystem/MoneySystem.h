/**
 * Subsystem Component of GameManager Facade
 * GameManager外观模式的子系统组件
 * 
 * Design Pattern: Facade Pattern (Subsystem Component)
 * 设计模式：外观模式（子系统组件）
 * 
 * Benefits:
 * 好处：
 * - Encapsulates all money-related functionality within a dedicated subsystem
 *   将所有金钱相关功能封装在专门的子系统中
 * - Provides a clean interface for the GameManager facade to interact with financial operations
 *   为GameManager外观提供与财务操作交互的清晰接口
 * - Enables independent evolution of money management without affecting other subsystems
 *   使金钱管理能够独立演进，而不影响其他子系统
 * - Simplifies money-related operations by abstracting complex implementation details
 *   通过抽象复杂的实现细节来简化金钱相关操作
 * - Promotes single responsibility principle by focusing solely on financial management
 *   通过专注于财务管理来促进单一职责原则
 * 
 * Problems Solved:
 * 解决的问题：
 * - Isolates money management logic from other game systems
 *   将金钱管理逻辑与其他游戏系统隔离
 * - Prevents money-related code from being scattered across multiple components
 *   防止金钱相关代码分散在多个组件中
 * - Provides a centralized point for financial state management and transaction control
 *   为财务状态管理和交易控制提供集中点
 * - Enables easier testing and debugging of money-specific functionality
 *   使金钱特定功能的测试和调试更容易
 * - Reduces coupling between financial operations and other game systems
 *   减少财务操作与其他游戏系统之间的耦合
 * 
 * Role in Facade Pattern:
 * 在外观模式中的角色：
 * - Acts as a subsystem component that is managed by the GameManager facade
 *   作为由GameManager外观管理的子系统组件
 * - Implements specific money-related functionality on behalf of the facade
 *   代表外观实现特定的金钱相关功能
 * - Hides the complexity of money management from the facade and clients
 *   向外观和客户端隐藏金钱管理的复杂性
 * - Provides specialized services that the facade can delegate to
 *   提供外观可以委托的专门服务
 */
 // Refactored with Manager Pattern
 // 使用管理者模式重构
 #pragma once

 #include <cstddef>

class GameManager;

class MoneySystem {
public:
    explicit MoneySystem(GameManager* owner);

    int GetMoney() const;
    void ChangeMoney(int delta);
    void SetMoney(int value, bool publishEvent = true);

private:
    void PublishMoneyChangedEvent(int delta);

    GameManager* owner_;
    int money_ = 0;
};

