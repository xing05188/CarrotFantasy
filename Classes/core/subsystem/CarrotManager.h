/**
 * Subsystem Component of GameManager Facade
 * GameManager外观模式的子系统组件
 * 
 * Design Pattern: Facade Pattern (Subsystem Component)
 * 设计模式：外观模式（子系统组件）
 * 
 * Benefits:
 * 好处：
 * - Encapsulates all carrot-related functionality within a dedicated subsystem
 *   将所有萝卜相关功能封装在专门的子系统中
 * - Provides a clean interface for the GameManager facade to interact with carrot operations
 *   为GameManager外观提供与萝卜操作交互的清晰接口
 * - Enables independent evolution of carrot management without affecting other subsystems
 *   使萝卜管理能够独立演进，而不影响其他子系统
 * - Simplifies carrot-related operations by abstracting complex implementation details
 *   通过抽象复杂的实现细节来简化萝卜相关操作
 * - Promotes single responsibility principle by focusing solely on carrot management
 *   通过专注于萝卜管理来促进单一职责原则
 * 
 * Problems Solved:
 * 解决的问题：
 * - Isolates carrot management logic from other game systems
 *   将萝卜管理逻辑与其他游戏系统隔离
 * - Prevents carrot-related code from being scattered across multiple components
 *   防止萝卜相关代码分散在多个组件中
 * - Provides a centralized point for carrot state management and lifecycle control
 *   为萝卜状态管理和生命周期控制提供集中点
 * - Enables easier testing and debugging of carrot-specific functionality
 *   使萝卜特定功能的测试和调试更容易
 * - Reduces coupling between carrot operations and other game systems
 *   减少萝卜操作与其他游戏系统之间的耦合
 * 
 * Role in Facade Pattern:
 * 在外观模式中的角色：
 * - Acts as a subsystem component that is managed by the GameManager facade
 *   作为由GameManager外观管理的子系统组件
 * - Implements specific carrot-related functionality on behalf of the facade
 *   代表外观实现特定的萝卜相关功能
 * - Hides the complexity of carrot management from the facade and clients
 *   向外观和客户端隐藏萝卜管理的复杂性
 * - Provides specialized services that the facade can delegate to
 *   提供外观可以委托的专门服务
 */
 // Refactored with Manager Pattern
 // 使用管理者模式重构
 #pragma once

 #include <memory>

class Carrot;
class GameManager;

class CarrotManager {
public:
    explicit CarrotManager(GameManager* owner);

    void InitCarrot();
    void Doudong();
    Carrot* GetCarrot() const;
    void Jineng1();
    void Jineng6();

private:
    GameManager* owner_;
    Carrot* carrot_ = nullptr;
};

