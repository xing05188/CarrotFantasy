#pragma once
#ifndef _music_H_
#define _music_H_
#include "cocos2d.h"
#include "AudioEngine.h"
#include <unordered_map>
#include <string>

USING_NS_CC;

// 音频类型枚举
enum class AudioType {
    BACKGROUND,
    UI_EFFECT,
    GAME_EFFECT
};

// 音频策略接口
class IAudioStrategy {
public:
    virtual ~IAudioStrategy() {}
    virtual int play(const std::string& filePath, bool loop = false, float volume = 1.0f) = 0;
    virtual void stop(int audioId) = 0;
    virtual void pause(int audioId) = 0;
    virtual void resume(int audioId) = 0;
    virtual void setVolume(int audioId, float volume) = 0;
};

// 背景音乐策略
class BackgroundMusicStrategy : public IAudioStrategy {
public:
    virtual int play(const std::string& filePath, bool loop = false, float volume = 1.0f) override;
    virtual void stop(int audioId) override;
    virtual void pause(int audioId) override;
    virtual void resume(int audioId) override;
    virtual void setVolume(int audioId, float volume) override;
};

// 音效策略
class SoundEffectStrategy : public IAudioStrategy {
public:
    virtual int play(const std::string& filePath, bool loop = false, float volume = 1.0f) override;
    virtual void stop(int audioId) override;
    virtual void pause(int audioId) override;
    virtual void resume(int audioId) override;
    virtual void setVolume(int audioId, float volume) override;
};

// 音频工厂
class AudioStrategyFactory {
public:
    static IAudioStrategy* createAudioStrategy(AudioType type);
};

// 音频管理器（单例）
class Music {
private:
    static Music* instance;
    bool musicEnabled;
    IAudioStrategy* backgroundStrategy;
    IAudioStrategy* soundEffectStrategy;
    std::unordered_map<std::string, int> audioIds;
    // 私有构造函数，防止外部创建实例
    Music();
    
public:
    // 获取单例实例
    static Music* getInstance();
    
    // 删除拷贝构造函数和赋值操作符
    Music(const Music&) = delete;
    Music& operator=(const Music&) = delete;
    
    // 析构函数
    ~Music();
    
    // 预加载音频文件
    void preloadSoundEffect(const std::string& music_file);
    
    // 播放背景音乐
    void background_music();
    
    // 播放音效
    void button_music();
    void page_music();
    void build_music();
    void sell_music();
    void born_music();
    void upgrade_music();
    void carrotSound();
    void normalSound();
    void countSound();
    void downSound();
    void bongSound();
    void tuSound();
    void guaiSound();
    void duanSound();
    void sunSound();
    void bottleSound();
    void planeSound();
    void ShootSelect();
    void TowerBuild();
    void TowerSell();
    void TowerUpdata();
    void Object();
    void ShitSound();

    // 检查背景音乐是否启用
    int is_play();
    
    // 设置音乐开关
    void set_music(int option);
};

// 音频配置类
class AudioConfig {
public:
    // 预定义的音频文件路径
    static const std::string BGM;
    static const std::string BUTTON;
    static const std::string PAGE;
    static const std::string BUILD;
    static const std::string SELL;
    static const std::string BORN;
    static const std::string UPGRADE;
    static const std::string CARROT;
    static const std::string DEAD;
    static const std::string COUNT;
    static const std::string DOWN;
    static const std::string BONG;
    static const std::string TU;
    static const std::string GUAISHOU;
    static const std::string DUAN;
    static const std::string SUN;
    static const std::string BOTTLE;
    static const std::string PLANE;
    static const std::string SHOOT_SELECT;
    static const std::string TOWER_BUILD;
    static const std::string TOWER_SELL;
    static const std::string TOWER_UPDATE;
    static const std::string OBJECT;
    static const std::string SHIT;
};

#endif __music_H__