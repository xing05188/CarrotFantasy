// Refactored with Manager Pattern
// 使用管理者模式重构
#pragma once

#include <map>
#include <string>
#include <vector>

#include "cocos2d.h"

class BaseLevelScene;

class PathManager {
public:
    void ResetCurrentPath();
    void InitPath(int levelId, BaseLevelScene* scene);
    bool LoadPathForLevel(int levelId, const std::string& filePath, BaseLevelScene* scene);
    cocos2d::Vec2 GridToScreenCenter(const cocos2d::Vec2& gridPoint, BaseLevelScene* scene) const;

    const std::vector<cocos2d::Vec2>& GetPath() const { return currentPath_; }
    const std::vector<cocos2d::Vec2>& GetScreenPath() const { return currentScreenPath_; }

private:
    std::map<int, std::vector<cocos2d::Vec2>> pathsCache_;
    std::map<int, std::vector<cocos2d::Vec2>> screenPathsCache_;
    std::vector<cocos2d::Vec2> currentPath_;
    std::vector<cocos2d::Vec2> currentScreenPath_;
};

