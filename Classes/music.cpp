#include "music.h"

// 初始化静态成员变量
Music* Music::instance = nullptr;

// AudioConfig 静态成员定义
const std::string AudioConfig::BGM = "Music/bgm.mp3";
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

// BackgroundMusicStrategy 实现
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

// SoundEffectStrategy 实现
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

// AudioStrategyFactory 实现
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

// Music 私有构造函数
Music::Music() : musicEnabled(true) {
    backgroundStrategy = AudioStrategyFactory::createAudioStrategy(AudioType::BACKGROUND);
    soundEffectStrategy = AudioStrategyFactory::createAudioStrategy(AudioType::GAME_EFFECT);
}

// Music 析构函数
Music::~Music() {
    delete backgroundStrategy;
    delete soundEffectStrategy;
}

// 获取单例实例
Music* Music::getInstance() {
    if (instance == nullptr) {
        instance = new Music();
    }
    return instance;
}

// 预加载音频文件
void Music::preloadSoundEffect(const std::string& music_file) {
    AudioEngine::preload(music_file);
}

// 播放背景音乐
void Music::background_music() {
    if (musicEnabled) {
        int audioId = backgroundStrategy->play(AudioConfig::BGM, true, 0.5f);
        audioIds[AudioConfig::BGM] = audioId;
    } else {
        backgroundStrategy->pause(audioIds[AudioConfig::BGM]);
    }
}

// 播放音效
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

// 检查背景音乐是否启用
int Music::is_play() {
    return musicEnabled ? 1 : 0;
}

// 设置音乐开关
void Music::set_music(int option) {
    musicEnabled = (option == 1);
}