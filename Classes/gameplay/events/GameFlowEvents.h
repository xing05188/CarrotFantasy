#pragma once

#include "../../core/EventBus.h"

namespace carrot {
namespace gameplay {
namespace events {

static const carrot::core::EventId kGameWonEventId("game.won");
static const carrot::core::EventId kGameLostEventId("game.lost");

struct GameWonEvent : public carrot::core::Event {
    int currentWave;
    int totalWave;
    int levelId;
};

struct GameLostEvent : public carrot::core::Event {
    int currentWave;
    int totalWave;
    int levelId;
};

}  // namespace events
}  // namespace gameplay
}  // namespace carrot


