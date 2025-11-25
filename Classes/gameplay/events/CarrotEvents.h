#pragma once

#include "../../core/EventBus.h"

namespace carrot {
namespace gameplay {
namespace events {

static const carrot::core::EventId kCarrotHpChangedEventId("carrot.hp.changed");
static const carrot::core::EventId kCarrotDiedEventId("carrot.died");

struct CarrotHpChangedEvent : public carrot::core::Event {
    int current;
    int delta;
    int max;
};

struct CarrotDiedEvent : public carrot::core::Event {};

}  // namespace events
}  // namespace gameplay
}  // namespace carrot

