#include "MoneyHud.h"

using namespace cocos2d;

namespace carrot {
namespace ui {
namespace widgets {

MoneyHud* MoneyHud::create(int initialMoney) {
    MoneyHud* hud = new (std::nothrow) MoneyHud();
    if (hud && hud->initWithMoney(initialMoney)) {
        hud->autorelease();
        return hud;
    }
    CC_SAFE_DELETE(hud);
    return nullptr;
}

bool MoneyHud::initWithMoney(int initialMoney) {
    if (!Node::init()) {
        return false;
    }
    label_ = Label::createWithTTF(std::to_string(initialMoney), "fonts/arial.ttf", 27);
    if (!label_) {
        return false;
    }
    addChild(label_);
    label_->setPosition(Vec2::ZERO);

    auto bus = carrot::core::EventBusProvider::Get();
    subscription_ = bus->Subscribe(carrot::gameplay::events::kMoneyChangedEventId,
                                   [this](const carrot::core::Event& baseEvent) {
                                       const carrot::gameplay::events::MoneyChangedEvent& evt =
                                           static_cast<const carrot::gameplay::events::MoneyChangedEvent&>(baseEvent);
                                       if (label_) {
                                           label_->setString(std::to_string(evt.current));
                                       }
                                   });
    return true;
}

void MoneyHud::setPosition(const cocos2d::Vec2& position) {
    Node::setPosition(position);
}

}  // namespace widgets
}  // namespace ui
}  // namespace carrot

