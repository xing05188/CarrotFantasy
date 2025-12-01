#pragma once

#include <array>
#include <string>

// 一波怪物的配置
struct WaveConfig {
    int wave;
    std::string monsterName;
    int count;
    std::array<float, 2> spawnInterval{ 1.0f, 2.0f };
};

constexpr int MAX_MONSTER_NUM = 150;

