#include "StateMenu.h"
#include "StateOptions.h"
#include "StatePlay.h"
#include "StatePlayCreative.h"
#include "LoadingScreen.h"
#include "InputHelper.h"
#include "TextureHelper.h"
#include <Aurora/System/NetworkManager.h>
#include <zlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

// font vars
#define default_size 0.5
#define default_big_size 0.687
#define PI 3.1415926535897f

#define ENGLISH 1
#define RUSSIAN 2

#define SURVIVAL 0
#define CREATIVE 1
#define HARDCORE 2

/*
q = �
w = �
e = �
y = �
u = y
i = �
o = o
p = �
g = �
j = �
z = �
x = �
c = �
v = �
~ = �

$ = �
& = �
^ = �
@ = �
# = �
*/

using namespace Aurora::Graphics;
using namespace Aurora::Utils;
using namespace Aurora::System;
using namespace Aurora;

StateMenu::StateMenu()
{

}

StateMenu::~StateMenu()
{

}

void StateMenu::Init()
{
    newW_width = 0;
    newW_height = 0;
    newW_length = 0;

    newW_gameMode = SURVIVAL;
    newW_deleteCaves = true;
	
    conversionStage = 0;
    errorType = 0;

    converterPos = 0;
    schematicExists = false;	//cuevas

    lol = "";


    mainStatistics.blockPlaced = 0;
    mainStatistics.blockDestroyed = 0;
    mainStatistics.daysInGame = 0;
    mainStatistics.minutesPlayed = 0;
    mainStatistics.itemsCrafted = 0;
    mainStatistics.itemsSmelted = 0;
    mainStatistics.jumps = 0;
    mainStatistics.dies = 0;
    mainStatistics.foodEaten = 0;
    mainStatistics.badlyFalls = 0;
    mainStatistics.blocksWalked = 0;
    mainStatistics.treesGrowned = 0;
    mainStatistics.cropsGrowned = 0;
    mainStatistics.soilPlowed = 0;
    mainStatistics.timeInAir = 0;
    mainStatistics.timeInWater = 0;
    mainStatistics.damageRecieved = 0;
    // end

    // �������������� ���������� �����
    mainOptions.detailedSky = 1;
    mainOptions.smoothLighting = true;
    mainOptions.sounds = 1;
    mainOptions.music = 1;
    mainOptions.fogRendering = 1;
    mainOptions.fastRendering = true;
    mainOptions.newSprintOption = true;
    mainOptions.autoJump = 1;
    mainOptions.fov = 65;
    mainOptions.freezeDayTime = 0;
    mainOptions.worldBlockAnimation = 1;
    mainOptions.particles = 0;
    mainOptions.difficult = 2; // normal
    mainOptions.horizontalViewDistance = 4;
    mainOptions.verticalViewDistance = 1;
    mainOptions.guiDrawing = 1;
	mainOptions.buttonsDrawing = 1;



    //set render manager instance
    mRender = RenderManager::InstancePtr();
    mSystemMgr = SystemManager::Instance();
    mSoundMgr = SoundManager::Instance();

    logoSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Logo),0,0,256,64);
    logoSprite->Scale(1.5f,1.5f);
    logoSprite->SetPosition(240,50);

    backgroundSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::background));
    backgroundSprite->Scale(2,2);
    backgroundSprite->SetPosition(240,136);

    bx = 240;
    by = 136;
    directionx = rand() % 2;
    directiony = rand() % 2;

    rectFilledSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Rectangles),0,0,230,37);
    rectFilledSprite->SetPosition(240,150);
    rectFilledSprite->Scale(2,2);

    rectEmptySprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Rectangles),0,37,230,37);
    rectEmptySprite->SetPosition(240,150);
    rectEmptySprite->Scale(2,2);

    buttonSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,0,95,12); // stand
    buttonSprite->SetPosition(240,150);
    buttonSprite->Scale(2,2);
	
	//menu
	menuSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::World),0,0,64,64);
    menuSprite->Scale(0.6,0.6);

	smenuSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::DeselectedWorld),0,0,64,64);
    smenuSprite->Scale(0.6,0.6);

    sbuttonSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,12,95,12); // stand selected
    sbuttonSprite->SetPosition(240,150);
    sbuttonSprite->Scale(2,2);

    nbuttonSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,24,95,12); // dark
    nbuttonSprite->SetPosition(240,150);
    nbuttonSprite->Scale(2,2);

    // small buttons
    buttonSmallSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,0,95,12); // stand
    buttonSmallSprite->SetPosition(240,150);
    buttonSmallSprite->Scale(0.45f,1.0f);
    buttonSmallSprite->Scale(2,2);

    sbuttonSmallSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,12,95,12); // stand selected
    sbuttonSmallSprite->SetPosition(240,150);
    sbuttonSmallSprite->Scale(0.45f,1.0f);
    sbuttonSmallSprite->Scale(2,2);

    nbuttonSmallSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,24,95,12); // dark
    nbuttonSmallSprite->SetPosition(240,150);
    nbuttonSmallSprite->Scale(0.45f,1.0f);
    nbuttonSmallSprite->Scale(2,2);
	
    mbuttonSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,36,95,12); // gray
    mbuttonSprite->SetPosition(240,150);
    mbuttonSprite->Scale(2,2);

    smbuttonSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,48,95,12); // gray selected
    smbuttonSprite->SetPosition(240,150);
    smbuttonSprite->Scale(2,2);

    backSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Dirt),0,0,32,32);
    backSprite->Scale(2,2);
	
	lenguageSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),12,60,12,12);
	lenguageSprite->SetPosition(0,0);
	lenguageSprite->Scale(2,2);
	
	slenguageSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),24,60,12,12);
	slenguageSprite->SetPosition(0,0);
	slenguageSprite->Scale(2,2);
	
	skinSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),36,60,12,12);
	skinSprite->SetPosition(0,0);
	skinSprite->Scale(2,2);
	
	sskinSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),48,60,12,12);
	sskinSprite->SetPosition(0,0);
	sskinSprite->Scale(2,2);

    lamecraftSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::lameCraft),0,0,320,90);
    lamecraftSprite->SetPosition(240,50);
    lamecraftSprite->Scale(0.85,0.85);

    blackBackground = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),87,60,8,8);
    blackBackground->SetPosition(240,116);
    blackBackground->Scale(60,22);
	
	///*-----STEVE MODEL-----*///
	
	steveHead = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Steve),8,8,8,8);
	steveHead->Scale(3,3);
	
	steveHeadCapa = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Steve),40,8,8,8);
	steveHeadCapa->Scale(3,3);
	
	steveBody = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Steve),20,20,8,12);
	steveBody->Scale(3,3);
	
	steveHand = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Steve),44,20,4,12);
	steveHand->Scale(3,3);
	
	steveLeg = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Steve),4,20,4,12);
	steveLeg->Scale(3,3);
	
	///*-----END-----*///

    selectPos = 0;

    //load save info
    ScanSaveFiles("Save/");
    ScanTexturePacks("Assets/Textures/");

    menuState = -1;
    loadSelectPos = 0;
    loadSavePos = 0;
    aboutPos = 0;

    newWorldName = "New World";
    newWorldNamestr = "";
    newWorldSeed = "";

    size_f = 0.347f;

    //for map generation
    makeTrees = true;
    makeDungeons = true;
    makeBonus = false;
    makeCaves = true;

    terrainBuilder = 0;
    worldType = 0; // default
    gameMode = SURVIVAL;
    seed_1 = 0;

    SplashNumber = rand() % 13;
    splashSize = 0.0f;

    saveSubmenu = false;
    saveSubMenuSelect = 2;
    generateSelectPose = 0;

    newWorldNamestr = newWorldName.c_str();
    currentVersion = 140;

    //input helper
    InputHelper::Instance()->Init();
    InputHelper::Instance()->Load();

	animationscreen = 1;
	fontcoloroption = 0;
	fontcolor = 0;
	srand(time(0));

    tpCurrent = 0;
	tpMax = 0;
	tpEnd = 0;
	tpStart = 0;
	tpPos = 0;
	tpSelectPos = 0;

	for(int i = 0; i <= 31; i++)
	{
	    worldName[i] = ' ';
	}
}

void StateMenu::Enter()
{
    mRender->SetOrtho(0,0,0,0,0,0);
    selectPos = 0;
}

void StateMenu::CleanUp()
{
    delete logoSprite;
    delete buttonSprite;
    delete sbuttonSprite;
    delete nbuttonSprite;
    delete mbuttonSprite;
    delete smbuttonSprite;
	delete backSprite;
    delete lenguageSprite;
    delete slenguageSprite;
	delete skinSprite;
    delete sskinSprite;
    delete lamecraftSprite;
	
	delete menuSprite;
	delete smenuSprite;

    delete buttonSmallSprite;
    delete sbuttonSmallSprite;
    delete nbuttonSmallSprite;

    delete backgroundSprite;
    delete blackBackground;
	
    delete steveHead;
    delete steveHeadCapa;
    delete steveBody;
    delete steveHand;
    delete steveLeg;
}

void StateMenu::Pause()
{
	
}

void StateMenu::Resume()
{
    menuState = 0;
    loadSelectPos = 0;
    loadSavePos = 0;
    mRender->SetOrtho(0,0,0,0,0,0);
}

void StateMenu::NetherEnteringSystem(StateManager* sManager,std::string mundoNombre)
{
	//mundoNombre = "prueba";
	bool compression = true;
	
	/*if(saveFilesList[loadSavePos].locked == false)
	{
		
		if(saveFilesList[loadSavePos].worldGameMode == SURVIVAL || saveFilesList[loadSavePos].worldGameMode == HARDCORE)
		{
			mSoundMgr->StopMenu();
			StatePlay *statePlay = new StatePlay();
			statePlay->LoadMap(mundoNombre,compression);
			statePlay->InitCamera();
			sManager->PushState(statePlay);
		}
		else
		{
			mSoundMgr->StopMenu();
			StatePlayCreative *statePlay = new StatePlayCreative();
			statePlay->LoadMap(mundoNombre,compression);
			statePlay->InitCamera();
			sManager->PushState(statePlay);
		}
	}*/
}

void StateMenu::HandleEvents(StateManager* sManager)
{
    //update input
    mSystemMgr->InputUpdate();
	
    switch(menuState)
    {
    case -1:// language menu
    {
        //up, down
        if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
        {
            selectPos--;
            if(selectPos < 0)
                selectPos = 1;

            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
        {
            selectPos++;
            if(selectPos > 1)
                selectPos = 0;
			mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
        {
            RenderManager::InstancePtr()->defaultFontType = selectPos+1;
            RenderManager::InstancePtr()->SetDefaultFont();
            selectPos = 0;
            menuState = 0;

            SplashNumber = rand() % 13;
			
			mSoundMgr->PlayRandomMenu(); //MENU
        }
    }
    break;
    case 0://main menu
    {	

        //up, down
		
        if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
        {
			if(selectPos == 7){
				
				selectPos = 6;
				mSoundMgr->PlayMenuSound();
			}
			else
			{
				selectPos--;
				if(selectPos < 0)
					selectPos = 5;

				mSoundMgr->PlayMenuSound();
			}
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
        {
			if(selectPos == 6){
				
				selectPos = 7;
				mSoundMgr->PlayMenuSound();
			}
			else
			{
				selectPos++;
				if(selectPos > 5)
					selectPos = 0;

				mSoundMgr->PlayMenuSound();
			}
        }
		
		if(mSystemMgr->KeyPressed(PSP_CTRL_LEFT))
		{
			if(selectPos == 0 || selectPos == 1 || selectPos == 2 || selectPos == 4)
			{
				selectPos = 6;
				mSoundMgr->PlayMenuSound();
			}
		}
		
		if(mSystemMgr->KeyPressed(PSP_CTRL_RIGHT))
		{
			if(selectPos == 6 || selectPos == 7)
			{
				selectPos = 0;
				mSoundMgr->PlayMenuSound();
			}
		}

        if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
        {
            if(selectPos == 0)//play state
            {
                ScanSaveFiles("Save/");

                loadSavePos = 0;
                loadSaveStart = 0;
                loadSaveEnd = saveFilesList.size();
                loadSaveMax = 4;
                if(loadSaveMax > loadSaveEnd)
                loadSaveMax = loadSaveEnd;

                menuState = 1;
                if(saveFilesList.empty() == false)
                {
                    loadSelectPos = 0;
                }
                else
                {
                    loadSelectPos = 3;
                }
            }
            if(selectPos == 1)
            {
                //options
                StateOptions *stateOptions = new StateOptions();
                stateOptions->Init();
                sManager->PushState(stateOptions);
            }
            if(selectPos == 2)
            {
                //about
                menuState = 3;
                aboutPos = 1;
            }
            if(selectPos == 3)
            {
                //textures
                tpCurrent = 0;
                tpMax = 0;
                tpEnd = 0;
                tpStart = 0;
                tpPos = 0;
                tpSelectPos = 0;

                menuState = 11;
                tpSelectPos = 0;
                tpEnd = texturePackList.size();
                tpMax = 3;
                ScanTexturePacks("Assets/Textures/");
            }
			
			if(selectPos == 4)
            {
                //donate
                menuState = 4;
                donatePos = 1;
            }
			
			if(selectPos == 5)
            {
				sManager->Quit();
            }
			
			if(selectPos == 6)
			{
				menuState = 7;
				selectPos = 0;
			}
			
			if(selectPos == 7)
			{
				menuState = -1;
				selectPos = 0;
				mSoundMgr->StopMenu();
			}
			
        }
    }
    break;
    case 1: //select world
    {
        if(saveSubmenu)
        {
            if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
            {
                saveSubMenuSelect--;
                if(saveSubMenuSelect < 1)
                    saveSubMenuSelect = 2;

                mSoundMgr->PlayMenuSound();
            }

            if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
            {
                saveSubMenuSelect++;
                if(saveSubMenuSelect > 2)
                    saveSubMenuSelect = 1;

                mSoundMgr->PlayMenuSound();
            }

            if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
            {
                if(saveSubMenuSelect == 1)//remove file
                {
                    //remove file
                    if(saveFilesList.size() > 0)
                    {
                        if(saveFilesList[loadSavePos].compression)
                        {
                            //remove normal file
                            remove(saveFilesList[loadSavePos].fileName.c_str());

                            //remove compressed one
                            saveFilesList[loadSavePos].fileName += "c";
                            remove(saveFilesList[loadSavePos].fileName.c_str());

                        }
                        else
                        {
                            remove(saveFilesList[loadSavePos].fileName.c_str());
                        }

                        ScanSaveFiles("Save/");

                        loadSavePos = 0;
                        loadSaveStart = 0;
                        loadSaveEnd = saveFilesList.size();
                        loadSaveMax = 4;
                        if(loadSaveMax > loadSaveEnd)
                            loadSaveMax = loadSaveEnd;

                        saveSubmenu = false;
                        saveSubMenuSelect = 2;

                        if(saveFilesList.empty() == false)
                        {
                            loadSelectPos = 0;
                        }
                        else
                        {
                            loadSelectPos = 2;
                        }
                    }
                }
                if(saveSubMenuSelect == 2)//return
                {
                    saveSubMenuSelect = 2;
                    saveSubmenu = false;
                }
            }
        }
        else
        {
            if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
            {
                if(loadSelectPos == 1 || loadSelectPos == 2)
                {
                    loadSelectPos = 0;
                    mSoundMgr->PlayMenuSound();
                }
                if(loadSelectPos == 4)
                {
                    loadSelectPos = 3;
                    mSoundMgr->PlayMenuSound();
                }
            }

            if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
            {
                if(loadSelectPos == 0 || loadSelectPos == 3)
                {
                    loadSelectPos++;
                    mSoundMgr->PlayMenuSound();
                }
            }

            if(mSystemMgr->KeyPressed(PSP_CTRL_LEFT))
            {
                if(saveFilesList.empty() == false)
                {
                    switch(loadSelectPos)
                    {
                        case 2:
                        loadSelectPos = 1;
                        mSoundMgr->PlayMenuSound();
                        break;
                        case 3:
                        loadSelectPos = 0;
                        mSoundMgr->PlayMenuSound();
                        break;
                        case 4:
                        loadSelectPos = 2;
                        mSoundMgr->PlayMenuSound();
                        break;
                    }
                }
            }


            if(mSystemMgr->KeyPressed(PSP_CTRL_RIGHT))
            {
                switch(loadSelectPos)
                {
                    case 0:
                    loadSelectPos = 3;
                    mSoundMgr->PlayMenuSound();
                    break;
                    case 1:
                    loadSelectPos = 2;
                    mSoundMgr->PlayMenuSound();
                    break;
                    case 2:
                    loadSelectPos = 4;
                    mSoundMgr->PlayMenuSound();
                    break;
                }
            }

            if(mSystemMgr->KeyPressed(PSP_CTRL_LTRIGGER))
            {
                if(saveFilesList.empty() == false)
                {
                    loadSavePos--;

                    if(loadSavePos < loadSaveStart)
                    {
                        loadSaveStart--;
                        loadSaveMax--;

                        if(loadSaveMax < 4)
                        {
                            loadSaveStart = 0;
                            loadSaveMax = 4;
                        }
                    }

                    if(loadSavePos < 0)
                    {
                        loadSavePos = saveFilesList.size() - 1;
                        loadSaveMax = loadSaveEnd;
                        loadSaveStart = loadSaveEnd - 4;
                        if(loadSaveStart < 0)
                            loadSaveStart = 0;
                    }
                }
            }

            if(mSystemMgr->KeyPressed(PSP_CTRL_RTRIGGER))
            {
                if(saveFilesList.empty() == false)
                {
                    loadSavePos++;
                    if(loadSavePos == loadSaveMax)
                    {
                        loadSaveStart++;
                        loadSaveMax++;
                        if(loadSaveMax > loadSaveEnd)
                        {
                            loadSaveStart = loadSaveEnd - 4;
                            if(loadSaveStart < 0)
                                loadSaveStart = 0;
                            loadSaveMax = loadSaveEnd;
                        }
                    }
                    if(loadSavePos >= saveFilesList.size())
                    {
                        loadSavePos = 0;
                        loadSaveStart = 0;
                        loadSaveMax = 4;
                        if(loadSaveMax > loadSaveEnd)
                            loadSaveMax = loadSaveEnd;
                    }
                }
            }

            if(mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE))
            {
                menuState = 0;
            }

            if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
            {
                if(loadSelectPos == 0)//play selected world
                {
                    if(saveFilesList.size() > 0)
                    {
                        if(saveFilesList[loadSavePos].locked == false)
                        {
							
                            if(saveFilesList[loadSavePos].worldGameMode == SURVIVAL || saveFilesList[loadSavePos].worldGameMode == HARDCORE)
                            {
								mSoundMgr->StopMenu(); //menu
                                StatePlay *statePlay = new StatePlay();
                                statePlay->LoadMap(saveFilesList[loadSavePos].fileName,saveFilesList[loadSavePos].compression);
                                statePlay->InitCamera();
                                sManager->PushState(statePlay);
                            }
                            else
                            {
								mSoundMgr->StopMenu(); //menu
                                StatePlayCreative *statePlay = new StatePlayCreative();
                                statePlay->LoadMap(saveFilesList[loadSavePos].fileName,saveFilesList[loadSavePos].compression);
                                statePlay->InitCamera();
                                sManager->PushState(statePlay);
                            }
                        }
                    }
                }
                if(loadSelectPos == 1)//rename world
                {
                    if(saveFilesList.size() > 0)
                    {
                        char worldNameTemp[50];

                        unsigned short test[128];
                        unsigned short opis[10] = {'W','o','r','l','d',' ','n','a','m','e'};
                        if(mSystemMgr->ShowOSK(opis,test,128) != -1)
                        {
                            std::string newWorldName = "";
                            for(int j = 0; test[j]; j++)
                            {
                                unsigned c = test[j];

                                if(32 <= c && c <= 127) // print ascii only
                                    newWorldName += c;
                            }

                            sprintf(worldNameTemp,"%s",newWorldName.c_str());
                        }

                        int saveVersionTemp = 3;
                        char worldGameModeTemp = saveFilesList[loadSavePos].worldGameMode;
                        bool locked = saveFilesList[loadSavePos].locked;

                        FILE * pFile;
                        pFile = fopen(saveFilesList[loadSavePos].fileName.c_str(),"wb");

                        if(pFile != NULL)
                        {
                            //version
                            fwrite(&saveVersionTemp, sizeof(int),1,pFile);

                            fwrite(&worldGameModeTemp, sizeof(char),1,pFile);

                            fwrite(&locked, sizeof(bool),1,pFile);
                            //name
                            fwrite(worldNameTemp ,sizeof(char),50,pFile);

                            fclose(pFile);
                        }

                        ScanSaveFiles("Save/");
                    }
                }
                if(loadSelectPos == 2)//delete world
                {
                    if(saveFilesList.size() > 0)
                    {
                        saveSubmenu = true;
                        saveSubMenuSelect = 2;
                    }
                }
                if(loadSelectPos == 3)//create new world
                {
                    generateSelectPose = 0;
                    menuState = 5;

                    makeTrees = true;
                    makeBonus = false;
                    makeCaves = true;
					makeDungeons = true;
                    terrainBuilder = 0;
                    worldType = 0;

                    newWorldName = "New World";
                    newWorldSeed = "";
                }
                if(loadSelectPos == 4)//cancel
                {
                    menuState = 0;
                }
            }
        }
    }
    break;
    case 3://about
    {
        if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
        {
            aboutPos--;
            if(aboutPos < 0)
                aboutPos = 1;

            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
        {
            aboutPos++;
            if(aboutPos > 1)
                aboutPos = 0;

            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
        {
            if(aboutPos == 1)
            {
                menuState = 0;
            }
            if(aboutPos == 0)
            {
                menuState = 6;
                converterPos = 0;

                schematicExists = fileExists("Converter/world.schematic");
                ScanSaveFiles("Save/");
            }
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE))
        {
            menuState = 0;
        }
    }
    break;
	case 4://updateinfo
    {
        if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
        {
            donatePos--;
            if(donatePos < 0)
                donatePos = 1;

            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
        {
            donatePos++;
            if(donatePos > 1)
                donatePos = 0;

            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE))
        {
            menuState = 0;
        }
		
		if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
        {
            if(donatePos == 1)
            {
                menuState = 0;
            }
		}
    }
    break;	
    case 5://parametric terrain
    {

        if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
        {
            switch(generateSelectPose)
			{
				case 7:
				generateSelectPose = 5;
				mSoundMgr->PlayMenuSound();
				break;
				case 5:
				generateSelectPose = 3;
				mSoundMgr->PlayMenuSound();
				break;
				case 3:
				generateSelectPose = 1;
				mSoundMgr->PlayMenuSound();
				break;
				case 6:
				generateSelectPose = 4;
				mSoundMgr->PlayMenuSound();
				break;
				case 4:
				generateSelectPose = 2;
				mSoundMgr->PlayMenuSound();
				break;
				case 2:
				generateSelectPose = 0;
				mSoundMgr->PlayMenuSound();
				break;
			}
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
        {
			switch(generateSelectPose)
			{
				case 0:
				generateSelectPose = 2;
				mSoundMgr->PlayMenuSound();
				break;
				case 2:
				generateSelectPose = 4;
				mSoundMgr->PlayMenuSound();
				break;
				case 4:
				generateSelectPose = 6;
				mSoundMgr->PlayMenuSound();
				break;
				case 1:
				generateSelectPose = 3;
				mSoundMgr->PlayMenuSound();
				break;
				case 3:
				generateSelectPose = 5;
				mSoundMgr->PlayMenuSound();
				break;
				case 5:
				generateSelectPose = 7;
				mSoundMgr->PlayMenuSound();
				break;
			}
        }
		
		if(mSystemMgr->KeyPressed(PSP_CTRL_LEFT))
		{
			switch(generateSelectPose)
			{
				case 1:
				generateSelectPose = 0;
				mSoundMgr->PlayMenuSound();
				break;
				case 3:
				generateSelectPose = 2;
				mSoundMgr->PlayMenuSound();
				break;
				case 5:
				generateSelectPose = 4;
				mSoundMgr->PlayMenuSound();
				break;
				case 7:
				generateSelectPose = 6;
				mSoundMgr->PlayMenuSound();
				break;
			}
		}
		
		if(mSystemMgr->KeyPressed(PSP_CTRL_RIGHT))
		{
			switch(generateSelectPose)
			{
				case 0:
				generateSelectPose = 1;
				mSoundMgr->PlayMenuSound();
				break;
				case 2:
				generateSelectPose = 3;
				mSoundMgr->PlayMenuSound();
				break;
				case 4:
				generateSelectPose = 5;
				mSoundMgr->PlayMenuSound();
				break;
				case 6:
				generateSelectPose = 7;
				mSoundMgr->PlayMenuSound();
				break;
			}
		}
		
		if(mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE))
		{
			ScanSaveFiles("Save/");

			menuState = 1;
			loadSavePos = 0;
			loadSaveStart = 0;
			loadSaveEnd = saveFilesList.size();
			loadSaveMax = 4;
			if(loadSaveMax > loadSaveEnd)
			loadSaveMax = loadSaveEnd;

			menuState = 1;

			loadSelectPos = 3;
		}

        if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
        {
            if(generateSelectPose == 0)
            {
                newWorldName = "";

                unsigned short test[128];
                unsigned short opis[10] = {'W','o','r','l','d',' ','n','a','m','e'};
                if(mSystemMgr->ShowOSK(opis,test,128) != -1)
                {
                    for(int j = 0; j < 14; j++)
                    {
                        unsigned c = test[j];

                        if(32 <= c && c <= 127) // print ascii only
                            newWorldName += c;
                    }
                }
                newWorldNamestr = newWorldName.c_str();
            }

            if(generateSelectPose == 1)
            {
                seed_1 = 0;
                newWorldSeed = "";

                unsigned short test[128];
                unsigned short opis[10] = {'W','o','r','l','d',' ','s','e','e','d'};
                if(mSystemMgr->ShowOSK(opis,test,128) != -1)
                {

                    for(int j = 0; j < 14; j++)
                    {
                        unsigned c = test[j];

                        if(c >= 32 && c <= 127)
                        {
                            newWorldSeed += c;
                        }
                    }
                    seed_1 = hash(newWorldSeed.c_str(),0);
                    //seed_1 = std::atoi(newWorldSeed.c_str());
                }
            }
			
			if(generateSelectPose == 2)
			{
				makeDungeons == true ? makeDungeons = false : makeDungeons = true;
			}
			
			if(generateSelectPose == 3)
			{
				makeBonus == true ? makeBonus = false : makeBonus = true;
			}

            if(generateSelectPose == 4)
            {
                gameMode += 1;
                if (gameMode == 3)
                {
                    gameMode = 0;
                }
            }

            if(generateSelectPose == 5)
            {
                worldType += 1;
                if(worldType > 1)
                {
                    worldType = 0;
                }
            }

            if(generateSelectPose == 6)
            {
                //terrainBuilder = 2;
                if(gameMode == 0 || gameMode == 2)
                {
					mSoundMgr->StopMenu(); //menu
                    StatePlay *statePlay = new StatePlay();
                    statePlay->InitParametric(makeTrees,makeDungeons,makeBonus,makeCaves,seed_1,worldType,gameMode);
                    statePlay->InitCamera();
                    statePlay->SetWorldAndSaveName(newWorldName,nextSaveFileName);
                    sManager->PushState(statePlay);
                }
                else
                {
					mSoundMgr->StopMenu(); //menu
                    StatePlayCreative *statePlay = new StatePlayCreative();
                    statePlay->InitParametric(makeTrees,makeDungeons,makeBonus,makeCaves,seed_1,worldType,gameMode);
                    statePlay->InitCamera();
                    statePlay->SetWorldAndSaveName(newWorldName,nextSaveFileName);
                    sManager->PushState(statePlay);
                }
                seed_1 = 0;
            }
			
			if(generateSelectPose == 7)
			{
				ScanSaveFiles("Save/");

				menuState = 1;
				loadSavePos = 0;
				loadSaveStart = 0;
				loadSaveEnd = saveFilesList.size();
				loadSaveMax = 4;
				if(loadSaveMax > loadSaveEnd)
				loadSaveMax = loadSaveEnd;

				menuState = 1;

				loadSelectPos = 3;
			}
        }
    }
    break;
    case 6://converter //cuevas
    {
        if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
        {
            converterPos--;
            if(converterPos < 0)
                converterPos = 3;

            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
        {
            converterPos++;
            if(converterPos > 3)
                converterPos = 0;

            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
        {
			if(converterPos == 0)
			{
				newW_deleteCaves == true ? newW_deleteCaves = false : newW_deleteCaves = true;
			}
            if(converterPos == 1)
            {
                newW_gameMode == SURVIVAL ? newW_gameMode = CREATIVE : newW_gameMode = SURVIVAL;
            }
            if(converterPos == 2)
            {
                errorType = 0;
                if(schematicExists)
                {
                    char buffer[40];
                    char empty;
                    gzFile saveFile;

                    /// (1) FETCH SCHEMATIC WIDTH HEIGHT LENGTH AND TAKING ITS BLOCK IDS
                    saveFile = gzopen("Converter/world.schematic","rb");
                    if(saveFile == 0)
                        return;
                    //compound
                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &empty,sizeof(char));

                    for(int i = 0; i < 9; i++)
                    {
                        gzread(saveFile, &empty,sizeof(char));
                        lol += empty;
                    }

                    //height
                    gzread(saveFile, &empty,sizeof(char));

                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &empty,sizeof(char));

                    for(int i = 0; i < 6; i++)
                    {
                        gzread(saveFile, &empty,sizeof(char));
                        lol += empty;
                    }
                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &newW_height,sizeof(char));


                    //length
                    gzread(saveFile, &empty,sizeof(char));

                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &empty,sizeof(char));

                    for(int i = 0; i < 6; i++)
                    {
                        gzread(saveFile, &empty,sizeof(char));
                        lol += empty;
                    }
                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &newW_length,sizeof(char));


                    //width
                    gzread(saveFile, &empty,sizeof(char));

                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &empty,sizeof(char));

                    for(int i = 0; i < 5; i++)
                    {
                        gzread(saveFile, &empty,sizeof(char));
                        lol += empty;
                    }
                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &newW_width,sizeof(char));

                    if(newW_height + newW_length + newW_width != 252+112+252)
                    {
                        errorType = 1;
                        return;
                    }

                    //blocks
                    unsigned char* m_Array1;
                    m_Array1 = new block_t[252 * 112 * 252];
                    memset(m_Array1, 0, sizeof(unsigned char) * 252 * 252 * 112);

                    char need[6];
                    bool again = false;
                    for(int i = 0; i < 100000000; i++)
                    {
                        for(int j = 1; j <= 5; j++)
                        {
                            need[j-1] = need[j];
                        }
                        gzread(saveFile, &empty,sizeof(char));
                        need[5] = empty;

                        if(need[0] == 'B' &&
                           need[1] == 'l' &&
                           need[2] == 'o' &&
                           need[3] == 'c' &&
                           need[4] == 'k' &&
                           need[5] == 's')
                        {
                            i = 100000000;
                            break;
                        }

                        if(need[2] == 'D' &&
                           need[3] == 'a' &&
                           need[4] == 't' &&
                           need[5] == 'a')
                        {
                            gzread(saveFile, &empty,sizeof(char));
                            gzread(saveFile, &empty,sizeof(char));
                            gzread(saveFile, &empty,sizeof(char));
                            gzread(saveFile, &empty,sizeof(char));

                            gzread(saveFile, m_Array1,sizeof(unsigned char)*(252 * 112 * 252));

                            again = true;;
                            i = 100000000;
                            break;
                        }
                    }

                    if(again)
                    {
                        memset(m_Array1, 0, sizeof(unsigned char) * 252 * 252 * 112);
                        for(int j = 0; j <= 6; j++)
                        {
                            need[j] = '/0';
                        }

                        for(int i = 0; i < 100000000; i++)
                        {
                            for(int j = 1; j <= 5; j++)
                            {
                                need[j-1] = need[j];
                            }
                            gzread(saveFile, &empty,sizeof(char));
                            need[5] = empty;

                            if(need[0] == 'B' &&
                               need[1] == 'l' &&
                               need[2] == 'o' &&
                               need[3] == 'c' &&
                               need[4] == 'k' &&
                               need[5] == 's')
                            {
                                i = 100000000;
                                break;
                            }
                        }
                    }
                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &empty,sizeof(char));

                    gzread(saveFile, m_Array1,sizeof(unsigned char)*(252 * 112 * 252));

                    gzclose(saveFile);

                    // id converter
                    std::vector <unsigned char> id_map;
                    id_map.push_back(0); //0
                    id_map.push_back(RockBlock::getID());
                    id_map.push_back(GrassBlock::getID());
                    id_map.push_back(DirtBlock::getID());
                    id_map.push_back(CobbleStone::getID());
                    id_map.push_back(OakPlanks::getID());
                    id_map.push_back(OakSapling::getID());
                    id_map.push_back(IronBlock::getID());
                    id_map.push_back(WaterBlock::getID());
                    id_map.push_back(WaterBlock::getID());
                    id_map.push_back(Lava::getID());
                    id_map.push_back(Lava::getID());
                    id_map.push_back(SandBlock::getID());
                    id_map.push_back(Gravel::getID());
                    id_map.push_back(GoldBlock::getID());
                    id_map.push_back(IronOre::getID());
                    id_map.push_back(CoalOre::getID());
                    id_map.push_back(WoodBlock::getID());
                    id_map.push_back(LeavesBlock::getID());
                    id_map.push_back(0);	//sponge
                    id_map.push_back(GlassBlock::getID());
                    id_map.push_back(LapisOre::getID());
                    id_map.push_back(LapisBlock::getID());
                    id_map.push_back(0);
                    id_map.push_back(SandStone::getID());
                    id_map.push_back(NoteBlock::getID());
                    id_map.push_back(0); // bed
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(WhiteWoolBlock::getID()); // 35
                    id_map.push_back(0);
                    id_map.push_back(Flower1::getID());
                    id_map.push_back(Flower2::getID());
                    id_map.push_back(Mooshroom1::getID());
                    id_map.push_back(Mooshroom2::getID());
                    id_map.push_back(GoldBlock::getID());
                    id_map.push_back(IronBlock::getID());
                    id_map.push_back(DoubleSlab::getID());
                    id_map.push_back(Slab::getID());
                    id_map.push_back(BrickBlock::getID());
                    id_map.push_back(TNTBlock::getID());
                    id_map.push_back(ShelfBlock::getID());
                    id_map.push_back(MossyCobblestone::getID());
                    id_map.push_back(Obsidian::getID());
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(BirchPlanks::getID());
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(DiamondOre::getID());
                    id_map.push_back(Diamond::getID());
                    id_map.push_back(CraftingTable::getID());
                    id_map.push_back(WheatBlock6::getID());
                    id_map.push_back(Soil::getID());
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(CobbleStone::getID());
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(RedstoneOre::getID());
                    id_map.push_back(RedstoneOre::getID());
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(Snow2::getID());
                    id_map.push_back(IceBlock::getID());
                    id_map.push_back(SnowBlock::getID());
                    id_map.push_back(CactusBlock::getID());
                    id_map.push_back(ClayBlock::getID());
                    id_map.push_back(CaneBlock::getID());
                    id_map.push_back(JukeBox::getID());
                    id_map.push_back(WoodenFence::getID());
                    id_map.push_back(Pumpkin1::getID());
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(JackOLantern1::getID());
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(StoneBrick::getID());
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(IronBars::getID());
                    id_map.push_back(GlassPanel::getID());
                    id_map.push_back(MelonBlock::getID());
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(BrickBlock::getID());
                    id_map.push_back(0);
                    id_map.push_back(GrassBlock::getID());
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(0);
                    id_map.push_back(OakHalfBlock::getID());


                    unsigned char* m_Array2;
                    m_Array2 = new block_t[252 * 112 * 252];
                    memset(m_Array2, 0, sizeof(unsigned char) * 252 * 252 * 112);

                    for(int x = 0; x < 252; x++)
                    {
                        for(int z = 0; z < 252; z++)
                        {
                            for(int y = 0; y < 112; y++)
                            {
                                int take_adress = x+z*252+y*252*252;
                                int take_id = m_Array1[take_adress];

                                int put_adress = x+y*252+z*252*112;
                                int put_id = 0;

                                if(take_id < id_map.size())
                                {
                                    put_id = id_map[take_id];
                                }

                                m_Array2[put_adress] = put_id;
                            }
                        }
                    }

                    // creating height map
                    unsigned char* m_Heightmap;
                    m_Heightmap = new block_t[252 * 252];
                    memset(m_Heightmap, 0, sizeof(unsigned char) * 252 * 252);

                    for(int x = 0; x < 252; x++)
                    {
                        for(int z = 0; z < 252; z++)
                        {
                            int adress = x+z*252;
                            for(int y = 111; y >= 0; y--)
                            {
                                int block_adress = x+y*252+z*252*112;
                                unsigned char& block = m_Array2[block_adress];

                                if(block == DirtBlock::getID() || block == GrassBlock::getID() || block == SandBlock::getID() || block == RockBlock::getID() || block == Gravel::getID() || block == SandStone::getID())
                                {
                                    m_Heightmap[adress] = y;
                                    y = -1;
                                }
                            }
                        }
                    }

                // cuevas
				if(newW_deleteCaves == true)
				{
                    for(int x = 0; x < 252; x++)
                    {
                        for(int z = 0; z < 252; z++)
                        {
                            int adress = x+z*252;
                            int height = m_Heightmap[adress];
                            for(int y = 0; y <= height-1; y++)
                            {
                                int block_adress = x+y*252+z*252*112;
                                unsigned char& block = m_Array2[block_adress];


                                if(block != 0)
                                {
                                    if(block != IronBlock::getID() && block != RockBlock::getID() &&  block != DirtBlock::getID() && block != GoldBlock::getID() && block != IronOre::getID() && block != LapisOre::getID() && block != RedstoneOre::getID()
                                       && block != Lava::getID() && block != WaterBlock::getID() && block != RedstoneOre::getID() && block != CoalOre::getID() && block != Gravel::getID() && block != DiamondOre::getID()
                                       && block != SandStone::getID() && block != SandBlock::getID() && block != Mooshroom1::getID() && block != Mooshroom2::getID())
                                    {
                                        y = 100;
                                        continue;
                                    }
                                }
                                if(block == 0 || block == Lava::getID() || block == WaterBlock::getID())
                                {
                                    block = RockBlock::getID();
                                }

                            }
                        }
                    }
                    free(m_Heightmap);
				}


                    /// (2) FETCH SCHEMATIC DATA
                    saveFile = gzopen("Converter/world.schematic","rb");
                    if(saveFile == 0)
                        return;

                    for(int j = 0; j <= 6; j++)
                    {
                        need[j] = '/0';
                    }

                    //data
                    again = false;
                    for(int i = 0; i < 100000000; i++)
                    {
                        for(int j = 1; j <= 5; j++)
                        {
                            need[j-1] = need[j];
                        }
                        gzread(saveFile, &empty,sizeof(char));
                        need[5] = empty;

                        if(need[0] == 'B' &&
                           need[1] == 'l' &&
                           need[2] == 'o' &&
                           need[3] == 'c' &&
                           need[4] == 'k' &&
                           need[5] == 's')
                        {
                            again = true;
                            i = 100000000;

                            gzread(saveFile, &empty,sizeof(char));
                            gzread(saveFile, &empty,sizeof(char));
                            gzread(saveFile, &empty,sizeof(char));
                            gzread(saveFile, &empty,sizeof(char));

                            gzread(saveFile, m_Array1,sizeof(unsigned char)*(252 * 112 * 252));

                            break;
                        }

                        if(need[2] == 'D' &&
                           need[3] == 'a' &&
                           need[4] == 't' &&
                           need[5] == 'a')
                        {
                            i = 100000000;
                            break;
                        }
                    }

                    if(again)
                    {
                        memset(m_Array1, 0, sizeof(unsigned char) * 252 * 252 * 112);
                        for(int j = 0; j <= 6; j++)
                        {
                            need[j] = '/0';
                        }

                        for(int i = 0; i < 100000000; i++)
                        {
                            for(int j = 1; j <= 5; j++)
                            {
                                need[j-1] = need[j];
                            }
                            gzread(saveFile, &empty,sizeof(char));
                            need[5] = empty;

                            if(need[2] == 'D' &&
                               need[3] == 'a' &&
                               need[4] == 't' &&
                               need[5] == 'a')
                            {
                                i = 100000000;
                                break;
                            }
                        }
                    }

                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &empty,sizeof(char));
                    gzread(saveFile, &empty,sizeof(char));

                    memset(m_Array1, 0, sizeof(unsigned char) * 252 * 252 * 112);
                    gzread(saveFile, m_Array1,sizeof(unsigned char)*(252 * 112 * 252));

                    gzclose(saveFile);

                    // place correct blocks in our world using schematic blocks data
                    for(int x = 0; x < 252; x++)
                    {
                        for(int z = 0; z < 252; z++)
                        {
                            for(int y = 0; y < 112; y++)
                            {
                                int address = x+y*252+z*252*112;
                                int data_address = x+z*252+y*252*252;
                                unsigned char& block = m_Array2[address];
                                unsigned char data = m_Array1[data_address];

                                if(y < 111)
                                {
                                    int up_address = x+(y+1)*252+z*252*112;
                                    unsigned char& up_block = m_Array2[up_address];

                                    if(up_block == Snow2::getID() && (block == GrassBlock::getID() || block == DirtBlock::getID()))
                                    {
                                        block = SnowSoil::getID();
                                        continue;
                                    }
                                }

                                if(block == LeavesBlock::getID())
                                {
                                    if(data == 1 || data == 5 || data == 9 || data == 13)
                                    {
                                        block = SpruceLeaves::getID();
                                    }
                                    if(data == 2 || data == 6 || data == 10 || data == 14)
                                    {
                                        block = BirchLeaves::getID();
                                    }
                                    continue;
                                }

                                if(block == OakPlanks::getID())
                                {
                                    if(data == 1)
                                    {
                                        block = SprucePlanks::getID();
                                    }
                                    if(data == 2)
                                    {
                                        block = BirchPlanks::getID();
                                    }
                                    continue;
                                }
                                if(block == WoodBlock::getID())
                                {
                                    if(data == 1)
                                    {
                                        block = DarkWoodBlock::getID();
                                    }
                                    if(data == 2)
                                    {
                                        block = WhiteWoodBlock::getID();
                                    }
                                    continue;
                                }
                                if(block == WheatBlock6::getID())
                                {
                                    if(data == 0)
                                    {
                                        block = WheatBlock1::getID();
                                    }
                                    if(data == 1 || data == 2)
                                    {
                                        block = WheatBlock2::getID();
                                    }
                                    if(data == 3 || data == 4)
                                    {
                                        block = WheatBlock3::getID();
                                    }
                                    if(data >= 5)
                                    {
                                        block = WheatBlock4::getID() + (data-5);
                                    }
                                    continue;
                                }

                                if(block == DoubleSlab::getID())
                                {
                                    if(data == 1)
                                    {
                                        block = SandStone::getID();
                                    }
                                    if(data == 2)
                                    {
                                        block = OakPlanks::getID();
                                    }
                                    if(data == 3)
                                    {
                                        block = CobbleStone::getID();
                                    }
                                    if(data == 4)
                                    {
                                        block = BrickBlock::getID();
                                    }
                                    if(data == 5)
                                    {
                                        block = StoneBrick::getID();
                                    }
                                    continue;
                                }
                                if(block == Slab::getID())
                                {
                                    if(data == 2)
                                    {
                                        block = OakHalfBlock::getID();
                                    }
                                    if(data == 3)
                                    {
                                        block = CobbleHalfBlock::getID();
                                    }
                                    if(data == 4)
                                    {
                                        block = BrickHalfBlock::getID();
                                    }
                                    if(data == 5)
                                    {
                                        block = HalfStoneBrick::getID();
                                    }
                                    continue;
                                }
                                if(block == OakHalfBlock::getID())
                                {
                                    if(data == 1)
                                    {
                                        block = SpruceHalfBlock::getID();
                                    }
                                    if(data == 2)
                                    {
                                        block = BirchHalfBlock::getID();
                                    }
                                    continue;
                                }
                                if(block == StoneBrick::getID())
                                {
                                    if(data == 2)
                                    {
                                        block = CrackedStoneBrick::getID();
                                    }
                                    if(data == 3)
                                    {
                                        block = CarvedStoneBrick::getID();
                                    }
                                    continue;
                                }
                                if(block == Pumpkin1::getID())
                                {
                                    block = Pumpkin1::getID() + data;
                                    continue;
                                }
                                if(block == WhiteWoolBlock::getID())
                                {
                                    if(data == 1)
                                    {
                                        block = OrangeWoolBlock::getID();
                                    }
                                    if(data == 2)
                                    {
                                        block = VioletWoolBlock::getID();
                                    }
                                    if(data == 3)
                                    {
                                        block = LightBlueWoolBlock::getID();
                                    }
                                    if(data == 4)
                                    {
                                        block = YellowWoolBlock::getID();
                                    }
                                    if(data == 5)
                                    {
                                        block = GreenWoolBlock::getID();
                                    }
                                    if(data == 6)
                                    {
                                        block = PinkWoolBlock::getID();
                                    }
                                    if(data == 7)
                                    {
                                        block = GrayWoolBlock::getID();
                                    }
                                    if(data == 8)
                                    {
                                        block = LightGrayWoolBlock::getID();
                                    }
                                    if(data == 9)
                                    {
                                        block = PastelWoolBlock::getID();
                                    }
                                    if(data == 10)
                                    {
                                        block == VioletWoolBlock::getID();
                                    }
                                    if(data == 11)
                                    {
                                        block = BlueWoolBlock::getID();
                                    }
                                    if(data == 12)
                                    {
                                        block = BrownWoolBlock::getID();
                                    }
                                    if(data == 13)
                                    {
                                        block = DarkGreenWoolBlock::getID();
                                    }
                                    if(data == 14)
                                    {
                                        block = RedWoolBlock::getID();
                                    }
                                    if(data == 15)
                                    {
                                        block = BlackWoolBlock::getID();
                                    }
                                    continue;
                                }
                            }
                        }
                    }

                    free(m_Array1);
                    saveFile=0;

                    /// (2.1) SET SUITABLE PLAYER POSITION

                    Vector3 playerPos = Vector3(128,69,128);
                    for(int y = 111; y >= 40; y--)
                    {
                        int address = 128+y*252+128*252*112;
                        unsigned char block = m_Array2[address];

                        if(block != 0)
                        {
                            playerPos = Vector3(128.5f,y+1.7f,128.5f);
                            break;
                        }
                    }


                    /// (3) SAVE WORLD
                    std::string filename = nextSaveFileName;

                    //first save version and name on normal file
                    FILE * pFile;
                    pFile = fopen(filename.c_str(),"wb");

                    if(pFile == NULL)
                        return;

                    //version
                    int _worldVersion = 150;
                    fwrite(&_worldVersion,sizeof(int),1,pFile);

                    //game mode
                    fwrite(&newW_gameMode,sizeof(char),1,pFile);

                    //game mode
                    bool locked = 0;
                    fwrite(&locked,sizeof(bool),1,pFile);

                    //name
                    char _worldName[50];
                    _worldName[0] = 'C';
                    _worldName[1] = 'o';
                    _worldName[2] = 'n';
                    _worldName[3] = 'v';
                    _worldName[4] = 'e';
                    _worldName[5] = 'r';
                    _worldName[6] = 't';
                    _worldName[7] = 'e';
                    _worldName[8] = 'd';
                    _worldName[9] = 1+rand()%126;
                    _worldName[10] = '\0';
                    fwrite(_worldName,sizeof(char),50,pFile);

                    //close file
                    fclose(pFile);


                    //now save compressed map info
                    filename = filename + "c";

                    saveFile = gzopen(filename.c_str(),"wb");
                    if(saveFile == 0)
                        return;
						
						
					bool createdHell = false;
					char currentWorld = 0;
					gzwrite(saveFile, &currentWorld,sizeof(char));
					gzwrite(saveFile, &createdHell,sizeof(bool));
					
                    //player pos

                    gzwrite(saveFile, &playerPos.x,sizeof(float));
                    gzwrite(saveFile, &playerPos.y,sizeof(float));
                    gzwrite(saveFile, &playerPos.z,sizeof(float));

                    //player spawn pos
                    Vector3 playerSpawnPointPosition = playerPos;
                    gzwrite(saveFile, &playerSpawnPointPosition.x,sizeof(float));
                    gzwrite(saveFile, &playerSpawnPointPosition.y,sizeof(float));
                    gzwrite(saveFile, &playerSpawnPointPosition.z,sizeof(float));

                    //snow biome pos
                    Vector3 snowBiomePosition = Vector3(0,0,0);
                    float snowBiomeRadius = -1;
                    gzwrite(saveFile, &snowBiomePosition.x,sizeof(float));
                    gzwrite(saveFile, &snowBiomePosition.z,sizeof(float));
                    gzwrite(saveFile, &snowBiomeRadius,sizeof(float));

                    //desert biome pos
                    Vector3 desertBiomePosition = Vector3(0,0,0);
                    float desertBiomeRadius = -1;
                    gzwrite(saveFile, &desertBiomePosition.x,sizeof(float));
                    gzwrite(saveFile, &desertBiomePosition.z,sizeof(float));
                    gzwrite(saveFile, &desertBiomeRadius,sizeof(float));
					
					//forest biome pos
                    //Vector3 forestBiomePosition = Vector3(0,0,0);
                    //float forestBiomeRadius = -1;
                    //gzwrite(saveFile, &forestBiomePosition.x,sizeof(float));
                    //gzwrite(saveFile, &forestBiomePosition.z,sizeof(float));
                    //gzwrite(saveFile, &forestBiomeRadius,sizeof(float));

                    //size
                    int WORLD_SIZE = 252;
                    gzwrite(saveFile, &WORLD_SIZE,sizeof(int));

                    //size
                    int WORLD_HEIGHT = 112;
                    gzwrite(saveFile, &WORLD_HEIGHT,sizeof(int));

                    //chunksize
                    int CHUNK_SIZE = 14;
                    gzwrite(saveFile, &CHUNK_SIZE,sizeof(int));

                    //cubes
                    gzwrite(saveFile, m_Array2,sizeof(unsigned char)*(WORLD_SIZE * WORLD_HEIGHT * WORLD_SIZE));

                    for(int x = 0; x < 252; x++)
                    {
                        for(int z = 0; z < 252; z++)
                        {
                            for(int y = 0; y < 112; y++)
                            {
                                m_Array2[z*112*252+y*252+x] = 0;
                            }
                        }
                    }

                    //settings
                    gzwrite(saveFile, m_Array2,sizeof(unsigned char)*(WORLD_SIZE * WORLD_HEIGHT * WORLD_SIZE));

                    float worldDayTime = 12.0f;
                    //world time
                    gzwrite(saveFile, &worldDayTime,sizeof(float));
                    //sun time
                    //gzwrite(saveFile, &sunTime,sizeof(float));

                    gzwrite(saveFile, &mainStatistics,sizeof(st));

                    gzwrite(saveFile, &mainOptions,sizeof(opt));


                    short invId[36];
                    short invAm[36];
                    bool invSt[36];
                    short armorId[4];
                    short armorAm[4];
                    bool armorSt[4];

                    for(int o = 0; o <= 35; o += 1) // ������������� ���������
                    {
                        invAm[o] = -1; // ���������� ����� � ������ = -1 (0)
                        invId[o] = -1; // Id ����� � ������ = -1 (������ �����)
                        invSt[o] = false; // ���� � ������ �� ��������� (�� ���������)
                    }

                    for(int o = 0; o <= 3; o += 1)
                    {
                        armorId[o] = 324+o;
                        armorAm[o] = -1;
                        armorSt[o] = false;
                    }
                    gzwrite(saveFile, &invId,sizeof(short)*(36));

                    gzwrite(saveFile, &invAm,sizeof(short)*(36));

                    gzwrite(saveFile, &invSt,sizeof(bool)*(36));

                    gzwrite(saveFile, &armorId,sizeof(short)*(4));

                    gzwrite(saveFile, &armorAm,sizeof(short)*(4));

                    gzwrite(saveFile, &armorSt,sizeof(bool)*(4));

                    float HP = 20;
                    gzwrite(saveFile, &HP,sizeof(unsigned int));

                    float HG = 20;
                    gzwrite(saveFile, &HG,sizeof(unsigned int));

                    float OS = 10;
                    gzwrite(saveFile, &OS,sizeof(unsigned int));
					
					int EXP = 0;
                    gzwrite(saveFile, &EXP,sizeof(int));
					
					int EXPcount = 0;
                    gzwrite(saveFile, &EXPcount,sizeof(int));

                    float brightFactor = 1.0f;
                    gzwrite(saveFile, &brightFactor,sizeof(float));

                    float starsFactor = 0.0f;
                    gzwrite(saveFile, &starsFactor,sizeof(float));

                    int worldSeed = 0;
                    gzwrite(saveFile, &worldSeed,sizeof(int));

                    float skyTime = 112.0f;
                    gzwrite(saveFile, &skyTime,sizeof(float));
					
                    int savedWeatherType = 0;
                    gzwrite(saveFile, &savedWeatherType,sizeof(int));

                    float savedWeatherDuration = 600+rand()%300;
                    gzwrite(saveFile, &savedWeatherDuration,sizeof(float));

                    int null_var = 0;
                    gzwrite(saveFile, &null_var,sizeof(int));

                    gzwrite(saveFile, &null_var,sizeof(int));

                    gzwrite(saveFile, &null_var,sizeof(int));

                    gzwrite(saveFile, &null_var,sizeof(int));

                    gzwrite(saveFile, &null_var,sizeof(int));

                    gzwrite(saveFile, &null_var,sizeof(int));

                    gzwrite(saveFile, &null_var,sizeof(int));

                    gzwrite(saveFile, &null_var,sizeof(int));

                    gzwrite(saveFile, &null_var,sizeof(int));

                    gzwrite(saveFile, &null_var,sizeof(int));

                    gzwrite(saveFile, &null_var,sizeof(int));

                    //close file
                    gzclose(saveFile);
                    saveFile=0;

                    free(m_Array2);
                }
            }
            if(converterPos == 3)
            {
                menuState = 3;
            }
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE))
        {
            menuState = 3;
        }
    }
    break;
	case 7://playerOptions
	{
		 if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
        {
            skinPos--;
            if(skinPos < 0)
                skinPos = 1;

            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
        {
            skinPos++;
            if(skinPos > 1)
                skinPos = 0;

            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE))
        {
            menuState = 0;
        }
		
		if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
        {
            if(skinPos == 1)
            {
                menuState = 0;
            }
		}
	}
	break;	
    case 10:
    {
        if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
        {
            saveSubMenuSelect == 0 ? saveSubMenuSelect = 1 : saveSubMenuSelect = 0;

            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
        {
            saveSubMenuSelect == 1 ? saveSubMenuSelect = 0 : saveSubMenuSelect = 1;

            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
        {
            char worldNameTemp[50];
            for(char i = 0; i <= 49; i++)
            {
                worldNameTemp[i] = saveFilesList[loadSavePos].worldName[i];
            }

            int saveVersionTemp = 3;
            char worldGameModeTemp = saveSubMenuSelect;
            bool locked = saveFilesList[loadSavePos].locked;

            FILE * pFile;
            pFile = fopen(saveFilesList[loadSavePos].fileName.c_str(),"wb");

            if(pFile != NULL)
            {
                //version
                fwrite(&saveVersionTemp, sizeof(int),1,pFile);

                fwrite(&worldGameModeTemp, sizeof(char),1,pFile);

                fwrite(&locked, sizeof(bool),1,pFile);
                //name
                fwrite(worldNameTemp ,sizeof(char),50,pFile);

                fclose(pFile);
            }

            ScanSaveFiles("Save/");

            if(saveFilesList[loadSavePos].locked == true)
            {
                loadSelectPos = 1;
            }

            menuState = 1;
        }
    }
    break;
    case 11://texturepack
    {
        if(mSystemMgr->KeyPressed(PSP_CTRL_LEFT))
        {
            tpSelectPos --;
            if(tpSelectPos < 0)
            {
                tpSelectPos = 1;
            }
            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_RIGHT))
        {
            tpSelectPos ++;
            if(tpSelectPos > 1)
            {
                tpSelectPos = 0;
            }
            mSoundMgr->PlayMenuSound();
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_LTRIGGER))
        {
            tpPos--;
            if(tpPos < tpStart)
            {
                tpStart--;
                tpMax--;

                if(tpMax < 3)
                {
                    tpStart = 0;
                    tpMax = 3;
                }
            }

            if(tpPos < 0)
            {
                tpPos = texturePackList.size() - 1;
                tpMax = tpEnd;
                tpStart = tpEnd - 3;
                if(tpStart < 0)
                    tpStart = 0;
            }
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_RTRIGGER))
        {
            tpPos++;
            if(tpPos == tpMax)
            {
                tpStart++;
                tpMax++;
                if(tpMax > tpEnd)
                {
                    tpStart = tpEnd - 3;
                    if(tpStart < 0)
                    {
                        tpStart = 0;
                    }
                    tpMax = tpEnd;
                }
            }
            if(tpPos >= texturePackList.size())
            {
                tpPos = 0;
                tpStart = 0;
                tpMax = 3;
                if(tpMax > tpEnd)
                    tpMax = tpEnd;
            }
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE))
        {
            menuState = 0;
        }

        if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
        {
            if(tpSelectPos == 0)//play state
            {
                if(texturePackList.empty() == false)
                {
                    TextureHelper::Instance()->SetTexturePack(texturePackList[tpPos].name);
                    tpCurrent = tpPos;
                }
            }
            if(tpSelectPos == 1)//delete save
            {
                menuState = 0;
            }
        }
    }
    break;
    }
	
}

void StateMenu::Update(StateManager* sManager)
{




}

void StateMenu::Draw(StateManager* sManager)
{
    //start rendering
    mRender->StartFrame(1,1,1);

    switch(menuState)
    {
    case -1://language menu
    {
        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        for(int x = 0; x < 8; x++)
        {
            for(int y = 0; y < 5; y++)
            {
                backSprite->SetPosition(x*64,y*64);
                backSprite->Draw();
            }
        }

        // english
        buttonSprite->SetPosition(240,120);
        buttonSprite->Draw();

        // russian
        buttonSprite->SetPosition(240,160);
        buttonSprite->Draw();

        //selected button
        sbuttonSprite->SetPosition(240,(selectPos * 40) + 120);
        sbuttonSprite->Draw();

        sceGuDisable(GU_BLEND);
        sceGuEnable(GU_DEPTH_TEST);

        selectPos == 0 ? DrawText(240,129,GU_COLOR(1,1,0.25,1) ,default_size,"English") : DrawText(240,129,GU_COLOR(1,1,1,1) ,default_size,"English");
        selectPos == 1 ? DrawText(240,169,GU_COLOR(1,1,0.25,1) ,default_size,"Spanish") : DrawText(240,169,GU_COLOR(1,1,1,1) ,default_size,"Spanish");

        DrawText(240,24,GU_COLOR(1,1,1,1) ,default_size,"Select your language");
    }
    break;
    case 0://main menu
    {
		
        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        if(bx >= 360)
        {
            directionx = false;
        }
        if(bx <= 120)
        {
            directionx = true;
        }

        if(by >= 272-68)
        {
            directiony = false;
        }
        if(by <= 68)
        {
            directiony = true;
        }

        if(directionx == true)
        {
            bx += 1/6.0f;
        }
        else
        {
            bx -= 1/6.0f;
        }

        if(directiony == true)
        {
            by += 272.0f/960.0f/6.0f;
        }
        else
        {
            by -= 272.0f/960.0f/6.0f;
        }


        backgroundSprite->SetPosition(bx,by);
        backgroundSprite->DrawLinear();
		
        //logo
		lamecraftSprite->SetPosition(240,40);
        lamecraftSprite->Draw();

        //play
		buttonSprite->SetPosition(240,110);
        buttonSprite->Draw();

        //options
        buttonSprite->SetPosition(240,140);
        buttonSprite->Draw();

        //about
        buttonSmallSprite->SetPosition(188,170);
        buttonSmallSprite->Draw();

        //texture pack
        buttonSmallSprite->SetPosition(292,170);
        buttonSmallSprite->Draw();
		
        //donate
        buttonSmallSprite->SetPosition(188,200);
        buttonSmallSprite->Draw();

        //quit
        buttonSmallSprite->SetPosition(292,200);
        buttonSmallSprite->Draw();
		
		//skin
		skinSprite->SetPosition(130,170);
		skinSprite->Draw();
		
		//idioma
		lenguageSprite->SetPosition(130,200);
		lenguageSprite->Draw();
		
		if(selectPos == 0)
		{
			sbuttonSprite->SetPosition(240,110);
			sbuttonSprite->Draw();
		}
		else if(selectPos == 1)
		{
			sbuttonSprite->SetPosition(240,140);
			sbuttonSprite->Draw();
		}
		else if(selectPos == 2)
		{
			sbuttonSmallSprite->SetPosition(188,170);
			sbuttonSmallSprite->Draw();
		}
		else if(selectPos == 3)
		{
			sbuttonSmallSprite->SetPosition(292,170);
			sbuttonSmallSprite->Draw();
		}
		else if(selectPos == 4)
		{
			sbuttonSmallSprite->SetPosition(188,200);
			sbuttonSmallSprite->Draw();
		}
		else if(selectPos == 5)
		{
			sbuttonSmallSprite->SetPosition(292,200);
			sbuttonSmallSprite->Draw();
		}
		else if(selectPos == 6)
		{
			sskinSprite->SetPosition(130,170);
			sskinSprite->Draw();
		}
		else
		{
			slenguageSprite->SetPosition(130,200);
			slenguageSprite->Draw();
		}
		
		steveHead->SetPosition(366,128);
		steveHead->Draw();
		
		steveHeadCapa->SetPosition(366,128);
		steveHeadCapa->Draw();
		
		steveBody->SetPosition(366,158);
		steveBody->Draw();
		
		steveLeg->SetPosition(360,194);
		steveLeg->Draw();
		
		steveLeg->SetPosition(372,194);
		steveLeg->Draw();
		
		steveHand->SetPosition(348,158);
		steveHand->Draw();
		
		steveHand->SetPosition(384,158);
		steveHand->Draw();
		
		
        sceGuDisable(GU_BLEND);
        sceGuEnable(GU_DEPTH_TEST);

        splashSize += 0.13f;
        if(splashSize > 2*PI)
        {
            splashSize = 0.0f;
        }

		DrawText(420,272,GU_COLOR(1,1,1,1) ,0.35,"Minecraft PSP BETA");

		if(RenderManager::InstancePtr()->GetFontLanguage() == ENGLISH)
        {
			selectPos == 0 ? DrawText(240,119,GU_COLOR(1,1,0.25,1) ,default_size,"Play") : DrawText(240,119,GU_COLOR(1,1,1,1) ,default_size,"Play");
			selectPos == 1 ? DrawText(240,149,GU_COLOR(1,1,0.25,1) ,default_size,"Options") : DrawText(240,149,GU_COLOR(1,1,1,1) ,default_size,"Options");
			selectPos == 2 ? DrawText(188,179,GU_COLOR(1,1,0.25,1) ,default_size,"About") : DrawText(188,179,GU_COLOR(1,1,1,1) ,default_size,"About");
			selectPos == 3 ? DrawText(292,179,GU_COLOR(1,1,0.25,1) ,default_size,"Textures") : DrawText(292,179,GU_COLOR(1,1,1,1) ,default_size,"Textures");
			selectPos == 4 ? DrawText(188,209,GU_COLOR(1,1,0.25,1) ,default_size,"More") : DrawText(188,209,GU_COLOR(1,1,1,1) ,default_size,"More");
			selectPos == 5 ? DrawText(292,209,GU_COLOR(1,1,0.25,1) ,default_size,"Quit") : DrawText(292,209,GU_COLOR(1,1,1,1) ,default_size,"Quit");
		}
		
		if(RenderManager::InstancePtr()->GetFontLanguage() == RUSSIAN)
        {
			selectPos == 0 ? DrawText(240,119,GU_COLOR(1,1,0.25,1) ,default_size,"Jugar") : DrawText(240,119,GU_COLOR(1,1,1,1) ,default_size,"Jugar");
            selectPos == 1 ? DrawText(240,149,GU_COLOR(1,1,0.25,1) ,default_size,"Opciones") : DrawText(240,149,GU_COLOR(1,1,1,1) ,default_size,"Opciones");
            selectPos == 2 ? DrawText(188,179,GU_COLOR(1,1,0.25,1) ,default_size,"Sobre") : DrawText(188,179,GU_COLOR(1,1,1,1) ,default_size,"Sobre");
            selectPos == 3 ? DrawText(292,179,GU_COLOR(1,1,0.25,1) ,default_size,"Texturas") : DrawText(292,179,GU_COLOR(1,1,1,1) ,default_size,"Texturas");
            selectPos == 4 ? DrawText(188,209,GU_COLOR(1,1,0.25,1) ,default_size,"Mas") : DrawText(188,209,GU_COLOR(1,1,1,1) ,default_size,"Mas");
            selectPos == 5 ? DrawText(292,209,GU_COLOR(1,1,0.25,1) ,default_size,"Salir") : DrawText(292,209,GU_COLOR(1,1,1,1) ,default_size,"Salir");
		}


		switch(SplashNumber)
		{
			case 0: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"More polygons!"); break;
			case 1: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"Find dungeons!"); break;
			case 2: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"Kaaneeeedaaaa!"); break;
			case 3: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"Music by C418!"); break;
			case 4: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"Ride the pig!"); break;
			case 5: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"Not approved by Mojang!"); break;
			case 6: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"Do not distribute!"); break;
			case 7: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"Spiders everywhere!"); break;
			case 8: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"Sublime!"); break;
			case 9: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"12345 is a bad password!"); break;
			case 10: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"Follow the train, CJ!"); break;
			case 11: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"Exploding creepers!"); break;
			case 12: DrawText(328,66,GU_COLOR(1,1,0,1) ,0.35+sinf(splashSize)*0.02f,"RegenStudio = new Regen();"); break;
		}
    }
    break;
    case 1://select world
    {
        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        for(int x = 0; x < 8; x++)
        {
            for(int y = 0; y < 5; y++)
            {
                backSprite->SetPosition(x*64,y*64);
                backSprite->Draw();
            }
        }

        if(saveSubmenu) // delete world
        {
            buttonSprite->SetPosition(240,235);
            buttonSprite->Draw();

            buttonSprite->SetPosition(240,260);
            buttonSprite->Draw();
        }
        else
        {
            /// left part
            if(saveFilesList.empty() == false)
            {
                buttonSprite->SetPosition(120,222); // play selected world
                buttonSprite->Draw();

                buttonSmallSprite->SetPosition(67.75,255); // rename
                buttonSmallSprite->Draw();

                buttonSmallSprite->SetPosition(172.25,255); // delete
                buttonSmallSprite->Draw();
            }
            else
            {
                nbuttonSprite->SetPosition(120,222); // play selected world
                nbuttonSprite->Draw();

                nbuttonSmallSprite->SetPosition(67.75,255); // rename
                nbuttonSmallSprite->Draw();

                nbuttonSmallSprite->SetPosition(172.25,255); // delete
                nbuttonSmallSprite->Draw();
            }

            ///right part
            buttonSprite->SetPosition(360,222); // create new world
            buttonSprite->Draw();

            buttonSprite->SetPosition(360,255); // cancel
            buttonSprite->Draw();
        }


        bool smallButton = false;
        if(saveSubmenu)
        {
            sbuttonSprite->SetPosition(240,(saveSubMenuSelect * 25) + 210);
        }
        else
        {
            switch(loadSelectPos)
            {
                case 0:
                sbuttonSprite->SetPosition(120,222);
                break;
                case 1:
                sbuttonSmallSprite->SetPosition(67.75,255);
                smallButton = true;
                break;
                case 2:
                sbuttonSmallSprite->SetPosition(172.25,255);
                smallButton = true;
                break;
                case 3:
                sbuttonSprite->SetPosition(360,222);
                break;
                case 4:
                sbuttonSprite->SetPosition(360,255);
                break;
            }
        }
        smallButton == true ? sbuttonSmallSprite->Draw() : sbuttonSprite->Draw();

        blackBackground->Draw();
		
		if(saveFilesList.size() > 0)
		{
			for(int i = loadSaveStart; i <loadSaveMax; i++)
			{
				smenuSprite->SetPosition(46,56 + (i * 41) - (loadSaveStart * 41));//menu
				smenuSprite->Draw();
			}
		}

        //select sprite
        if(saveFilesList.size() > 0)
        {
            //save files
            for(int i = loadSaveStart; i <loadSaveMax; i++)
            {
                if(loadSavePos == i)
                {
                    mRender->SetFont(ENGLISH);
					
                    if(saveFilesList[i].worldName[0] != '\0')
                    {
                        mRender->SetFontStyle(0.6f,GU_COLOR(1,1,0,1),0,0x00000000);
                        mRender->DebugPrint(70,54 + (i * 41) - (loadSaveStart * 41),"%s",saveFilesList[i].worldName);
						
						menuSprite->SetPosition(46,56 + (i * 41) - (loadSaveStart * 41));//menu
						menuSprite->Draw();
                    }
                    else
                    {
                        mRender->SetFontStyle(0.6f,GU_COLOR(0.6,0.6,0,1),0,0x00000000);
                        mRender->DebugPrint(70,54 + (i * 41) - (loadSaveStart * 41),"<no name>");
                    }

                    mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0,1),0,0x00000000);
                    mRender->DebugPrint(70,66 + (i * 41) - (loadSaveStart * 41),"%s",saveFilesList[i].fileName.c_str());

                    mRender->SetDefaultFont();

                    if(mRender->GetFontLanguage() == ENGLISH)
                    {
                        switch(saveFilesList[i].worldGameMode)
                        {
                            case 0:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0,1),0,0x00000000);
                            mRender->DebugPrint(70,78 + (i * 41) - (loadSaveStart * 41),"Survival mode (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                            case 1:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0,1),0,0x00000000);
                            mRender->DebugPrint(70,78 + (i * 41) - (loadSaveStart * 41),"Creative mode (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                            case 2:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0,1),0,0x00000000);
                            mRender->DebugPrint(70,78 + (i * 41) - (loadSaveStart * 41),"Hardcore mode (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                        }
                    }
                    if(mRender->GetFontLanguage() == RUSSIAN)
                    {
                        switch(saveFilesList[i].worldGameMode)
                        {
                            case 0:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0,1),0,0x00000000);
                            mRender->DebugPrint(70,78 + (i * 41) - (loadSaveStart * 41),"Modo survival (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                            case 1:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0,1),0,0x00000000);
                            mRender->DebugPrint(70,78 + (i * 41) - (loadSaveStart * 41),"Modo creativo (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                            case 2:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0,1),0,0x00000000);
                            mRender->DebugPrint(70,78 + (i * 41) - (loadSaveStart * 41),"Modo hardcore (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                        }
                    }
                }
                else
                {
                    mRender->SetFont(ENGLISH);
					
                    if(saveFilesList[i].worldName[0] != '\0')
						
                    {
                        mRender->SetFontStyle(0.6f,GU_COLOR(1,1,1,1),0,0x00000000);
                        mRender->DebugPrint(70,54 + (i * 41) - (loadSaveStart * 41),"%s",saveFilesList[i].worldName);
						
                    }
                    else
                    {
                        mRender->SetFontStyle(0.6f,GU_COLOR(0.6,0.6,0.6,1),0,0x00000000);
                        mRender->DebugPrint(70,54 + (i * 41) - (loadSaveStart * 41),"<no name>");
                    }

                    mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0.5,1),0,0x00000000);
                    mRender->DebugPrint(70,66 + (i * 41) - (loadSaveStart * 41),"%s",saveFilesList[i].fileName.c_str());

                    mRender->SetDefaultFont();

                    if(mRender->GetFontLanguage() == ENGLISH)
                    {
                        switch(saveFilesList[i].worldGameMode)
                        {
                            case 0:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0.5,1),0,0x00000000);
                            mRender->DebugPrint(70,78 + (i * 41) - (loadSaveStart * 41),"Survival mode (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                            case 1:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0.5,1),0,0x00000000);
                            mRender->DebugPrint(70,78 + (i * 41) - (loadSaveStart * 41),"Creative mode (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                            case 2:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0.5,1),0,0x00000000);
                            mRender->DebugPrint(70,78  + (i * 41) - (loadSaveStart * 41),"Hardcore mode (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                        }
                    }

                    if(mRender->GetFontLanguage() == RUSSIAN)
                    {
                        switch(saveFilesList[i].worldGameMode)
                        {
                            case 0:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0.5,1),0,0x00000000);
                            mRender->DebugPrint(70,78 + (i * 41) - (loadSaveStart * 41),"Modo survival (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                            case 1:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0.5,1),0,0x00000000);
                            mRender->DebugPrint(70,78 + (i * 41) - (loadSaveStart * 41),"Modo creativo (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                            case 2:
                            mRender->SetFontStyle(0.4f,GU_COLOR(0.5,0.5,0.5,1),0,0x00000000);
                            mRender->DebugPrint(70,78  + (i * 41) - (loadSaveStart * 41),"Modo hardcore (%i KB)",saveFilesList[i].saveSize/1024);
                            break;
                        }
                    }
                }
            }
        }

        sceGuDisable(GU_BLEND);
        sceGuEnable(GU_DEPTH_TEST);

        if(mRender->GetFontLanguage() == ENGLISH)
        {
            if(saveSubmenu)
            {
                saveSubMenuSelect == 0 ? DrawText(240,219,GU_COLOR(1,1,0.25,1) ,default_size,"Are you sure?") : DrawText(240,219,GU_COLOR(1,1,1,1) ,default_size,"Are you sure?");
                saveSubMenuSelect == 1 ? DrawText(240,244,GU_COLOR(1,1,0.25,1) ,default_size,"Yes") : DrawText(240,244,GU_COLOR(1,1,1,1) ,default_size,"Yes");
                saveSubMenuSelect == 2 ? DrawText(240,269,GU_COLOR(1,1,0.25,1) ,default_size,"No") : DrawText(240,269,GU_COLOR(1,1,1,1) ,default_size,"No");
            }
            else
            {
                float buttonTextColor = 1.0f; // for left part
                if(saveFilesList.empty() == true)
                {
                    buttonTextColor = 0.5f;
                }

                loadSelectPos == 0 ? DrawText(120,231,GU_COLOR(1,1,0.25,1),default_size,"Play Selected World") : DrawText(120,231,GU_COLOR(buttonTextColor,buttonTextColor,buttonTextColor,1),default_size,"Play Selected World");
                loadSelectPos == 1 ? DrawText(67.75f,264,GU_COLOR(1,1,0.25,1) ,default_size,"Rename") : DrawText(67.75f,264,GU_COLOR(buttonTextColor,buttonTextColor,buttonTextColor,1) ,default_size,"Rename");
                loadSelectPos == 2 ? DrawText(172.25f,264,GU_COLOR(1,1,0.25,1) ,default_size,"Delete") : DrawText(172.25f,264,GU_COLOR(buttonTextColor,buttonTextColor,buttonTextColor,1) ,default_size,"Delete");
                loadSelectPos == 3 ? DrawText(360,231,GU_COLOR(1,1,0.25,1) ,default_size,"Create New World") : DrawText(360,231,GU_COLOR(1.0f,1.0f,1.0f,1) ,default_size,"Create New World");
                loadSelectPos == 4 ? DrawText(360,264,GU_COLOR(1,1,0.25,1) ,default_size,"Cancel") : DrawText(360,264,GU_COLOR(1.0f,1.0f,1.0f,1) ,default_size,"Cancel");
            }
            DrawText(240,24,GU_COLOR(1,1,1,1) ,default_size,"Select World");
        }

        if(mRender->GetFontLanguage() == RUSSIAN)
        {
            if(saveSubmenu)
            {
                saveSubMenuSelect == 0 ? DrawText(240,219,GU_COLOR(1,1,0.25,1) ,default_size,"Estas seguro?") : DrawText(240,219,GU_COLOR(1,1,1,1) ,default_size,"Estas seguro?");
                saveSubMenuSelect == 1 ? DrawText(240,244,GU_COLOR(1,1,0.25,1) ,default_size,"Si") : DrawText(240,244,GU_COLOR(1,1,1,1) ,default_size,"Si");
                saveSubMenuSelect == 2 ? DrawText(240,269,GU_COLOR(1,1,0.25,1) ,default_size,"No") : DrawText(240,269,GU_COLOR(1,1,1,1) ,default_size,"No");
            }
            else
            {
                float buttonTextColor = 1.0f; // for left part
                if(saveFilesList.empty() == true)
                {
                    buttonTextColor = 0.5f;
                }

                loadSelectPos == 0 ? DrawText(120,231,GU_COLOR(1,1,0.25,1),default_size,"Jugar Mundo") : DrawText(120,231,GU_COLOR(buttonTextColor,buttonTextColor,buttonTextColor,1),default_size,"Jugar Mundo");
                loadSelectPos == 1 ? DrawText(67.75f,264,GU_COLOR(1,1,0.25,1) ,default_size,"Renombrar") : DrawText(67.75f,264,GU_COLOR(buttonTextColor,buttonTextColor,buttonTextColor,1) ,default_size,"Renombrar");
                loadSelectPos == 2 ? DrawText(172.25f,264,GU_COLOR(1,1,0.25,1) ,default_size,"Borrar") : DrawText(172.25f,264,GU_COLOR(buttonTextColor,buttonTextColor,buttonTextColor,1) ,default_size,"Borrar");
                loadSelectPos == 3 ? DrawText(360,231,GU_COLOR(1,1,0.25,1) ,default_size,"Crear nuevo mundo") : DrawText(360,231,GU_COLOR(1.0f,1.0f,1.0f,1) ,default_size,"Crear nuevo mundo");
                loadSelectPos == 4 ? DrawText(360,264,GU_COLOR(1,1,0.25,1) ,default_size,"Regresar") : DrawText(360,264,GU_COLOR(1.0f,1.0f,1.0f,1) ,default_size,"Regresar");
            }
            DrawText(240,24,GU_COLOR(1,1,1,1) ,default_size,"Seleccionar Mundo");
        }
    }
    break;
    case 3://about
    {


        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        for(int x = 0; x < 8; x++)
        {
            for(int y = 0; y < 5; y++)
            {
                backSprite->SetPosition(x*64,y*64);
                backSprite->Draw();
            }
        }

        //check for update
        buttonSprite->SetPosition(240,225);
        buttonSprite->Draw();

        buttonSprite->SetPosition(240,255);
        buttonSprite->Draw();

        //back
        sbuttonSprite->SetPosition(240,(aboutPos * 30) + 225);
        sbuttonSprite->Draw();

        blackBackground->Draw();

        sceGuDisable(GU_BLEND);
        sceGuEnable(GU_DEPTH_TEST);

        if(mRender->GetFontLanguage() == ENGLISH)
        {
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,100-40,"Developer  :");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,100-40,"Wandemberg Armijos(RegenStudio)");

            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,120-40,"Past Modder:");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,120-40,"Kirill Skibin(Woolio)");
			
			mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,140-40,"Created By :");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,140-40,"Marcin Ploska(Drakon)");

            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,180-40,"Website:");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,180-40,"vk.com/mine_psp_game");
			
			mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,200-40,"Donate :");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,200-40,"Paypal.me/Mikegaming210");

            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,220-40,"Version:");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,220-40,"Minecraft PSP BETA");
			
			mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,240-40,"Stage  :");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,240-40,"Under Development");

            aboutPos == 0 ? DrawText(240,234,GU_COLOR(1,1,0.25,1) ,default_size,"Converter") : DrawText(240,234,GU_COLOR(1,1,1,1) ,default_size,"Converter");
            aboutPos == 1 ? DrawText(240,264,GU_COLOR(1,1,0.25,1) ,default_size,"Cancel") : DrawText(240,264,GU_COLOR(1,1,1,1) ,default_size,"Cancel");
            DrawText(240,19,GU_COLOR(1,1,1,1) ,default_size,"About");
        }
        if(mRender->GetFontLanguage() == RUSSIAN)
        {
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,100-40,"Desarrollador:");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,100-40,"Wandemberg Armijos(RegenStudio)");

            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,120-40,"Past Modder  :");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,120-40,"Kirill Skibin(Woolio)");
			
			mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,140-40,"Creador por  :");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,140-40,"Marcin Ploska(Drakon)");

            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,180-40,"SitioWeb:");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,180-40,"vk.com/mine_psp_game");
			
			mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,200-40,"Donacion:");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,200-40,"Paypal.me/Mikegaming210");

            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,220-40,"Version :");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,220-40,"Minecraft PSP v3.3.0");
			
			mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000000);
            mRender->DebugPrint(40,240-40,"Estado  :");
            mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),2,0x00000400);
            mRender->DebugPrint(440,240-40,"Bajo Desarrollo");

            aboutPos == 0 ? DrawText(240,234,GU_COLOR(1,1,0.25,1) ,default_size,"Convertidor") : DrawText(240,234,GU_COLOR(1,1,1,1) ,default_size,"Convertidor");
            aboutPos == 1 ? DrawText(240,264,GU_COLOR(1,1,0.25,1) ,default_size,"Regresar") : DrawText(240,264,GU_COLOR(1,1,1,1) ,default_size,"Regresar");
            DrawText(240,19,GU_COLOR(1,1,1,1) ,default_size,"Sobre");
        }
    }
    break;

    case 4://update info
    {


        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        for(int x = 0; x < 8; x++)
        {
            for(int y = 0; y < 5; y++)
            {
                backSprite->SetPosition(x*64,y*64);
                backSprite->Draw();
            }
        }

        //check for update
        buttonSprite->SetPosition(240,225);
        buttonSprite->Draw();

        buttonSprite->SetPosition(240,255);
        buttonSprite->Draw();

        //back
        sbuttonSprite->SetPosition(240,(donatePos * 30) + 225);
        sbuttonSprite->Draw();

        blackBackground->Draw();

        sceGuDisable(GU_BLEND);
        sceGuEnable(GU_DEPTH_TEST);

        if(mRender->GetFontLanguage() == ENGLISH)
        {
            mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,80-40,"-Clouds 3D");

            mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,90-40,"-Fixed some bugs");

            mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,100-40,"-Fixed world generation");

            mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,110-40,"-Legacy Inventory");

            mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,120-40,"-Spanish lenguage updated");
			
			mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,130-40,"-Fix XP number system");
			
			mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,140-40,"-New font");

            donatePos == 0 ? DrawText(240,234,GU_COLOR(1,1,0.25,1) ,default_size,"Nothing Here") : DrawText(240,234,GU_COLOR(1,1,1,1) ,default_size,"Nothing Here");
            donatePos == 1 ? DrawText(240,264,GU_COLOR(1,1,0.25,1) ,default_size,"Cancel") : DrawText(240,264,GU_COLOR(1,1,1,1) ,default_size,"Cancel");
            DrawText(240,20,GU_COLOR(1,1,1,1) ,default_size,"Whats new?!");
        }
        if(mRender->GetFontLanguage() == RUSSIAN)
        {
            mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,80-40,"-Clouds 3D");

            mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,90-40,"-Fixed some bugs");

            mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,100-40,"-Fixed world generation");

            mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,110-40,"-Legacy Inventory");

            mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,120-40,"-Spanish lenguage updated");
			
			mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,130-40,"-Fix XP number system");
			
			mRender->SetFontStyle(0.36,GU_COLOR(1,1,1,1),1,0x00000000);
            mRender->DebugPrint(30,140-40,"-New font");

            donatePos == 0 ? DrawText(240,234,GU_COLOR(1,1,0.25,1) ,default_size,"Como jugar") : DrawText(240,234,GU_COLOR(1,1,1,1) ,default_size,"Como jugar");
            donatePos == 1 ? DrawText(240,264,GU_COLOR(1,1,0.25,1) ,default_size,"Regresar") : DrawText(240,264,GU_COLOR(1,1,1,1) ,default_size,"Regresar");
            DrawText(240,20,GU_COLOR(1,1,1,1) ,default_size,"Novedades!");
        }
    }
    break;

    case 5://paramateric view
    {


        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        for(int x = 0; x < 8; x++)
        {
            for(int y = 0; y < 5; y++)
            {
                backSprite->SetPosition(x*64,y*64);
                backSprite->Draw();
            }
        }


        //name
        nbuttonSprite->SetPosition(120,70);
        nbuttonSprite->Draw();
        //seed
        nbuttonSprite->SetPosition(360,70);
        nbuttonSprite->Draw();
		
		//estructuras
        buttonSprite->SetPosition(120,130);//+60
        buttonSprite->Draw();
		//cofre bonus
        buttonSprite->SetPosition(360,130);
        buttonSprite->Draw();
		//game mode
        buttonSprite->SetPosition(120,160);//+30
        buttonSprite->Draw();
		//tipo de mundo
        buttonSprite->SetPosition(360,160);
        buttonSprite->Draw();

		//crear mundo
        buttonSprite->SetPosition(120,230);
        buttonSprite->Draw();
		//salir
        buttonSprite->SetPosition(360,230);
        buttonSprite->Draw();
		
		switch(generateSelectPose)
		{
			case 0:
			smbuttonSprite->SetPosition(120,70);
            smbuttonSprite->Draw();
			break;
			case 1:
			smbuttonSprite->SetPosition(360,70);
            smbuttonSprite->Draw();
			break;
			case 2:
			sbuttonSprite->SetPosition(120,130);
            sbuttonSprite->Draw();
			break;
			case 3:
			sbuttonSprite->SetPosition(360,130);
            sbuttonSprite->Draw();
			break;
			case 4:
			sbuttonSprite->SetPosition(120,160);
            sbuttonSprite->Draw();
			break;
			case 5:
			sbuttonSprite->SetPosition(360,160);
            sbuttonSprite->Draw();
			break;
			case 6:
			sbuttonSprite->SetPosition(120,230);
            sbuttonSprite->Draw();
			break;
			case 7:
			sbuttonSprite->SetPosition(360,230);
            sbuttonSprite->Draw();
			break;
		}

        sceGuDisable(GU_BLEND);
        sceGuEnable(GU_DEPTH_TEST);

        if(mRender->GetFontLanguage() == ENGLISH)
        {
            DrawText(240,29,GU_COLOR(1,1,1,1) ,default_size,"Create New World");
            DrawText(120,50,GU_COLOR(0.25,0.25,0.25,1) ,default_size,"Name");
            DrawText(360,50,GU_COLOR(0.25,0.25,0.25,1) ,default_size,"Seed");

            //draw subtitles on buttons
            if(gameMode == SURVIVAL)
            {
                generateSelectPose == 4 ? DrawText(120,169,GU_COLOR(1,1,0.25,1) ,default_size,"Game Mode: Survival") : DrawText(120,169,GU_COLOR(1,1,1,1) ,default_size,"Game Mode: Survival");
            }
            if(gameMode == CREATIVE)
            {
                generateSelectPose == 4 ? DrawText(120,169,GU_COLOR(1,1,0.25,1) ,default_size,"Game Mode: Creative") : DrawText(120,169,GU_COLOR(1,1,1,1) ,default_size,"Game Mode: Creative");
            }
            if(gameMode == HARDCORE)
            {
                generateSelectPose == 4 ? DrawText(120,169,GU_COLOR(1,1,0.25,1) ,default_size,"Game Mode: Hardcore") : DrawText(120,169,GU_COLOR(1,1,1,1) ,default_size,"Game Mode: Hardcore");
            }
			
			if(makeDungeons)
			{
				generateSelectPose == 2 ? DrawText(120,139,GU_COLOR(1,1,0.25,1) ,default_size,"Structures: ON") : DrawText(120,139,GU_COLOR(1,1,1,1) ,default_size,"Structures: ON");
			}
			else
			{
				generateSelectPose == 2 ? DrawText(120,139,GU_COLOR(1,1,0.25,1) ,default_size,"Structures: OFF") : DrawText(120,139,GU_COLOR(1,1,1,1) ,default_size,"Structures: OFF");
			}
			
			if(makeBonus)
			{
				generateSelectPose == 3 ? DrawText(360,139,GU_COLOR(1,1,0.25,1) ,default_size,"Bonus Chest: ON") : DrawText(360,139,GU_COLOR(1,1,1,1) ,default_size,"Bonus Chest: ON");
			}
			else
			{
				generateSelectPose == 3 ? DrawText(360,139,GU_COLOR(1,1,0.25,1) ,default_size,"Bonus Chest: OFF") : DrawText(360,139,GU_COLOR(1,1,1,1) ,default_size,"Bonus Chest: OFF");
			}

            if(worldType == 0)
            {
                generateSelectPose == 5 ? DrawText(360,169,GU_COLOR(1,1,0.25,1) ,default_size,"World Type: Default") : DrawText(360,169,GU_COLOR(1,1,1,1) ,default_size,"World Type: Default");
            }
            if(worldType == 1)
            {
                generateSelectPose == 5 ? DrawText(360,169,GU_COLOR(1,1,0.25,1) ,default_size,"World Type: Flat") : DrawText(360,169,GU_COLOR(1,1,1,1) ,default_size,"World Type: Flat");
            }

            generateSelectPose == 6 ? DrawText(120,239,GU_COLOR(1,1,0.25,1) ,default_size,"Create New World") : DrawText(120,239,GU_COLOR(1,1,1,1) ,default_size,"Create New World");
            generateSelectPose == 7 ? DrawText(360,239,GU_COLOR(1,1,0.25,1) ,default_size,"Cancel") : DrawText(360,239,GU_COLOR(1,1,1,1) ,default_size,"Cancel");
        }
        if(mRender->GetFontLanguage() == RUSSIAN)
        {
			DrawText(240,29,GU_COLOR(1,1,1,1) ,default_size,"Crear nuevo mundo");
            DrawText(120,50,GU_COLOR(0.25,0.25,0.25,1) ,default_size,"Nombre");
            DrawText(360,50,GU_COLOR(0.25,0.25,0.25,1) ,default_size,"Semilla");

            //draw subtitles on buttons
            if(gameMode == SURVIVAL)
            {
                generateSelectPose == 4 ? DrawText(120,169,GU_COLOR(1,1,0.25,1) ,default_size,"Game Mode: Survival") : DrawText(120,169,GU_COLOR(1,1,1,1) ,default_size,"Game Mode: Survival");
            }
            if(gameMode == CREATIVE)
            {
                generateSelectPose == 4 ? DrawText(120,169,GU_COLOR(1,1,0.25,1) ,default_size,"Game Mode: Creativo") : DrawText(120,169,GU_COLOR(1,1,1,1) ,default_size,"Game Mode: Creativo");
            }
            if(gameMode == HARDCORE)
            {
                generateSelectPose == 4 ? DrawText(120,169,GU_COLOR(1,1,0.25,1) ,default_size,"Game Mode: Hardcore") : DrawText(120,169,GU_COLOR(1,1,1,1) ,default_size,"Game Mode: Hardcore");
            }

            if(worldType == 0)
            {
                generateSelectPose == 5 ? DrawText(360,169,GU_COLOR(1,1,0.25,1) ,default_size,"World Type: Default") : DrawText(360,169,GU_COLOR(1,1,1,1) ,default_size,"World Type: Default");
            }
            if(worldType == 1)
            {
                generateSelectPose == 5 ? DrawText(360,169,GU_COLOR(1,1,0.25,1) ,default_size,"World Type: Plano") : DrawText(360,169,GU_COLOR(1,1,1,1) ,default_size,"World Type: Plano");
            }
			
			if(makeDungeons)
			{
				generateSelectPose == 2 ? DrawText(120,139,GU_COLOR(1,1,0.25,1) ,default_size,"Estructuras: ON") : DrawText(120,139,GU_COLOR(1,1,1,1) ,default_size,"Estructuras: ON");
			}
			else
			{
				generateSelectPose == 2 ? DrawText(120,139,GU_COLOR(1,1,0.25,1) ,default_size,"Estructuras: OFF") : DrawText(120,139,GU_COLOR(1,1,1,1) ,default_size,"Estructuras: OFF");
			}
			
			if(makeBonus)
			{
				generateSelectPose == 3 ? DrawText(360,139,GU_COLOR(1,1,0.25,1) ,default_size,"Cofre Bonus: ON") : DrawText(360,139,GU_COLOR(1,1,1,1) ,default_size,"Cofre Bonus: ON");
			}
			else
			{
				generateSelectPose == 3 ? DrawText(360,139,GU_COLOR(1,1,0.25,1) ,default_size,"Cofre Bonus: OFF") : DrawText(360,139,GU_COLOR(1,1,1,1) ,default_size,"Cofre Bonus: OFF");
			}

            generateSelectPose == 6 ? DrawText(120,239,GU_COLOR(1,1,0.25,1) ,default_size,"Crear nuevo mundo") : DrawText(120,239,GU_COLOR(1,1,1,1) ,default_size,"Crear nuevo mundo");
            generateSelectPose == 7 ? DrawText(360,239,GU_COLOR(1,1,0.25,1) ,default_size,"Cancelar") : DrawText(360,239,GU_COLOR(1,1,1,1) ,default_size,"Cancelar");
        }

        mRender->SetFont(ENGLISH);
        mRender->SetFontStyle(default_size ,GU_COLOR(1,1,1,1),0,0x00000000|0x00004000);
        mRender->DebugPrint(39,79,"%s",newWorldName.c_str());
        if(seed_1 == 0)
        {
            mRender->SetFontStyle(default_size ,GU_COLOR(0.65,0.65,0.65,1),999,0x00000200|0x00004000);
            if(mRender->GetFontLanguage() == ENGLISH)
            {
                mRender->DebugPrint(360,79,"random");
            }
            if(mRender->GetFontLanguage() == RUSSIAN)
            {
                mRender->DebugPrint(360,79,"aleatorio");
            }
        }
        else
        {
			mRender->SetFontStyle(default_size ,GU_COLOR(1,1,1,1),0,0x00000200|0x00004000);
            mRender->DebugPrint(360,79,"%s",newWorldSeed.c_str());
        }
        mRender->SetDefaultFont();
    }
    break;
    case 6://about
    {


        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        for(int x = 0; x < 8; x++)
        {
            for(int y = 0; y < 5; y++)
            {
                backSprite->SetPosition(x*64,y*64);
                backSprite->Draw();
            }
        }

        //check for update
		buttonSprite->SetPosition(240,130);
        buttonSprite->Draw();
		
        buttonSprite->SetPosition(240,165);
        buttonSprite->Draw();

        buttonSprite->SetPosition(240,200);
        buttonSprite->Draw();

        buttonSprite->SetPosition(240,245);
        buttonSprite->Draw();

        //back	//cuevas
        int y_pos = 130;
        if(converterPos == 1)
        {
            y_pos = 165;
        }
        if(converterPos == 2)
        {
            y_pos = 200;
        }
		if(converterPos == 3)
        {
            y_pos = 245;
        }
        sbuttonSprite->SetPosition(240,y_pos);
        sbuttonSprite->Draw();

        sceGuDisable(GU_BLEND);
        sceGuEnable(GU_DEPTH_TEST);

        if(mRender->GetFontLanguage() == ENGLISH)
        {
            if(schematicExists)
            {
                DrawText(240,60,GU_COLOR(0.1,0.9,0.1,1),default_size,"world.schematic exists!");
            }
            else
            {
                DrawText(240,60,GU_COLOR(0.9,0.1,0.1,1),default_size,"world.schematic doesn't exist!");
            }
            if(errorType == 1)
            {
                DrawText(240,80,GU_COLOR(0.9,0.1,0.1,1),default_size,"invalid schematic size");
            }
            else
            {
                DrawText(240,80,GU_COLOR(0.9,0.1,0.1,1),default_size,"process of conversion can take about 1 minute");
            }
			
			if(newW_deleteCaves == true)
            {
				DrawText(240,114,GU_COLOR(0.5,0.5,0.5,1),0.35,"recommended for optimizing normal maps");
                converterPos == 0 ? DrawText(240,139,GU_COLOR(1,1,0.25,1) ,default_size,"Clean caves: Enabled") : DrawText(240,139,GU_COLOR(1,1,1,1) ,default_size,"Clean caves: Enabled");
            }
            if(newW_deleteCaves == false)
            {
				DrawText(240,114,GU_COLOR(0.5,0.5,0.5,1),0.35,"recommended for competitive maps [skywars, uhc, etc]");
                converterPos == 0 ? DrawText(240,139,GU_COLOR(1,1,0.25,1) ,default_size,"Clean caves: Disable") : DrawText(240,139,GU_COLOR(1,1,1,1) ,default_size,"Clean caves: Disable");
            }

            if(newW_gameMode == SURVIVAL)
            {
                converterPos == 1 ? DrawText(240,174,GU_COLOR(1,1,0.25,1) ,default_size,"Game Mode: Survival") : DrawText(240,174,GU_COLOR(1,1,1,1) ,default_size,"Game Mode: Survival");
            }
            if(newW_gameMode == CREATIVE)
            {
                converterPos == 1 ? DrawText(240,174,GU_COLOR(1,1,0.25,1) ,default_size,"Game Mode: Creative") : DrawText(240,174,GU_COLOR(1,1,1,1) ,default_size,"Game Mode: Creative");
            }
            converterPos == 2 ? DrawText(240,209,GU_COLOR(1,1,0.25,1) ,default_size,"Try to convert") : DrawText(240,209,GU_COLOR(1,1,1,1) ,default_size,"Try to convert");
            converterPos == 3 ? DrawText(240,254,GU_COLOR(1,1,0.25,1) ,default_size,"Cancel") : DrawText(240,254,GU_COLOR(1,1,1,1) ,default_size,"Cancel");

            DrawText(240,29,GU_COLOR(1,1,1,1) ,default_size,"Converter");
        }
        if(mRender->GetFontLanguage() == RUSSIAN)
        {
            if(schematicExists)
            {
                DrawText(240,60,GU_COLOR(0.1,0.9,0.1,1),default_size,"world.schematic existe!");
            }
            else
            {
                DrawText(240,60,GU_COLOR(0.9,0.1,0.1,1),default_size,"world.schematic no existe!");
            }
            if(errorType == 1)
            {
                DrawText(240,80,GU_COLOR(0.9,0.1,0.1,1),default_size,"Tamano de schematic no compatible");
            }
            else
            {
                DrawText(240,80,GU_COLOR(0.9,0.1,0.1,1),default_size,"el proceso de conversion puede durar 1 minuto!");
            }
			
			if(newW_deleteCaves == true)
            {
				DrawText(240,114,GU_COLOR(0.5,0.5,0.5,1),0.35,"recomendado para optimizar mapas normales");
                converterPos == 0 ? DrawText(240,139,GU_COLOR(1,1,0.25,1) ,default_size,"Cuevas: Enabled") : DrawText(240,139,GU_COLOR(1,1,1,1) ,default_size,"Cuevas: Enabled");
            }
            if(newW_deleteCaves == false)
            {
				DrawText(240,114,GU_COLOR(0.5,0.5,0.5,1),0.35,"recomendado para mapas competitivos [skywars, uhc, etc]");
                converterPos == 0 ? DrawText(240,139,GU_COLOR(1,1,0.25,1) ,default_size,"Cuevas: Disable") : DrawText(240,139,GU_COLOR(1,1,1,1) ,default_size,"Cuevas: Disable");
            }

            if(newW_gameMode == SURVIVAL)
            {
                converterPos == 1 ? DrawText(240,174,GU_COLOR(1,1,0.25,1) ,default_size,"Game Mode: Survival") : DrawText(240,174,GU_COLOR(1,1,1,1) ,default_size,"Game Mode: Survival");
            }
            if(newW_gameMode == CREATIVE)
            {
                converterPos == 1 ? DrawText(240,174,GU_COLOR(1,1,0.25,1) ,default_size,"Game Mode: Creativo") : DrawText(240,174,GU_COLOR(1,1,1,1) ,default_size,"Game Mode: Creativo");
            }
            converterPos == 2 ? DrawText(240,209,GU_COLOR(1,1,0.25,1) ,default_size,"Convertir") : DrawText(240,209,GU_COLOR(1,1,1,1) ,default_size,"Convertir");
            converterPos == 3 ? DrawText(240,254,GU_COLOR(1,1,0.25,1) ,default_size,"Regresar") : DrawText(240,254,GU_COLOR(1,1,1,1) ,default_size,"Regresar");

            DrawText(240,29,GU_COLOR(1,1,1,1) ,default_size,"Convertidor");
        }
    }
    break;
	case 7://player options
	{
		sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        for(int x = 0; x < 8; x++)
        {
            for(int y = 0; y < 5; y++)
            {
                backSprite->SetPosition(x*64,y*64);
                backSprite->Draw();
            }
        }
		
		//sprites aqui
		
		sceGuDisable(GU_BLEND);
        sceGuEnable(GU_DEPTH_TEST);

        DrawText(240,24,GU_COLOR(1,1,1,1) ,default_size,"Skin selector");
	}
	break;
    case 10://New or load map
    {
        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        for(int x = 0; x < 8; x++)
        {
            for(int y = 0; y < 5; y++)
            {
                backSprite->SetPosition(x*64,y*64);
                backSprite->Draw();
            }
        }

        buttonSprite->SetPosition(240,100);
        buttonSprite->Draw();

        buttonSprite->SetPosition(240,140);
        buttonSprite->Draw();

        sbuttonSprite->SetPosition(240,100+saveSubMenuSelect*40);
        sbuttonSprite->Draw();

        sceGuDisable(GU_BLEND);
        sceGuEnable(GU_DEPTH_TEST);

        if(mRender->GetFontLanguage() == ENGLISH)
        {
            DrawText(240,64,GU_COLOR(1,1,1,1) ,default_size,"Choose Game Mode");

            saveSubMenuSelect == 0 ? DrawText(240,109,GU_COLOR(1,1,0.25,1) ,default_size,"Survival") : DrawText(240,109,GU_COLOR(1,1,1,1) ,default_size,"Survival");
            saveSubMenuSelect == 1 ? DrawText(240,149,GU_COLOR(1,1,0.25,1) ,default_size,"Creative") : DrawText(240,149,GU_COLOR(1,1,1,1) ,default_size,"Creative");
        }
        if(mRender->GetFontLanguage() == RUSSIAN)
        {
            DrawText(240,64,GU_COLOR(1,1,1,1) ,default_size,"Escojer modo de juego");

            saveSubMenuSelect == 0 ? DrawText(240,109,GU_COLOR(1,1,0.25,1) ,default_size,"Survival") : DrawText(240,109,GU_COLOR(1,1,1,1) ,default_size,"Survival");
            saveSubMenuSelect == 1 ? DrawText(240,149,GU_COLOR(1,1,0.25,1) ,default_size,"Creativo") : DrawText(240,149,GU_COLOR(1,1,1,1) ,default_size,"Creativo");
        }
    }
    break;
    case 11://textures
    {
        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        for(int x = 0; x < 8; x++)
        {
            for(int y = 0; y < 5; y++)
            {
                backSprite->SetPosition(x*64,y*64);
                backSprite->Draw();
            }
        }

        //select sprite
        if(texturePackList.size() > 0)
        {

        }

        for(int i = tpStart; i < tpMax; i++)
        {
            if(i < texturePackList.size())
            {
                sceGuEnable(GU_BLEND);

                rectFilledSprite->SetPosition(240,56 - 8 + (tpCurrent * 72) - (tpStart * 72));

                if(i == tpCurrent)
                {
                    rectFilledSprite->Draw();
                }

                rectEmptySprite->SetPosition(240,56 - 8 + (tpPos * 72) - (tpStart * 72));
                if(i == tpPos)
                {
                    sceGuBlendFunc(GU_ADD, GU_FIX,GU_FIX, 0xFFFFFFFF, 0xFFFFFFFF);
                    rectEmptySprite->Draw();
                    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
                }

                Sprite* DrawSprite = texturePackList[i].packSprite;
                DrawSprite->SetPosition(46,56 - 8 + (i * 72) - (tpStart * 72));
                DrawSprite->ConstDraw();

                sceGuDisable(GU_BLEND);

                mRender->SetFont(ENGLISH);
                if(i == tpPos)
                {
                    mRender->SetFontStyle(default_big_size,GU_COLOR(0.25,0.25,0,1),0,0x00000000);
                    mRender->DebugPrint(94,52 - 8 + (i * 72) - (tpStart * 72),"%s",texturePackList[i].name.c_str());

                    mRender->SetFontStyle(default_big_size,GU_COLOR(1,1,0,1),0,0x00000000);
                    mRender->DebugPrint(92,50 - 8 + (i * 72) - (tpStart * 72),"%s",texturePackList[i].name.c_str());

                    mRender->SetFontStyle(default_size,GU_COLOR(0.5,0.5,0,1),0,0x00000000);
                    mRender->DebugPrint(92,50 - 8 + (i * 72) - (tpStart * 72) + 18,"%s",texturePackList[i].description.c_str());
                }
                else
                {
                    mRender->SetFontStyle(default_big_size,GU_COLOR(0.25,0.25,0.25,1),0,0x00000000);
                    mRender->DebugPrint(94,52 - 8 + (i * 72) - (tpStart * 72),"%s",texturePackList[i].name.c_str());

                    mRender->SetFontStyle(default_big_size,GU_COLOR(1,1,1,1),0,0x00000000);
                    mRender->DebugPrint(92,50 - 8 + (i * 72) - (tpStart * 72),"%s",texturePackList[i].name.c_str());

                    mRender->SetFontStyle(default_size,GU_COLOR(0.5,0.5,0.5,1),0,0x00000000);
                    mRender->DebugPrint(92,50 - 8 + (i * 72) - (tpStart * 72) + 18,"%s",texturePackList[i].description.c_str());
                }
                mRender->SetDefaultFont();
            }
        }



        buttonSprite->SetPosition(120,254);
        buttonSprite->Draw();

        buttonSprite->SetPosition(360,254);
        buttonSprite->Draw();


        sbuttonSprite->SetPosition(120+(tpSelectPos * 240),254);
        sbuttonSprite->Draw();

        sceGuDisable(GU_BLEND);
        sceGuEnable(GU_DEPTH_TEST);

        if(mRender->GetFontLanguage() == ENGLISH)
        {
            tpSelectPos == 0 ? DrawText(120,263,GU_COLOR(1,1,0.25,1),default_size,"Select") : DrawText(120,263,GU_COLOR(1,1,1,1),default_size,"Select");
            tpSelectPos == 1 ? DrawText(360,263,GU_COLOR(1,1,0.25,1) ,default_size,"Cancel") : DrawText(360,263,GU_COLOR(1,1,1,1) ,default_size,"Cancel");
        }
        if(mRender->GetFontLanguage() == RUSSIAN)
        {
            tpSelectPos == 0 ? DrawText(120,263,GU_COLOR(1,1,0.25,1),default_size,"Seleccionar") : DrawText(120,263,GU_COLOR(1,1,1,1),default_size,"Seleccionar");
            tpSelectPos == 1 ? DrawText(360,263,GU_COLOR(1,1,0.25,1) ,default_size,"Regresar") : DrawText(360,263,GU_COLOR(1,1,1,1) ,default_size,"Regresar");
        }
    }
    break;
    }

    //mRender->SetFontStyle(0.5f,GU_COLOR(1,1,1,1),0,0x00000000);
    //mRender->DebugPrint(30,50,"%f",size_f);

    //draw debug text at the end
    /*mRender->DebugPrint(40,30,"cpu: %d%%",mRender->GetCpuUsage());
    mRender->DebugPrint(40,40,"gpu: %d%%",mRender->GetGpuUsage());
    mRender->DebugPrint(40,50,"saves: %d",saveFilesList.size());*/

    //mRender->SetFontStyle(0.5f,0xFFFFFFFF,0xFF000000,0x00000400);
    //mRender->DebugPrint(475,15,"CraftSite.pl Edition");
    //mRender->SetFontStyle(0.5f,0xFFFFFFFF,0xFF000000,0x00000200);

    //end frame
    mRender->EndFrame();
}

//additional functions
void StateMenu::ScanSaveFiles(const char* dirName)
{
    //clear list with save files
    saveFilesList.clear();

    DIR *Dir;
    struct dirent *DirEntry;
    Dir = opendir(dirName);

    while((DirEntry = readdir(Dir)) != NULL)
    {
        if ( DirEntry->d_stat.st_attr & FIO_SO_IFREG) // we found file
        {
            SaveFile newSaveFile;
            std::string plik = dirName;
            plik += DirEntry->d_name;
            //dont load lmsc files
            size_t found = plik.find(".lmsc");
            size_t found2 = plik.find(".LMSc");
            if(found==std::string::npos && found2==std::string::npos)
            {
                newSaveFile.fileName = plik;
                saveFilesList.push_back(newSaveFile);
            }
        }
    }

    closedir(Dir);


    //now update all info in save files
    for(unsigned int i = 0; i < saveFilesList.size(); i++)
    {
        FILE * pFile;
        pFile = fopen(saveFilesList[i].fileName.c_str(),"rb");

        if(pFile != NULL)
        {
            //version
            fread(&saveFilesList[i].saveVersion,sizeof(int),1,pFile);

            fread(&saveFilesList[i].worldGameMode,sizeof(char),1,pFile);

            fread(&saveFilesList[i].locked,sizeof(bool),1,pFile);
            //name
            fread(saveFilesList[i].worldName,sizeof(char),50,pFile);

            if(saveFilesList[i].saveVersion >= 2)
                saveFilesList[i].compression = true;
            else
                saveFilesList[i].compression = false;

            fclose(pFile);
        }

        saveFilesList[i].saveSize = fileSize(saveFilesList[i].fileName+"c");
    }

    //set next save name and numer
    nextSaveFileNumber = saveFilesList.size() + 1;
    {
        //ride thru all saves and check if i can use new name
        char liczba[3];
        sprintf(liczba,"%d",nextSaveFileNumber);
        nextSaveFileName = "Save/world";
        nextSaveFileName += liczba;
        nextSaveFileName += ".lms";

        for(unsigned int i = 0; i < saveFilesList.size(); i++)
        {
            if(nextSaveFileName.compare(saveFilesList[i].fileName) == 0)
            {
                nextSaveFileNumber+=1;
                char liczba[3];
                sprintf(liczba,"%d",nextSaveFileNumber);
                nextSaveFileName = "Save/world";
                nextSaveFileName += liczba;
                nextSaveFileName += ".lms";

               /* if (newWorldName == "World")
                {
                    newWorldName += liczba;
                } */
            }
        }
    }
}

void StateMenu::ScanTexturePacks(const char* dirName)
{
    if(texturePackList.empty() != false)
    {
        TextureHelper::Instance()->RemoveConstTextures();
        for(int j = 0; j < texturePackList.size(); j++)
        {
            if (texturePackList[j].packSprite != NULL)
            {
                delete texturePackList[j].packSprite;
            }
        }
        texturePackList.clear();
    }
    texturePackList.clear();

    TextureManager::Instance()->LoadConstTexture("Assets/unknown_pack.png");

    int TPcheck = 0;

    DIR *dir = opendir(dirName);
    struct dirent *entry;

    while((entry = readdir(dir)) != NULL)
    {
        if (FIO_SO_ISDIR(entry->d_stat.st_attr))
        {
            std::string plik = "";
            plik += entry->d_name;

            size_t found = plik.find(".");
            size_t found2 = plik.find("..");
            size_t found3 = plik.find(".svn");

            if(found==std::string::npos && found2==std::string::npos && found3==std::string::npos)//�� �������
            {
                TP newTP;

                std::string plik2 = plik + "/";
                if(plik2 == TextureHelper::Instance()->defaultFolder)
                {
                    tpCurrent = TPcheck;
                }

                newTP.name = plik;

                texturePackList.push_back(newTP);
                TPcheck++;
            }
        }
    }

    closedir(dir);

    for(int j = 0; j < texturePackList.size(); j++)
    {
        std::string packPath = "Assets/Textures/"+texturePackList[j].name+"/pack.png";

        if(fileExists(packPath) == true) // if we have pack sprite
        {
            TextureManager::Instance()->LoadConstTexture(packPath);
            texturePackList[j].packSprite = new Sprite(TextureManager::Instance()->GetConstTextureNumber(packPath),true);
        }
        else
        {
            texturePackList[j].packSprite = new Sprite(TextureManager::Instance()->GetConstTextureNumber("Assets/unknown_pack.png"),true);
        }

        std::string packDescriptionPath = "Assets/Textures/"+texturePackList[j].name+"/pack.txt";

        if(fileExists(packDescriptionPath) == true) // if we have pack description file
        {
            std::string str;
            std::string file_contents;

            std::ifstream file(packDescriptionPath.c_str());

            if (file.is_open())
            {
                while (std::getline(file, str))
                {
                    file_contents += str;
                    file_contents.push_back('\n');
                }
            }

            file.close();
            texturePackList[j].description = file_contents;
        }
    }
}

/*
    texturePackList.clear();

	FILE *infile;
	char textLine[80];
	char textPack[80];

	infile = fopen("Assets/texturePacks.txt","rt");

	 while(fgets(textLine, 80, infile) != NULL)
	 {
		 sscanf(textLine,"%s",textPack);
		 std::string texturePack = textPack;

		 size_t found = texturePack.find(".tp");
		 size_t found2 = texturePack.find(".TP");
		 if(found != std::string::npos || found2 != std::string::npos)// found
		 {
		 	texturePackList.push_back(texturePack);
		 }
	 }
	 fclose(infile);

*/

inline bool StateMenu::fileExists (const std::string& name)
{
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

unsigned int StateMenu::hash(const char* s, unsigned int seed)
{
    unsigned int hash = seed;
    while (*s)
    {
        hash = hash * 101  +  *s++;
    }
    return hash;
}

int StateMenu::fileSize (const std::string& name)
{
    struct stat stat_buf;
    int rc = stat(name.c_str(), &stat_buf);
    return rc == 0 ? (int)stat_buf.st_size : -1;
}

void StateMenu::DrawText(int x,int y, unsigned int color, float size, const char *message, ...)
{
    mRender->SetFontStyle(size,color,0,0x00000200|0x00000000);
    mRender->DebugPrint(x,y,message);
}
