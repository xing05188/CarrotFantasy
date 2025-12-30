# CarrotFantasy

A refactored tower defense game built with **Cocos2d-x**, inspired by "Protect the Carrot" (保卫萝卜).

## About
This project is a comprehensive refactoring of the CattotFantasy game. The original project has been restructured with modern software design patterns to improve code maintainability, extensibility, and overall architecture quality.

## Refactoring Highlights
The following table summarizes the main refactoring tasks, their corresponding design patterns, and application modules/components:

| Refactoring Task / Area | Corresponding Design Pattern | Pattern Type | Application Module / Component | Core Purpose |
|------------------------|-----------------------------|--------------|--------------------------------|--------------|
| Architecture Refactoring | Layered Architecture Design | Not a specific pattern | Global Architecture (`core/`, `entities/`, `scenes/`, `gameplay/`) | Solve structural chaos, achieve separation of concerns and simplified dependencies |
| Object Creation | Factory Pattern | Creational Pattern (1/1) | Entity Layer (`TowerFactory`, `MonsterFactory`, `ObstacleFactory`) | Centralize object creation logic, decouple clients from concrete classes |
| Game Manager | Facade Pattern | Structural Pattern (1/2) | Core System (`GameManager`) | Decompose the "God Class" into a facade that manages subsystems, simplifying client interaction |
| Boss Skill Implementation | Decorator Pattern | Structural Pattern (2/2) | Monster Entity (`BossSkill`, `SkillDecorator`) | Dynamically combine and extend Boss skills, decouple skill logic from the monster class |
| Defense Tower Attack | Strategy Pattern | Behavioral Pattern (1/2) | Tower Entity (`AttackStrategy` interface) | Decouple attack algorithms from the tower entity, allowing runtime interchange of attack behaviors |
| Game Flow Management & Carrot State | State Pattern | Behavioral Pattern (2/2) | Core System (`GameStateContext`, `GameFlowController`) and Carrot Entity (`Carrot`) | Transform complex conditional logic into object-oriented design for explicit state transition management |
| UI/Logic Decoupling | Observer Pattern (via EventBus) | Behavioral Pattern (additional) | Gameplay Layer and UI Layer (`EventBus`, `MoneySystem`, `MoneyHud`) | Achieve publish/subscribe decoupling through an event bus, separate data changes from UI updates |
| Scene Initialization | Template Method Pattern | Behavioral Pattern (additional) | Scene Layer (`BaseNavigableScene`) | Eliminate duplicate initialization code across multiple scene classes, ensure UI consistency |
| Projectile Management | Object Pool Pattern | Additional Pattern | Projectile System (`Projectile` class, `ObjectPool` class) | Optimize performance by reusing projectile objects, eliminating memory/CPU overhead from frequent creation/destruction |

This project successfully applies the following design patterns to meet assignment requirements:
- **Creational Pattern** (Requirement: 1 type): Implemented the **Factory Pattern**.
- **Structural Patterns** (Requirement: 2 types): Implemented the **Facade Pattern** and **Decorator Pattern**.
- **Behavioral Patterns** (Requirement: 2 types): Implemented the **Strategy Pattern** and **State Pattern**.
- **Additional, Our Choice**: Implemented the **Object Pool Pattern** for optimizing projectile management.

**Additionally**: Applied the **Observer Pattern** (based on EventBus) for UI-logic decoupling and the **Template Method Pattern** to unify scene initialization logic.

## Key Improvements
- **Decoupled Architecture**: Event-driven communication reduces tight coupling between components
- **Better Code Organization**: Clear separation of concerns with dedicated subsystems
- **Enhanced Extensibility**: Easy to add new tower types, monsters, and game features
- **Improved Maintainability**: Design patterns make the codebase easier to understand and modify
- **Modular Design**: Subsystems can be developed and tested independently

## Project Structure
CarrotFantasy/
├── Classes/
│   ├── core/           # Core game systems (GameManager, EventBus, State management)
│   ├── entities/       # Game entities (Tower, Monster, Carrot, Obstacle)
│   ├── scenes/         # Game scenes and controllers
│   ├── gameplay/       # Gameplay logic and events
│   ├── ui/             # UI components and widgets
│   ├── audio/          # Audio system
│   └── config/         # Game configuration files
├── Resources/          # Game assets (sprites, maps, music, etc.)
└── cocos2d/           # Cocos2d-x engine
## Building
The project uses **CMake** for building. Refer to the `CMakeLists.txt` file for build configuration.

## Technologies
- **C++**: Core programming language
- **Cocos2d-x**: Cross-platform game engine
- **CMake**: Build system
- **JSON**: Data serialization (saves, configurations)

## Features
- Multiple tower types with unique attack strategies
- Diverse monster types with different behaviors
- Boss monsters with special abilities
- Save/load game functionality
- Level progression system
- Audio system with background music and sound effects
- Event-driven game architecture

## Related Project
This project is a refactoring of **CattotFantasy**.
