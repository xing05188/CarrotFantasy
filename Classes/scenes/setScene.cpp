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

bool setscene::init() {
	if (!Layer::init())
		return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();//�ֱ��ʴ�С
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size screenSize = Director::getInstance()->getWinSize();    //��ȡ��Ļ��С

	//�ű���
	auto backgroundImage = Sprite::create("CarrotGuardRes/UI/SettingBackground.png");
	float scaleX = screenSize.width / backgroundImage->getContentSize().width;     //������ű���
	float scaleY = screenSize.height / backgroundImage->getContentSize().height;
	if (backgroundImage == nullptr)
		problemLoading("'SettingBackground.png'");
	else
	{
		backgroundImage->setScaleX(scaleX);
		backgroundImage->setScaleY(scaleY);
		backgroundImage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
		this->addChild(backgroundImage, 0);
	}

	//���ӷ��ذ�ť
	auto menu = Menu::create();
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	auto returnButton = MenuItemImage::create("CarrotGuardRes/UI/SettingReturnNormal.png", "CarrotGuardRes/UI/SettingReturnSelected.png", CC_CALLBACK_1(setscene::to_menuScene, this));
	if (returnButton == nullptr ||
		returnButton->getContentSize().width <= 0 ||
		returnButton->getContentSize().height <= 0)
	{
		problemLoading("'SettingReturnNormal.png' and 'SettingReturnSelected.png'");
	}
	else
	{
		float x = origin.x + returnButton->getContentSize().width / 2 + visibleSize.width * 0.05f;
		float y = origin.y + visibleSize.height - returnButton->getContentSize().width / 2 - visibleSize.height * 0.04f;
		returnButton->setScale(1.8f * scaleX);
		returnButton->setPosition(Vec2(x, y));
		menu->addChild(returnButton);
	}

	//�������ְ�ť
	auto musicLabel_1 = Label::createWithSystemFont(StringUtils::format("music"), "Arial", 27);
	musicLabel_1->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.75));
	musicLabel_1->setColor(Color3B::BLACK);
	this->addChild(musicLabel_1, 2);
	auto playMusicButton = MenuItemImage::create("CarrotGuardRes/UI/MusicOn.png", "CarrotGuardRes/UI/MusicClose.png", CC_CALLBACK_1(setscene::paly_music, this));
	if (playMusicButton == nullptr)
	{
		problemLoading("'MusicOn.png' and 'MusicClose.png'");
	}
	else
	{
		playMusicButton->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.68));
		playMusicButton->setScale(1.7f * scaleX);
		menu->addChild(playMusicButton);
		if (Music::getInstance()->is_play()) {
			playMusicButton->setNormalImage(Sprite::create("CarrotGuardRes/UI/MusicOn.png"));
			playMusicButton->setSelectedImage(Sprite::create("CarrotGuardRes/UI/MusicOn.png"));
		}
		else {
			playMusicButton->setNormalImage(Sprite::create("CarrotGuardRes/UI/MusicClose.png"));
			playMusicButton->setSelectedImage(Sprite::create("CarrotGuardRes/UI/MusicClose.png"));
		}
	}
// 创建下拉框主按钮
    auto bgmSelectButton = Button::create("CarrotGuardRes/UI/musicselect0.png", "CarrotGuardRes/UI/musicselect1.png");
    bgmSelectButton->setTitleText("BGM 0");
    bgmSelectButton->setTitleFontSize(18);
    bgmSelectButton->setPosition(Vec2(visibleSize.width * 0.5 , visibleSize.height * 0.5));
    bgmSelectButton->setScale(1.10f * scaleX);
    
    // 下拉选项列表（初始隐藏）
    auto dropdownLayout = Layout::create();
    dropdownLayout->setContentSize(Size(180 * scaleX, 120 * scaleX));
    dropdownLayout->setPosition(Vec2(visibleSize.width * 0.5  - 90 * scaleX, visibleSize.height * 0.5 - 60 * scaleX));
    dropdownLayout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    dropdownLayout->setBackGroundColor(Color3B(250, 250, 250));
	dropdownLayout->setBackGroundColorOpacity(150);
    dropdownLayout->setVisible(false);
    dropdownLayout->setLocalZOrder(10); // 确保在最上层
    
    // 下拉框选项数据
    std::vector<std::string> bgmOptions = {"BGM 0", "BGM 1", "BGM 2"};
    
    // 创建选项按钮
    float buttonHeight = 40 * scaleX;
    for (int i = 0; i < bgmOptions.size(); i++) {
        auto optionButton = Button::create("CarrotGuardRes/UI/musicselect0.png", "CarrotGuardRes/UI/musicselect1.png");
        optionButton->setTitleText(bgmOptions[i]);
        optionButton->setTitleFontSize(16);
        optionButton->setPosition(Vec2(90 * scaleX, buttonHeight * (bgmOptions.size() - i - 0.5f)));
        optionButton->setScaleX(1.0f);
        optionButton->addTouchEventListener([this, i, bgmOptions, bgmSelectButton, dropdownLayout](Ref* sender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                // 更新主按钮文本
                bgmSelectButton->setTitleText(bgmOptions[i]);
                // 隐藏下拉列表
                dropdownLayout->setVisible(false);
                // 更新选择的BGM
                Music::getInstance()->changeBackgroundMusic(i);
            }
        });
        dropdownLayout->addChild(optionButton);
    }
    
    // 主按钮点击事件 - 显示/隐藏下拉列表
    bgmSelectButton->addTouchEventListener([dropdownLayout](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            dropdownLayout->setVisible(!dropdownLayout->isVisible());
        }
    });
    
    // 点击其他地方隐藏下拉列表
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [dropdownLayout, bgmSelectButton](Touch* touch, Event* event) {
        if (!dropdownLayout->isVisible()) return false;
        
        Vec2 location = touch->getLocation();
        Rect bgmButtonRect = bgmSelectButton->getBoundingBox();
        Rect dropdownRect = dropdownLayout->getBoundingBox();
        
        // 如果点击在下拉框或主按钮之外，隐藏下拉框
        if (!bgmButtonRect.containsPoint(location) && !dropdownRect.containsPoint(location)) {
            dropdownLayout->setVisible(false);
        }
        
        return false;
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    this->addChild(bgmSelectButton, 2);
    this->addChild(dropdownLayout, 9);

	// ����������Ϣ
	auto authorLabel_1 = Label::createWithSystemFont(StringUtils::format("Developers: zhou,yu,guan"), "Arial", 27);
	auto authorLabel_2 = Label::createWithSystemFont(StringUtils::format("Development time: 2024/11/26"), "Arial", 27);
	authorLabel_1->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.35));
	authorLabel_2->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.3));
	authorLabel_1->setColor(Color3B::WHITE);
	authorLabel_2->setColor(Color3B::WHITE);
	this->addChild(authorLabel_1, 2);
	this->addChild(authorLabel_2, 2);
	return true;
}
// a selector callback
void setscene::to_menuScene(Ref* pSender) {
	Music::getInstance()->button_music();
	Director::getInstance()->popScene();
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