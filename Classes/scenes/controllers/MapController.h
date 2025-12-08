#pragma once
#ifndef __MAP_CONTROLLER_H__
#define __MAP_CONTROLLER_H__

#include "cocos2d.h"
#include <vector>
#include <string>
#include <map>

USING_NS_CC;

// 前向声明
class Obstacle;
class BaseLevelScene;

// 定义网格数据结构
struct Cell {
    char flag; // 0:空, 1:塔, 2:障碍物, 3:障碍物占位
    int key;   // 在 towers 或 obstacles 容器中的索引
};

#define MAP_X 15
#define MAP_Y 9

/**
 * MapController (组件模式)
 * 职责：封装地图加载、渲染、网格数据管理及坐标转换逻辑
 */
class MapController {
public:
    MapController();
    ~MapController();

    // 初始化组件
    // scene: 地图将添加到的父节点
    // levelId: 关卡ID
    bool init(Node* scene, int levelId);

    // 初始化网格数据 (对应原 InitMapData)
    void initGridData(int levelId);

    // 生成障碍物 (对应原 ProduceObstacles)
    // 需要传入场景的 Obstacles 容器引用来进行填充
    void generateObstacles(BaseLevelScene* scene, std::map<int, Obstacle*>& obstaclesMap);

    // 坐标转换：网格 -> 屏幕中心
    Vec2 getScreenPosition(const Vec2& gridPos);

    // 坐标转换：屏幕 -> 网格
    Vec2 getGridPosition(const Vec2& screenPos);

    // 检查是否可以种植 (边界检查 + TMX图层检查)
    bool checkPlantable(const Vec2& gridPos);

    // 调试绘制
    void drawDebugGrid(Node* scene);

    // 数据访问接口
    Cell& getCell(int x, int y); // 获取网格数据的引用，允许外部修改
    Size getTileSize() const { return _tileSize; }
    TMXTiledMap* getTiledMap() const { return _tileMap; }

private:

    BaseLevelScene* _owner;
    static const std::vector<std::string> _mapFiles;
};

#endif // __MAP_CONTROLLER_H__