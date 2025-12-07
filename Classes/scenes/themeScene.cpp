#include"advantureScene.h"
#include"themeScene.h"
#include"music.h"
#include "ui/CocosGUI.h"
#include"BaseLevelScene.h"
#include "core/state/GameFlowController.h"
#include "core/state/GameFlowProvider.h"
#include "core/state/GameStateProvider.h"
#include "core/state/LoadingState.h"
#include "json/document.h"
#include "json/rapidjson.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include<fstream>
using namespace rapidjson;
extern bool level_is_win[3];
extern bool isNewGame[3];

void themescene::initCustomUI() {
    loadGameState();

    // Hello World
    auto topLabel = Label::createWithSystemFont("Hello World", "Arial", 32);
    topLabel->setColor(Color3B::WHITE);
    topLabel->setPosition(screenSize.width * 0.49, screenSize.height * 0.95);
    this->addChild(topLabel, 2);

    // PageView
    auto pageView = PageView::create();
    pageView->setContentSize(Size(screenSize.width, screenSize.height));
    pageView->setPosition(Vec2::ZERO);
    this->addChild(pageView);

    std::vector<std::string> mapImages = {
        "CarrotGuardRes/UI/SkyLineLevel_1.png", "CarrotGuardRes/UI/SkyLineLevel_2.png",
        "CarrotGuardRes/UI/SkyLineLevel_3.png", "CarrotGuardRes/UI/SkyLineLevel_4.png"
    };

    for (int i = 0; i < mapImages.size(); ++i) {
        Layout* layout = Layout::create();
        layout->setContentSize(Size(screenSize.width, screenSize.height));
        ImageView* imageView = ImageView::create(mapImages[i]);
        imageView->setContentSize(Size(screenSize.width, screenSize.height));
        imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
        imageView->setScale(2.0f);
        layout->addChild(imageView, 1);
        
        // 逻辑保持不变：金萝卜与锁
        if ((i == 0 && level_is_win[0]) || (i == 1 && level_is_win[1]) || (i == 2 && level_is_win[2])) {
            ImageView* a = ImageView::create("CarrotGuardRes/UI/goldenCarrot.png");
            a->setPosition(Vec2(layout->getContentSize().width / 2 * 1.45, layout->getContentSize().height / 2 * 0.65));
            a->setScale(1.5f);
            layout->addChild(a, 2);
        }
        if ((i == 1 && !level_is_win[0]) || (i == 2 && !level_is_win[1])) {
            ImageView* b = ImageView::create("CarrotGuardRes/UI/suo.png");
            b->setPosition(Vec2(layout->getContentSize().width / 2 * 1.45, layout->getContentSize().height / 2 * 0.65));
            layout->addChild(b, 2);
        }
        pageView->addPage(layout);
    }

    // 事件监听
    pageView->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::BEGAN) clearRelatedButtons();
        if (type == Widget::TouchEventType::ENDED) {
            Music::getInstance()->button_music();
            PageView* pageView = dynamic_cast<PageView*>(pSender);
            clearRelatedButtons();
            int currentIndex = pageView->getCurrentPageIndex();
            
            // 关卡选择逻辑
            if (currentIndex == 0 || (currentIndex == 1 && level_is_win[0]) || (currentIndex == 2 && level_is_win[1])) {
                auto triggerLoading = [currentIndex]() {
                    int levelId = currentIndex + 1;
                    auto flowController = carrot::core::state::GameFlowProvider::Get();
                    if (flowController) {
                        flowController->StartLoadingLevel(levelId);
                        return;
                    }
                    auto context = carrot::core::state::GameStateProvider::Get();
                    if (context) {
                        context->SetState(std::make_shared<carrot::core::state::LoadingState>(levelId));
                    }
                };

                // 读档与开始按钮
                auto confirmButton = Button::create("CarrotGuardRes/UI/fileNormal.png", "CarrotGuardRes/UI/fileSelected.png");
                confirmButton->setName("confirmButton");
                confirmButton->setPosition(Vec2(screenSize.width * 0.6, screenSize.height * 0.13));
                confirmButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
                    if (type == Widget::TouchEventType::ENDED) {
                        Music::getInstance()->button_music();
                        isNewGame[currentIndex] = false;
                        triggerLoading();
                    }
                });
                this->addChild(confirmButton);

                auto cancelButton = Button::create("CarrotGuardRes/UI/startNormal.png", "CarrotGuardRes/UI/startSelected.png");
                cancelButton->setName("cancelButton");
                cancelButton->setPosition(Vec2(screenSize.width * 0.4, screenSize.height * 0.13));
                cancelButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
                    if (type == Widget::TouchEventType::ENDED) {
                        Music::getInstance()->button_music();
                        isNewGame[currentIndex] = true;
                        triggerLoading();
                    }
                });
                this->addChild(cancelButton);
            }
            else {
                // 锁定提示
                auto lockedButton = Button::create("CarrotGuardRes/UI/locked.png");
                lockedButton->setName("lockedButton");
                lockedButton->setPosition(Vec2(screenSize.width * 0.5, screenSize.height * 0.13));
                this->addChild(lockedButton);

                // 未解锁提示弹窗
                auto boss1 = Sprite::create("CarrotGuardRes/UI/un_select2.png");
                boss1->setTag(100);
                boss1->setScale(0.5);
                boss1->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
                this->addChild(boss1, 2);

                auto menu1 = Menu::create(NULL);
                menu1->setName("know_menu");
                menu1->setPosition(Vec2::ZERO);
                this->addChild(menu1, 3);
                
                auto know = MenuItemImage::create("CarrotGuardRes/UI/know.png", "CarrotGuardRes/UI/know.png", CC_CALLBACK_1(themescene::know_Callback, this));
                know->setScale(1);
                know->setPosition(Vec2(origin.x + visibleSize.width / 1.8, origin.y + visibleSize.width / 3.8));
                menu1->addChild(know);
            }
        }
    });

    // 左右按钮
    auto leftButton = Button::create("CarrotGuardRes/UI/leftButtonNormal.png", "CarrotGuardRes/UI/leftButtonSelected.png");
    leftButton->setPosition(Vec2(screenSize.width * 0.1, screenSize.height / 2));
    leftButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Music::getInstance()->button_music();
            int currentIndex = pageView->getCurrentPageIndex();
            if (currentIndex > 0) {
                pageView->scrollToPage(currentIndex - 1);
                clearRelatedButtons();
            }
        }
    });
    this->addChild(leftButton, 2);

    auto rightButton = Button::create("CarrotGuardRes/UI/rightButtonNormal.png", "CarrotGuardRes/UI/rightButtonSelected.png");
    rightButton->setPosition(Vec2(screenSize.width * 0.9, screenSize.height / 2));
    rightButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Music::getInstance()->button_music();
            int currentIndex = pageView->getCurrentPageIndex();
            if (currentIndex < 0) currentIndex = 0;
            if (currentIndex < mapImages.size() - 1) {
                pageView->scrollToPage(currentIndex + 1);
                clearRelatedButtons();
            }
        }
    });
    this->addChild(rightButton, 2);
}

void themescene::loadGameState() {
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string filePath = writablePath + "level_state.json";
    std::ifstream ifs(filePath);
    if (ifs.is_open()) {
        std::string jsonContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        ifs.close();
        Document document;
        document.Parse(jsonContent.c_str());
        if (document.HasMember("levels") && document["levels"].IsArray()) {
            const rapidjson::Value& levels = document["levels"];
            for (SizeType i = 0; i < levels.Size(); i++) {
                level_is_win[i] = levels[i].GetBool();
            }
        }
    }
}

void themescene::clearRelatedButtons() {
    auto confirm = this->getChildByName("confirmButton");
    auto cancel = this->getChildByName("cancelButton");
    auto locked = this->getChildByName("lockedButton");
    if (confirm && cancel) {
        this->removeChildByName("confirmButton");
        this->removeChildByName("cancelButton");
    }
    if (locked) this->removeChildByName("lockedButton");
    auto know_menu = this->getChildByName("know_menu");
    if (know_menu) this->removeChild(know_menu);
    auto boss1 = this->getChildByTag(100);
    if (boss1) this->removeChild(boss1);
}

void themescene::know_Callback(Ref* pSender) {
    clearRelatedButtons();
}