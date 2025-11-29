#include "CountDownController.h"
#include "audio/music.h"

CountDownController::CountDownController()
    : initialized_(false)
{
}

CountDownController::~CountDownController()
{
}

void CountDownController::init(const CountDownCallbacks& callbacks)
{
    callbacks_ = callbacks;
    initialized_ = true;
}

void CountDownController::startCountDown(std::function<void()> onComplete)
{
    if (!initialized_) {
        CCLOG("CountDownController: Not initialized!");
        if (onComplete) {
            onComplete();  // 如果未初始化，直接执行完成回调
        }
        return;
    }

    // 创建倒计时元素（保持与原始实现完全一致）
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

    callbacks_.addChildToScene(countBackground);
    callbacks_.addChildToScene(count1);
    callbacks_.addChildToScene(count2);
    callbacks_.addChildToScene(count3);
    callbacks_.addChildToScene(count0);
    
    // 创建倒计时序列（保持与原始实现完全一致）
    auto countdown = Sequence::create(
        CallFunc::create([=] {
            countBackground->setVisible(true);
            count3->setVisible(true);
            Music::getInstance()->countSound();
        }),
        DelayTime::create(1),

        CallFunc::create([=] {
            callbacks_.removeChild(count3);
        }),
        CallFunc::create([=] {
            count2->setVisible(true);
            Music::getInstance()->countSound();
        }),
        DelayTime::create(1),

        CallFunc::create([=] {
            callbacks_.removeChild(count2);
        }),
        CallFunc::create([=] {
            count1->setVisible(true);
            Music::getInstance()->countSound();
        }),
        DelayTime::create(1),

        CallFunc::create([=] {
            callbacks_.removeChild(count1);
            count0->setVisible(true);
            Music::getInstance()->countSound();
        }),
        DelayTime::create(1),

        CallFunc::create([=] {
            callbacks_.removeChild(count0);
            callbacks_.removeChild(countBackground);

            if (onComplete) {
                onComplete();
            }
        }),
        nullptr
    );

    callbacks_.runActionOnScene(countdown);
}

