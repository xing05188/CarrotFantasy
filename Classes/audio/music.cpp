/**
 * Design Pattern: Singleton Pattern
 * 设计模式：单例模式
 * 
 * Benefits:
 * 好处：
 * - Ensures only one instance of the audio manager exists throughout the game lifecycle
 *   确保整个游戏生命周期中只存在一个音频管理器实例
 * - Provides global access point to the audio management functionality
 *   提供音频管理功能的全局访问点
 * - Controls concurrent access to shared audio resources
 *   控制对共享音频资源的并发访问
 * - Manages audio state consistently across different game scenes
 *   在不同游戏场景中一致地管理音频状态
 * - Reduces memory overhead by preventing multiple audio manager instances
 *   通过防止多个音频管理器实例来减少内存开销
 * 
 * Problems Solved:
 * 解决的问题：
 * - Prevents multiple audio instances from causing conflicts and resource waste
 *   防止多个音频实例导致冲突和资源浪费
 * - Eliminates issues with audio state synchronization across different components
 *   消除不同组件间音频状态同步的问题
 * - Provides a centralized control point for all audio operations in the game
 *   为游戏中所有音频操作提供集中控制点
 * - Ensures proper resource cleanup when the game exits
 *   确保游戏退出时正确清理资源
 * - Maintains consistent audio settings (volume, enabled/disabled) across the entire application
 *   在整个应用程序中保持一致的音频设置（音量、启用/禁用）
 */
#include "music.h"

Music* Music::instance = nullptr;

const std::string AudioConfig::BGM = "Music/bgm.mp3";
const std::string AudioConfig::BGM1 = "Music/bgm1.mp3";
const std::string AudioConfig::BGM2 = "Music/bgm2.mp3";
const std::string AudioConfig::BUTTON = "Music/button.mp3";
const std::string AudioConfig::PAGE = "Music/page.mp3";
const std::string AudioConfig::BUILD = "Music/build.mp3";
const std::string AudioConfig::SELL = "Music/sell.mp3";
const std::string AudioConfig::BORN = "Music/born.mp3";
const std::string AudioConfig::UPGRADE = "Music/upgrade.mp3";
const std::string AudioConfig::CARROT = "Music/carrot.mp3";
const std::string AudioConfig::DEAD = "Music/dead.mp3";
const std::string AudioConfig::COUNT = "Music/count.mp3";
const std::string AudioConfig::DOWN = "Music/down.mp3";
const std::string AudioConfig::BONG = "Music/bong.mp3";
const std::string AudioConfig::TU = "Music/tu.mp3";
const std::string AudioConfig::GUAISHOU = "Music/guaishou.mp3";
const std::string AudioConfig::DUAN = "Music/duan.mp3";
const std::string AudioConfig::SUN = "Music/sun.mp3";
const std::string AudioConfig::BOTTLE = "Music/bottle.mp3";
const std::string AudioConfig::PLANE = "Music/Plane.mp3";
const std::string AudioConfig::SHOOT_SELECT = "Music/ShootSelect.mp3";
const std::string AudioConfig::TOWER_BUILD = "Music/TowerBuild.mp3";
const std::string AudioConfig::TOWER_SELL = "Music/TowerSell.mp3";
const std::string AudioConfig::TOWER_UPDATE = "Music/TowerUpdata.mp3";
const std::string AudioConfig::OBJECT = "Music/Object.mp3";
const std::string AudioConfig::SHIT = "Music/Shit.mp3";
const std::string AudioConfig::GAME_OVER = "Music/gameover.mp3";
const std::string AudioConfig::KAI = "Music/kai.mp3";
const std::string AudioConfig::FAN = "Music/fan.mp3";
const std::string AudioConfig::MAGIC = "Music/magic.mp3";

int BackgroundMusicStrategy::play(const std::string& filePath, bool loop, float volume) {
    return AudioEngine::play2d(filePath, loop, volume);
}

void BackgroundMusicStrategy::stop(int audioId) {
    AudioEngine::stop(audioId);
}

void BackgroundMusicStrategy::pause(int audioId) {
    AudioEngine::pause(audioId);
}

void BackgroundMusicStrategy::resume(int audioId) {
    AudioEngine::resume(audioId);
}

void BackgroundMusicStrategy::setVolume(int audioId, float volume) {
    AudioEngine::setVolume(audioId, volume);
}

int SoundEffectStrategy::play(const std::string& filePath, bool loop, float volume) {
    return AudioEngine::play2d(filePath, loop, volume);
}

void SoundEffectStrategy::stop(int audioId) {
    AudioEngine::stop(audioId);
}

void SoundEffectStrategy::pause(int audioId) {
    AudioEngine::pause(audioId);
}

void SoundEffectStrategy::resume(int audioId) {
    AudioEngine::resume(audioId);
}

void SoundEffectStrategy::setVolume(int audioId, float volume) {
    AudioEngine::setVolume(audioId, volume);
}

IAudioStrategy* AudioStrategyFactory::createAudioStrategy(AudioType type) {
    switch (type) {
        case AudioType::BACKGROUND:
            return new BackgroundMusicStrategy();
        case AudioType::UI_EFFECT:
        case AudioType::GAME_EFFECT:
            return new SoundEffectStrategy();
        default:
            return nullptr;
    }
}

Music::Music() : musicEnabled(true), currentBGMIndex(0) {
    backgroundStrategy = AudioStrategyFactory::createAudioStrategy(AudioType::BACKGROUND);
    soundEffectStrategy = AudioStrategyFactory::createAudioStrategy(AudioType::GAME_EFFECT);
}

Music::~Music() {
    delete backgroundStrategy;
    delete soundEffectStrategy;
}

Music* Music::getInstance() {
    if (instance == nullptr) {
        instance = new Music();
    }
    return instance;
}

void Music::preloadSoundEffect(const std::string& music_file) {
    AudioEngine::preload(music_file);
}

void Music::background_music() {
    if (musicEnabled) {
        // 停止所有背景音乐
        std::vector<std::string> bgmPaths = {AudioConfig::BGM, AudioConfig::BGM1, AudioConfig::BGM2};
        for (const auto &path : bgmPaths){
            auto iter = audioIds.find(path);
            if (iter != audioIds.end()){
                backgroundStrategy->stop(iter->second);
                audioIds.erase(iter);
            }
        }
        std::string bgmPath = bgmPaths[currentBGMIndex];
        // 播放新的背景音乐
        int audioId = backgroundStrategy->play(bgmPath, true, 1.0f);
        audioIds[bgmPath] = audioId;
    } else {
        // 暂停所有背景音乐
        std::vector<std::string> bgmPaths = {AudioConfig::BGM, AudioConfig::BGM1, AudioConfig::BGM2};
        for (const auto& path : bgmPaths) {
            auto iter = audioIds.find(path);
            if (iter != audioIds.end()) {
                backgroundStrategy->pause(iter->second);
            }
        }
    }
}

void Music::changeBackgroundMusic(int bgmIndex) {
    if (bgmIndex >= 0 && bgmIndex <= 2) {
        currentBGMIndex = bgmIndex;
        background_music();
    }
}

void Music::button_music() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::BUTTON, false, 0.5f);
        audioIds[AudioConfig::BUTTON] = audioId;
    }
}

void Music::page_music() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::PAGE, false, 0.2f);
        audioIds[AudioConfig::PAGE] = audioId;
    }
}

void Music::build_music() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::BUILD, false, 1.0f);
        audioIds[AudioConfig::BUILD] = audioId;
    }
}

void Music::sell_music() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::SELL, false, 1.0f);
        audioIds[AudioConfig::SELL] = audioId;
    }
}

void Music::born_music() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::BORN, false, 2.0f);
        audioIds[AudioConfig::BORN] = audioId;
    }
}

void Music::upgrade_music() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::UPGRADE, false, 1.0f);
        audioIds[AudioConfig::UPGRADE] = audioId;
    }
}

void Music::carrotSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::CARROT, false, 1.0f);
        audioIds[AudioConfig::CARROT] = audioId;
    }
}

void Music::normalSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::DEAD, false, 1.0f);
        audioIds[AudioConfig::DEAD] = audioId;
    }
}

void Music::countSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::COUNT, false, 1.0f);
        audioIds[AudioConfig::COUNT] = audioId;
    }
}

void Music::downSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::DOWN, false, 1.0f);
        audioIds[AudioConfig::DOWN] = audioId;
    }
}

void Music::bongSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::BONG, false, 1.0f);
        audioIds[AudioConfig::BONG] = audioId;
    }
}

void Music::tuSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::TU, false, 1.0f);
        audioIds[AudioConfig::TU] = audioId;
    }
}

void Music::guaiSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::GUAISHOU, false, 1.0f);
        audioIds[AudioConfig::GUAISHOU] = audioId;
    }
}

void Music::duanSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::DUAN, false, 1.0f);
        audioIds[AudioConfig::DUAN] = audioId;
    }
}

void Music::sunSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::SUN, false, 1.0f);
        audioIds[AudioConfig::SUN] = audioId;
    }
}

void Music::bottleSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::BOTTLE, false, 1.0f);
        audioIds[AudioConfig::BOTTLE] = audioId;
    }
}

void Music::planeSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::PLANE, false, 1.0f);
        audioIds[AudioConfig::PLANE] = audioId;
    }
}

void Music::ShootSelect() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::SHOOT_SELECT, false, 1.0f);
        audioIds[AudioConfig::SHOOT_SELECT] = audioId;
    }
}

void Music::TowerBuild() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::TOWER_BUILD, false, 1.0f);
        audioIds[AudioConfig::TOWER_BUILD] = audioId;
    }
}

void Music::TowerSell() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::TOWER_SELL, false, 1.0f);
        audioIds[AudioConfig::TOWER_SELL] = audioId;
    }
}

void Music::TowerUpdata() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::TOWER_UPDATE, false, 1.0f);
        audioIds[AudioConfig::TOWER_UPDATE] = audioId;
    }
}

void Music::Object() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::OBJECT, false, 1.0f);
        audioIds[AudioConfig::OBJECT] = audioId;
    }
}

void Music::ShitSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::SHIT, false, 1.0f);
        audioIds[AudioConfig::SHIT] = audioId;
    }
}

void Music::fanSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::FAN, false, 1.0f);
        audioIds[AudioConfig::FAN] = audioId;
    }
}

void Music::magicSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::MAGIC, false, 1.0f);
        audioIds[AudioConfig::MAGIC] = audioId;
    }
}

void Music::gameOverSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::GAME_OVER, false, 1.0f);
        audioIds[AudioConfig::GAME_OVER] = audioId;
    }
}

void Music::kaiSound() {
    if (musicEnabled) {
        int audioId = soundEffectStrategy->play(AudioConfig::KAI, false, 10.0f);
        audioIds[AudioConfig::KAI] = audioId;
    }
}

int Music::is_play() {
    return musicEnabled ? 1 : 0;
}

void Music::set_music(int option) {
    musicEnabled = (option == 1);
}