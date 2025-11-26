#pragma once

#include "../../core/EventBus.h"

namespace carrot {
namespace gameplay {
namespace events {

static const carrot::core::EventId kCarrotHpChangedEventId("carrot.hp.changed");
static const carrot::core::EventId kCarrotDiedEventId("carrot.died");
// 请求播放萝卜抖动特效（纯表现层事件）
static const carrot::core::EventId kCarrotShakeRequestedEventId("carrot.shake.requested");

struct CarrotHpChangedEvent : public carrot::core::Event {
    int current;
    int delta;
    int max;
};

struct CarrotDiedEvent : public carrot::core::Event {};

struct CarrotShakeRequestedEvent : public carrot::core::Event {
    float x = 0.0f;
    float y = 0.0f;
};

}  // namespace events
}  // namespace gameplay
}  // namespace carrot

