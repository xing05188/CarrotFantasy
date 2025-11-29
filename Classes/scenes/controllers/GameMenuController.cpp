#include "GameMenuController.h"
#include "BaseLevelScene.h"
#include "core/GameManager.h"
#include "audio/music.h"
#include "ui/CocosGUI.h"

using namespace ui;

GameMenuController::GameMenuController()
    : initialized_(false)
{
}

GameMenuController::~GameMenuController()
{
}

void GameMenuController::init(const MenuCallbacks& callbacks)
{
    callbacks_ = callbacks;
    initialized_ = true;
}

void GameMenuController::showGameMenu(Ref* pSender)
{
    if (!initialized_) {
        CCLOG("GameMenuController: Not initialized!");
        return;
    }

    Music::getInstance()->button_music();
    callbacks_.pauseDirector();
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();
    
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
    menuLayer->setPosition(Vec2::ZERO);
    callbacks_.addChildToScene(menuLayer);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [menuLayer](Touch* touch, Event* event) {
        return true;
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, menuLayer);
    
    auto menuBackground = Sprite::create("CarrotGuardRes/UI/gameMenu.png");
    menuBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    menuBackground->setScale(1.5f);
    menuLayer->addChild(menuBackground, 0);
    
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    menuLayer->addChild(menu, 1);
    
    // 继续按钮
    auto continueButton = MenuItemImage::create("CarrotGuardRes/UI/continueNormal.png", "CarrotGuardRes/UI/continueSelected.png");
    continueButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.649));
    continueButton->setScale(1.5);
    continueButton->setCallback([this, menuLayer](Ref* psender) {
        Music::getInstance()->button_music();
        callbacks_.removeChild(menuLayer);
        if (!callbacks_.isGamePaused()) {
            callbacks_.resumeDirector();
        }
    });
    
    // 重启按钮
    auto restartButton = MenuItemImage::create("CarrotGuardRes/UI/restartNormal.png", "CarrotGuardRes/UI/restartSelected.png");
    restartButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.51));
    restartButton->setScale(1.5);
    restartButton->setCallback([this, menuLayer](Ref* psender) {
        Music::getInstance()->button_music();
        callbacks_.removeChild(menuLayer);
        callbacks_.stopAllSchedulers();
        callbacks_.removeListener();
        auto scene = callbacks_.createLevelScene(callbacks_.getLevelId());
        callbacks_.setTimeScale(1.0f);
        callbacks_.replaceScene(scene);
        callbacks_.resumeDirector();
    });
    
    // 选择关卡按钮
    auto chooseButton = MenuItemImage::create("CarrotGuardRes/UI/chooseLevelNormal.png", "CarrotGuardRes/UI/chooseLevelSelected.png");
    chooseButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.375));
    chooseButton->setScale(1.5);
    chooseButton->setCallback([this, menuLayer](Ref* psender) {
        Music::getInstance()->button_music();
        callbacks_.saveGameState();
        callbacks_.saveMonstersData("level" + std::to_string(callbacks_.getLevelId()) + "Monster.json");
        callbacks_.saveTowerData();
        callbacks_.removeChild(menuLayer);
        callbacks_.stopAllSchedulers();
        callbacks_.removeListener();
        callbacks_.transitionToLevelSelectState();
    });
    
    menu->addChild(continueButton, 1);
    menu->addChild(chooseButton, 1);
    menu->addChild(restartButton, 1);
}

void GameMenuController::showHelpMenu(Ref* pSender)
{
    if (!initialized_) {
        CCLOG("GameMenuController: Not initialized!");
        return;
    }

    Music::getInstance()->button_music();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();
    
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
    menuLayer->setPosition(Vec2::ZERO);
    callbacks_.addChildToScene(menuLayer);
    
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
    
    createHelpPageView(menuLayer, screenSize);
    
    // 关闭按钮
    auto continueButton = MenuItemImage::create("Carrot/jieshao/cha_0.png", "Carrot/jieshao/cha_1.png");
    continueButton->setPosition(814, 503);
    continueButton->setScale(1.9);
    continueButton->setCallback([this, menuLayer](Ref* psender) {
        Music::getInstance()->button_music();
        callbacks_.removeChild(menuLayer);
    });
    menu->addChild(continueButton, 1);
}

void GameMenuController::createHelpPageView(Node* parentLayer, const Size& screenSize)
{
    auto pageView = PageView::create();
    pageView->setContentSize(Size(screenSize.width, screenSize.height));
    pageView->setPosition(Vec2::ZERO);
    parentLayer->addChild(pageView);
    
    std::vector<std::string> mapImages = {
        "Carrot/jieshao/jie_1.png",
        "Carrot/jieshao/jie_2.png",
        "Carrot/jieshao/jie_3.png",
        "Carrot/jieshao/jie_4.png",
        "Carrot/jieshao/jie_5.png",
        "Carrot/jieshao/jie_6.png"
    };
    
    for (int i = 0; i < mapImages.size(); ++i) {
        Layout* layout = Layout::create();
        layout->setContentSize(Size(screenSize.width, screenSize.height));
        ImageView* imageView = ImageView::create(mapImages[i]);
        imageView->setContentSize(Size(screenSize.width, screenSize.height));
        imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
        imageView->setScale(2);
        layout->addChild(imageView, 1);
        pageView->addPage(layout);
    }
    
    // 左翻页按钮
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
    parentLayer->addChild(leftButton, 1);
    
    // 右翻页按钮
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
    parentLayer->addChild(rightButton, 1);
}

