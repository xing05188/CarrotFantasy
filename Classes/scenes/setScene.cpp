#include"menuScene.h"
#include"setScene.h"
#include"music.h"
#include"ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in setScene.cpp\n");
}

// 【重构】将原 init() 的逻辑移入 initCustomUI
void setscene::initCustomUI() {
    // 1. 音乐标签
    auto musicLabel_1 = Label::createWithSystemFont("music", "Arial", 27);
    musicLabel_1->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.75));
    musicLabel_1->setColor(Color3B::BLACK);
    this->addChild(musicLabel_1, 2);

    // 2. 音乐按钮
    auto playMusicButton = MenuItemImage::create("CarrotGuardRes/UI/MusicOn.png", "CarrotGuardRes/UI/MusicClose.png", CC_CALLBACK_1(setscene::paly_music, this));
    playMusicButton->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.68));
    playMusicButton->setScale(1.7f * scaleX);
    
    if (Music::getInstance()->is_play()) {
        playMusicButton->setNormalImage(Sprite::create("CarrotGuardRes/UI/MusicOn.png"));
        playMusicButton->setSelectedImage(Sprite::create("CarrotGuardRes/UI/MusicOn.png"));
    } else {
        playMusicButton->setNormalImage(Sprite::create("CarrotGuardRes/UI/MusicClose.png"));
        playMusicButton->setSelectedImage(Sprite::create("CarrotGuardRes/UI/MusicClose.png"));
    }
    menu->addChild(playMusicButton); // 加入基类的 menu

    // 3. 下拉框逻辑 (原样保留)
    auto bgmSelectButton = Button::create("CarrotGuardRes/UI/musicselect0.png", "CarrotGuardRes/UI/musicselect1.png");
    bgmSelectButton->setTitleText("BGM 0");
    bgmSelectButton->setTitleFontSize(18);
    bgmSelectButton->setPosition(Vec2(visibleSize.width * 0.5 , visibleSize.height * 0.5));
    bgmSelectButton->setScale(1.10f * scaleX);
    
    auto dropdownLayout = Layout::create();
    dropdownLayout->setContentSize(Size(180 * scaleX, 120 * scaleX));
    dropdownLayout->setPosition(Vec2(visibleSize.width * 0.5  - 90 * scaleX, visibleSize.height * 0.5 - 60 * scaleX));
    dropdownLayout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    dropdownLayout->setBackGroundColor(Color3B(250, 250, 250));
    dropdownLayout->setBackGroundColorOpacity(150);
    dropdownLayout->setVisible(false);
    dropdownLayout->setLocalZOrder(10);
    
    std::vector<std::string> bgmOptions = {"BGM 0", "BGM 1", "BGM 2"};
    float buttonHeight = 40 * scaleX;
    for (int i = 0; i < bgmOptions.size(); i++) {
        auto optionButton = Button::create("CarrotGuardRes/UI/musicselect0.png", "CarrotGuardRes/UI/musicselect1.png");
        optionButton->setTitleText(bgmOptions[i]);
        optionButton->setTitleFontSize(16);
        optionButton->setPosition(Vec2(90 * scaleX, buttonHeight * (bgmOptions.size() - i - 0.5f)));
        optionButton->setScaleX(1.0f);
        optionButton->addTouchEventListener([this, i, bgmOptions, bgmSelectButton, dropdownLayout](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                bgmSelectButton->setTitleText(bgmOptions[i]);
                dropdownLayout->setVisible(false);
                Music::getInstance()->changeBackgroundMusic(i);
            }
        });
        dropdownLayout->addChild(optionButton);
    }
    
    bgmSelectButton->addTouchEventListener([dropdownLayout](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            dropdownLayout->setVisible(!dropdownLayout->isVisible());
        }
    });
    
    // 点击外部隐藏下拉框
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [dropdownLayout, bgmSelectButton](Touch* touch, Event* event) {
        if (!dropdownLayout->isVisible()) return false;
        Vec2 location = touch->getLocation();
        Rect bgmButtonRect = bgmSelectButton->getBoundingBox();
        Rect dropdownRect = dropdownLayout->getBoundingBox();
        if (!bgmButtonRect.containsPoint(location) && !dropdownRect.containsPoint(location)) {
            dropdownLayout->setVisible(false);
        }
        return false;
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    this->addChild(bgmSelectButton, 2);
    this->addChild(dropdownLayout, 9);

    // 4. 作者信息
    auto authorLabel_1 = Label::createWithSystemFont("Developers: zhou,yu,guan", "Arial", 27);
    auto authorLabel_2 = Label::createWithSystemFont("Development time: 2024/11/26", "Arial", 27);
    authorLabel_1->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.35));
    authorLabel_2->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.3));
    this->addChild(authorLabel_1, 2);
    this->addChild(authorLabel_2, 2);
}

void setscene::paly_music(Ref* pSender) {
	Music::getInstance()->button_music();
	MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
	//��֮ǰû����Ч
	if (!Music::getInstance()->is_play()) {
		button->setNormalImage(Sprite::create("CarrotGuardRes/UI/soundOn.png"));
		button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/soundOn.png"));
		Music::getInstance()->set_music(1);
		Music::getInstance()->background_music();
	}
	//��֮ǰ������Ч
	else {
		button->setNormalImage(Sprite::create("CarrotGuardRes/UI/soundClose.png"));
		button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/soundClose.png"));
		Music::getInstance()->set_music(0);
		Music::getInstance()->background_music();
	}
}