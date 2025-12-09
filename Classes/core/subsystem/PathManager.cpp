// Refactored with Manager Pattern
// 使用管理者模式重构
#include "PathManager.h"

#include "../../scenes/BaseLevelScene.h"
#include "../../json/document.h"

USING_NS_CC;

void PathManager::ResetCurrentPath() {
    currentPath_.clear();
    currentScreenPath_.clear();
}

void PathManager::InitPath(int levelId, BaseLevelScene* scene) {
    if (pathsCache_.find(levelId) == pathsCache_.end()) {
        if (!LoadPathForLevel(levelId, "paths.json", scene)) {
            CCLOG("Failed to load path for level %d.", levelId);
            return;
        }
    }
    currentPath_ = pathsCache_[levelId];
    CCLOG("Path for level %d:", levelId);
    for (const auto& point : currentPath_) {
        CCLOG("Point: (%f, %f)", point.x, point.y);
    }

    currentScreenPath_.clear();
    if (!scene || !scene->tileMap) {
        CCLOG("PathManager::InitPath: scene or tileMap is null, cannot generate screen path.");
        return;
    }
    CCLOG("%f", scene->tileMap->getMapSize().height);
    for (const auto& gridPoint : currentPath_) {
        cocos2d::Vec2 screenCenter = GridToScreenCenter(gridPoint, scene);
        currentScreenPath_.push_back(screenCenter);
        CCLOG("ScreenPoint: (%f, %f)", screenCenter.x, screenCenter.y);
    }
}

bool PathManager::LoadPathForLevel(int levelId, const std::string& filePath, BaseLevelScene* scene) {
    if (pathsCache_.find(levelId) != pathsCache_.end()) {
        return true;
    }

    std::string fileContent = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
    if (fileContent.empty()) {
        CCLOG("Failed to load JSON file: %s", filePath.c_str());
        return false;
    }

    rapidjson::Document document;
    document.Parse(fileContent.c_str());

    if (document.HasParseError() || !document.IsObject()) {
        CCLOG("Failed to parse JSON or invalid format: %s", filePath.c_str());
        return false;
    }

    for (auto& level : document.GetObject()) {
        int levelIdInFile = std::stoi(level.name.GetString());
        if (levelIdInFile == levelId) {

            if (!level.value.IsArray()) {
                CCLOG("Path data for level %d is not an array.", levelId);
                return false;
            }

            const auto& points = level.value.GetArray();
            std::vector<cocos2d::Vec2> path;

            for (rapidjson::Value::ConstValueIterator it = points.Begin(); it != points.End(); ++it) {
                if (!it->IsArray() || it->Size() != 2) {
                    CCLOG("Invalid point format in level %d.", levelId);
                    continue;
                }

                float x = (*it)[0].GetFloat();
                float y = (*it)[1].GetFloat();
                path.emplace_back(x, y);
            }

            if (path.empty()) {
                CCLOG("No valid points found for level %d.", levelId);
                return false;
            }

            pathsCache_[levelId] = path;
            screenPathsCache_[levelId].clear();
            if (!scene || !scene->tileMap) {
                CCLOG("PathManager::LoadPathForLevel: scene or tileMap is null, skipping screen path cache.");
                return true;
            }
            for (const auto& point : pathsCache_[levelId]) {
                CCLOG("Grid Point: (%f, %f)", point.x, point.y);
                Vec2 screenCenter = GridToScreenCenter(point, scene);
                screenPathsCache_[levelId].emplace_back(screenCenter);
                CCLOG("Center Screen Point: (%f, %f)", screenCenter.x, screenCenter.y);
            }
            return true;
        }
    }

    CCLOG("Path for level %d not found in file: %s", levelId, filePath.c_str());
    return false;
}

cocos2d::Vec2 PathManager::GridToScreenCenter(const cocos2d::Vec2& gridPoint, BaseLevelScene* scene) const {
    if (!scene || !scene->tileMap) {
        CCLOG("PathManager::GridToScreenCenter: scene or tileMap is null.");
        return Vec2::ZERO;
    }
    float mapHeight = scene->tileMap->getMapSize().height;
    float screenX = gridPoint.x * (scene->tileSize.height) + (scene->tileSize.width) / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * (scene->tileSize.height) + (scene->tileSize.height) / 2;
    return Vec2(screenX, screenY);
}

