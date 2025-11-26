#include "BaseLevelScene.h"
#include<vector>
#include"MonsterConfigs.h"
#include"Monster.h"
#include "json/document.h"
#include "json/rapidjson.h"
#include "ui/CocosGUI.h"
#include"music.h"
#include"themeScene.h"
#include"Tower.h"
#include"GameManager.h"
#include<string>
#include "json/writer.h"
#include"obpos.h"
#include "json/stringbuffer.h"
#include<fstream>
#include "core/EventBusProvider.h"
#include "core/state/GameState.h"
#include "core/state/GameFlowProvider.h"
#include "core/state/GameFlowController.h"
#include "core/state/GameStateProvider.h"
#include "core/state/PausedState.h"
#include "core/state/MenuState.h"
#include "core/state/LevelSelectState.h"
#include "core/state/GameOverState.h"
#include "gameplay/events/MoneyEvents.h"
#include "ui/widgets/MoneyHud.h"
using namespace rapidjson;
using namespace ui;
//#define DEBUG_MODE
#define CELL_SIZE 64
const Color3B moneyColor(154, 101, 25);

extern float beishu;           //��Ϸ����
extern bool level_is_win[3];   //ͨ�����
extern bool isNewGame[3];      //�浵���
GameManager* manager;          //�������������ֽ���
float tower_jiasu = 1;         //����������
// �ؿ���ͼ�ļ�·������
const std::vector<std::string> BaseLevelScene::mapFiles 
= {
    "map/map1.tmx",  // �ؿ� 1
    "map/map2.tmx",  // �ؿ� 2
    "map/map3.tmx",  // �ؿ� 3
};


static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BaseLevelScene.cpp\n");
}
/****************��ǩ����******************/
//���½�ұ�ǩ
void BaseLevelScene::updateMoney(int add) {
    // Adjust internal bookkeeping first; MoneyHud 订阅事件后刷新 UI
    money += add;
    carrot::gameplay::events::MoneyChangedEvent evt{};
    evt.delta = add;
    evt.current = money;
    carrot::core::EventBusProvider::Get()->Publish(carrot::gameplay::events::kMoneyChangedEventId, evt);
}

//���µ�ǰ������ǩ
void BaseLevelScene::updateCurrentWaveLabe() {
    _curNumberLabel->setString(StringUtils::format("%d", std::min(manager->getCurrentWaveNum() + 1, manager->getAllWaveNum())));
}
/******************************************/

/****************UI����******************/
//�����ٰ�ť
void BaseLevelScene::doublespeed(Ref* pSender) {
    isDoubleSpeed = !isDoubleSpeed; // �л�������״̬
    Music::getInstance()->button_music();
    MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
    if (isDoubleSpeed) {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/doubleSpeed.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/doubleSpeed.png"));
        tower_jiasu = 2;
        guaisou_jiansu(2.0f);
        beishu = 2;//ʵ�ּ���Ч��
    }
    else {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/normalSpeed.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/normalSpeed.png"));
        tower_jiasu = 1;
        guaisou_jiansu(1.0f);
        beishu = 1; //ʵ�ּ���Ч��
    }
}
//��ͣ��ť
void BaseLevelScene::pause_all(Ref* pSender) {
    isGamePaused = !isGamePaused; // Switch pause state
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

void BaseLevelScene::wenhao(Ref* pSender) {
    Music::getInstance()->button_music();
    auto visibleSize = Director::getInstance()->getVisibleSize();//�ֱ��ʴ�С
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();    //��ȡ��Ļ��С
    //������ɫ���ֲ�
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
    menuLayer->setPosition(Vec2::ZERO);
    this->addChild(menuLayer, 10);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [menuLayer](Touch* touch, Event* event) {
        return true;
        };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, menuLayer);
    auto jie = Sprite::create("Carrot/jieshao/jie_0.png");
    jie->setPosition(480, 500);
    jie->setScale(2);
    menuLayer->addChild(jie, 1);
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    menuLayer->addChild(menu, 1);
    //�������ҷ�ҳ��ť
   // ����PageView

    auto pageView = PageView::create();
    pageView->setContentSize(Size(screenSize.width, screenSize.height));
    pageView->setPosition(Vec2::ZERO);
    menuLayer->addChild(pageView);
    // �����ͼͼƬ����
    std::vector<std::string> mapImages = {
        "Carrot/jieshao/jie_1.png",
        "Carrot/jieshao/jie_2.png",
        "Carrot/jieshao/jie_3.png",
        "Carrot/jieshao/jie_4.png",
        "Carrot/jieshao/jie_5.png"
    };
    // ����ҳ��
    for (int i = 0; i < mapImages.size(); ++i) {
        Layout* layout = Layout::create();
        layout->setContentSize(Size(screenSize.width, screenSize.height));
        ImageView* imageView = ImageView::create(mapImages[i]);
        imageView->setContentSize(Size(screenSize.width, screenSize.height));
        imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
        imageView->setScale(2);  //����
        layout->addChild(imageView, 1);
        pageView->addPage(layout);
    }
    auto continueButton = MenuItemImage::create("Carrot/jieshao/cha_0.png", "Carrot/jieshao/cha_1.png");
    continueButton->setPosition(814,503);
    continueButton->setScale(1.9);
    // ������Ϸѡ��
    continueButton->setCallback([this, menuLayer](Ref* psender) {
        Music::getInstance()->button_music();
        this->removeChild(menuLayer);
        });
    menu->addChild(continueButton, 1);
    // �������ҷ�ҳ��ť
    auto leftButton = Button::create("CarrotGuardRes/UI/leftButtonNormal.png", "CarrotGuardRes/UI/leftButtonSelected.png");
    leftButton->setPosition(Vec2(screenSize.width * 0.1, screenSize.height / 2));
    leftButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Music::getInstance()->button_music();
            int currentIndex = pageView->getCurrentPageIndex();
            CCLOG("currentIndex:  %d", currentIndex);
            if (currentIndex > 0)
                pageView->scrollToPage(currentIndex - 1);
        }
        });
    menuLayer->addChild(leftButton, 1);
    auto rightButton = Button::create("CarrotGuardRes/UI/rightButtonNormal.png", "CarrotGuardRes/UI/rightButtonSelected.png");
    rightButton->setPosition(Vec2(screenSize.width * 0.9, screenSize.height / 2));
    rightButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Music::getInstance()->button_music();
            int currentIndex = pageView->getCurrentPageIndex();
            if (currentIndex < 0)
               currentIndex = 0;
            CCLOG("currentIndex:  %d", currentIndex);
            if (currentIndex < mapImages.size() - 1)
                pageView->scrollToPage(currentIndex + 1);
        }
        });
    menuLayer->addChild(rightButton, 1); 
}
//�˵���ť
void BaseLevelScene::menu_all(Ref* pSender) {
    Music::getInstance()->button_music();
    Director::getInstance()->pause();
    auto visibleSize = Director::getInstance()->getVisibleSize();//�ֱ��ʴ�С
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();    //��ȡ��Ļ��С
    //������ɫ���ֲ�
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
    menuLayer->setPosition(Vec2::ZERO);
    this->addChild(menuLayer, 10);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [menuLayer](Touch* touch, Event* event) {
        return true;
        };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, menuLayer);
    //������ͣ�˵�����
    auto menuBackground = Sprite::create("CarrotGuardRes/UI/gameMenu.png");
    menuBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    menuBackground->setScale(1.5f);
    menuLayer->addChild(menuBackground, 0);
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    menuLayer->addChild(menu, 1);
    //������ͣ�˵�����ع��ܰ�ť
    auto continueButton = MenuItemImage::create("CarrotGuardRes/UI/continueNormal.png", "CarrotGuardRes/UI/continueSelected.png");
    continueButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.649));
    continueButton->setScale(1.5);
    auto restartButton = MenuItemImage::create("CarrotGuardRes/UI/restartNormal.png", "CarrotGuardRes/UI/restartSelected.png");
    restartButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.51));
    restartButton->setScale(1.5);
    auto chooseButton = MenuItemImage::create("CarrotGuardRes/UI/chooseLevelNormal.png", "CarrotGuardRes/UI/chooseLevelSelected.png");
    chooseButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.375));
    chooseButton->setScale(1.5);
    // ������Ϸѡ��
    continueButton->setCallback([this, menuLayer](Ref* psender) {
        Music::getInstance()->button_music();
        this->removeChild(menuLayer);
        // �ж��ڵ���˵���ť֮ǰ�Ƿ�������ͣ��ť����ֹ����bug
        if (!isGamePaused) {
            Director::getInstance()->resume();
        }
        });
    //���¿�ʼ��Ϸѡ��
    restartButton->setCallback([this, menuLayer](Ref* psender) {
        Music::getInstance()->button_music();
        this->removeChild(menuLayer);
        // ȡ���� GameManager ��ص����е�����
        GameManager::getInstance()->stopAllSchedulers();
        //ȡ���¼�����
        manager->removeListener();
        auto scene = BaseLevelScene::createScene(levelId);
        scheduler->setTimeScale(1.0f); //ʵ�ּ���Ч��
        Director::getInstance()->replaceScene(scene);
        Director::getInstance()->resume();
        });

    //ѡ��ؿ�ѡ��
    chooseButton->setCallback([this, menuLayer](Ref* psender) {
        Music::getInstance()->button_music();
        saveGameState();
        //���޴浵
        manager->saveMonstersDataToJson("level"+std::to_string(levelId)+"Monster.json");
        //ֲ��浵
        saveTowerData();
        this->removeChild(menuLayer);
        // ȡ���� GameManager ��ص����е�����
        GameManager::getInstance()->stopAllSchedulers();
        //ȡ���¼�����
        manager->removeListener();
        transitionToLevelSelectState();
        });
    menu->addChild(continueButton, 1);
    menu->addChild(chooseButton, 1);
    menu->addChild(restartButton, 1);
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
    for (auto it = manager->monsters.begin(); it != manager->monsters.end(); it++) {
        if ((*it)->getHealth() > 0) 
            (*it)->speedaction->setSpeed(guai_jiansu);   
    }
}
//����1��ť
void BaseLevelScene::Jineng1(Ref* pSender) {
    Music::getInstance()->button_music();
    if (money >= 200) {
        updateMoney(-200);
        manager->Jineng1();
    }
}
//����2��ť
void BaseLevelScene::Jineng2(Ref* pSender) {
    Music::getInstance()->button_music();
    if (money >= 200) {
        updateMoney(-200); 
        guaisou_jiansu(0.01f);
        beishu = 0.01f;
        auto delayaction = Sequence::create(
            DelayTime::create(5.0f),
            CallFunc::create([=] {guaisou_jiansu(1); beishu = 1.0f; }),
            nullptr);
        this->runAction(delayaction);
    }
}
//����3��ť
void BaseLevelScene::Jineng3(Ref* pSender) {
    Music::getInstance()->button_music();
    if (money >= 500) {
        auto bong = Sprite::create();
        if (!bong) {
            CCLOG("Failed to create bong sprite.");
            return;
        }
        bong->setPosition(480, 320);
        bong->setScale(2);
        this->addChild(bong);
        cocos2d::Vector<cocos2d::SpriteFrame*> frames;
        for (int i = 0; i <= 3; ++i) {
            std::string frameName = "Carrot/bong/bong_" + std::to_string(i) + ".png";
            auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 164, 160)); // ����ÿ��ͼƬ�Ĵ�С�� 64x64
            if (frame) {
                frames.pushBack(frame);
            }
            else {
                CCLOG("Failed to load frame: %s", frameName.c_str());
            }
        }
        if (frames.empty()) {
            CCLOG("No frames found for bong, skipping.");
            return;
        }
        auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
        auto animate = cocos2d::Animate::create(animation);
        auto onbong = cocos2d::CallFunc::create([bong]() {
            CCLOG("bong.");
            bong->removeFromParent();
            });
        Music::getInstance()->bongSound();
        bong->runAction(cocos2d::Sequence::create(animate,onbong, nullptr));
        updateMoney(-500);
        for (auto it = manager->monsters.begin(); it != manager->monsters.end(); it++) {
            if ((*it)->getHealth() > 0) {
                (*it)->setHealth(0);
                (*it)->toDie(this);
            }    
        }
    }
}
//����4��ť
void BaseLevelScene::Jineng4(Ref* pSender) {
    Music::getInstance()->button_music();
    if (money >= 200) {
        updateMoney(-200);
        tower_jiasu = 2;
        auto delayaction = Sequence::create(DelayTime::create(5.0f), CallFunc::create([=] {
            tower_jiasu = 1;
            }), nullptr);
        this->runAction(delayaction);
    }
}
//����5��ť
void BaseLevelScene::Jineng5(Ref* pSender) {
    Music::getInstance()->button_music();
    if (money >= 150) {
        updateMoney(-150);
        guaisou_jiansu(0.5f);
        beishu = 0.5f;
        auto delayaction = Sequence::create(
            DelayTime::create(5.0f),
            CallFunc::create([=] {guaisou_jiansu(1); beishu = 1.0f; }),
            nullptr);
        this->runAction(delayaction);
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
//��Ϸ�����Ľ��洦��
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

/****************��ʼ������******************/
// ��������ʱ����ؿ����
Scene* BaseLevelScene::createScene(int level) {
    auto scene = BaseLevelScene::create();  // ���� BaseLevelScene
    if (scene && scene->initWithLevel(level))
    {
        return scene;
    }
    return scene;
}
// ���ص�ͼ
void BaseLevelScene::loadMap() {
    if (levelId <= 0 || levelId > mapFiles.size()) {
        CCLOG("Invalid level number!");
        return;
    }

    // ���عؿ���ͼ
    tileMap = TMXTiledMap::create(mapFiles[levelId - 1]);
    if (tileMap) {
        // ��ȡ��Ļ��С
        auto screenSize = Director::getInstance()->getVisibleSize();
        auto tileMapSize = tileMap->getContentSize();
        //��ȡ��Ƭ��С
        tileSize = tileMap->getTileSize();
        CCLOG("Tile size: (%f, %f)", tileSize.width, tileSize.height);
        // ������Ƭ��ͼ��ʹ������������Ļ
        tileMap->setScaleX(screenSize.width / tileMapSize.width);
        tileMap->setScaleY(screenSize.height / tileMapSize.height);

        // ������Ƭ��ͼ��ê��Ϊ (0, 0)��ʹ��ͼ���½�����Ļ���½Ƕ���
        tileMap->setAnchorPoint(Vec2::ZERO);
        tileMap->setPosition(Vec2::ZERO);
        // ��ȡ������ plantable ��
        plantableLayer = tileMap->getLayer("plantable");
        if (plantableLayer) {
            plantableLayer->setVisible(false);  // ����ʾ plantable ��
        }
        else {
            CCLOG("Layer 'plantable' not found in map: %s", mapFiles[levelId - 1].c_str());
        }
        // ����Ƭ��ͼ���ӵ���ǰ������
        this->addChild(tileMap, 0);
#ifdef DEBUG_MODE
        drawGrid();
#endif
    }
    else {
        CCLOG("Failed to load map: %s", mapFiles[levelId - 1].c_str());
    }
}
//��ʼ��ui���
void BaseLevelScene::initUI()
{
    // 1. ��ʾ�����˶��ٲ�����  
    _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", std::min(manager->getCurrentWaveNum(), manager->getAllWaveNum())), "Arial", 32);
    _curNumberLabel->setColor(Color3B::YELLOW);
    _curNumberLabel->setPosition(960 * 0.42, 640 * 0.95);
    this->addChild(_curNumberLabel, 2);
    // 2. һ���ж��ٲ�����
    _numberLabel = Label::createWithSystemFont(StringUtils::format(" / %d times", manager->getAllWaveNum()), "Arial", 32);
    _numberLabel->setColor(Color3B::YELLOW);
    _numberLabel->setPosition(960 * 0.50, 640 * 0.95);
    this->addChild(_numberLabel, 2);
    //��money
    moneyHud = carrot::ui::widgets::MoneyHud::create(money);
    if (moneyHud) {
        moneyHud->setPosition(Vec2(160, 610));
        this->addChild(moneyHud, 3);
    }
    //���ӷ��ذ�ť
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 2);
    //����
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
    //��ͣ
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
    //�˵�
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
    //����
    auto wenhaoButton = MenuItemImage::create("CarrotGuardRes/UI/wenhao_0.png", "CarrotGuardRes/UI/wenhao_1.png", CC_CALLBACK_1(BaseLevelScene::wenhao, this));
    if (menuButton == nullptr)
    {
        problemLoading("'wenhao_0.png' and 'wenhao_1.png'");
    }
    else
    {
        wenhaoButton->setPosition(350, 28);
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
        jineng1Button->setPosition(525, 33);//525,33
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
        jineng2Button->setPosition(710, 33);//710
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
        jineng3Button->setPosition(432, 33);//432
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
        jineng4Button->setPosition(615, 33);//615
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
        jineng5Button->setPosition(796, 33);
        jineng5Button->setScale(1.7);
        menu->addChild(jineng5Button);
    }

    auto jinengtiao = Sprite::create("Carrot/jinengtiao.png");
    jinengtiao->setPosition(480, 30);
    jinengtiao->setScale(0.7);
    this->addChild(jinengtiao, 1);
}
//���ֵ���ʱ
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
    // ���õ���sequence����
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

            // ����ʱ�����������ص�
            if (onComplete) {
                onComplete();  // ִ�лص�����
            }
            }),
        nullptr  // Sequence����
    );
    // ���е���ʱ����
    this->runAction(countdown);
}
//��ʼ���ؿ�
bool BaseLevelScene::initWithLevel(int level)
{
    if (!Scene::init())
    {
        return false;
    }
    hasGameOverTriggered = false;
    Director::getInstance()->resume();
    this->levelId = level;                     //�洢�ؿ����
    this->loadMap();                           // ���ض�Ӧ�ؿ��ĵ�ͼ
    manager = GameManager::getInstance(this);  // ��ʼ��GameManager
    manager->initLevel(level, !isNewGame[levelId - 1]);          //��ʼ������ ·���ʹ浵����Դ
    initUI();                                 //��ʼ��ui


    // 1. ����ֲ��ͼ��
    plantsLayer = Layer::create();  // ����һ���µ�ͼ�㣬���ڴ��ֲ��
    this->addChild(plantsLayer, 10); // ��ֲ��ͼ�����ӵ�������zOrderΪ3��ȷ����λ��������֮��
    addMouseListener();  // ����������

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

    // ���õ���ʱ���������ݻص�
    CountDown([=] {
        CCLOG("READY");
        //��ʱ��������ܿ�ʼ���޲�
        manager->startMonsterWaves();
        tower_jiasu = 1;
        });

    this->scheduleUpdate();                    //���������߼�


    return true;
}
// Ĭ�ϵ� init ����
bool BaseLevelScene::init() {
    if (!Scene::init()) {
        return false;
    }
    return true;
}
//������Ϸ״̬
void BaseLevelScene::update(float deltaTime) {
    
    //���µ�ǰ����
    updateCurrentWaveLabe();
    //���������͹��޽���
    for (auto it = towers.begin(); it != towers.end(); it++) {

        if (it->second->interval >= it->second->interval_table[it->second->GetIndex()]) {
            it->second->attack(this, GameManager::getInstance()->monsters, isTarget, tar_m, tar_o,tower_jiasu);
            continue;
        }

        it->second->interval += deltaTime*tower_jiasu;
    }
    //���¼�����״̬
    if(manager->CheckLose())
    {
       gameover(false,manager->getCurrentWaveNum(), manager->getAllWaveNum());
    }
    //���¼���Ӯ״̬
    if(manager->CheckWin())
    {
        gameover(true, manager->getCurrentWaveNum(), manager->getAllWaveNum());
    }
    time_total += deltaTime;
    if (time_total >= 5) {
        time_total -= 5;
        manager->doudong();
    }
}
//��ʼ��map
void BaseLevelScene::InitMapData()//��ʼ����ͼ����
{
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++) {
            map_data[i][j].flag = obpos[levelId - 1][i][j];
            map_data[i][j].key = Y * i + j;
        }//3��ʾ�˵ؿ����������ϰ���ں��������ϰ���ʱ���õ� 
}
//�����ϰ���
void BaseLevelScene::ProduceObstacles()//�ڳ����������ϰ���
{
    srand(time(0));
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++) {
            if (map_data[i][j].flag == 3) {
                int rd;//�������
                if (i + 1 < X && j + 1 < Y && map_data[i + 1][j].flag == 3 && map_data[i][j + 1].flag == 3 && map_data[i + 1][j + 1].flag == 3)
                    rd = 10;
                else if (i + 1 < X && map_data[i + 1][j].flag == 3) rd = 6;
                else rd = 5;
                int index = rand() % (rd + 5);
                if (index >= rd) {//��������ÿ���������ϰ���ĵط��������ϰ���
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

/*****************�������*************************/
 //����������
void BaseLevelScene::addMouseListener() {
    auto listener = EventListenerMouse::create();
    listener->onMouseDown = CC_CALLBACK_1(BaseLevelScene::handleMouseDown, this); // ��갴��ʱ�Ļص�

    // �����������ӵ��¼��ɷ���
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
//����������¼�
void BaseLevelScene::handleMouseDown(EventMouse* event) {
    // ��ȡ�����λ��
    Vec2 mousePos = event->getLocation();
    CCLOG("Mouse clicked at screen position: (%f, %f)", mousePos.x, mousePos.y);
    // ���� handlePlant ������������ֲ
    handlePlant(mousePos);
}
//������ֲ�������ж�
void BaseLevelScene::handlePlant(const Vec2& position) {
    CCLOG("position: (%f, %f)", position.x, position.y);
    // �����������Ļ����ת��Ϊ��Ƭ����
    Vec2 tileCoord = Vec2(floor(position.x / tileSize.width), floor(position.y / tileSize.height));
    //������־֤���Ѿ�ʵ���������ת��
    CCLOG("tileCoord: (%f, %f)", tileCoord.x, tileCoord.y);

    // �߽���
    if (tileCoord.x < 0 || tileCoord.y < 0 ||
        tileCoord.x >= tileMap->getMapSize().width ||
        tileCoord.y >= tileMap->getMapSize().height) {
        CCLOG("Tile coordinate is out of bounds.");
        return;
    }

    if (isTarget != 0) {//������������ڼ�������Ŀ�꣬��ȡ������
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

    // �жϸ���Ƭ�Ƿ����ֲ
    auto tileGID = tileMap->getLayer("plantable")->getTileGIDAt(tileCoord);

    if (!cell_flag) {//����Ѿ�������ֲ���������˵������ٴε��ʱ�ж��Ƿ������ֲ������(ɾ��)��������ʹ�˵���ʧ
        if (map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].flag == 0)
            PlantMenuGone(Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y));
        else if (map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].flag == 1)
            UpMenuGone(Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y));
        return;
    }

    if (tileGID != 0) {
        CCLOG("Tile at (%f, %f) is plantable.", tileCoord.x, tileCoord.y);
        // ����ʵ����ֲ�߼�
       // ����Ƭ����ת��Ϊ��Ļ����
       // ��ȡ��ͼ�ĸ߶ȣ���Ƭ��������
        float mapHeight = tileMap->getMapSize().height;

        // ת��Ϊ��Ļ����
        float screenX = tileCoord.x * tileSize.width + tileSize.width / 2;
        float screenY = (mapHeight - tileCoord.y - 1) * tileSize.height + tileSize.height / 2;

        Vec2 mapPos = Vec2(screenX, screenY);

        // �����Ļ����
        CCLOG("Screen Position: (%f, %f)", mapPos.x, mapPos.y);

        //�����ط�û��������ô����ֲ�˵�������������˵�
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
//��ֲ�������
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
        int tool = Tower::build_cost[index_table[i]] > money ? 1 : 0;
        auto tower_graph = Sprite::create(buy_tower[tool][i]);
        tower_graph->setScale((float)CELL_SIZE / tower_graph->getContentSize().width);
        tower_graph->setPosition(Vec2(location.x - CELL_SIZE * (size - 1) / 2 + CELL_SIZE * i, location.y + CELL_SIZE));
        this->addChild(tower_graph); remove_table.push_back(tower_graph);
    }
}
//��ֲ������ʧ
void BaseLevelScene::PlantMenuGone(Vec2 position)
{
    auto location = position;
    int size = remove_table.size();
    if ((location.x > last_position.x - CELL_SIZE * size / 2 && location.x < last_position.x + CELL_SIZE * size / 2)
        && (location.y > last_position.y + CELL_SIZE / 2 && location.y < last_position.y + 3 * CELL_SIZE / 2)) {
        int index = index_table[(location.x - last_position.x + CELL_SIZE * size / 2) / CELL_SIZE];
        if (money >= Tower::build_cost[index]) {
            Tower* this_tower = createTower(index);
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
//�жϹ����Ƿ�����
Monster* BaseLevelScene::IsTargetMonster(const Vec2& pos)//�ж��Ƿ��й��ﱻ����
{
    for (auto it = GameManager::getInstance()->monsters.begin(); it != GameManager::getInstance()->monsters.end(); it++) {
        if ((*it)->getHealth() <= 0)continue;
        auto m_pos = (*it)->getPosition();
        auto m_size = (*it)->getContentSize();
        if (pos.x > m_pos.x - m_size.width && pos.x < m_pos.x + m_size.width
            && pos.y>m_pos.y - m_size.height && pos.y < m_pos.y + m_size.height) return (*it);
    }
    return nullptr;
}
/******************************************/

/*****************���ߺ���*************************/
 //��������
//#ifdef DEBUG_MODE
void BaseLevelScene::drawGrid() {
    // ��ȡ��ͼ�Ĵ�С����Ƭ�Ĵ�С
    Size mapSize = tileMap->getContentSize();
    float tileWidth = 64.0f;  // ����ÿ����Ƭ����Ϊ 64 ����
    float tileHeight = 64.0f; // ����ÿ����Ƭ�߶�Ϊ 64 ����

    // ����һ�� DrawNode ���ڻ�������
    auto drawNode = DrawNode::create();
    this->addChild(drawNode, 100);  // �� DrawNode ���ӵ������У��㼶����Ϊ 100��ȷ���ڵ�ͼ֮��

    // ���ƴ�ֱ��
    for (float x = 0; x <= mapSize.width; x += tileWidth) {
        drawNode->drawLine(Vec2(x, 0), Vec2(x, mapSize.height), Color4F::WHITE);
    }

    // ����ˮƽ��
    for (float y = 0; y <= mapSize.height; y += tileHeight) {
        drawNode->drawLine(Vec2(0, y), Vec2(mapSize.width, y), Color4F::WHITE);
    }
}
//#endif // DEBUG_MODE
//�߸�����ת��ͼ����Ĺ��ߺ���
Vec2 BaseLevelScene::gridToScreenCenter(const Vec2& gridPoint) {
    // �� tileMap ��ȡ��ͼ�߶ȣ�����������
    float mapHeight = tileMap->getMapSize().height;
    // ת��Ϊ��Ļ����
    float screenX = gridPoint.x * tileSize.height + tileSize.width / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * tileSize.height + tileSize.height / 2;
    return Vec2(screenX, screenY);
}
/************************************************/


/*****************�浵���*************************/
// �ؿ��浵����
void BaseLevelScene::saveGameState() {

    // ���� JSON �ĵ�����
    Document document;
    document.SetObject();  // ����Ϊ JSON ����

    // ����һ�� JSON ���飬�����洢�ؿ�״̬
    rapidjson::Value GameState(kArrayType);

    // ���ؿ�״̬������ӵ� GameState ������
    for (auto level : level_is_win) {
        GameState.PushBack(rapidjson::Value(level), document.GetAllocator());
    }

    // �� GameState �������ӵ� JSON �ĵ�������
    document.AddMember("levels", GameState, document.GetAllocator());

    // �� JSON ����ת��Ϊ�ַ���
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    document.Accept(writer);  // д�� JSON �ַ���

    // ��ȡ��д·��
    std::string writablePath = FileUtils::getInstance()->getWritablePath();  // ��ȡ��д·��

    // �����ļ�·���������ļ����� level_state.json��
    std::string filePath = writablePath + "level_state.json";  // ���������ļ�·��

    // д���ļ�
    std::ofstream ofs(filePath);  // ʹ�ÿ�д·��
    if (ofs.is_open()) {
        ofs << buffer.GetString();  // д�� JSON ����
        ofs.close();  // �ر��ļ�
        CCLOG("�浵�ɹ���%s", filePath.c_str());
    }
    else {
        CCLOG("�浵ʧ�ܣ�%s", filePath.c_str());
    }
}
//�����浵����
void BaseLevelScene::saveTowerData()
{
    // ���� JSON �ĵ�����
    Document document;
    document.SetObject();  // ����Ϊ JSON ����
    //����һ��rapidjson����洢������Ϣ
    rapidjson::Value towerArray(rapidjson::kArrayType);
    for(int i=0;i<X;i++)
    {
        //����һ��rapidjson����洢ÿ����Ϣ
     rapidjson::Value rowArray(rapidjson::kArrayType);
     for (int j = 0; j < Y; j++)
     {
     rapidjson::Value towerObj(rapidjson::kObjectType);
     //��䵥����Ԫ
     //�洢flag
     towerObj.AddMember("flag",map_data[i][j].flag, document.GetAllocator());
     //�洢index
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
     //�����
     rowArray.PushBack(towerObj, document.GetAllocator());
     }
     //����ĵ�
     towerArray.PushBack(rowArray, document.GetAllocator());
    }
    document.AddMember("towers", towerArray,document.GetAllocator());
    document.AddMember("money", money, document.GetAllocator()); // ���뵱ǰ������
    // �� JSON ����ת��Ϊ�ַ���
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    document.Accept(writer);  // д�� JSON �ַ���

    // ��ȡ��д·��
    std::string writablePath = FileUtils::getInstance()->getWritablePath();  // ��ȡ��д·��

    // �����ļ�·���������ļ����� level_state.json��
    std::string filePath = writablePath + "level"+std::to_string(levelId)+"_tower.json";  // ���������ļ�·��

    // д���ļ�
    std::ofstream ofs(filePath);  // ʹ�ÿ�д·��
    if (ofs.is_open()) {
        ofs << buffer.GetString();  // д�� JSON ����
        ofs.close();  // �ر��ļ�
        CCLOG("�浵�ɹ���%s", filePath.c_str());
    }
    else {
        CCLOG("�浵ʧ�ܣ�%s", filePath.c_str());
    }
}
//������������
bool BaseLevelScene::loadTowerData(const std::string& filename)
{
    // �� JSON �ļ�
    std::string writablePath = FileUtils::getInstance()->getWritablePath();  // ��ȡ��д·��
    std::string path = writablePath + filename ;//�����ļ�·��
    std::string fileContent = FileUtils::getInstance()->getStringFromFile(path);
    // ʹ�� rapidjson �����ļ�����
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    if (doc.HasParseError()) {
        CCLOG("Error parsing JSON file: %s", filename.c_str());
        return false;
    }
    // ����Ƿ��������
    if (doc.HasMember("towers") && doc["towers"].IsArray()) {
        // ��ȡ tower����
        const rapidjson::Value& towersArray = doc["towers"];
            if (towersArray.IsArray()) {
                for (rapidjson::SizeType i = 0; i < towersArray.Size(); ++i) {
                    const rapidjson::Value& row = towersArray[i];
                    if (row.IsArray()) {
                        for (rapidjson::SizeType j = 0; j < row.Size(); ++j) {
                            const rapidjson::Value& towerObj = row[j];
                            if (towerObj.IsObject()) {
                                // ��ȡÿ������ "flag"��"index" �� "data"
                                int flag = towerObj["flag"].GetInt();
                                int index = towerObj["index"].GetInt();
                                int data = towerObj["data"].GetInt();
                                
                               if(flag==1){
                                   Vec2 pos= pos = Vec2((i+ 0.5) * CELL_SIZE, (j+0.5) * CELL_SIZE);
                                   auto tower=createTower(index,data);
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
    //�����
    if (doc.HasMember("money") && doc["money"].IsInt()) {
        money = doc["money"].GetInt();
        updateMoney(0);
        CCLOG("INIT_MONEY:currentIndex: %d", money);
    }
    else {
        CCLOG("No such level: %s", filename);
        return false;
    }
    return true;
}
/************************************************/