# CarrotFantasy
A refactored tower defense game built with Cocos2d-x, inspired by "Protect the Carrot" (保卫萝卜).

## About
This project is a comprehensive refactoring of the [CattotFantasy](https://github.com/xing05188/CattotFantasy) game. The original project has been restructured with modern software design patterns to improve code maintainability, extensibility, and overall architecture quality.

## Refactoring Highlights
<font style="color:rgb(15, 17, 21);">The following table summarizes the main refactoring tasks, their corresponding design patterns, and application modules/components:</font>

| _**<font style="color:rgb(15, 17, 21);">Refactoring Task / Area</font>**_ | _**<font style="color:rgb(15, 17, 21);">Corresponding Design Pattern</font>**_ | _**<font style="color:rgb(15, 17, 21);">Pattern Type</font>**_ | _**<font style="color:rgb(15, 17, 21);">Application Module / Component</font>**_ | _**<font style="color:rgb(15, 17, 21);">Core Purpose</font>**_ |
| --- | --- | --- | --- | --- |
| **<font style="color:rgb(15, 17, 21);">Architecture Refactoring</font>** | <font style="color:rgb(15, 17, 21);">Layered Architecture Design</font> | <font style="color:rgb(15, 17, 21);">Not a specific pattern</font> | <font style="color:rgb(15, 17, 21);">Global Architecture (</font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">core/</font>`<br/><font style="color:rgb(15, 17, 21);">,</font><font style="color:rgb(15, 17, 21);"> </font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">entities/</font>`<br/><font style="color:rgb(15, 17, 21);">,</font><font style="color:rgb(15, 17, 21);"> </font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">scenes/</font>`<br/><font style="color:rgb(15, 17, 21);">,</font><font style="color:rgb(15, 17, 21);"> </font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">gameplay/</font>`<br/><font style="color:rgb(15, 17, 21);">)</font> | <font style="color:rgb(15, 17, 21);">Solve structural chaos, achieve separation of concerns and simplified dependencies</font> |
| **<font style="color:rgb(15, 17, 21);">Object Creation</font>** | **<font style="color:rgb(15, 17, 21);">Factory Pattern</font>** | <font style="color:rgb(15, 17, 21);">Creational Pattern (1/1)</font> | <font style="color:rgb(15, 17, 21);">Entity Layer (</font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">TowerFactory</font>`<br/><font style="color:rgb(15, 17, 21);">,</font><font style="color:rgb(15, 17, 21);"> </font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">MonsterFactory</font>`<br/><font style="color:rgb(15, 17, 21);">,</font><font style="color:rgb(15, 17, 21);"> </font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">ObstacleFactory</font>`<br/><font style="color:rgb(15, 17, 21);">)</font> | <font style="color:rgb(15, 17, 21);">Centralize object creation logic, decouple clients from concrete classes</font> |
| **<font style="color:rgb(15, 17, 21);">Game Manager</font>** | **<font style="color:rgb(15, 17, 21);">Facade Pattern</font>** | <font style="color:rgb(15, 17, 21);">Structural Pattern (1/2)</font> | <font style="color:rgb(15, 17, 21);">Core System (</font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">GameManager</font>`<br/><font style="color:rgb(15, 17, 21);">)</font> | <font style="color:rgb(15, 17, 21);">Decompose the "God Class" into a facade that manages subsystems, simplifying client interaction</font> |
| **<font style="color:rgb(15, 17, 21);">Boss Skill Implementation</font>** | **<font style="color:rgb(15, 17, 21);">Decorator Pattern</font>** | <font style="color:rgb(15, 17, 21);">Structural Pattern (2/2)</font> | <font style="color:rgb(15, 17, 21);">Monster Entity (</font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">BossSkill</font>`<br/><font style="color:rgb(15, 17, 21);">,</font><font style="color:rgb(15, 17, 21);"> </font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">SkillDecorator</font>`<br/><font style="color:rgb(15, 17, 21);">)</font> | <font style="color:rgb(15, 17, 21);">Dynamically combine and extend Boss skills, decouple skill logic from the monster class</font> |
| **<font style="color:rgb(15, 17, 21);">Defense Tower Attack</font>** | **<font style="color:rgb(15, 17, 21);">Strategy Pattern</font>** | <font style="color:rgb(15, 17, 21);">Behavioral Pattern (1/2)</font> | <font style="color:rgb(15, 17, 21);">Tower Entity (</font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">AttackStrategy</font>`<br/><font style="color:rgb(15, 17, 21);"> </font><font style="color:rgb(15, 17, 21);">interface)</font> | <font style="color:rgb(15, 17, 21);">Decouple attack algorithms from the tower entity, allowing runtime interchange of attack behaviors</font> |
| **<font style="color:rgb(15, 17, 21);">Game Flow Management & Carrot State</font>** | **<font style="color:rgb(15, 17, 21);">State Pattern</font>** | <font style="color:rgb(15, 17, 21);">Behavioral Pattern (2/2)</font> | <font style="color:rgb(15, 17, 21);">Core System (</font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">GameStateContext</font>`<br/><font style="color:rgb(15, 17, 21);">,</font><font style="color:rgb(15, 17, 21);"> </font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">GameFlowController</font>`<br/><font style="color:rgb(15, 17, 21);">) and Carrot Entity (</font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">Carrot</font>`<br/><font style="color:rgb(15, 17, 21);">)</font> | <font style="color:rgb(15, 17, 21);">Transform complex conditional logic into object-oriented design for explicit state transition management</font> |
| **<font style="color:rgb(15, 17, 21);">UI/Logic Decoupling</font>** | **<font style="color:rgb(15, 17, 21);">Observer Pattern</font>**<font style="color:rgb(15, 17, 21);"> </font><font style="color:rgb(15, 17, 21);">(via EventBus)</font> | <font style="color:rgb(15, 17, 21);">Behavioral Pattern (additional)</font> | <font style="color:rgb(15, 17, 21);">Gameplay Layer and UI Layer (</font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">EventBus</font>`<br/><font style="color:rgb(15, 17, 21);">,</font><font style="color:rgb(15, 17, 21);"> </font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">MoneySystem</font>`<br/><font style="color:rgb(15, 17, 21);">,</font><font style="color:rgb(15, 17, 21);"> </font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">MoneyHud</font>`<br/><font style="color:rgb(15, 17, 21);">)</font> | <font style="color:rgb(15, 17, 21);">Achieve publish/subscribe decoupling through an event bus, separate data changes from UI updates</font> |
| **<font style="color:rgb(15, 17, 21);">Scene Initialization</font>** | **<font style="color:rgb(15, 17, 21);">Template Method Pattern</font>** | <font style="color:rgb(15, 17, 21);">Behavioral Pattern (additional)</font> | <font style="color:rgb(15, 17, 21);">Scene Layer (</font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">BaseNavigableScene</font>`<br/><font style="color:rgb(15, 17, 21);">)</font> | <font style="color:rgb(15, 17, 21);">Eliminate duplicate initialization code across multiple scene classes, ensure UI consistency</font> |
| **<font style="color:rgb(15, 17, 21);">Projectile Management</font>** | **<font style="color:rgb(15, 17, 21);">Object Pool Pattern</font>** | <font style="color:rgb(15, 17, 21);">Additional Pattern</font> | <font style="color:rgb(15, 17, 21);">Projectile System (</font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">Projectile</font>`<br/><font style="color:rgb(15, 17, 21);"> </font><font style="color:rgb(15, 17, 21);">class,</font><font style="color:rgb(15, 17, 21);"> </font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">ObjectPool</font>`<br/><font style="color:rgb(15, 17, 21);"> </font><font style="color:rgb(15, 17, 21);">class)</font> | <font style="color:rgb(15, 17, 21);">Optimize performance by reusing projectile objects, eliminating memory/CPU overhead from frequent creation/destruction</font> |


<font style="color:rgb(15, 17, 21);">This project successfully applies the following design patterns to meet assignment requirements:</font>

+ **<font style="color:rgb(15, 17, 21);">Creational Pattern (Requirement: 1 type)</font>**<font style="color:rgb(15, 17, 21);">: Implemented the</font><font style="color:rgb(15, 17, 21);"> </font>**<font style="color:rgb(15, 17, 21);">Factory Pattern</font>**<font style="color:rgb(15, 17, 21);">.</font>
+ **<font style="color:rgb(15, 17, 21);">Structural Patterns (Requirement: 2 types)</font>**<font style="color:rgb(15, 17, 21);">: Implemented the</font><font style="color:rgb(15, 17, 21);"> </font>**<font style="color:rgb(15, 17, 21);">Facade Pattern</font>**<font style="color:rgb(15, 17, 21);"> </font><font style="color:rgb(15, 17, 21);">and</font><font style="color:rgb(15, 17, 21);"> </font>**<font style="color:rgb(15, 17, 21);">Decorator Pattern</font>**<font style="color:rgb(15, 17, 21);">.</font>
+ **<font style="color:rgb(15, 17, 21);">Behavioral Patterns (Requirement: 2 types)</font>**<font style="color:rgb(15, 17, 21);">: Implemented the</font><font style="color:rgb(15, 17, 21);"> </font>**<font style="color:rgb(15, 17, 21);">Strategy Pattern</font>**<font style="color:rgb(15, 17, 21);"> </font><font style="color:rgb(15, 17, 21);">and</font><font style="color:rgb(15, 17, 21);"> </font>**<font style="color:rgb(15, 17, 21);">State Pattern</font>**<font style="color:rgb(15, 17, 21);">.</font>
+ **<font style="color:rgb(15, 17, 21);">Additional, Our Choice</font>**<font style="color:rgb(15, 17, 21);">: Implemented the</font><font style="color:rgb(15, 17, 21);"> </font>**<font style="color:rgb(15, 17, 21);">Object Pool Pattern</font>**<font style="color:rgb(15, 17, 21);"> </font><font style="color:rgb(15, 17, 21);">for optimizing projectile management.</font>

**<font style="color:rgb(15, 17, 21);">Additionally</font>**<font style="color:rgb(15, 17, 21);">: Applied the </font>**<font style="color:rgb(15, 17, 21);">Observer Pattern</font>**<font style="color:rgb(15, 17, 21);"> (based on </font>`<font style="color:rgb(15, 17, 21);background-color:rgb(235, 238, 242);">EventBus</font>`<font style="color:rgb(15, 17, 21);">) for UI-logic decoupling and the </font>**<font style="color:rgb(15, 17, 21);">Template Method Pattern</font>**<font style="color:rgb(15, 17, 21);"> to unify scene initialization logic.</font>

<font style="color:rgb(15, 17, 21);"></font>

**Key Improvements**

+ **Decoupled Architecture**: Event-driven communication reduces tight coupling between components
+ **Better Code Organization**: Clear separation of concerns with dedicated subsystems
+ **Enhanced Extensibility**: Easy to add new tower types, monsters, and game features
+ **Improved Maintainability**: Design patterns make the codebase easier to understand and modify
+ **Modular Design**: Subsystems can be developed and tested independently

## Project Structure
```plain
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
```

## Building
The project uses CMake for building. Refer to the CMakeLists.txt file for build configuration.

## Technologies
+ **C++**: Core programming language
+ **Cocos2d-x**: Cross-platform game engine
+ **CMake**: Build system
+ **JSON**: Data serialization (saves, configurations)

## Features
+ Multiple tower types with unique attack strategies
+ Diverse monster types with different behaviors
+ Boss monsters with special abilities
+ Save/load game functionality
+ Level progression system
+ Audio system with background music and sound effects
+ Event-driven game architecture

## Related Project
This project is a refactoring of [CattotFantasy](https://github.com/xing05188/CattotFantasy).

