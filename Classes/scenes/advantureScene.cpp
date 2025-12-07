#include"menuScene.h"
#include"advantureScene.h"
#include"themeScene.h"
#include"music.h"
#include "ui/CocosGUI.h"
bool level_is_win[3] = { false };
bool isNewGame[3] = { true,true,true };
float beishu=1.0f;
USING_NS_CC;
using namespace ui;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in seeScene.cpp\n");
}

// 【重构】原来的 init() 逻辑移到这里
void advanturescene::initCustomUI() {
    // 1. Hello World Label
    auto topLabel = Label::createWithSystemFont("Hello World", "Arial", 32);
    topLabel->setColor(Color3B::WHITE);
    topLabel->setPosition(screenSize.width * 0.49, screenSize.height * 0.95);
    this->addChild(topLabel, 2);

    // 2. PageView 逻辑
    auto pageView = PageView::create();
    pageView->setContentSize(Size(screenSize.width, screenSize.height));
    pageView->setPosition(Vec2::ZERO);
    this->addChild(pageView);

    std::vector<std::string> mapImages = {
        "CarrotGuardRes/UI/advanture1.png", "CarrotGuardRes/UI/advanture2.png",
        "CarrotGuardRes/UI/advanture3.png", "CarrotGuardRes/UI/advanture4.png",
        "CarrotGuardRes/UI/advanture5.png"
    };

    for (int i = 0; i < mapImages.size(); ++i) {
        Layout* layout = Layout::create();
        layout->setContentSize(Size(screenSize.width, screenSize.height));
        ImageView* imageView = ImageView::create(mapImages[i]);
        imageView->setContentSize(Size(screenSize.width, screenSize.height));
        imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
        imageView->setScale(0.6);
        layout->addChild(imageView, 1);
        pageView->addPage(layout);
    }

    // 事件监听
    pageView->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Music::getInstance()->button_music();
            PageView* pageView = dynamic_cast<PageView*>(pSender);
            int currentIndex = pageView->getCurrentPageIndex();
            
            if (currentIndex == 0) {
                auto themeScene = themescene::createScene();
                Director::getInstance()->pushScene(themeScene);
            } else if (currentIndex >= 1 && currentIndex <= 4) {
                auto boss1 = Sprite::create("CarrotGuardRes/UI/un_select.png");
                boss1->setTag(100);
                boss1->setScale(0.55);
                boss1->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
                this->addChild(boss1, 2);

                auto know = MenuItemImage::create("CarrotGuardRes/UI/know.png", "CarrotGuardRes/UI/know.png", CC_CALLBACK_1(advanturescene::know_Callback, this));
                know->setScale(1);
                know->setPosition(Vec2(origin.x + visibleSize.width / 1.8, origin.y + visibleSize.width / 3.8));
                
                // 注意：这里需要创建一个临时 menu 还是直接加到基类 menu？
                // 原代码是创建了一个新 menu1。为了逻辑一致，我们创建一个临时的 menu1
                auto menu1 = Menu::create(know, NULL);
                menu1->setPosition(Vec2::ZERO);
                // 给 menu1 一个名字方便 clearRelatedButtons 删除？原代码似乎没删 menu1，可能有内存泄露风险，或者依靠场景切换销毁。
                // 我们按照原逻辑，作为子节点添加
                menu1->setName("know_menu"); 
                this->addChild(menu1, 3);
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
            if (currentIndex > 0) pageView->scrollToPage(currentIndex - 1);
            clearRelatedButtons();
        }
    });
    this->addChild(leftButton, 2);

    auto rightButton = Button::create("CarrotGuardRes/UI/rightButtonNormal.png", "CarrotGuardRes/UI/rightButtonSelected.png");
    rightButton->setPosition(Vec2(screenSize.width * 0.9, screenSize.height / 2));
    rightButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Music::getInstance()->button_music();
            int currentIndex = pageView->getCurrentPageIndex();
            if (currentIndex < mapImages.size() - 1) pageView->scrollToPage(currentIndex + 1);
            clearRelatedButtons();
        }
    });
    this->addChild(rightButton, 2);
}

void advanturescene::clearRelatedButtons() {
    auto sprite = this->getChildByTag(100);
    if (sprite) this->removeChild(sprite);
    // 还需要移除那个临时的菜单
    auto menu1 = this->getChildByName("know_menu");
    if (menu1) this->removeChild(menu1);
}

void advanturescene::know_Callback(Ref* pSender) {
    clearRelatedButtons(); // 复用清理逻辑
}