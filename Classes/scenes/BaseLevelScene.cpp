#include "BaseLevelScene.h"
#include<vector>
#include"MonsterConfigs.h"
#include "json/document.h"
#include "json/rapidjson.h"
#include "ui/CocosGUI.h"
#include"music.h"
#include"themeScene.h"
#include"GameManager.h"
#include "scenes/controllers/SkillController.h"
#include "scenes/controllers/GameMenuController.h"
#include "scenes/controllers/GameSpeedController.h"
#include<string>
#include "json/writer.h"
#include"obpos.h"
#include "json/stringbuffer.h"
#include<fstream>
#include "core/state/GameState.h"
#include "core/state/GameFlowProvider.h"
#include "core/state/GameFlowController.h"
#include "core/state/GameStateProvider.h"
#include "core/state/PausedState.h"
#include "core/state/MenuState.h"
#include "core/state/LevelSelectState.h"
#include "core/state/GameOverState.h"
#include "core/EventBusProvider.h"
#include "ui/widgets/MoneyHud.h"
#include "gameplay/events/GameFlowEvents.h"
#include "gameplay/events/MonsterEvents.h"
#include "gameplay/events/CarrotEvents.h"
#include "entities/Tower/TowerFactory.h"
using namespace rapidjson;
using namespace ui;
//#define DEBUG_MODE
#define CELL_SIZE 64
const Color3B moneyColor(154, 101, 25);

// 运行时全局倍率：怪物行进速度相关
extern float beishu;
// 每一关是否已经通关的标记，用于关卡解锁显示
extern bool level_is_win[3];
// 是否是新游戏（而不是读档进入）的标记
extern bool isNewGame[3];
// 当前关卡共享的 GameManager 指针（后续可逐步移除全局）
GameManager* manager;
// 全局攻击间隔加速因子（影响所有塔）
float tower_jiasu = 1;
// 关卡对应的地图资源文件列表，下标与 levelId 对应
const std::vector<std::string> BaseLevelScene::mapFiles = {
    "map/map1.tmx",
    "map/map2.tmx",
    "map/map3.tmx",
};


// 资源加载失败时打印提示，帮助定位缺失资源
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BaseLevelScene.cpp\n");
}
// 该场景只通过 GameManager 获取金币，避免本地重复状态
int BaseLevelScene::getMoney() const {
    if (manager) {
        return manager->GetMoney();
    }
    auto instance = GameManager::getInstance();
    return instance ? instance->GetMoney() : 0;
}

// 金币变动全部委托 GameManager 处理，HUD 依赖事件刷新
void BaseLevelScene::updateMoney(int add) {
    if (manager) {
        manager->ChangeMoney(add);
        return;
    }
    auto instance = GameManager::getInstance();
    if (instance) {
        instance->ChangeMoney(add);
    }
}

void BaseLevelScene::updateCurrentWaveLabe() {
    _curNumberLabel->setString(StringUtils::format("%d", std::min(manager->getCurrentWaveNum() + 1, manager->getAllWaveNum())));
}
/******************************************/

// 切换 1 倍 / 2 倍速度（已迁移到 GameSpeedController）
void BaseLevelScene::doublespeed(Ref* pSender) {
    if (speedController_) {
        speedController_->toggleSpeed(pSender);
        isDoubleSpeed = speedController_->isDoubleSpeed();  // 同步状态
    }
}
// 暂停 / 继续按钮：同时控制本地 UI 和 GameFlow 状态机
void BaseLevelScene::pause_all(Ref* pSender) {
    isGamePaused = !isGamePaused;
    Music::getInstance()->button_music();
    MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
    if (isGamePaused) {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/continueButton.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/continueButton.png"));
        auto pauseTop = Sprite::create("CarrotGuardRes/UI/pausing.png");
        pauseTop->setName("pauseTop");
        pauseTop->setPosition(464, 610);
        pauseTop->setScale(2.0f);
        this->addChild(pauseTop, 10);
    } else {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/pauseButton.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/pauseButton.png"));
        this->removeChildByName("pauseTop");
    }

    auto flowController = carrot::core::state::GameFlowProvider::Get();
    if (flowController) {
        flowController->SetPaused(isGamePaused);
        return;
    }

    CCLOG("BaseLevelScene: GameFlowController not available, falling back to direct pause handling");
    static std::shared_ptr<carrot::core::state::GameState> fallbackPausedState =
        std::make_shared<carrot::core::state::PausedState>();
    auto context = carrot::core::state::GameStateProvider::Get();
    if (context) {
        context->SetState(isGamePaused ? fallbackPausedState : nullptr);
    }
}

// 右下角问号：弹出多页新手引导 / 说明界面（已迁移到 GameMenuController）
void BaseLevelScene::wenhao(Ref* pSender) {
    if (menuController_) {
        menuController_->showHelpMenu(pSender);
    }
}
// 游戏菜单（已迁移到 GameMenuController）
void BaseLevelScene::menu_all(Ref* pSender) {
    if (menuController_) {
        menuController_->showGameMenu(pSender);
    }
}

void BaseLevelScene::transitionToMenuState() {
    Director::getInstance()->resume();
    auto flowController = carrot::core::state::GameFlowProvider::Get();
    if (flowController) {
        flowController->TransitionToMenu();
        return;
    }

    CCLOG("BaseLevelScene: GameFlowController not available, falling back to direct state switch");
    auto context = carrot::core::state::GameStateProvider::Get();
    if (context) {
        context->SetState(std::make_shared<carrot::core::state::MenuState>());
    }
}

void BaseLevelScene::transitionToLevelSelectState() {
    Director::getInstance()->resume();
    auto flowController = carrot::core::state::GameFlowProvider::Get();
    if (flowController) {
        flowController->TransitionToLevelSelect();
        return;
    }

    CCLOG("BaseLevelScene: GameFlowController not available, falling back to direct level select state");
    auto context = carrot::core::state::GameStateProvider::Get();
    if (context) {
        context->SetState(std::make_shared<carrot::core::state::LevelSelectState>());
    }
}

void BaseLevelScene::guaisou_jiansu(float guai_jiansu) {
    if (manager) {
        manager->ApplyMonsterSpeed(guai_jiansu);
    } else {
        auto instance = GameManager::getInstance();
        if (instance) {
            instance->ApplyMonsterSpeed(guai_jiansu);
        }
    }
}
// 技能方法已迁移到 SkillController，这里作为包装方法
void BaseLevelScene::Jineng1(Ref* pSender) {
    if (skillController_) {
        skillController_->executeSkill1(pSender);
    }
}
void BaseLevelScene::Jineng2(Ref* pSender) {
    if (skillController_) {
        skillController_->executeSkill2(pSender);
    }
}
void BaseLevelScene::Jineng3(Ref* pSender) {
    if (skillController_) {
        skillController_->executeSkill3(pSender);
    }
}
void BaseLevelScene::Jineng4(Ref* pSender) {
    if (skillController_) {
        skillController_->executeSkill4(pSender);
    }
}
void BaseLevelScene::Jineng5(Ref* pSender) {
    if (skillController_) {
        skillController_->executeSkill5(pSender);
    }
}
void BaseLevelScene::Jineng6(Ref* pSender) {
    if (skillController_) {
        skillController_->executeSkill6(pSender);
    }
}
void BaseLevelScene::UpMenuAppear(Vec2& position)
{
    last_position = position;
    int key = map_data[int(position.x / CELL_SIZE)][int(position.y / CELL_SIZE)].key;
    towers[key]->UpMenuAppear(this, position);
    cell_flag = 0;
}
void BaseLevelScene::UpMenuGone(Vec2& position)
{
    int key = BaseLevelScene::map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].key;
    towers[key]->UpMenuGone(this);
    if (position.x > last_position.x - CELL_SIZE / 2 && position.x < last_position.x + CELL_SIZE / 2 &&
        position.y > last_position.y - 3 * CELL_SIZE / 2 && position.y < last_position.y - CELL_SIZE / 2) {
        towers[key]->destroy(this);
        delete towers[key];
        towers.erase(key);
        map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].flag = 0;
    }
    else if (position.x > last_position.x - CELL_SIZE / 2 && position.x < last_position.x + CELL_SIZE / 2 &&
        position.y > last_position.y + CELL_SIZE / 2 && position.y < last_position.y + 3 * CELL_SIZE / 2) {
        towers[key]->update(this, last_position);
    }
    cell_flag = 1;
}
void BaseLevelScene::gameover(bool is_win, int currentWaveNum, int allWaveNum) {
    if (hasGameOverTriggered) {
        return;
    }
    hasGameOverTriggered = true;
    auto flowController = carrot::core::state::GameFlowProvider::Get();
    if (flowController) {
        flowController->ShowGameOver(is_win, levelId, currentWaveNum, allWaveNum);
        return;
    }

    CCLOG("BaseLevelScene: GameFlowController not available, falling back to direct game over state");
    auto context = carrot::core::state::GameStateProvider::Get();
    if (context) {
        context->SetState(std::make_shared<carrot::core::state::GameOverState>(
            is_win,
            levelId,
            currentWaveNum,
            allWaveNum));
    }
}
/******************************************/

Scene* BaseLevelScene::createScene(int level) {
    auto scene = BaseLevelScene::create();
    if (scene && scene->initWithLevel(level))
    {
        return scene;
    }
    return scene;
}
void BaseLevelScene::loadMap() {
    if (levelId <= 0 || levelId > mapFiles.size()) {
        CCLOG("Invalid level number!");
        return;
    }

    tileMap = TMXTiledMap::create(mapFiles[levelId - 1]);
    if (tileMap) {
        auto screenSize = Director::getInstance()->getVisibleSize();
        auto tileMapSize = tileMap->getContentSize();
        tileSize = tileMap->getTileSize();
        CCLOG("Tile size: (%f, %f)", tileSize.width, tileSize.height);
        tileMap->setScaleX(screenSize.width / tileMapSize.width);
        tileMap->setScaleY(screenSize.height / tileMapSize.height);

        tileMap->setAnchorPoint(Vec2::ZERO);
        tileMap->setPosition(Vec2::ZERO);
        plantableLayer = tileMap->getLayer("plantable");
        if (plantableLayer) {
            plantableLayer->setVisible(false);
        }
        else {
            CCLOG("Layer 'plantable' not found in map: %s", mapFiles[levelId - 1].c_str());
        }
        this->addChild(tileMap, 0);
#ifdef DEBUG_MODE
        drawGrid();
#endif
    }
    else {
        CCLOG("Failed to load map: %s", mapFiles[levelId - 1].c_str());
    }
}
void BaseLevelScene::initUI()
{
    _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", std::min(manager->getCurrentWaveNum(), manager->getAllWaveNum())), "Arial", 32);
    _curNumberLabel->setColor(Color3B::YELLOW);
    _curNumberLabel->setPosition(960 * 0.42, 640 * 0.95);
    this->addChild(_curNumberLabel, 2);
    _numberLabel = Label::createWithSystemFont(StringUtils::format(" / %d times", manager->getAllWaveNum()), "Arial", 32);
    _numberLabel->setColor(Color3B::YELLOW);
    _numberLabel->setPosition(960 * 0.50, 640 * 0.95);
    this->addChild(_numberLabel, 2);
    moneyHud = carrot::ui::widgets::MoneyHud::create(getMoney());
    if (moneyHud) {
        moneyHud->setPosition(Vec2(160, 610));
        this->addChild(moneyHud, 3);
    }
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 2);
    auto doubleSpeedButton = MenuItemImage::create("CarrotGuardRes/UI/normalSpeed.png", "CarrotGuardRes/UI/doubleSpeed.png", CC_CALLBACK_1(BaseLevelScene::doublespeed, this));
    if (doubleSpeedButton == nullptr)
    {
        problemLoading("'normalSpeed.png' and 'doubleSpeed.png'");
    }
    else
    {
        doubleSpeedButton->setPosition(670, 610);
        doubleSpeedButton->setScale(1);
        menu->addChild(doubleSpeedButton);
    }
    auto pauseSpeedButton = MenuItemImage::create("CarrotGuardRes/UI/pauseButton.png", "CarrotGuardRes/UI/continueButton.png", CC_CALLBACK_1(BaseLevelScene::pause_all, this));
    if (pauseSpeedButton == nullptr)
    {
        problemLoading("'pauseButton.png' and 'continueButton.png'");
    }
    else
    {
        pauseSpeedButton->setPosition(770, 610);
        pauseSpeedButton->setScale(1);
        menu->addChild(pauseSpeedButton);
    }
    auto menuButton = MenuItemImage::create("CarrotGuardRes/UI/gameMenuNormal.png", "CarrotGuardRes/UI/gameMenuSelected.png", CC_CALLBACK_1(BaseLevelScene::menu_all, this));
    if (menuButton == nullptr)
    {
        problemLoading("'gameMenuNormal.png' and 'gameMenuSelected'");
    }
    else
    {
        menuButton->setPosition(870, 610);
        menuButton->setScale(1);
        menu->addChild(menuButton);
    }
    auto wenhaoButton = MenuItemImage::create("CarrotGuardRes/UI/wenhao_0.png", "CarrotGuardRes/UI/wenhao_1.png", CC_CALLBACK_1(BaseLevelScene::wenhao, this));
    if (menuButton == nullptr)
    {
        problemLoading("'wenhao_0.png' and 'wenhao_1.png'");
    }
    else
    {
        wenhaoButton->setPosition(320, 28);
        wenhaoButton->setScale(1.4);
        menu->addChild(wenhaoButton);
    }
    auto jineng1Button = MenuItemImage::create("Carrot/jineng1.png", "Carrot/jineng1_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng1, this));
    if (jineng1Button == nullptr)
    {
        problemLoading("'jineng1.png'");
    }
    else
    {
        jineng1Button->setPosition(487, 33);//525,33
        jineng1Button->setScale(1.9);
        menu->addChild(jineng1Button);
    }
    auto jineng2Button = MenuItemImage::create("Carrot/jineng2.png", "Carrot/jineng2_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng2, this));
    if (jineng2Button == nullptr)
    {
        problemLoading("'jineng2.png'");
    }
    else
    {
        jineng2Button->setPosition(665, 33);//710
        jineng2Button->setScale(1.9);
        menu->addChild(jineng2Button);
    }
    auto jineng3Button = MenuItemImage::create("Carrot/jineng3.png", "Carrot/jineng3_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng3, this));
    if (jineng3Button == nullptr)
    {
        problemLoading("'jineng3.png'");
    }
    else
    {
        jineng3Button->setPosition(395, 33);//432
        jineng3Button->setScale(1.7);
        menu->addChild(jineng3Button);
    }
    auto jineng4Button = MenuItemImage::create("Carrot/jineng4.png", "Carrot/jineng4_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng4, this));
    if (jineng4Button == nullptr)
    {
        problemLoading("'jineng4.png'");
    }
    else
    {
        jineng4Button->setPosition(568, 33);//615
        jineng4Button->setScale(1.7);
        menu->addChild(jineng4Button);
    }
    auto jineng5Button = MenuItemImage::create("Carrot/jineng5.png", "Carrot/jineng5_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng5, this));
    if (jineng5Button == nullptr)
    {
        problemLoading("'jineng5.png'");
    }
    else
    {
        jineng5Button->setPosition(760, 33);
        jineng5Button->setScale(1.7);
        menu->addChild(jineng5Button);
    }
    auto jineng6Button = MenuItemImage::create("Carrot/jineng6.png", "Carrot/jineng6_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng6, this));
    if (jineng6Button == nullptr)
    {
        problemLoading("'jineng6.png'");
    }
    else
    {
        jineng6Button->setPosition(850, 33);
        jineng6Button->setScale(1.7);
        menu->addChild(jineng6Button);
    }

    auto jinengtiao = Sprite::create("Carrot/jinengtiao.png");
    jinengtiao->setPosition(480, 30);
    jinengtiao->setScale(0.7);
    this->addChild(jinengtiao, 1);
}

// 订阅胜负事件，确保 Scene 仅根据 GameManager 的结果执行一次结算
void BaseLevelScene::registerOutcomeListeners() {
    auto bus = carrot::core::EventBusProvider::Get();
    if (!bus) {
        CCLOG("BaseLevelScene: EventBus not available, outcome listeners not registered");
        return;
    }

    gameWonSubscription = bus->Subscribe(
        carrot::gameplay::events::kGameWonEventId,
        [this](const carrot::core::Event& baseEvent) {
            const auto& evt = static_cast<const carrot::gameplay::events::GameWonEvent&>(baseEvent);
            gameover(true, evt.currentWave, evt.totalWave);
        });

    gameLostSubscription = bus->Subscribe(
        carrot::gameplay::events::kGameLostEventId,
        [this](const carrot::core::Event& baseEvent) {
            const auto& evt = static_cast<const carrot::gameplay::events::GameLostEvent&>(baseEvent);
            gameover(false, evt.currentWave, evt.totalWave);
        });
}

// 订阅怪物死亡事件，在当前场景中执行具体的死亡表现（爆炸、移除等）
void BaseLevelScene::registerMonsterListeners() {
    auto bus = carrot::core::EventBusProvider::Get();
    if (!bus) {
        CCLOG("BaseLevelScene: EventBus not available, monster listeners not registered");
        return;
    }

    monsterDiedSubscription = bus->Subscribe(
        carrot::gameplay::events::kMonsterDiedEventId,
        [this](const carrot::core::Event& baseEvent) {
            const auto& evt = static_cast<const carrot::gameplay::events::MonsterDiedEvent&>(baseEvent);
            if (evt.monster) {
                evt.monster->toDie(this);
            }
        });
}

// 订阅怪物创建事件，在当前场景中把怪物节点挂到场景树上
void BaseLevelScene::registerMonsterSpawnListeners() {
    auto bus = carrot::core::EventBusProvider::Get();
    if (!bus) {
        CCLOG("BaseLevelScene: EventBus not available, monster spawn listener not registered");
        return;
    }

    monsterSpawnSubscription = bus->Subscribe(
        carrot::gameplay::events::kMonsterSpawnedEventId,
        [this](const carrot::core::Event& baseEvent) {
            const auto& evt = static_cast<const carrot::gameplay::events::MonsterSpawnedEvent&>(baseEvent);
            if (evt.monster) {
                this->addChild(evt.monster);
            }
        });
}

// 订阅怪物出生特效事件，在当前场景中创建并播放出生动画
void BaseLevelScene::registerSpawnEffectListeners() {
    auto bus = carrot::core::EventBusProvider::Get();
    if (!bus) {
        CCLOG("BaseLevelScene: EventBus not available, spawn effect listener not registered");
        return;
    }

    spawnEffectSubscription = bus->Subscribe(
        carrot::gameplay::events::kSpawnEffectRequestedEventId,
        [this](const carrot::core::Event& baseEvent) {
            const auto& evt = static_cast<const carrot::gameplay::events::SpawnEffectRequestedEvent&>(baseEvent);
            cocos2d::Vec2 spawnPosition(evt.x, evt.y);

            auto spawnEffect = cocos2d::Sprite::create("Monsters/monster_start_1.png");
            if (!spawnEffect) {
                CCLOG("BaseLevelScene: Failed to create spawn effect sprite.");
                return;
            }
            spawnEffect->setPosition(spawnPosition);
            this->addChild(spawnEffect);

            cocos2d::Vector<cocos2d::SpriteFrame*> frames;
            frames.pushBack(cocos2d::SpriteFrame::create("Monsters/monster_start_1.png", cocos2d::Rect(0, 0, 64, 64)));
            frames.pushBack(cocos2d::SpriteFrame::create("Monsters/monster_start_2.png", cocos2d::Rect(0, 0, 64, 64)));

            auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
            auto animate = cocos2d::Animate::create(animation);
            auto repeatAnimation = cocos2d::Repeat::create(animate, 2);
            auto removeEffect = cocos2d::CallFunc::create([spawnEffect]() {
                spawnEffect->removeFromParent();
            });
            spawnEffect->setScale(1.5f);
            auto sequence = cocos2d::Sequence::create(repeatAnimation, removeEffect, nullptr);
            spawnEffect->runAction(sequence);
        });
}

// 订阅萝卜抖动请求事件，在当前场景中播放抖动动画
void BaseLevelScene::registerCarrotShakeListeners() {
    auto bus = carrot::core::EventBusProvider::Get();
    if (!bus) {
        CCLOG("BaseLevelScene: EventBus not available, carrot shake listener not registered");
        return;
    }

    carrotShakeSubscription = bus->Subscribe(
        carrot::gameplay::events::kCarrotShakeRequestedEventId,
        [this](const carrot::core::Event& baseEvent) {
            const auto& evt = static_cast<const carrot::gameplay::events::CarrotShakeRequestedEvent&>(baseEvent);
            cocos2d::Vec2 pos(evt.x, evt.y);

            // 隐藏原萝卜，播放抖动动画（沿用原先 GameManager::doudong 的实现）
            if (manager && manager->getCarrot()) {
                manager->getCarrot()->getCarrotSprite()->setVisible(false);
            }

            auto b = cocos2d::Sprite::create();
            if (!b) {
                CCLOG("BaseLevelScene: Failed to create carrot shake sprite.");
                return;
            }
            b->setPosition(pos);
            b->setScale(1.5f);
            this->addChild(b);

            cocos2d::Vector<cocos2d::SpriteFrame*> frames;
            for (int i = 0; i <= 2; ++i) {
                std::string frameName = "Carrot/dou_" + std::to_string(i) + ".png";
                auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 85, 72));
                if (frame) {
                    frames.pushBack(frame);
                } else {
                    CCLOG("BaseLevelScene: Failed to load carrot shake frame: %s", frameName.c_str());
                }
            }
            if (frames.empty()) {
                CCLOG("BaseLevelScene: No frames found for carrot shake, skipping.");
                return;
            }
            auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.5f);
            auto animate = cocos2d::Animate::create(animation);
            auto onComplete = cocos2d::CallFunc::create([this, b]() {
                CCLOG("Carrot shake animation completed, removing sprite.");
                b->removeFromParent();
                if (manager && manager->getCarrot()) {
                    manager->getCarrot()->getCarrotSprite()->setVisible(true);
                }
            });
            Music::getInstance()->tuSound();
            b->runAction(cocos2d::Sequence::create(animate, onComplete, nullptr));
        });
}
void BaseLevelScene::CountDown(std::function<void()> onComplete)
{
    auto countBackground = Sprite::create("CarrotGuardRes/UI/countBackground.png");
    auto count1 = Sprite::create("CarrotGuardRes/UI/countOne.png");
    auto count2 = Sprite::create("CarrotGuardRes/UI/countTwo.png");
    auto count3 = Sprite::create("CarrotGuardRes/UI/countThree.png");
    Label* count0 = Label::createWithSystemFont("GO", "Arial-BoldMT", 100);

    countBackground->setPosition(480, 320);
    count1->setPosition(480, 320);
    count2->setPosition(480, 320);
    count3->setPosition(480, 320);
    count0->setPosition(480, 320);

    countBackground->setVisible(false);
    count1->setVisible(false);
    count2->setVisible(false);
    count3->setVisible(false);
    count0->setVisible(false);

    this->addChild(countBackground, 2);
    this->addChild(count1, 2);
    this->addChild(count2, 2);
    this->addChild(count3, 2);
    this->addChild(count0, 2);
    auto countdown = Sequence::create(
        CallFunc::create([=] {
            countBackground->setVisible(true);
            count3->setVisible(true);
            Music::getInstance()->countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count3);
            }),
        CallFunc::create([=] {
            count2->setVisible(true);
            Music::getInstance()->countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count2);
            }),
        CallFunc::create([=] {
            count1->setVisible(true);
            Music::getInstance()->countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count1);
            count0->setVisible(true);
            Music::getInstance()->countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count0);
            this->removeChild(countBackground);

            if (onComplete) {
                onComplete();
            }
            }),
        nullptr
    );
    this->runAction(countdown);
}



bool BaseLevelScene::initWithLevel(int level)
{
    if (!Scene::init())
    {
        return false;
    }
    hasGameOverTriggered = false;
    Director::getInstance()->resume();
    this->levelId = level;
    this->loadMap();
    manager = GameManager::getInstance(this);
    manager->initLevel(level, !isNewGame[levelId - 1]);
    
    // 初始化技能控制器
    skillController_ = new SkillController();
    SkillController::SkillCallbacks callbacks;
    callbacks.getMoney = [this]() { return this->getMoney(); };
    callbacks.updateMoney = [this](int add) { this->updateMoney(add); };
    callbacks.applyMonsterSpeed = [this](float speed) { this->guaisou_jiansu(speed); };
    callbacks.addChildToScene = [this](Node* node) { this->addChild(node); };
    callbacks.runActionOnScene = [this](Action* action) { this->runAction(action); };
    skillController_->init(callbacks, manager);
    
    // 初始化游戏菜单控制器
    menuController_ = new GameMenuController();
    GameMenuController::MenuCallbacks menuCallbacks;
    menuCallbacks.addChildToScene = [this](Node* node) { this->addChild(node, 10); };
    menuCallbacks.removeChildByName = [this](const std::string& name) { this->removeChildByName(name); };
    menuCallbacks.removeChild = [this](Node* node) { this->removeChild(node); };
    menuCallbacks.pauseDirector = []() { Director::getInstance()->pause(); };
    menuCallbacks.resumeDirector = []() { Director::getInstance()->resume(); };
    menuCallbacks.replaceScene = [](Scene* scene) { Director::getInstance()->replaceScene(scene); };
    menuCallbacks.saveGameState = [this]() { this->saveGameState(); };
    menuCallbacks.saveTowerData = [this]() { this->saveTowerData(); };
    menuCallbacks.transitionToLevelSelectState = [this]() { this->transitionToLevelSelectState(); };
    menuCallbacks.createLevelScene = [](int level) { return BaseLevelScene::createScene(level); };
    menuCallbacks.getLevelId = [this]() { return this->levelId; };
    menuCallbacks.isGamePaused = [this]() { return this->isGamePaused; };
    menuCallbacks.stopAllSchedulers = []() { GameManager::getInstance()->stopAllSchedulers(); };
    menuCallbacks.removeListener = [this]() { manager->removeListener(); };
    menuCallbacks.saveMonstersData = [this](const std::string& filename) { manager->saveMonstersDataToJson(filename); };
    menuCallbacks.setTimeScale = [this](float scale) { scheduler->setTimeScale(scale); };
    menuController_->init(menuCallbacks);
    
    // 初始化游戏速度控制器
    speedController_ = new GameSpeedController();
    GameSpeedController::SpeedCallbacks speedCallbacks;
    speedCallbacks.applyMonsterSpeed = [this](float speed) { this->guaisou_jiansu(speed); };
    speedController_->init(speedCallbacks);
    
    initUI();
    registerOutcomeListeners();
    registerMonsterListeners();
    registerSpawnEffectListeners();
    registerCarrotShakeListeners();
    registerMonsterSpawnListeners();
    registerSpawnEffectListeners();
    plantsLayer = Layer::create();
    this->addChild(plantsLayer, 10);
    addMouseListener();

    cell_flag = 1;
    buy_tower[0].push_back("Towers/affordhuo.png"); buy_tower[1].push_back("Towers/unaffordhuo.png");
    buy_tower[0].push_back("Towers/affordsun.png"); buy_tower[1].push_back("Towers/unaffordsun.png");
    buy_tower[0].push_back("Towers/affordplane.png"); buy_tower[1].push_back("Towers/unaffordplane.png");
    buy_tower[0].push_back("Towers/affordshit.png"); buy_tower[1].push_back("Towers/unaffordshit.png");
    index_table.push_back(0);
    index_table.push_back(1);
    index_table.push_back(2);
    index_table.push_back(3);
    InitMapData();
    if (!isNewGame[levelId - 1])
    {
        if (loadTowerData("level" + std::to_string(levelId) + "_tower.json") == false)
            ProduceObstacles();
    }
    if (isNewGame[levelId - 1])  ProduceObstacles();

    tower_jiasu = 0.01;
    beishu = 1;

    CountDown([=] {
        CCLOG("READY");
        manager->startMonsterWaves();
        tower_jiasu = 1;
        });

    this->scheduleUpdate();


    return true;
}
bool BaseLevelScene::init() {
    if (!Scene::init()) {
        return false;
    }
    skillController_ = nullptr;
    menuController_ = nullptr;
    speedController_ = nullptr;
    return true;
}

BaseLevelScene::~BaseLevelScene() {
    if (skillController_) {
        delete skillController_;
        skillController_ = nullptr;
    }
    if (menuController_) {
        delete menuController_;
        menuController_ = nullptr;
    }
    if (speedController_) {
        delete speedController_;
        speedController_ = nullptr;
    }
}
// 每帧更新：刷新波次 UI、驱动塔攻击，并交给 GameManager 判定胜负
void BaseLevelScene::update(float deltaTime) {
    
    updateCurrentWaveLabe();
    for (auto it = towers.begin(); it != towers.end(); it++) {

        if (it->second->interval >= it->second->interval_table[it->second->GetIndex()]) {
            it->second->attack(this, GameManager::getInstance()->GetMonsters(), isTarget, tar_m, tar_o,tower_jiasu);
            continue;
        }

        it->second->interval += deltaTime*tower_jiasu;
    }
    manager->CheckLose();
    manager->CheckWin();
    time_total += deltaTime;
    if (time_total >= 5) {
        time_total -= 5;
        manager->doudong();
    }
}
void BaseLevelScene::InitMapData()
{
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++) {
            map_data[i][j].flag = obpos[levelId - 1][i][j];
            map_data[i][j].key = Y * i + j;
        }
}
void BaseLevelScene::ProduceObstacles()
{
    srand(time(0));
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++) {
            if (map_data[i][j].flag == 3) {
                int rd;
                if (i + 1 < X && j + 1 < Y && map_data[i + 1][j].flag == 3 && map_data[i][j + 1].flag == 3 && map_data[i + 1][j + 1].flag == 3)
                    rd = 10;
                else if (i + 1 < X && map_data[i + 1][j].flag == 3) rd = 6;
                else rd = 5;
                int index = rand() % (rd + 5);
                if (index >= rd) {
                    map_data[i][j].flag = 0;
                    continue;
                }
                map_data[i][j].flag = 2;
                auto obb = new Obstacle(index);
                obb->Produce(this, i, j);
                Obstacles[map_data[i][j].key] = obb;
                if (index == 5) {
                    map_data[i + 1][j].flag = 2;
                    Obstacles[map_data[i + 1][j].key] = obb;
                }
                else if (index > 5) {
                    map_data[i][j + 1].flag = map_data[i + 1][j].flag = map_data[i + 1][j + 1].flag = 2;
                    Obstacles[map_data[i + 1][j].key] = Obstacles[map_data[i][j + 1].key] = Obstacles[map_data[i + 1][j + 1].key] = obb;
                }
            }
        }
}
/******************************************/

void BaseLevelScene::addMouseListener() {
    auto listener = EventListenerMouse::create();
    listener->onMouseDown = CC_CALLBACK_1(BaseLevelScene::handleMouseDown, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
void BaseLevelScene::handleMouseDown(EventMouse* event) {
    Vec2 mousePos = event->getLocation();
    CCLOG("Mouse clicked at screen position: (%f, %f)", mousePos.x, mousePos.y);
    handlePlant(mousePos);
}
void BaseLevelScene::handlePlant(const Vec2& position) {
    CCLOG("position: (%f, %f)", position.x, position.y);
    Vec2 tileCoord = Vec2(floor(position.x / tileSize.width), floor(position.y / tileSize.height));
    CCLOG("tileCoord: (%f, %f)", tileCoord.x, tileCoord.y);

    if (tileCoord.x < 0 || tileCoord.y < 0 ||
        tileCoord.x >= tileMap->getMapSize().width ||
        tileCoord.y >= tileMap->getMapSize().height) {
        CCLOG("Tile coordinate is out of bounds.");
        return;
    }

    if (isTarget != 0) {
        Vec2 pos = Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y);
        if (isTarget == 1) {
            auto m_pos = tar_m->getPosition(); auto m_size = tar_m->getContentSize();
            if (pos.x > m_pos.x - m_size.width / 2 && pos.x < m_pos.x + m_size.width / 2
                && pos.y>m_pos.y - m_size.height / 2 && pos.y < m_pos.y + m_size.height / 2) {
                isTarget = 0;
                tar_m = nullptr;
                lock->removeFromParent();
                return;
            }
        }
        else {
            auto o_pos = tar_o->getPosition(); auto o_size = tar_o->curr->getContentSize();
            if (pos.x > o_pos.x - o_size.width / 2 && pos.x < o_pos.x + o_size.width / 2
                && pos.y>o_pos.y - o_size.height / 2 && pos.y < o_pos.y + o_size.height / 2) {
                isTarget = 0;
                tar_o = nullptr;
                lock->removeFromParent();
                return;
            }
        }
    }

    auto tileGID = tileMap->getLayer("plantable")->getTileGIDAt(tileCoord);

    if (!cell_flag) {
        if (map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].flag == 0)
            PlantMenuGone(Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y));
        else if (map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].flag == 1)
            UpMenuGone(Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y));
        return;
    }

    if (tileGID != 0) {
        CCLOG("Tile at (%f, %f) is plantable.", tileCoord.x, tileCoord.y);
        float mapHeight = tileMap->getMapSize().height;

        float screenX = tileCoord.x * tileSize.width + tileSize.width / 2;
        float screenY = (mapHeight - tileCoord.y - 1) * tileSize.height + tileSize.height / 2;

        Vec2 mapPos = Vec2(screenX, screenY);

        CCLOG("Screen Position: (%f, %f)", mapPos.x, mapPos.y);

        if (map_data[int(mapPos.x / CELL_SIZE)][int(mapPos.y / CELL_SIZE)].flag == 0)
            PlantMenuAppear(mapPos);
        else if (map_data[int(mapPos.x / CELL_SIZE)][int(mapPos.y / CELL_SIZE)].flag == 1)
            UpMenuAppear(mapPos);
        else {
            if (isTarget != 0) lock->removeFromParent();
            Music::getInstance()->ShootSelect();
            isTarget = 2;
            tar_m = nullptr;
            tar_o = Obstacles[map_data[int(mapPos.x / CELL_SIZE)][int(mapPos.y / CELL_SIZE)].key];
            lock = Sprite::create("Towers/chosed.png");
            Vec2 o_pos = tar_o->getPosition();
            lock->setPosition(Vec2(o_pos.x, o_pos.y + tar_o->curr->getContentSize().height * 1.0));
            this->addChild(lock);
        }
    }

    else
    {
        Monster* tool = IsTargetMonster(Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y));
        if (tool != nullptr) {
            if (isTarget != 0) lock->removeFromParent();
            Music::getInstance()->ShootSelect();
            isTarget = 1;
            tar_m = tool;
            tar_o = nullptr;
            lock = Sprite::create("Towers/chosed.png");
            lock->setPosition(Vec2(tar_m->getContentSize().width * 0.6, tar_m->getContentSize().height * 1.4));
            tar_m->addChild(lock, 100);
            CCLOG("-------------------------------------------------locked");
        }
        CCLOG("Tile at (%f, %f) is not plantable.", tileCoord.x, tileCoord.y);
    }

}
void BaseLevelScene::PlantMenuAppear(Vec2 mapPos)
{
    cell_flag = 0;
    auto location = mapPos;
    last_position = location;
    auto cell = Sprite::create("Towers/cell.png");
    cell->setScale((float)64 / 200);
    cell->setPosition(location);
    this->addChild(cell); curr_cell = cell;
    for (int i = 0, size = buy_tower[0].size(); i < size; i++) {
        int tool = Tower::build_cost[index_table[i]] > getMoney() ? 1 : 0;
        auto tower_graph = Sprite::create(buy_tower[tool][i]);
        tower_graph->setScale((float)CELL_SIZE / tower_graph->getContentSize().width);
        tower_graph->setPosition(Vec2(location.x - CELL_SIZE * (size - 1) / 2 + CELL_SIZE * i, location.y + CELL_SIZE));
        this->addChild(tower_graph); remove_table.push_back(tower_graph);
    }
}
void BaseLevelScene::PlantMenuGone(Vec2 position)
{
    auto location = position;
    int size = remove_table.size();
    if ((location.x > last_position.x - CELL_SIZE * size / 2 && location.x < last_position.x + CELL_SIZE * size / 2)
        && (location.y > last_position.y + CELL_SIZE / 2 && location.y < last_position.y + 3 * CELL_SIZE / 2)) {
        int index = index_table[(location.x - last_position.x + CELL_SIZE * size / 2) / CELL_SIZE];
        if (getMoney() >= Tower::build_cost[index]) {
            Tower* this_tower = TowerFactoryProvider::createTower(index);
            this_tower->build(this, last_position);
            updateMoney(-Tower::build_cost[index]);
            Music::getInstance()->TowerBuild();
            int x = int(last_position.x / CELL_SIZE), y = int(last_position.y / CELL_SIZE);
            towers[map_data[x][y].key] = this_tower;
            map_data[x][y].flag = 1;
        }
    }
    this->removeChild(curr_cell); curr_cell->release();
    for (int i = 0; i < remove_table.size(); i++) { this->removeChild(remove_table[i]); remove_table[i]->release(); }
    remove_table.clear();
    cell_flag = 1;
}
Monster* BaseLevelScene::IsTargetMonster(const Vec2& pos)
{
    auto& monsters = GameManager::getInstance()->GetMonsters();
    for (auto it = monsters.begin(); it != monsters.end(); it++) {
        if ((*it)->getHealth() <= 0)continue;
        auto m_pos = (*it)->getPosition();
        auto m_size = (*it)->getContentSize();
        if (pos.x > m_pos.x - m_size.width && pos.x < m_pos.x + m_size.width
            && pos.y>m_pos.y - m_size.height && pos.y < m_pos.y + m_size.height) return (*it);
    }
    return nullptr;
}
/******************************************/

//#ifdef DEBUG_MODE
void BaseLevelScene::drawGrid() {
    Size mapSize = tileMap->getContentSize();
    float tileWidth = 64.0f;
    float tileHeight = 64.0f;

    auto drawNode = DrawNode::create();
    this->addChild(drawNode, 100);

    for (float x = 0; x <= mapSize.width; x += tileWidth) {
        drawNode->drawLine(Vec2(x, 0), Vec2(x, mapSize.height), Color4F::WHITE);
    }

    for (float y = 0; y <= mapSize.height; y += tileHeight) {
        drawNode->drawLine(Vec2(0, y), Vec2(mapSize.width, y), Color4F::WHITE);
    }
}
//#endif // DEBUG_MODE
Vec2 BaseLevelScene::gridToScreenCenter(const Vec2& gridPoint) {
    float mapHeight = tileMap->getMapSize().height;
    float screenX = gridPoint.x * tileSize.height + tileSize.width / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * tileSize.height + tileSize.height / 2;
    return Vec2(screenX, screenY);
}
/************************************************/


void BaseLevelScene::saveGameState() {

    Document document;
    document.SetObject();

    rapidjson::Value GameState(kArrayType);

    for (auto level : level_is_win) {
        GameState.PushBack(rapidjson::Value(level), document.GetAllocator());
    }

    document.AddMember("levels", GameState, document.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string writablePath = FileUtils::getInstance()->getWritablePath();

    std::string filePath = writablePath + "level_state.json";

    std::ofstream ofs(filePath);
    if (ofs.is_open()) {
        ofs << buffer.GetString();
        ofs.close();
        CCLOG("�浵�ɹ���%s", filePath.c_str());
    }
    else {
        CCLOG("�浵ʧ�ܣ�%s", filePath.c_str());
    }
}
void BaseLevelScene::saveTowerData()
{
    Document document;
    document.SetObject();
    rapidjson::Value towerArray(rapidjson::kArrayType);
    for(int i=0;i<X;i++)
    {
     rapidjson::Value rowArray(rapidjson::kArrayType);
     for (int j = 0; j < Y; j++)
     {
     rapidjson::Value towerObj(rapidjson::kObjectType);
     towerObj.AddMember("flag",map_data[i][j].flag, document.GetAllocator());
     if(map_data[i][j].flag==1)
     {
        towerObj.AddMember("index", towers[map_data[i][j].key]->GetIndex(), document.GetAllocator());
        towerObj.AddMember("data", towers[map_data[i][j].key]->GetGrade(), document.GetAllocator());
     }
     else if(map_data[i][j].flag == 2)
     {
         towerObj.AddMember("index", Obstacles[map_data[i][j].key]->GetIndex(), document.GetAllocator());
         towerObj.AddMember("data", Obstacles[map_data[i][j].key]->getHealth(), document.GetAllocator());
     }
     else {
         towerObj.AddMember("index", 0, document.GetAllocator());
         towerObj.AddMember("data", 0, document.GetAllocator());
     }
     rowArray.PushBack(towerObj, document.GetAllocator());
     }
     towerArray.PushBack(rowArray, document.GetAllocator());
    }
    document.AddMember("towers", towerArray,document.GetAllocator());
    document.AddMember("money", getMoney(), document.GetAllocator());
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string writablePath = FileUtils::getInstance()->getWritablePath();

    std::string filePath = writablePath + "level"+std::to_string(levelId)+"_tower.json";

    std::ofstream ofs(filePath);
    if (ofs.is_open()) {
        ofs << buffer.GetString();
        ofs.close();
        CCLOG("�浵�ɹ���%s", filePath.c_str());
    }
    else {
        CCLOG("�浵ʧ�ܣ�%s", filePath.c_str());
    }
}
bool BaseLevelScene::loadTowerData(const std::string& filename)
{
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string path = writablePath + filename ;
    std::string fileContent = FileUtils::getInstance()->getStringFromFile(path);
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    if (doc.HasParseError()) {
        CCLOG("Error parsing JSON file: %s", filename.c_str());
        return false;
    }
    if (doc.HasMember("towers") && doc["towers"].IsArray()) {
        const rapidjson::Value& towersArray = doc["towers"];
            if (towersArray.IsArray()) {
                for (rapidjson::SizeType i = 0; i < towersArray.Size(); ++i) {
                    const rapidjson::Value& row = towersArray[i];
                    if (row.IsArray()) {
                        for (rapidjson::SizeType j = 0; j < row.Size(); ++j) {
                            const rapidjson::Value& towerObj = row[j];
                            if (towerObj.IsObject()) {
                                int flag = towerObj["flag"].GetInt();
                                int index = towerObj["index"].GetInt();
                                int data = towerObj["data"].GetInt();
                                
                               if(flag==1){
                                   Vec2 pos= pos = Vec2((i+ 0.5) * CELL_SIZE, (j+0.5) * CELL_SIZE);
                                   auto tower=TowerFactoryProvider::createTower(index,data);
                                   tower->build(this,pos);
                                   towers[map_data[i][j].key]=tower;
                               }
                               else if(flag==2&&map_data[i][j].flag==3){
                                   auto obb = new Obstacle(index);
                                   obb->Produce(this, i, j);
                                    obb->setHealth(data);
                                    obb->updateHealthBar();
                                   Obstacles[map_data[i][j].key] = obb;
                                   if(obb->GetSize()==2){
                                       map_data[i + 1][j].flag = 2;
                                       Obstacles[map_data[i + 1][j].key] = obb;
                                   }
                                   else if (obb->GetSize() ==4) {
                                       map_data[i][j + 1].flag = map_data[i + 1][j].flag = map_data[i + 1][j + 1].flag = 2;
                                       Obstacles[map_data[i + 1][j].key] = Obstacles[map_data[i][j + 1].key] = Obstacles[map_data[i + 1][j + 1].key] = obb;
                                   }
                               }
                               map_data[i][j].flag = flag;
                            }
                        }
                    }
                }
            }
            else {
                CCLOG("Towers data is not an array in level");
                return false;
            }
        
    }
    if (doc.HasMember("money") && doc["money"].IsInt()) {
        int savedMoney = doc["money"].GetInt();
        if (manager) {
            manager->SetMoney(savedMoney);
        } else {
            auto instance = GameManager::getInstance();
            if (instance) {
                instance->SetMoney(savedMoney);
            }
        }
        CCLOG("INIT_MONEY:currentIndex: %d", getMoney());
    } else {
        CCLOG("No such level: %s", filename);
        return false;
    }
    return true;
}
/************************************************/