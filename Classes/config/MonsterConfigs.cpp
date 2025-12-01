#include "MonsterConfigs.h"

 MonsterConfig MonsterConfigManager::getMonsterConfigByName(const std::string& monsterName)
 {
     // ��ȡ�ļ�·��
     std::string filePath = "monsters_config.json";

     // ��ȡ JSON �ļ�
     std::string fileContent = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
     rapidjson::Document doc;
     doc.Parse(fileContent.c_str());
     MonsterConfig config;

     // ����ļ����Ƿ�����ù��������
     if (doc.HasMember(monsterName.c_str()) && doc[monsterName.c_str()].IsObject()) {
         rapidjson::Value& monsterData = doc[monsterName.c_str()];

         // ��ȡ��������
         if (monsterData.HasMember("spriteFrameName")) {
             config.spriteFrameName = monsterData["spriteFrameName"].GetString();
         }
         if (monsterData.HasMember("health")) {
             config.health = monsterData["health"].GetInt();
         }
         if (monsterData.HasMember("speed")) {
             config.speed = monsterData["speed"].GetFloat();
         }
         if (monsterData.HasMember("damage")) {
             config.damage = monsterData["damage"].GetInt();
         }
         if (monsterData.HasMember("reward")) {
             config.reward = monsterData["reward"].GetInt();
         }
     }
     return config;
 }