#ifndef STATEOPTIONS_H_
#define STATEOPTIONS_H_

#include <stdlib.h>

#include <Aurora/Utils/StateManager.h>
#include <Aurora/Utils/GameState.h>

#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Utils/Logger.h>
#include <Aurora/Utils/Timer.h>
#include <Aurora/System/SystemManager.h>
#include <Aurora/Graphics/Models/ObjModel.h>
#include <Aurora/Graphics/Camera.h>
#include <Aurora/Graphics/Sprite.h>

#include "SoundManager.h"

using namespace Aurora::Graphics;
using namespace Aurora::Utils;
using namespace Aurora::System;
using namespace Aurora;

class StateOptions : public CGameState
{
public:
	StateOptions();
	virtual ~StateOptions();

	void Init();
	void Enter();
	void CleanUp();

	void Pause();
	void Resume();

	void HandleEvents(StateManager* sManager);
	void Update(StateManager* sManager);
	void Draw(StateManager* sManager);
	void DrawText(int x,int y, unsigned int color, float size, const char *message, ...);

private:
	Sprite *buttonSprite;
	Sprite *sbuttonSprite;
	Sprite *nbuttonSprite;
	Sprite *halfbuttonSprite;
	Sprite *halfsbuttonSprite;
	Sprite *moverSprite;
	Sprite *smoverSprite;
	Sprite *backSprite;

	RenderManager *mRender;
	SystemManager *mSystemMgr;
	SoundManager *mSoundMgr;

	int selectPos;
	int menuState;//0 main,1 load,2 options

	//controls
	int controlPos;
	int controlStart;
	int controlEnd;

	//choosing key
	bool chooseKeyState;
	bool configChanged;
	int currentKey;
	int newKey;

	//analog stick
	int currentAnalogPos;

};

#endif
