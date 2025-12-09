/**
 * Refactored with State Pattern
 * 使用状态模式重构
 * 
 * Design Pattern: State Pattern
 * 设计模式：状态模式
 * 
 * Benefits:
 * 1. Allows the game to change its behavior when in menu state
 * 2. Encapsulates menu-specific behavior into a separate state class
 * 3. Makes state transitions explicit and easier to manage
 * 4. Eliminates large conditional statements that depend on the game's state
 * 
 * 好处：
 * 1. 允许游戏在菜单状态下改变其行为
 * 2. 将菜单特定行为封装到单独的状态类中
 * 3. 使状态转换显式且更易于管理
 * 4. 消除了依赖于游戏状态的大型条件语句
 * 
 * Problems Solved:
 * 1. Avoids code duplication when implementing menu-specific behaviors
 * 2. Makes it easier to modify menu behavior without affecting other states
 * 3. Provides a clean way to manage transitions to and from the menu state
 * 4. Separates menu logic from the main game object, improving maintainability
 * 
 * 解决的问题：
 * 1. 避免在实现菜单特定行为时的代码重复
 * 2. 使修改菜单行为而不影响其他状态变得更容易
 * 3. 提供了一种管理进出菜单状态转换的清晰方式
 * 4. 将菜单逻辑与主游戏对象分离，提高了可维护性
 */
 // Refactored with State Pattern
 // 使用状态模式重构
 #pragma once

 #include "GameState.h"

namespace carrot {
namespace core {
namespace state {

// State Pattern: Menu screen controller.
class MenuState : public GameState {
public:
    void Enter(GameStateContext& context) override;
    void Exit(GameStateContext& context) override;
};

}  // namespace state
}  // namespace core
}  // namespace carrot


