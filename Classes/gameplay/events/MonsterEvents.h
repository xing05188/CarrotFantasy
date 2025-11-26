#pragma once

#include "../../core/EventBus.h"

class Monster;

namespace carrot {
namespace gameplay {
namespace events {

// 怪物死亡（由 GameManager 或战斗逻辑发出，由场景/渲染层处理表现）
static const carrot::core::EventId kMonsterDiedEventId("monster.died");

struct MonsterDiedEvent : public carrot::core::Event {
    Monster* monster = nullptr;
};

// 怪物创建完成（但尚未挂载到场景），由 GameManager 发出，Scene 负责 addChild
static const carrot::core::EventId kMonsterSpawnedEventId("monster.spawned");

struct MonsterSpawnedEvent : public carrot::core::Event {
    Monster* monster = nullptr;
};

// 请求在某个位置播放怪物出生特效（仅表现层关心）
static const carrot::core::EventId kSpawnEffectRequestedEventId("monster.spawn_effect");

struct SpawnEffectRequestedEvent : public carrot::core::Event {
    float x = 0.0f;
    float y = 0.0f;
};

}  // namespace events
}  // namespace gameplay
}  // namespace carrot


