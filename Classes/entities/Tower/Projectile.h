#pragma once
#include <iostream>
#include <queue>
#include <unordered_set>
#include <functional>
#include <memory>
#include <string>
#include <stdexcept>
#include <mutex>

#include"cocos2d.h"
#include "entities/Monster/Monster.h"
#include "entities/Obstacle/Obstacle.h"
#include <vector>
#include <cmath>

template<typename T> class ObjectPool;


class BaseLevelScene;
class Monster;
class Bottle;
class Sun;
class MyPlane;
class Shit;
class Fan;



bool isColliding__(Sprite* spriteA, Sprite* spriteB);
// 对精灵造成伤害的模板函数
template<class T>
void DemageSprite(T* sp, int demage);

// 可池化对象接口
class Poolable {
public:
    virtual ~Poolable() = default;
    virtual void reset() = 0;
};

// 瓶子炮子弹类的实现
class BottleProjectile : public Poolable {
private:
    static std::string bottle_shell[3];
    template <class T>
    void bottleDemage(Bottle *bottle, BaseLevelScene *my_scene, T* sp);
    static const float speed;  // 炮弹飞行速度
public:
    std::string shell_path;
    Sprite* shell_sprite;
    ObjectPool<BottleProjectile>* pool;
    BottleProjectile() : shell_path(""), shell_sprite(nullptr), pool(nullptr) {}
    
    void setIndex(int index) {
        if (index >= 0 && index < 3) {
            shell_path = bottle_shell[index];
        } else {
            shell_path = "";
        }
    }

    void setPool(ObjectPool<BottleProjectile>* p) {
        pool = p;
    }

    
    void reset() override {
        shell_path = "";
        shell_sprite = nullptr;
        pool = nullptr;
    }

    
    template <class T>
    void attack(std::unique_ptr<BottleProjectile> self,Bottle *bottle, BaseLevelScene *my_scene,float jiasu,T*sp);
};


class SunProjectile : public Poolable {
    static std::string sun_halo;  // 光环图片资源表
public:
    Sprite* halo_sprite;
    std::string halo_path;
    ObjectPool<SunProjectile>* pool;
    SunProjectile() : halo_sprite(nullptr),halo_path(sun_halo), pool(nullptr) {}
    void setSprite(){
        halo_sprite= Sprite::create(halo_path);
    }

    void setPool(ObjectPool<SunProjectile>* p) {
        pool = p;
    }

    void reset() override {
        halo_sprite = nullptr;
        pool = nullptr;
    }

    void attack(std::unique_ptr<SunProjectile> self,Sun *sun, BaseLevelScene *my_scene,std::vector<Monster *> &monsters, float jiasu);
};

//飞机射线类的实现
class PlaneProjectile : public Poolable {
private:
    static std::string ray[3];  // 光线图片资源表
    void planeDemage(MyPlane *plane, BaseLevelScene *my_scene, std::vector<Monster *> &monsters,Sprite* ray_sprite);
public:
    std::string ray_path;
    cocos2d::Sprite* ray_sprite;
    ObjectPool<PlaneProjectile>* pool;
    PlaneProjectile() : ray_path(""), ray_sprite(nullptr), pool(nullptr) {}
    
    void setIndex(int index) {
        if (index >= 0 && index < 3) {
            ray_path = ray[index];
        } else {
            ray_path = "";
        }
    }

    void setPool(ObjectPool<PlaneProjectile>* p) {
        pool = p;
    }

    
    void reset() override {
        ray_path = "";
        ray_sprite = nullptr;
        pool = nullptr;
    }


    void attack(std::unique_ptr<PlaneProjectile> self,MyPlane *plane, BaseLevelScene *my_scene,std::vector<Monster *> &monsters, float jiasu);
};


//大便抛射物处理
class ShitProjectile : public Poolable {
    static std::string shit_shell[3];  // 炮弹图片资源表
    template <class T>
    void shitDemage(Shit *shit, BaseLevelScene *my_scene,T*sp,Sprite* shit_sprite);
    static const float speed;  // 粪便飞行速度
public:
    std::string shit_path;
    Sprite* shit_sprite;
    ObjectPool<ShitProjectile>* pool;
    ShitProjectile() :shit_path(""), shit_sprite(nullptr), pool(nullptr) {}

    void setIndex(int index) {
        if (index >= 0 && index < 3) {
            shit_path = shit_shell[index];
        } else {
            shit_path = "";
        }
    }


    void setPool(ObjectPool<ShitProjectile>* p) {
        pool = p;
    }

    void reset() override {
        shit_path = "";
        shit_sprite = nullptr;
        pool = nullptr;
    }
    template <class T>
    void attack(std::unique_ptr<ShitProjectile> self,Shit *shit, BaseLevelScene *my_scene,float jiasu,T*sp);
};


//风扇抛射物处理
class FanProjectile:public Poolable{
private:
    static std::string clover[3];  // 四叶草图片资源表
    static const float speed;//四叶草飞行速度

    template<class T>
    void cloverDemage(Fan *fan, BaseLevelScene *my_scene, T *sp);
public:
    std::string clover_path;
    Sprite* clover_sprite;
    ObjectPool<FanProjectile>* pool;
    FanProjectile() : clover_path(""), clover_sprite(nullptr), pool(nullptr) {}

    void setIndex(int index) {
        if (index >= 0 && index < 3) {
            clover_path = clover[index];
        } else {
            clover_path = "";
        }
    }

    void setPool(ObjectPool<FanProjectile>* p) {
        pool = p;
    }

    void reset() override {
        clover_path = "";
        clover_sprite = nullptr;
        pool = nullptr;
    }

    void attack(std::unique_ptr<FanProjectile> self,Fan *fan, BaseLevelScene *my_scene,float jiasu,std::vector<Monster *> &monsters,Vec2&direction);
};


// 通用对象池类 - 模板实现必须在头文件中
template<typename T>
class ObjectPool {
private:
    std::queue<std::unique_ptr<T>> freeList;
    std::unordered_set<T*> usedList;
    size_t maxSize;
    std::function<std::unique_ptr<T>()> factory;
    mutable std::mutex poolMutex;

public:
    ObjectPool(size_t initSize, size_t maxSize, std::function<std::unique_ptr<T>()> factory)
        : maxSize(maxSize), factory(std::move(factory)) {
        for (size_t i = 0; i < initSize; ++i) {
            freeList.push(this->factory());
        }
    }

    std::unique_ptr<T> borrowObject() {
        std::lock_guard<std::mutex> lock(poolMutex);
        std::unique_ptr<T> obj;

        if (!freeList.empty()) {
            obj = std::move(freeList.front());
            freeList.pop();
        } else if (usedList.size() + freeList.size() < maxSize) {
            obj = factory();
        } else {
            throw std::runtime_error("no available object");
        }

        T* rawPtr = obj.get();
        usedList.insert(rawPtr);


        return obj;
    }

    void returnObject(std::unique_ptr<T> obj) {

        if (!obj) return;

        std::lock_guard<std::mutex> lock(poolMutex);
        T* rawPtr = obj.get();
        /*

        */
    
        if (usedList.find(rawPtr) != usedList.end()) {
            usedList.erase(rawPtr);
            obj->reset();
            freeList.push(std::move(obj));
        } else {
            throw std::invalid_argument("object does not belong to this pool");
        }
    }

    size_t getFreeCount() const {
        std::lock_guard<std::mutex> lock(poolMutex);
        return freeList.size();
    }

    size_t getUsedCount() const {
        std::lock_guard<std::mutex> lock(poolMutex);
        return usedList.size();
    }

    size_t getTotalCount() const {
        std::lock_guard<std::mutex> lock(poolMutex);
        return freeList.size() + usedList.size();
    }
};