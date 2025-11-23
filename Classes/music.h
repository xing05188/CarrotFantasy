#pragma once
#ifndef _music_H_
#define _music_H_
#include "cocos2d.h"
#include "AudioEngine.h"
USING_NS_CC;
class Music {
private:
	int music_play=1;  //
	int soundEffectID; //
public:
	Music() {};
	~Music() {};
	void Music::preloadSoundEffect(const std::string& music_file) {
		AudioEngine::preload(music_file);
	}
	int is_play() { return music_play; };  //�������ֿ���
	void set_music(int option) { music_play = option; };  //�������ֿ���
	void background_music() {   //��������
		if (music_play)
			soundEffectID = AudioEngine::play2d("Music/bgm.mp3", true, 0.5f);
		else
			AudioEngine::pause(soundEffectID);
	}//

	void button_music(){        //��ť����
		if(music_play)
		AudioEngine::play2d("Music/button.mp3", false, 0.5f);
	}
	void page_music() {        //��ҳ����
		if (music_play)
			AudioEngine::play2d("Music/page.mp3", false, 0.2f);
	}
	void build_music() {        //����ֲ��ʱ����
		if (music_play)
			AudioEngine::play2d("Music/build.mp3", false, 1.0f);
	}
	void sell_music() {        //��������
		if (music_play)
			AudioEngine::play2d("Music/sell.mp3", false, 1.0f);
	}
	void born_music() {        //��������
		if (music_play)
			AudioEngine::play2d("Music/born.mp3", false, 2.0f);
	}
	void upgrade_music() {        //��������
		if (music_play)
			AudioEngine::play2d("Music/upgrade.mp3", false, 1.0f);
	}
	//�����ܲ�������Ч
	void  carrotSound() {
		if (music_play)
			AudioEngine::play2d("Music/carrot.mp3", false, 1.0f);
	}
	//������ͨ��������Ч
	void normalSound() {
		if (music_play)
			AudioEngine::play2d("Music/dead.mp3", false, 1.0f);
	}
	void countSound() {
		if (music_play)
			AudioEngine::play2d("Music/count.mp3", false, 1.0f);
	}
	void downSound() {
		if (music_play)
			AudioEngine::play2d("Music/down.mp3", false, 1.0f);
	}
	void bongSound() {
		if (music_play)
			AudioEngine::play2d("Music/bong.mp3", false, 1.0f);
	}
	void tuSound() {
		if (music_play)
			AudioEngine::play2d("Music/tu.mp3", false, 1.0f);
	}
	void guaiSound() {
		if (music_play)
			AudioEngine::play2d("Music/guaishou.mp3", false, 1.0f);
	}
	void duanSound() {
		if (music_play)
			AudioEngine::play2d("Music/duan.mp3", false, 1.0f);
	}
	//ֲ�������
	void  sunSound() {
		if (music_play)
			AudioEngine::play2d("Music/sun.mp3", false, 1.0f);
	}
	//����ƿ�ӹ�����Ч
	void  bottleSound() {
		if (music_play)
			AudioEngine::play2d("Music/bottle.mp3", false, 1.0f);
	}
	//���ŷɻ�������Ч
	void  planeSound() {
		if (music_play)
			AudioEngine::play2d("Music/Plane.mp3", false, 1.0f);
	}

	void  ShootSelect() {
		if (music_play)
			AudioEngine::play2d("Music/ShootSelect.mp3", false, 1.0f);
	}

	void  TowerBuild() {
		if (music_play)
			AudioEngine::play2d("Music/TowerBuild.mp3", false, 1.0f);
	}

	void  TowerSell() {
		if (music_play)
			AudioEngine::play2d("Music/TowerSell.mp3", false, 1.0f);
	}

	void  TowerUpdata() {
		if (music_play)
			AudioEngine::play2d("Music/TowerUpdata.mp3", false, 1.0f);
	}

	void Object() {//�ϰ��ﱻ�ݻ�ʱ
		if (music_play)
			AudioEngine::play2d("Music/Object.mp3", false, 1.0f);
	}

	void ShitSound() {
		if (music_play)
			AudioEngine::play2d("Music/Shit.mp3", false, 1.0f);
	}
};
#endif __music_H__