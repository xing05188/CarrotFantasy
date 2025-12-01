#pragma once

#include <string>
#include "cocos2d.h"
#include "json/document.h"
#include "json/rapidjson.h"
struct MonsterConfig {
    std::string spriteFrameName;  // ����ͼƬ��Դ
    int health;                   // ����ֵ
    float speed;                  // �ƶ��ٶ�
    int damage;                   // ������
    int reward;                   // ��ɱ����
};

class MonsterConfigManager {
public:
    static MonsterConfig getMonsterConfigByName(const std::string& monsterName) ;
};
