#include"menuScene.h"
#include"setScene.h"
#include"music.h"
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
		playMusicButton->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.64));
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

	// ����������Ϣ
	auto authorLabel_1 = Label::createWithSystemFont(StringUtils::format("Developers: zhou,yu,guan,zheng,yuan"), "Arial", 27);
	auto authorLabel_2 = Label::createWithSystemFont(StringUtils::format("Development time: 2025/11/25"), "Arial", 27);
	auto authorLabel_3 = Label::createWithSystemFont(StringUtils::format("Thank you"), "Arial", 27);
	authorLabel_1->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.4));
	authorLabel_2->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.3));
	authorLabel_3->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.5));
	authorLabel_1->setColor(Color3B::WHITE);
	authorLabel_2->setColor(Color3B::WHITE);
	authorLabel_3->setColor(Color3B::BLACK);
	this->addChild(authorLabel_1, 2);
	this->addChild(authorLabel_2, 2);
	this->addChild(authorLabel_3, 2);

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