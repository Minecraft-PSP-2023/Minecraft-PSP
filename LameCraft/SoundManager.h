#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

extern "C"
{
    #include "sound_utils/pgeWav.h"
    #include "sound_utils/oslib.h"
}

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <Aurora/Utils/Logger.h>

#include<fstream>
#include<iostream>


class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	static SoundManager m_SoundManager;
	static SoundManager* Instance();

	void Init();
	void AudioSync();

    void PlayBreakSound();
    void PlayHitSound();
    void PlayEatSound();

    void PlayZombieHurtSound(float distanceToPlayer);
    void PlayZombieDieSound(float distanceToPlayer);
    void PlayZombieSaySound(float distanceToPlayer);
	
	void PlaySpiderHurtSound(float distanceToPlayer);
    void PlaySpiderDieSound(float distanceToPlayer);

    void PlayCowHurtSound(float distanceToPlayer);
    void PlayCowSaySound(float distanceToPlayer);
	
	void PlayChickenHurtSound(float distanceToPlayer);
    void PlayChickenSaySound(float distanceToPlayer);

    void PlayPigHurtSound(float distanceToPlayer);
	void PlayPigDieSound(float distanceToPlayer);

    void PlayCreeperHurtSound(float distanceToPlayer);
    void PlaySheepHurtSound(float distanceToPlayer);

    void PlayFissSound();
	void PlayChestSound();
	void PlayChestClosedSound();
	void PlayIgniteSound();
    void PlaySplashSound();
    void PlayFuseSound();
    void PlayShearSound();
    void PlayBowSound();
	void PlayMenuSound();
	void PlayPlopSound();
	void PlayWalkSound(int type);
	void PlayDigSound(int type);
	void PlayEndDigSound(int type);
	void PlayTNTSound();
	//void PlayThunderSound();
	void PlayRainSound(int volume);
	void PlayDoorSound(bool open);
	void PlayFallSound(float i);
	void PlayGlassSound();
    void PlayNoteSound(unsigned int noteType, float pitch);
	
	void PlayOrbSound();
	void PlayExpSound();

    void PlayDiskTrack(int diskNumber);
    void StopDiskTrack();

	int PlayRandomAmbient();

	void StopAmbient();
	void PlayAmbient(std::string name);
	//menu
	void StopMenu();
	void PlayMenu(std::string name);
	int PlayRandomMenu();

	//creative
	void StopCreative();
	void PlayCreative(std::string name);
	int PlayRandomCreative();

	bool playerSounds;
	bool ambientSoundsEnabled;

	int currentAmbientSound;
	int lastAmbientSound;

	bool delayAmbient;

private:

	int currentWalkSound;
	int lastWalkSound;

	//sounds
	pgeWav *buttonSound;
	pgeWav *openchestSound;
	pgeWav *closedchestSound;
	pgeWav *igniteSound;
	
	pgeWav *orbSound;
	pgeWav *expSound;
	//plop sound
	pgeWav *plopSound;
	//grass sounds
	pgeWav *grassSounds[4];
	pgeWav *grawelSounds[4];
	pgeWav *stoneSounds[4];
	pgeWav *woodSounds[4];
	pgeWav *sandSounds[4];
	pgeWav *snowSounds[4];

	pgeWav *eatSounds[3];
	
	//pgeWav *thunderSounds[3];

	pgeWav *hitSounds[3];
    pgeWav *fallSounds[2];

    pgeWav *zombieHurtSounds[2];
    pgeWav *zombieSaySound;
    pgeWav *zombieDieSound;
	
	pgeWav *spiderHurtSounds[2];
    pgeWav *spiderDieSound;

    pgeWav *cowHurtSounds[2];
    pgeWav *cowSaySound;
	
	pgeWav *chickenHurtSounds[2];
    pgeWav *chickenSaySound;

    pgeWav *pigHurtSounds[2];
	pgeWav *pigDieSound;

    pgeWav *creeperHurtSounds[2];

    pgeWav *sheepHurtSounds[2];
	
	pgeWav *tntSounds;

	pgeWav *doorOpen;
	pgeWav *doorClose;
	pgeWav *bow;
	pgeWav *breakSound;
	pgeWav *fuse;
	pgeWav *fiss;
	pgeWav *splash;
	pgeWav *glass;
	pgeWav *shear;

	pgeWav *rainSound;

	pgeWav *digSounds[3];

	pgeWav *noteSounds[2]; // bass and pling

	// this lib can stream
	OSL_SOUND *ambientTrack;
	std::string ambientTrackName;
	
	OSL_SOUND *menuTrack;
	std::string menuTrackName;

	OSL_SOUND *creativeTrack;
	std::string creativeTrackName;

	OSL_SOUND *diskTrack;
	std::string diskTrackName;

};

#endif

