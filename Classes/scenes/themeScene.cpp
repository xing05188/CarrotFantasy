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
extern bool level_is_win[3];//1ͨ���˽�����2��
extern bool isNewGame[3];
extern float beishu;

USING_NS_CC;
using namespace ui;
// ����������һ��ȫ�ֱ������߳�Ա�������洢ҳ��������ݣ������Ƿ�ɲ�����״̬��ʶ
    // ���庬���������Ŀʵ�����ȷ��������ֻ��ʾ��

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in themeScene.cpp\n");
}


// ���������������ض�������صİ�ť����ȷ�ϡ�ȡ����������ť��
   // �����ڶദ�ظ���ͬ�������߼�����
void themescene::clearRelatedButtons() {
    // ��ȡ��Ϊ"confirmButton"���ӽڵ�
    auto confirm = this->getChildByName("confirmButton");
    // ��ȡ��Ϊ"cancelButton"���ӽڵ�
    auto cancel = this->getChildByName("cancelButton");
    // ��ȡ��Ϊ"lockedButton"���ӽڵ�
    auto locked = this->getChildByName("lockedButton");
    // ���ȷ�ϰ�ť��ȡ����ť���ڣ��ʹӵ�ǰ�ڵ��Ƴ�����
    if (confirm && cancel) {
        this->removeChildByName("confirmButton");
        this->removeChildByName("cancelButton");
    }
    // ���������ť���ڣ��ʹӵ�ǰ�ڵ��Ƴ���
    if (locked)
        this->removeChildByName("lockedButton");
}

void themescene::loadGameState() {
	// ��ȡ��д·��
	std::string writablePath = FileUtils::getInstance()->getWritablePath();  // ��ȡ��д·��

	// �����ļ�·�����ļ���Ϊ level_state��
	std::string filePath = writablePath + "level_state.json";  // ���������ļ�·��

	// ���ļ���ȡ
	std::ifstream ifs(filePath);  // ʹ�ÿɶ�·��
	if (ifs.is_open()) {
		// ��ȡ�ļ����ݵ��ַ���
		std::string jsonContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();  // �ر��ļ�

		// ���� JSON �ַ���
		Document document;
		document.Parse(jsonContent.c_str());

		if (document.HasMember("levels") && document["levels"].IsArray()) {
			// ��ȡ levels ����
			const rapidjson::Value& levels = document["levels"];


			// ���� levels ���飬��� level_is_win ����
			for (SizeType i = 0; i < levels.Size(); i++) {
				level_is_win[i] = levels[i].GetBool();  // ��ȡ����ֵ�����ӵ� level_is_win ��
			}

			// ��ӡ�ؿ�״̬
			for (size_t i = 0; i < sizeof(level_is_win) / sizeof(level_is_win[0]); i++) {
				CCLOG("Level %d: %s", (int)i + 1, level_is_win[i] ? "Win" : "Lose");
			}
		}
		else {
			CCLOG("Error: Invalid JSON format or missing 'levels' field.");
		}
	}
	else {
		CCLOG("Failed to open file: %s", filePath.c_str());
	}
}
bool themescene::init() {
    if (!Layer::init())
        return false;
	loadGameState();
    auto visibleSize = Director::getInstance()->getVisibleSize();//�ֱ��ʴ�С
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();    //��ȡ��Ļ��С

	//������ʾ��
	auto topLabel = Label::createWithSystemFont(StringUtils::format("Hello World"), "Arial", 32);
	topLabel->setColor(Color3B::WHITE);
	topLabel->setPosition(screenSize.width * 0.49, screenSize.height * 0.95);
	this->addChild(topLabel, 2);
	//���ӱ���
	auto backgroundImage = Sprite::create("CarrotGuardRes/UI/AdvantureBackground.png");
	float scaleX = screenSize.width / backgroundImage->getContentSize().width;     //������ű���
	float scaleY = screenSize.height / backgroundImage->getContentSize().height;
	if (backgroundImage == nullptr)
		problemLoading("'AdvantureBackground.png'");
	else
	{
		backgroundImage->setScaleX(scaleX);
		backgroundImage->setScaleY(scaleY);
		backgroundImage->setPosition(Vec2(screenSize.width / 2 + origin.x, screenSize.height / 2 + origin.y));
		this->addChild(backgroundImage, 0);
	}
	
	auto menu = Menu::create();
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	//���ӷ��ذ�ť
	auto returnButton = MenuItemImage::create("CarrotGuardRes/UI/ReturnButtonNormal.png", "CarrotGuardRes/UI/ReturnButtonSelected.png", CC_CALLBACK_1(themescene::to_advantureScene, this));
	if (returnButton == nullptr ||
		returnButton->getContentSize().width <= 0 ||
		returnButton->getContentSize().height <= 0)
	{
		problemLoading("'ReturnButtonNormal.png' and 'ReturnButtonSelected.png'");
	}
	else
	{
		float x = origin.x + returnButton->getContentSize().width / 2 + screenSize.width * 0.05f;
		float y = origin.y + screenSize.height - returnButton->getContentSize().width / 2 - screenSize.height * 0.04f;
		returnButton->setPosition(Vec2(x, y));
		returnButton->setScale(3.0f * scaleX);
		menu->addChild(returnButton);
	}

	//�������ҷ�ҳ��ť
	// ����PageView
	auto pageView = PageView::create();
	pageView->setContentSize(Size(screenSize.width, screenSize.height));
	pageView->setPosition(Vec2::ZERO);
	this->addChild(pageView);

	// �����ͼͼƬ����
	std::vector<std::string> mapImages = {
		"CarrotGuardRes/UI/SkyLineLevel_1.png",
		"CarrotGuardRes/UI/SkyLineLevel_2.png",
		"CarrotGuardRes/UI/SkyLineLevel_3.png",
		"CarrotGuardRes/UI/SkyLineLevel_4.png"
	};

	// ����ҳ��
	for (int i = 0; i < mapImages.size(); ++i) {
		Layout* layout = Layout::create();
		layout->setContentSize(Size(screenSize.width, screenSize.height));
		ImageView* imageView = ImageView::create(mapImages[i]);
		imageView->setContentSize(Size(screenSize.width, screenSize.height));
		imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
		imageView->setScale(2.0f);
		layout->addChild(imageView, 1);
		if ((i == 0 && level_is_win[0] == true) || (i == 1 && level_is_win[1] == true) || (i == 2 && level_is_win[2] == true)) {
			ImageView* a = ImageView::create("CarrotGuardRes/UI/goldenCarrot.png");
			a->setPosition(Vec2(layout->getContentSize().width / 2 * 1.45, layout->getContentSize().height / 2 * 0.65));
			a->setScale(1.5f);
			layout->addChild(a, 2);
		}
		if ((i == 1 && level_is_win[0] == false) || (i == 2 && level_is_win[1] == false)) {
			ImageView* b = ImageView::create("CarrotGuardRes/UI/suo.png");
			b->setPosition(Vec2(layout->getContentSize().width / 2 * 1.45, layout->getContentSize().height / 2 * 0.65));
			layout->addChild(b, 2);
		}
		pageView->addPage(layout);
	}

	// ���Ӵ����¼�������
	pageView->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::BEGAN)
			clearRelatedButtons();
		if (type == Widget::TouchEventType::ENDED) {
			Music::getInstance()->button_music();
			PageView* pageView = dynamic_cast<PageView*>(pSender);
			clearRelatedButtons();
			int currentIndex = pageView->getCurrentPageIndex();
			if (currentIndex == 0 || (currentIndex == 1 && level_is_win[0] == true) || (currentIndex == 2 && level_is_win[1] == true)) {
                auto triggerLoading = [currentIndex]() {
                    int levelId = currentIndex + 1;
                    auto flowController = carrot::core::state::GameFlowProvider::Get();
                    if (flowController) {
                        flowController->StartLoadingLevel(levelId);
                        return;
                    }
                    CCLOG("themescene: GameFlowController unavailable, falling back to GameStateProvider for level %d", levelId);
                    auto context = carrot::core::state::GameStateProvider::Get();
                    if (context) {
                        context->SetState(std::make_shared<carrot::core::state::LoadingState>(levelId));
                    }
                };
				// �����浵��ť
				auto confirmButton = Button::create("CarrotGuardRes/UI/fileNormal.png", "CarrotGuardRes/UI/fileSelected.png");
				confirmButton->setName("confirmButton");
				confirmButton->setPosition(Vec2(screenSize.width * 0.6, screenSize.height * 0.13));
				confirmButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
					if (type == Widget::TouchEventType::ENDED) {
						Music::getInstance()->button_music();
						
						isNewGame[currentIndex] = false;
						triggerLoading();
						// ����浵��tmp=1
						//int tmp = 1;
						//auto gameScene = GameScene::createSceneWithLevel(currentIndex + 1, tmp);
						//Director::getInstance()->replaceScene(gameScene);
					}
					});
				this->addChild(confirmButton);

				// ������ʼ��ť
				auto cancelButton = Button::create("CarrotGuardRes/UI/startNormal.png", "CarrotGuardRes/UI/startSelected.png");
				cancelButton->setName("cancelButton");
				cancelButton->setPosition(Vec2(screenSize.width * 0.4, screenSize.height * 0.13));  // ����λ��
				cancelButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
					if (type == Widget::TouchEventType::ENDED) {
						Music::getInstance()->button_music();

						isNewGame[currentIndex] = true;
						// �������л�����һ��
						triggerLoading();
						// ������浵��tmp=0
						//int tmp = 0;
						//auto gameScene = GameScene::createSceneWithLevel(currentIndex + 1, tmp);
						//Director::getInstance()->replaceScene(gameScene);
					}
					});
				this->addChild(cancelButton);
			}
			else {
				auto lockedButton = Button::create("CarrotGuardRes/UI/locked.png");
				lockedButton->setName("lockedButton");
				lockedButton->setPosition(Vec2(screenSize.width * 0.5, screenSize.height * 0.13));  // ����λ��
				this->addChild(lockedButton);


				auto visibleSize = Director::getInstance()->getVisibleSize();//�ֱ��ʴ�С
				Vec2 origin = Director::getInstance()->getVisibleOrigin();

				auto boss1 = Sprite::create("CarrotGuardRes/UI/un_select2.png");
				boss1->setTag(100);
				if (boss1 == nullptr)
					problemLoading("'un_select2.png'");
				else
				{
					boss1->setScale(0.5);
					boss1->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
					this->addChild(boss1, 2);
				}
				auto menu1 = Menu::create(NULL);
				menu1->setPosition(Vec2::ZERO);
				this->addChild(menu1, 3);
				auto know = MenuItemImage::create("CarrotGuardRes/UI/know.png", "CarrotGuardRes/UI/know.png", CC_CALLBACK_1(themescene::know_Callback, this));

				if (know == nullptr ||
					know->getContentSize().width <= 0 ||
					know->getContentSize().height <= 0)
				{
					problemLoading("'know.png'");
				}
				else
				{
					float x = origin.x + visibleSize.width / 1.8;
					float y = origin.y + visibleSize.width / 3.8;
					know->setScale(1);
					know->setPosition(Vec2(x, y));
					menu1->addChild(know);  //���뵽�˵���
				}
			}
		}
		});


	// �������ҷ�ҳ��ť
	auto leftButton = Button::create("CarrotGuardRes/UI/leftButtonNormal.png", "CarrotGuardRes/UI/leftButtonSelected.png");
	leftButton->setPosition(Vec2(screenSize.width * 0.1, screenSize.height / 2));
	leftButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			Music::getInstance()->button_music();
			int currentIndex = pageView->getCurrentPageIndex();
			if (currentIndex > 0) {
				pageView->scrollToPage(currentIndex - 1);

				//������һҳ��ͼ��ѡ��ť
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
			if (currentIndex < 0)
				currentIndex = 0;  //  δ֪ԭ���¿�ʼ��ʱ��index��-1����ʱ������������
			if (currentIndex < mapImages.size() - 1) {
				pageView->scrollToPage(currentIndex + 1);
				clearRelatedButtons();
			}
		}
		});
	this->addChild(rightButton, 2);
	return true;
}

void themescene::to_advantureScene(Ref* pSender) {
	Music::getInstance()->button_music();
	Director::getInstance()->popScene();
}

void themescene::know_Callback(Ref* pSender) {
	auto sprite = this->getChildByTag(100);
	// ����ҵ��˾��飬�ʹ������ڸ��ڵ����Ƴ���
	if (sprite)
		this->removeChild(sprite);
	auto menu = ((MenuItem*)pSender)->getParent();
	// �Ӳ˵����Ƴ���ť
	menu->removeChild((MenuItem*)pSender);
}