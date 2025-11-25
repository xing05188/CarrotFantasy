#pragma once

#include "../../core/EventBus.h"

namespace carrot {
namespace gameplay {
namespace events {

static const carrot::core::EventId kMoneyChangedEventId("money.changed");

struct MoneyChangedEvent : public carrot::core::Event {
    int delta;
    int current;
};

}  // namespace events
}  // namespace gameplay
}  // namespace carrot

