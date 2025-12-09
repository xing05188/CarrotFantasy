/**
 * Refactored with State Pattern
 * 使用状态模式重构
 * 
 * Design Pattern: State Pattern
 * 设计模式：状态模式
 * 
 * Benefits:
 * 1. Allows an object to change its behavior when its internal state changes
 * 2. Encapsulates state-specific behavior into separate state classes
 * 3. Makes state transitions explicit and easier to manage
 * 4. Eliminates large conditional statements that depend on an object's state
 * 
 * 好处：
 * 1. 允许对象在其内部状态改变时改变其行为
 * 2. 将特定于状态的行为封装到单独的状态类中
 * 3. 使状态转换显式且更易于管理
 * 4. 消除了依赖于对象状态的大型条件语句
 * 
 * Problems Solved:
 * 1. Avoids code duplication when implementing similar behaviors in different states
 * 2. Makes it easier to add new states without modifying existing code
 * 3. Provides a clean way to manage complex state transitions
 * 4. Separates state logic from the main game object, improving maintainability
 * 
 * 解决的问题：
 * 1. 避免在不同状态下实现类似行为时的代码重复
 * 2. 使添加新状态而无需修改现有代码变得更容易
 * 3. 提供了一种管理复杂状态转换的清晰方式
 * 4. 将状态逻辑与主游戏对象分离，提高了可维护性
 */
 // Refactored with State Pattern
 // 使用状态模式重构
 #pragma once

 #include <memory>

namespace carrot {
namespace core {
namespace state {

class GameStateContext;

class GameState {
public:
    virtual ~GameState() {}
    virtual void Enter(GameStateContext& context) = 0;
    virtual void Exit(GameStateContext& context) = 0;
    virtual void Pause(GameStateContext& context) {}
    virtual void Resume(GameStateContext& context) {}
};

class GameStateContext {
public:
    GameStateContext();

    void SetState(std::shared_ptr<GameState> state);
    void Pause();
    void Resume();

private:
    std::shared_ptr<GameState> currentState_;
};

}  // namespace state
}  // namespace core
}  // namespace carrot

