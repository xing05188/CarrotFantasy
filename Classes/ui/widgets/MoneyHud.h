#pragma once

#include "cocos2d.h"
#include <memory>
#include "../../core/EventBusProvider.h"
#include "../../gameplay/events/MoneyEvents.h"

namespace carrot {
namespace ui {
namespace widgets {

class MoneyHud : public cocos2d::Node {
public:
    static MoneyHud* create(int initialMoney);

    void setPosition(const cocos2d::Vec2& position);
    cocos2d::Label* getLabel() const { return label_; }

protected:
    bool initWithMoney(int initialMoney);

private:
    cocos2d::Label* label_;
    std::shared_ptr<carrot::core::Subscription> subscription_;
};

}  // namespace widgets
}  // namespace ui
}  // namespace carrot

