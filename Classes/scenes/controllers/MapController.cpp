#include "MapController.h"
#include "../BaseLevelScene.h" // [关键] 必须包含，才能访问 _owner 的成员
#include "../../config/obpos.h" // 包含 obpos 数组
#include "../../entities/Obstacle/ObstacleFactory.h" // 包含障碍物工厂

// 初始化静态成员变量
const std::vector<std::string> MapController::_mapFiles = {
    "map/map1.tmx",
    "map/map2.tmx",
    "map/map3.tmx",
};

MapController::MapController() 
    : _owner(nullptr)
{
}

MapController::~MapController() {
    // _owner 是外部对象，不由 Controller 管理生命周期，不需要 delete
}

bool MapController::init(BaseLevelScene* scene, int levelId) {
    if (!scene || levelId <= 0 || levelId > _mapFiles.size()) {
        CCLOG("MapController::init failed: Invalid scene or levelId %d", levelId);
        return false;
    }

    _owner = scene;

    // 1. 加载 TMX 地图并赋值给 _owner->tileMap
    _owner->tileMap = TMXTiledMap::create(_mapFiles[levelId - 1]);
    if (!_owner->tileMap) {
        CCLOG("MapController::init failed: Could not load map %s", _mapFiles[levelId - 1].c_str());
        return false;
    }

    // 2. 屏幕适配与属性设置
    auto screenSize = Director::getInstance()->getVisibleSize();
    auto mapSize = _owner->tileMap->getContentSize();
    
    // 设置 _owner->tileSize
    _owner->tileSize = _owner->tileMap->getTileSize();
    CCLOG("Tile size: (%f, %f)", _owner->tileSize.width, _owner->tileSize.height);

    _owner->tileMap->setScaleX(screenSize.width / mapSize.width);
    _owner->tileMap->setScaleY(screenSize.height / mapSize.height);
    _owner->tileMap->setAnchorPoint(Vec2::ZERO);
    _owner->tileMap->setPosition(Vec2::ZERO);

    // 3. 隐藏 plantable 逻辑层
    auto plantableLayer = _owner->tileMap->getLayer("plantable");
    if (plantableLayer) {
        plantableLayer->setVisible(false);
    } else {
        CCLOG("Warning: Layer 'plantable' not found in map.");
    }

    // 4. 将地图添加到场景底层
    _owner->addChild(_owner->tileMap, 0);

    return true;
}

void MapController::initGridData(int levelId) {
    if (!_owner) return;

    // 直接操作 _owner->map_data 进行初始化
    for (int i = 0; i < MAP_X; i++) {
        for (int j = 0; j < MAP_Y; j++) {
            // obpos 是定义在 config/obpos.h 中的全局数组
            _owner->map_data[i][j].flag = obpos[levelId - 1][i][j];
            _owner->map_data[i][j].key = MAP_Y * i + j;
        }
    }
}

void MapController::generateObstacles() {
    if (!_owner) return;

    srand((unsigned)time(0));

    // 遍历网格，根据 flag 生成障碍物
    for (int i = 0; i < MAP_X; i++) {
        for (int j = 0; j < MAP_Y; j++) {
            // flag == 3 表示这是障碍物的生成点
            if (_owner->map_data[i][j].flag == 3) {
                int rd;
                // 判断周围是否有足够的空间生成大障碍物
                if (i + 1 < MAP_X && j + 1 < MAP_Y && 
                    _owner->map_data[i + 1][j].flag == 3 && 
                    _owner->map_data[i][j + 1].flag == 3 && 
                    _owner->map_data[i + 1][j + 1].flag == 3)
                    rd = 10; // 可以生成最大型的障碍物
                else if (i + 1 < MAP_X && _owner->map_data[i + 1][j].flag == 3) 
                    rd = 6;  // 可以生成中型的障碍物
                else 
                    rd = 5;  // 只能生成小型的

                // 随机选择障碍物类型
                int index = rand() % (rd + 5);
                
                // 如果随机数过大，不生成障碍物（变成空地）
                if (index >= rd) {
                    _owner->map_data[i][j].flag = 0;
                    continue;
                }

                // 标记为障碍物 (flag = 2)
                _owner->map_data[i][j].flag = 2;
                
                // 使用工厂创建障碍物
                // 注意：ObstacleFactory 还是需要 _owner (Scene) 指针作为参数
                auto obb = ObstacleFactoryProvider::createObstacle(index, _owner, i, j);
                
                // 存入 map_data 和 Obstacles 容器
                _owner->Obstacles[_owner->map_data[i][j].key] = obb;

                // 处理占多格的情况
                if (index == 5) { 
                    // 占横向 2 格
                    _owner->map_data[i + 1][j].flag = 2;
                    _owner->Obstacles[_owner->map_data[i + 1][j].key] = obb;
                }
                else if (index > 5) {
                    // 占 2x2 格
                    _owner->map_data[i][j + 1].flag = 2;
                    _owner->map_data[i + 1][j].flag = 2;
                    _owner->map_data[i + 1][j + 1].flag = 2;
                    
                    _owner->Obstacles[_owner->map_data[i][j + 1].key] = obb;
                    _owner->Obstacles[_owner->map_data[i + 1][j].key] = obb;
                    _owner->Obstacles[_owner->map_data[i + 1][j + 1].key] = obb;
                }
            }
        }
    }
}

Vec2 MapController::getScreenPosition(const Vec2& gridPos) {
    if (!_owner || !_owner->tileMap) return Vec2::ZERO;

    float mapHeight = _owner->tileMap->getMapSize().height;
    float w = _owner->tileSize.width;
    float h = _owner->tileSize.height;

    // 这里的 Y 轴计算逻辑保留了原项目的习惯 (Y轴可能反转)
    float screenX = gridPos.x * w + w / 2;
    float screenY = (mapHeight - gridPos.y - 1) * h + h / 2;
    
    return Vec2(screenX, screenY);
}

Vec2 MapController::getGridPosition(const Vec2& screenPos) {
    if (!_owner) return Vec2::ZERO;
    
    // 简单的向下取整
    return Vec2(floor(screenPos.x / _owner->tileSize.width), 
                floor(screenPos.y / _owner->tileSize.height));
}

bool MapController::checkPlantable(const Vec2& gridPos) {
    if (!_owner || !_owner->tileMap) return false;

    Size mapSize = _owner->tileMap->getMapSize();
    
    // 1. 边界检查
    if (gridPos.x < 0 || gridPos.y < 0 || 
        gridPos.x >= mapSize.width || gridPos.y >= mapSize.height) {
        return false;
    }

    // 2. 图层检查
    auto layer = _owner->tileMap->getLayer("plantable");
    if (!layer) return false;

    // GID != 0 表示该位置有瓦片，即设定为可种植区域
    int tileGID = layer->getTileGIDAt(gridPos);
    return tileGID != 0;
}

Cell& MapController::getCell(int x, int y) {
    // 如果 _owner 为空，返回一个静态的哑元对象防止崩溃（虽然不应该发生）
    static Cell dummyCell = {0, 0};
    if (!_owner) return dummyCell;

    // 简单的边界保护
    if (x < 0) x = 0; if (x >= MAP_X) x = MAP_X - 1;
    if (y < 0) y = 0; if (y >= MAP_Y) y = MAP_Y - 1;

    return _owner->map_data[x][y];
}

void MapController::drawDebugGrid() {
    if (!_owner || !_owner->tileMap) return;

    Size mapSize = _owner->tileMap->getContentSize();
    float width = _owner->tileSize.width;
    float height = _owner->tileSize.height;

    auto drawNode = DrawNode::create();
    _owner->addChild(drawNode, 100);

    for (float x = 0; x <= mapSize.width; x += width) {
        drawNode->drawLine(Vec2(x, 0), Vec2(x, mapSize.height), Color4F::WHITE);
    }

    for (float y = 0; y <= mapSize.height; y += height) {
        drawNode->drawLine(Vec2(0, y), Vec2(mapSize.width, y), Color4F::WHITE);
    }
}

TMXTiledMap* MapController::getTiledMap() const {
    return _owner ? _owner->tileMap : nullptr;
}

Size MapController::getTileSize() const {
    return _owner ? _owner->tileSize : Size::ZERO;
}