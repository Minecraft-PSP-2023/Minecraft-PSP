#include "StatePlay.h"
#include "TextureHelper.h"

#include "WorldGenerator.h"
#include "LoadingScreen.h"

#define PI 3.14159265f
#define DEG_TO_RAD (PI / 180.0f)
#define TEXTURE_CHUNKS 8
#define SKY_MOVE 0.003f

#define WORLD_SIZE 252
#define WORLD_HEIGHT 112
#define CHUNK_SIZE 14

#define default_size 0.5
#define default_big_size 0.687
#define default_normal_font 0.587

#define ENGLISH 1
#define RUSSIAN 2

StatePlay::StatePlay()
{
    time12 = 0.0f;
    mRender = NULL;
    mSystemMgr = NULL;
    fppCam = NULL;
    UseChest = NULL;
    UseFurnace = NULL;

    temp_total_time_ms = 0;

    statisticsPage = 0;

    freeMemory = 30;
    freeMemoryTimer = 0.0f;

    ram1 = 0;
    ram2 = 0;
    dt = 0.0f;
	
	HGP = 0;
	HPE = 0;

    showCube = true;

    GRAVITY = -6.8f;
    JUMPVELOCITY = 4.6f;
    CLIMBVELOCITY = 2.5f;

    moveForward = false;
    walkingOnGround = false;
    jumping = false;
    moving = false;
    headInWater = false;
    footInWater = false;
    headInLava = false;
    footInLava = false;
    footInFire = false;
    playerInCobweb = false;
    touchingCactus = false;
	burning = false;
	
	sprint = false;
	crouch = false;
	
	HungerPoisoned = false;
	HPPoisoned = false;

    invEn = false;
    craft3xEn = false;
    upEn = false;
    chestEn = false;
    furnaceEn = false;
    armorEn = false;

    cycle = 0;
    dtt = 0.0f;

    shift_x = 0.0f;
    shift_y = 0.0f;
    idle_y = 0.0f;

    musicTimeGo = 0;

    chunkId = 0;
    chunks = 0;
	
    angleFactor = 0.0f;

    startDt = false;
    dET = 0;
    dT = 0;
    dStd = -1;
    tryToDt = false;

    testPos1.x = -1;
    testPos1.y = -1;
    testPos1.z = -1;

    barPosition = 0;
    invXPosition = 0;
    invYPosition = 0;

    craftItemId = -1;
    craftItemAm = -1;
    craftItemSt = 0;

    craftItemId3 = -1;
    craftItemAm3 = -1;
    craftItemSt3 = 0;

    chestId = -1;
    furnaceId = -1;

    cameraSpeed = 2.0f / 60.0f;
    cameraMoveSpeed = 0.0f;

    menuState = 0;
    selectPos = 0;
	diePos = 0;

    cloudsOffset = 0.0f;

    devMode = false;
    makeScreen = false;

	canHeadBob = true;
	invSteveAngle = 0.0f;
	bobCycle = 0.0f;
	bobCiclo = 0.0f;
	changeY = 0.0f;
	changeScale = 0.0f;

    slotForChangeScale = -1;

	anim[0] = 0;
	anim[1] = 0;
	anim[2] = 0;

	hpAnim = 0.0f;

	canFly = false;

	for(int i = 0; i <= 3; i += 1)
    {
        craftSlotId[i] = -1;
        craftSlotAm[i] = -1;
        craftSlotSt[i] = 0;
    }

    for(int i = 0; i <= 8; i += 1)
    {
        craftSlotId3[i] = -1;
        craftSlotAm3[i] = -1;
        craftSlotSt3[i] = 0;
    }

    tickHunger = 0.8f;
    tickHealth = 0.0f;
    tickCave = 0.0f;
    tickChunk = 0.0f;
    tickOS = 0.4f;
	tickFR = 0.4f; //fire
    tickShowSlotName = 0.0f;
    tickSprintTime = 1.0f;
    tickCactusDamage = 0.0f;
    tickLavaDamage = 0.6f;
	tickFireDamage = 0.6f; //fire
    furnaceTimes = 0.0f;
    tutorialTimer = -1.0f;

	tickHungerPoisoned = 1.0f;
	tickHPveneno = 0.0f;

    rainSoundFrequency = 1.7f;

    inputDiskNameTimer = 0.0f;
    inputDiskName = "";

    hurt = false;
    hurt_time = 0;
    musicTime = 1;

    dieFactor = true;

    animDest = 0.0f;
    animSpeed = 1.0f;
    animGo = false;

    cubeLight = 0.0f;

    sleepMessageTime = 0.0f;
    sleepMessageType = 0;
    sleepTime = 0.0f;
    loadReady = false;

    pre_fps = 0.0f;
    average_fps = 0;
    tick_fps = 0.0f;
    ticks = 0;

    last_HP = 0;
}

StatePlay::~StatePlay()
{

}

void StatePlay::InitCamera()
{
    fppCam = new Camera();
    fppCam->PositionCamera(playerPosition.x,playerPosition.y,playerPosition.z, playerPosition.x,playerPosition.y,playerPosition.z-5.0f, 0.0f,1.0f,0.0f);
    mRender->SetActiveCamera(fppCam);

    mWorld->UpdatePlayerZoneBB(fppCam->m_vPosition);
    mWorld->RebuildVisibleChunks();
}

void StatePlay::Init()
{
    //set render manager instance
    mRender = RenderManager::InstancePtr();
    mSystemMgr = SystemManager::Instance();
    mSoundMgr = SoundManager::Instance();
    mIhelper = InputHelper::Instance();

    WorldGenerator *mGen = new WorldGenerator();

    //then create our perfect world
    mWorld = new CraftWorld();
    mWorld->initWorld(WORLD_SIZE, WORLD_HEIGHT, CHUNK_SIZE);
    delete mGen;
    //mWorld->initRandompMap(128,16);
    mWorld->setTextureSize(256,16,64,16,256,16);
    mWorld->initWorldBlocksLight();
    mWorld->SetWolrdTime(10);
    mWorld->UpdatePlayerZoneBB(playerPosition);
    mWorld->buildMap();

    playerPosition = newPlayerPos = oldPlayerPos = Vector3(64.0f,mWorld->groundHeight(64,64)+1.65,64.0f);

    int	curchunkTarget = mWorld->getChunkId(playerPosition);

    dt = mTimer.GetDeltaTime();

    bobCycle = 3.14/2;
    bobCiclo = 3.14/2;
    cubeLight = 0.0f;

    LoadTextures();
    mWorld->GetTexturesIds();
    mWorld->buildblocksVerts();

    menuOptions = false;
    optionsMenuPos = 0;

    analogLeft = analogRight = analogUp = analogDown = false;

    walkSoundAccu = 0.0f;
    isWalking = false;
    loadReady = false;
}

void StatePlay::InitParametric(bool makeTrees,bool makeDungeons,bool makeBonus,bool makeCaves,unsigned int seed_1, int worldType, char gameMode)
{
    //set render manager instance
    mRender = RenderManager::InstancePtr();
    mSystemMgr = SystemManager::Instance();
    mSoundMgr = SoundManager::Instance();
    mIhelper = InputHelper::Instance();

    //then create our perfect world
    mWorld = new CraftWorld();
    mWorld->initWorld(WORLD_SIZE, WORLD_HEIGHT, CHUNK_SIZE);
    mWorld->gameModeWorld = gameMode;

    WorldGenerator *mGen = new WorldGenerator();
    mGen->initRandompMap(WORLD_SIZE, WORLD_HEIGHT, CHUNK_SIZE, mWorld, makeTrees, makeDungeons, makeBonus, makeCaves, seed_1, worldType);
    delete mGen;
	
    mWorld->setTextureSize(256,16,64,16,256,16);
    mWorld->SetWolrdTime(10);
    mWorld->UpdatePlayerZoneBB(playerPosition);
    mWorld->buildMap();

    playerPosition = newPlayerPos = oldPlayerPos = mWorld->playerSpawnPointPosition;

    int	curchunkTarget = mWorld->getChunkId(playerPosition);

    dt = mTimer.GetDeltaTime();

    LoadTextures();

    mWorld->GetTexturesIds();
    mWorld->buildblocksVerts();
    mWorld->buildcloudsVerts();
    mWorld->buildmobVerts();

    menuOptions = false;
    optionsMenuPos = 0;

    analogLeft = analogRight = analogUp = analogDown = false;

    walkSoundAccu = 0.0f;
    isWalking = false;

    cubeLight = 0.0f;
    bobCycle = PI/2.0f;
    bobCiclo = PI/2.0f;
    loadReady = true;

    mWorld->haveCompass = mWorld->HaveItemInBarSlots(Compass::getID());


}

void StatePlay::SetWorldAndSaveName(std::string worldName,std::string fileName)
{
    if(mWorld != NULL)
    {
        sprintf(mWorld->worldName,"%s",worldName.c_str());
    }
    saveFileName = fileName;
}

void StatePlay::LoadMap(std::string fileName,bool compressed)
{
    //enter loading screen
    LoadingScreen* loading = new LoadingScreen();

    loading->readiness = 0;
    loading->stateName = 0;

    //set render manager instance
    mRender = RenderManager::InstancePtr();
    mSystemMgr = SystemManager::Instance();
    mSoundMgr = SoundManager::Instance();
    mIhelper = InputHelper::Instance();

    //save name
    saveFileName = fileName;
    //set start position
    playerPosition = newPlayerPos = oldPlayerPos = Vector3(64.0f,64,64);

    //then create our perfect world
    mWorld = new CraftWorld();

    //load data
    if(compressed)
    {
        mWorld->LoadCompressedWorld(saveFileName);
        //set player pos from map
        playerPosition = mWorld->GetPlayerPos();
        newPlayerPos  = mWorld->GetPlayerPos();
        oldPlayerPos = mWorld->GetPlayerPos();
        mWorld->SetWolrdTime(mWorld->worldDayTime);
    }
    else
    {
        mWorld->LoadWorld(saveFileName.c_str());
        mWorld->SetWolrdTime(6);
    }
    //player zone size and bbZone
    mWorld->UpdatePlayerZoneBB(playerPosition);
    //create chunks
    mWorld->setTextureSize(256,16,64,16,256,16);
    mWorld->initWorldBlocksLight();
    mWorld->buildMap();

    int	curchunkTarget = mWorld->getChunkId(playerPosition);
    dt = mTimer.GetDeltaTime();
    // load textures
    LoadTextures();
    mWorld->GetTexturesIds();

    // build custom mesh
    mWorld->buildblocksVerts();
    mWorld->buildcloudsVerts();
    mWorld->buildmobVerts();

    menuOptions = false;
    optionsMenuPos = 0;

    analogLeft = analogRight = analogUp = analogDown = false;

    walkSoundAccu = 0.0f;
    isWalking = false;

    mSoundMgr->playerSounds = mWorld->mainOptions.sounds;
    mWorld->playerZoneSize = Vector3(CHUNK_SIZE*mWorld->mainOptions.horizontalViewDistance,CHUNK_SIZE*mWorld->mainOptions.verticalViewDistance,CHUNK_SIZE*mWorld->mainOptions.horizontalViewDistance);
    mRender->fovv = mWorld->mainOptions.fov;
    RenderManager::InstancePtr()->SetPerspective(0, 480.0f / 272.0f, 0.18f, 1000.f);

    loadReady = true;

    mWorld->haveCompass = mWorld->HaveItemInBarSlots(Compass::getID());

    // cancel loading screen
    loading->KillLoadingScreen();
    delete loading;
}

void StatePlay::LoadTextures()
{
    //terrain texure
    texture = TextureHelper::Instance()->GetTexture(TextureHelper::Terrain1);
    texture_mips = TextureHelper::Instance()->GetTexture(TextureHelper::Terrain2);
    waterAnimation = TextureHelper::Instance()->GetTexture(TextureHelper::WaterAnimation);

    invPlayerTex = TextureHelper::Instance()->GetTexture(TextureHelper::Steve);
    cloudsTex = TextureHelper::Instance()->GetTexture(TextureHelper::clouds1);
    clouds2Tex = TextureHelper::Instance()->GetTexture(TextureHelper::clouds2);
    zombieTex = TextureHelper::Instance()->GetTexture(TextureHelper::zombieTexture);
    spiderTex = TextureHelper::Instance()->GetTexture(TextureHelper::spiderTexture);
    spidercaveTex = TextureHelper::Instance()->GetTexture(TextureHelper::cavespiderTexture);
    cowTex = TextureHelper::Instance()->GetTexture(TextureHelper::cowTexture);
	chickenTex = TextureHelper::Instance()->GetTexture(TextureHelper::chickenTexture);
	pigTex = TextureHelper::Instance()->GetTexture(TextureHelper::pigTexture);

    blue = TextureHelper::Instance()->GetTexture(TextureHelper::Blue);
    red = TextureHelper::Instance()->GetTexture(TextureHelper::Red);
    black = TextureHelper::Instance()->GetTexture(TextureHelper::Black);

    snowBall4 = TextureHelper::Instance()->GetTexture(TextureHelper::SnowBall3);

    suntex = TextureHelper::Instance()->GetTexture(TextureHelper::Sun);
    moontex = TextureHelper::Instance()->GetTexture(TextureHelper::Moon);
    stars = TextureHelper::Instance()->GetTexture(TextureHelper::Stars);
    rainTex  = TextureHelper::Instance()->GetTexture(TextureHelper::rainTexture);

    barItems = TextureHelper::Instance()->GetTexture(TextureHelper::Items1);

    pumpkinMask = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::PumpkinMask));
    pumpkinMask->SetPosition(240,136);
    pumpkinMask->Scale(2.87f,2.84f);

    waterScreen = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Blue));
    waterScreen->SetPosition(240,136);
    waterScreen->Scale(30,17);

    // hud section //ui
    int utilsSize = TextureManager::Instance()->getWidth(TextureHelper::Instance()->GetTexture(TextureHelper::Utils));
    float utilScale = 364.0f/(float)utilsSize;
	
	int iconsSize = TextureManager::Instance()->getWidth(TextureHelper::Instance()->GetTexture(TextureHelper::Icons));
    float iconScale = 364.0f/(float)iconsSize;

    barSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),0*utilsSize/182,0*utilsSize/182,182*utilsSize/182,22*utilsSize/182);
    barSprite->SetPosition(240,230);
    barSprite->Scale(1,1);
	
	///XP
	XPbarSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),0*iconsSize/182,0*iconsSize/182,182*iconsSize/182,5*iconsSize/182,true);
    XPbarSprite->SetPosition(149,212);
    XPbarSprite->Scale(1,1);
	
	for(int j = 0; j <= 181; j++)
    {
		XPprogressSprite[j] = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),0*iconsSize/182,5*iconsSize/182,j*iconsSize/182,5*iconsSize/182,true);
		XPprogressSprite[j]->SetPosition(149,212);
		XPprogressSprite[j]->Scale(1,1);
	}
	
	///END

    selectSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),0*utilsSize/182,22*utilsSize/182,24*utilsSize/182,24*utilsSize/182);
    selectSprite->SetPosition(160,230);
    selectSprite->Scale(1,1);

    crossSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),24*utilsSize/182,22*utilsSize/182,9*utilsSize/182,9*utilsSize/182);
    crossSprite->SetPosition(240,136);
    crossSprite->Scale(270.0f/utilsSize,270.0f/utilsSize);
	
	///-----* BOTONES *-----///
	
	lSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),111*iconsSize/182,51*iconsSize/182,16*iconsSize/182,9*iconsSize/182,true);
    lSprite->SetPosition(20,255);
    lSprite->Scale(1,1);
	
	rSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),128*iconsSize/182,51*iconsSize/182,16*iconsSize/182,9*iconsSize/182,true);
    rSprite->SetPosition(70,255);
    rSprite->Scale(1,1);
	
	l2Sprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),114*iconsSize/182,62*iconsSize/182,13*iconsSize/182,13*iconsSize/182,true);
    l2Sprite->SetPosition(20,255);
    l2Sprite->Scale(1,1);
	
	r2Sprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),128*iconsSize/182,62*iconsSize/182,13*iconsSize/182,13*iconsSize/182,true);
    r2Sprite->SetPosition(20,255);
    r2Sprite->Scale(1,1);
	
	///-----* END *-----///

    hpCellSpriteW = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),60*utilsSize/182,36*utilsSize/182,82*utilsSize/182,10*utilsSize/182);
    hpCellSpriteW->SetPosition(100,100);
    hpCellSpriteW->Scale(1,1);
	
	hpCellSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),60*utilsSize/182,46*utilsSize/182,82*utilsSize/182,10*utilsSize/182);
    hpCellSprite->SetPosition(100,100);
    hpCellSprite->Scale(1,1);

    hgCellSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),60*utilsSize/182,56*utilsSize/182,82*utilsSize/182,10*utilsSize/182);
    hgCellSprite->SetPosition(100,24);
    hgCellSprite->Scale(1,1);
	
	hg2CellSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),20*iconsSize/182,56*iconsSize/182,82*iconsSize/182,10*iconsSize/182);
    hg2CellSprite->SetPosition(100,24);
    hg2CellSprite->Scale(1,1);

    arCellSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),60*utilsSize/182,66*utilsSize/182,82*utilsSize/182,10*utilsSize/182);
    arCellSprite->SetPosition(100,27);
    arCellSprite->Scale(1,1);

    hpSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),0*utilsSize/182,66*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpSprite->SetPosition(100,22);
    hpSprite->Scale(1,1);

    hpSpriteW = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),20*utilsSize/182,66*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpSpriteW->SetPosition(100,22);
    hpSpriteW->Scale(1,1);

    hpHardSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),0*utilsSize/182,56*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpHardSprite->SetPosition(100,22);
    hpHardSprite->Scale(1,1);

    hpHardSpriteW = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),20*utilsSize/182,56*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpHardSpriteW->SetPosition(100,22);
    hpHardSpriteW->Scale(1,1);

    hpHalfSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),10*utilsSize/182,66*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpHalfSprite->SetPosition(100,22);
    hpHalfSprite->Scale(1,1);

    hpHalfSpriteW = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),30*utilsSize/182,66*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpHalfSpriteW->SetPosition(100,22);
    hpHalfSpriteW->Scale(1,1);

    hpHardHalfSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),10*utilsSize/182,56*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpHardHalfSprite->SetPosition(100,22);
    hpHardHalfSprite->Scale(1,1);

    hpHardHalfSpriteW = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),30*utilsSize/182,56*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpHardHalfSpriteW->SetPosition(100,22);
    hpHardHalfSpriteW->Scale(1,1);
	
	//araña veneno
	hpVenenoSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),0*utilsSize/182,56*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpVenenoSprite->SetPosition(100,22);
    hpVenenoSprite->Scale(1,1);
	
	hpVenenoHalfSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),10*utilsSize/182,56*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpVenenoHalfSprite->SetPosition(100,22);
    hpVenenoHalfSprite->Scale(1,1);
	
	hpVenenoSpriteW = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),0*utilsSize/182,66*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpVenenoSpriteW->SetPosition(100,22);
    hpVenenoSpriteW->Scale(1,1);
	
	hpVenenoHalfSpriteW = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),10*utilsSize/182,66*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hpVenenoHalfSpriteW->SetPosition(100,22);
    hpVenenoHalfSpriteW->Scale(1,1);
	//-----**-----//

    hgSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),30*utilsSize/182,46*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hgSprite->SetPosition(100,26);
    hgSprite->Scale(1,1);

    hgHalfSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),40*utilsSize/182,46*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    hgHalfSprite->SetPosition(100,26);
    hgHalfSprite->Scale(1,1);
	
	hg2Sprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),20*iconsSize/182,46*iconsSize/182,10*iconsSize/182,10*iconsSize/182);
    hg2Sprite->SetPosition(100,26);
    hg2Sprite->Scale(1,1);

    hg2HalfSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Icons),30*iconsSize/182,46*iconsSize/182,10*iconsSize/182,10*iconsSize/182);
    hg2HalfSprite->SetPosition(100,26);
    hg2HalfSprite->Scale(1,1);
	
    bubbleSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),0*utilsSize/182,46*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    bubbleSprite->SetPosition(100,27);
    bubbleSprite->Scale(1,1);

    arSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),10*utilsSize/182,46*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    arSprite->SetPosition(100,27);
    arSprite->Scale(1,1);

    arHalfSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),20*utilsSize/182,46*utilsSize/182,10*utilsSize/182,10*utilsSize/182);
    arHalfSprite->SetPosition(100,27);
    arHalfSprite->Scale(1,1);

    for(int j = 13; j >= 0; j--)
    {
        toolPointSprite[j] = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Utils),169*utilsSize/182,(49+(13-j)*2)*utilsSize/182,13*utilsSize/182,2*utilsSize/182,true);//200
        toolPointSprite[j]->SetPosition(240,136);
        toolPointSprite[j]->Scale(1,1);
    }
    //menu section
    buttonSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,0,95,12); // stand
    buttonSprite->SetPosition(240,150);
    buttonSprite->Scale(2,2);

    sbuttonSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,12,95,12); // stand selected
    sbuttonSprite->SetPosition(240,150);
    sbuttonSprite->Scale(2,2);

    nbuttonSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,24,95,12); // dark
    nbuttonSprite->SetPosition(240,150);
    nbuttonSprite->Scale(2,2);

	moverSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Buttons),0,60,6,12);
	moverSprite->Scale(2,2);

    // inventory section

    int invGuiSize = TextureManager::Instance()->getWidth(TextureHelper::Instance()->GetTexture(TextureHelper::inv));
    float invGuiScale = 352.0f/(float)invGuiSize;

    invSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::inv));//200
    invSprite->SetPosition(240,136);
    invSprite->Scale(1,1);

    crtSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::crt));//200
    crtSprite->SetPosition(240,136);
    crtSprite->Scale(1,1);

    chtSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::cht));//200
    chtSprite->SetPosition(240,136);
    chtSprite->Scale(1,1);

    furSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::fur));//200
    furSprite->SetPosition(240,136);
    furSprite->Scale(1,1);

    int invArrowSize = TextureManager::Instance()->getWidth(TextureHelper::Instance()->GetTexture(TextureHelper::furArrow));
    float invArrowScale = 44.0f/(float)invArrowSize;
	//22*16
    for(int j = 0; j <= 21; j++)
    {
        furArrowSprite[j] = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::furArrow),0,0,(1*j)*invArrowSize/22.0f,16*invArrowSize/22.0f,true);//200
        furArrowSprite[j]->SetPosition(240,136);
        furArrowSprite[j]->Scale(invArrowScale,invArrowScale);
    }

    int invFireSize = TextureManager::Instance()->getWidth(TextureHelper::Instance()->GetTexture(TextureHelper::furFire));
    float invFireScale = 28.0f/(float)invFireSize;

    for(int j = 0; j <= 13; j++)
    {
        furFireSprite[13-j] = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::furFire),0,j*invFireSize/14.0f,14*invFireSize/14.0f,(14-j)*invFireSize/14.0f,true);//200
        furFireSprite[13-j]->SetPosition(240,136);
        furFireSprite[13-j]->Scale(invFireScale,invFireScale);
    }

    selectInvSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::cursor));//200
    selectInvSprite->SetPosition(168,149);

    invCellSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::invCell));//200
    invCellSprite->SetPosition(240,136);
    invCellSprite->Scale(1,1);

    skyLight = new SkyLight();
    skyMoonLight = new SkyLight();
    skyBox = new SkyBox();
    destroyer = new Destroyer();
    starsBox = new StarsBox();
    invPlayer = new InventoryPlayer();
}



void StatePlay::SetDayTimeAfterLoad()
{

}

void StatePlay::Enter()
{
    RenderManager::InstancePtr()->SetPerspective(55.0f, 480.0f / 272.0f, 0.18f, 1000.0f);//1000
}

void StatePlay::CleanUp()
{
    if(mSnowBalls.empty() == false)
    {
        for(unsigned int i = 0; i < mSnowBalls.size(); i++)
        {
            delete mSnowBalls[i];
        }
        mSnowBalls.clear();
    }
	
	if(mArrows.empty() == false)
    {
        for(unsigned int i = 0; i < mArrows.size(); i++)
        {
            delete mArrows[i];
        }
        mArrows.clear();
    }

    delete mRender->mCam;
    mRender->mCam = NULL;

    delete invCellSprite;
    delete buttonSprite;
    delete sbuttonSprite;
    delete nbuttonSprite;
    delete moverSprite;
    delete barSprite;
    delete XPbarSprite;
	
	for(int j = 0; j <= 181; j++)
    {
		delete XPprogressSprite[j];
	}
	
    delete selectSprite;
    delete crossSprite;
	delete lSprite;
    delete rSprite;
    delete l2Sprite;
    delete r2Sprite;
    delete invSprite;
    delete crtSprite;
    delete chtSprite;
    delete selectInvSprite;

    delete skyLight;
    delete skyMoonLight;
    delete skyBox;
    delete destroyer;
    delete starsBox;
    delete invPlayer;

    delete furSprite;
    delete waterScreen;
    delete pumpkinMask;

    for(unsigned int i = 0; i <= 13; i++)
    {
        delete furFireSprite[i];
    }

    for(unsigned int i = 0; i <= 21; i++)
    {
        delete furArrowSprite[i];
    }

    for(unsigned int i = 0; i <= 13; i++)
    {
        delete toolPointSprite[i];
    }

    delete hpCellSprite;
    delete hpSprite;
    delete hpHardSprite;
    delete hpHalfSprite;
    delete hpHardHalfSprite;
    delete hpCellSpriteW;
    delete hpSpriteW;
    delete hpHardSpriteW;
    delete hpHalfSpriteW;
    delete hpHardHalfSpriteW;
	
	delete hpVenenoSprite;
	delete hpVenenoHalfSprite;
	delete hpVenenoSpriteW;
	delete hpVenenoHalfSpriteW;

    delete hgCellSprite;
    delete hgSprite;
    delete hgHalfSprite;
	
	delete hg2CellSprite;
    delete hg2Sprite;
    delete hg2HalfSprite;

    delete arSprite;
    delete arHalfSprite;
    delete arCellSprite;

    delete bubbleSprite;

    delete mWorld;
}

void StatePlay::Pause()
{

}

void StatePlay::Resume()
{

}

int StatePlay::FindChestId(int x, int y, int z)
{
    int o;
    int address;
    o = -1;
    address = x + y * WORLD_SIZE + z * WORLD_SIZE * WORLD_HEIGHT;

    for(unsigned int i = 0; i < mWorld->mChests.size(); i++)
    {
        Chest* NewChest = mWorld->mChests[i];
        if(NewChest->chestAddress == address)
        {
            o = i;
            break;
        }
    }

    return o;
}

int StatePlay::FindFurnaceId(int x, int y, int z)
{
    int o;
    o = -1;
    for(unsigned int i = 0; i < mWorld->mFurnaces.size(); i++)
    {
        Furnace* NewFurnace = mWorld->mFurnaces[i];
        if(NewFurnace->furnaceX == x && NewFurnace->furnaceY == y && NewFurnace->furnaceZ == z)
        {
            o = i;
            break;
        }
    }

    return o;
}

void StatePlay::CheckForFurnFuel(Furnace* Fur)
{
    int furnItem;

    furnItem = -1;

    if(Fur->furnaceSlotId[0] < 250 && Fur->furnaceSlotId[0] != -1)
    {
        furnItem = mWorld->blockTypes[Fur->furnaceSlotId[0]].furnItem;
    }

    if(Fur->furnaceSlotId[0] >= 250)
    {
        furnItem = mWorld->itemTypes[Fur->furnaceSlotId[0]-250].furnItem;
    }

    if(Fur->fuelTime <= 0 && furnItem != -1)
    {
        if(Fur->furnaceSlotId[1] == -1)
            return;

        bool used = false;

        switch(Fur->furnaceSlotId[1])
        {
        case 8:
            Fur->fuelTime = 15;
            used = true;
        break;
        case 31:
            Fur->fuelTime = 15;
            used = true;
        break;
        case 34:
            Fur->fuelTime = 15;
            used = true;
        break;
        case 296:
            Fur->fuelTime = 5;
            used = true;
        break;
        case 276:
            Fur->fuelTime = 5;
            used = true;
        break;
        case 277:
            Fur->fuelTime = 80;
            used = true;
        break;
        case 292:
            Fur->fuelTime = 1000;
            used = true;
        break;
        case 306:
            Fur->fuelTime = 30;
            used = true;
        break;
        case 59:
            Fur->fuelTime = 15;
            used = true;
        break;
        case 71:
            Fur->fuelTime = 15;
            used = true;
        break;
        case 72:
            Fur->fuelTime = 15;
            used = true;
        break;
        case 100:
            Fur->fuelTime = 15;
            used = true;
        break;
        case 105:
            Fur->fuelTime = 15;
            used = true;
        break;
        case 125:
            Fur->fuelTime = 15;
            used = true;
        break;
        case 155:
            Fur->fuelTime = 15;
            used = true;
        break;
        case 156:
            Fur->fuelTime = 15;
            used = true;
        break;
        case 214:
            Fur->fuelTime = 800;
            used = true;
        break;
        case 250:
            Fur->fuelTime = 10;
            used = true;
        break;
        case 255:
            Fur->fuelTime = 10;
            used = true;
        break;
        case 260:
            Fur->fuelTime = 10;
            used = true;
        break;
        case 265:
            Fur->fuelTime = 10;
            used = true;
        break;
        case 270:
            Fur->fuelTime = 10;
            used = true;
        break;
        }

        if(used == true)
        {
            Fur->furnaceSlotAm[1] -= 1;
            if(Fur->furnaceSlotAm[1] == 0)
            {
                Fur->furnaceSlotAm[1] = -1;
                Fur->furnaceSlotId[1] = -1;
                Fur->furnaceSlotSt[1] = 0;
            }
        }
        else
        {
            Fur->meltingTime = 0.0f;
        }
    }
}

void StatePlay::CheckForFurnWorking(Furnace* Fur)
{
    int furnItem;

    furnItem = -1;

    if(Fur->furnaceSlotId[0] < 250 && Fur->furnaceSlotId[0] != -1)
    {
        furnItem = mWorld->blockTypes[Fur->furnaceSlotId[0]].furnItem;
    }

    if(Fur->furnaceSlotId[0] >= 250)
    {
        furnItem = mWorld->itemTypes[Fur->furnaceSlotId[0]-250].furnItem;
    }

    if(furnItem != -1)
    {
        if(Fur->fuelTime > 0)
        {
            if(Fur->furnaceSlotId[2] == furnItem || Fur->furnaceSlotId[2] == -1)
            {
                if(Fur->furnaceSlotAm[2] <= 63)
                {
                    Fur->working = true;
                }
            }
            if(Fur->furnaceSlotId[2] != furnItem && Fur->furnaceSlotId[2] != -1)
            {
                Fur->working = false;
                Fur->meltingTime = 0;
            }
        }
        else
        {
            Fur->working = false;
            Fur->meltingTime = 0;
        }
        if(Fur->fuelTime == 0)
        {
            Fur->working = false;
            Fur->meltingTime = 0;
        }
    }
    else
    {
        Fur->working = false;
        Fur->meltingTime = 0;
    }
}

void StatePlay::ReadyFurnSmelting(Furnace* Fur)
{
    int furnItem;

    if(Fur->furnaceSlotId[0] < 250)
    {
        furnItem = mWorld->blockTypes[Fur->furnaceSlotId[0]].furnItem;
    }

    if(Fur->furnaceSlotId[0] >= 250)
    {
        furnItem = mWorld->itemTypes[Fur->furnaceSlotId[0]-250].furnItem;
    }

    if(furnItem != -1)
    {
        if(Fur->furnaceSlotId[2] == furnItem)
        {
           Fur->furnaceSlotAm[2] += 1;
        }
        if(Fur->furnaceSlotId[2] == -1)
        {
            Fur->furnaceSlotId[2] = furnItem;
            Fur->furnaceSlotSt[2] = true;
            Fur->furnaceSlotAm[2] = 1;
        }

        Fur->furnaceSlotAm[0] -= 1;
        if(Fur->furnaceSlotAm[0] <= 0)
        {
            Fur->furnaceSlotId[0] = -1;
            Fur->furnaceSlotAm[0] = -1;
            Fur->furnaceSlotSt[0] = 0;
        }
    }
}

void StatePlay::CraftItem2x2()
{
    craftItemId = -1;
    craftItemAm = -1;
    craftItemSt = 0;

    int result = 0;
    bool enableMainCraftBody = true;

    std::vector<int> itemsIDs;
    std::string materialString;

    for(int i = 0; i <= 3; i ++)
    {
        if(craftSlotId[i] != -1)
        {
            result += craftSlotId[i];
            itemsIDs.push_back(craftSlotId[i]);
        }

        if(craftSlotId[i] >= BlackWoolBlock::getID() && craftSlotId[i] <= WhiteWoolBlock::getID())
        {
            materialString += "W";
        }
        else if(craftSlotId[i] == OakPlanks::getID() || craftSlotId[i] == SprucePlanks::getID() || craftSlotId[i] == BirchPlanks::getID())
        {
            materialString += "P";
        }
        else
        {
            materialString += "0";
        }
    }

    // carpets
    if(materialString == "WW00" || materialString == "00WW")
    {
        if(itemsIDs.size() == 2)
        {
            if(itemsIDs[0] == itemsIDs[1])
            {
                craftItemId = itemsIDs[0]+188;
                craftItemSt = 1;
                craftItemAm = 3;

                enableMainCraftBody = false;
            }
        }
    }
	
    if(itemsIDs.size() == 2) // dyes and colored wool crafting
    {
        if((itemsIDs[0] == RoseRed::getID() && itemsIDs[1] == DandelionYellow::getID()) ||
           (itemsIDs[1] == RoseRed::getID() && itemsIDs[0] == DandelionYellow::getID()))
        {
            craftItemId = OrangeDye::getID();
            craftItemSt = 1;
            craftItemAm = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == RoseRed::getID() && itemsIDs[1] == BoneMeal::getID()) ||
           (itemsIDs[1] == RoseRed::getID() && itemsIDs[0] == BoneMeal::getID()))
        {
            craftItemId = PinkDye::getID();
            craftItemSt = 1;
            craftItemAm = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == CactusGreen::getID() && itemsIDs[1] == BoneMeal::getID()) ||
           (itemsIDs[1] == CactusGreen::getID() && itemsIDs[0] == BoneMeal::getID()))
        {
            craftItemId = LimeDye::getID();
            craftItemSt = 1;
            craftItemAm = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == LapisLazuli::getID() && itemsIDs[1] == BoneMeal::getID()) ||
           (itemsIDs[1] == LapisLazuli::getID() && itemsIDs[0] == BoneMeal::getID()))
        {
            craftItemId = LightBlueDye::getID();
            craftItemSt = 1;
            craftItemAm = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == LapisLazuli::getID() && itemsIDs[1] == RoseRed::getID()) ||
           (itemsIDs[1] == LapisLazuli::getID() && itemsIDs[0] == RoseRed::getID()))
        {
            craftItemId = PurpleDye::getID();
            craftItemSt = 1;
            craftItemAm = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == PurpleDye::getID() && itemsIDs[1] == PinkDye::getID()) ||
           (itemsIDs[1] == PurpleDye::getID() && itemsIDs[0] == PinkDye::getID()))
        {
            craftItemId = MagnetaDye::getID();
            craftItemSt = 1;
            craftItemAm = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == LapisLazuli::getID() && itemsIDs[1] == RoseRed::getID()) ||
           (itemsIDs[1] == LapisLazuli::getID() && itemsIDs[0] == RoseRed::getID()))
        {
            craftItemId = PurpleDye::getID();
            craftItemSt = 1;
            craftItemAm = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == InkSac::getID() && itemsIDs[1] == BoneMeal::getID()) ||
           (itemsIDs[1] == InkSac::getID() && itemsIDs[0] == BoneMeal::getID()))
        {
            craftItemId = GrayDye::getID();
            craftItemSt = 1;
            craftItemAm = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == GrayDye::getID() && itemsIDs[1] == BoneMeal::getID()) ||
           (itemsIDs[1] == GrayDye::getID() && itemsIDs[0] == BoneMeal::getID()))
        {
            craftItemId = LightGrayDye::getID();
            craftItemSt = 1;
            craftItemAm = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == InkSac::getID() && itemsIDs[1] == OrangeDye::getID()) ||
           (itemsIDs[1] == InkSac::getID() && itemsIDs[0] == OrangeDye::getID()))
        {
            craftItemId = CocoaBeans::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }


        /// wool


        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == RoseRed::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == RoseRed::getID()))
        {
            craftItemId = RedWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == DandelionYellow::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == DandelionYellow::getID()))
        {
            craftItemId = YellowWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == OrangeDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == OrangeDye::getID()))
        {
            craftItemId = OrangeWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == CactusGreen::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == CactusGreen::getID()))
        {
            craftItemId = DarkGreenWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == LimeDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == LimeDye::getID()))
        {
            craftItemId = GreenWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == LapisLazuli::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == LapisLazuli::getID()))
        {
            craftItemId = BlueWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == LightBlueDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == LightBlueDye::getID()))
        {
            craftItemId = LightBlueWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == PurpleDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == PurpleDye::getID()))
        {
            craftItemId = VioletWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == MagnetaDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == MagnetaDye::getID()))
        {
            craftItemId = PastelWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == InkSac::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == InkSac::getID()))
        {
            craftItemId = BlackWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == GrayDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == GrayDye::getID()))
        {
            craftItemId = GrayWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == LightGrayDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == LightGrayDye::getID()))
        {
            craftItemId = LightGrayWoolBlock::getID();
            craftItemSt = 1;
            craftItemAm = 1;

            enableMainCraftBody = false;
        }
    }

    if(itemsIDs.size() == 3)
    {
        if(result == RoseRed::getID() + PinkDye::getID() + LapisLazuli::getID())
        {
            if(itemsIDs[0] == RoseRed::getID() || itemsIDs[1] == RoseRed::getID() || itemsIDs[2] == RoseRed::getID())
            {
                craftItemId = MagnetaDye::getID();
                craftItemSt = 1;
                craftItemAm = 3;

                enableMainCraftBody = false;
            }
        }
    }

    itemsIDs.clear();

    if(enableMainCraftBody)
    {
        switch(result)
        {
        // wood to planks
        case 8:
            if(craftSlotId[0] == 8 || craftSlotId[1] == 8 || craftSlotId[2] == 8 || craftSlotId[3] == 8)
            {
                craftItemId = 34;
                craftItemSt = 1;
                craftItemAm = 4;
            }
        break;
		
		// string to white wool
        case 1628:
            if(craftSlotId[0] == 407 && craftSlotId[1] == 407 && craftSlotId[2] == 407 && craftSlotId[3] == 407)
            {
                craftItemId = WhiteWoolBlock::getID();
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;

        // birch wood to planks
        case 31:
            if(craftSlotId[0] == 31 || craftSlotId[1] == 31 || craftSlotId[2] == 31 || craftSlotId[3] == 31)
            {
                craftItemId = BirchPlanks::getID();
                craftItemSt = 1;
                craftItemAm = 4;
            }
        break;

        // spruce wood to planks
        case 30 :
            if(craftSlotId[0] == 30 || craftSlotId[1] == 30 || craftSlotId[2] == 30 || craftSlotId[3] == 30)
            {
                craftItemId = SprucePlanks::getID();
                craftItemSt = 1;
                craftItemAm = 4;
            }
        break;

        // bone to bone meals
        case 321:
            if(craftSlotId[0] == 321 || craftSlotId[1] == 321 || craftSlotId[2] == 321 || craftSlotId[3] == 321)
            {
                craftItemId = 322;
                craftItemSt = 1;
                craftItemAm = 3;
            }
        break;

        // snowballs to snow block
        case 1196:
            if(craftSlotId[0] == 299 && craftSlotId[1] == 299 && craftSlotId[2] == 299 && craftSlotId[3] == 299)
            {
                craftItemId = 47;
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;

        // melon to seeds
        case 311:
            if(craftSlotId[0] == 311 || craftSlotId[1] == 311 || craftSlotId[2] == 311 || craftSlotId[3] == 311)
            {
                craftItemId = 312;
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;

        // melon to seeds
        case 293:
            if(craftSlotId[0] == 293 || craftSlotId[1] == 293 || craftSlotId[2] == 293 || craftSlotId[3] == 293)
            {
                craftItemId = 313;
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;

        // diamond block to diamonds
        case 64:
            if(craftSlotId[0] == 64 || craftSlotId[1] == 64 || craftSlotId[2] == 64 || craftSlotId[3] == 64)
            {
                craftItemId = 279;
                craftItemSt = 1;
                craftItemAm = 9;
            }
        break;

        // iron block to bars
        case 66:
            if(craftSlotId[0] == 66 || craftSlotId[1] == 66 || craftSlotId[2] == 66 || craftSlotId[3] == 66)
            {
                craftItemId = 278;
                craftItemSt = 1;
                craftItemAm = 9;
            }
        break;

        // golden block to bars
        case 65:
            if(craftSlotId[0] == 65 || craftSlotId[1] == 65 || craftSlotId[2] == 65 || craftSlotId[3] == 65)
            {
                craftItemId = 280;
                craftItemSt = 1;
                craftItemAm = 9;
            }
        break;

        // wheat block to wheats
        case 213:
            if(craftSlotId[0] == HayBale::getID() || craftSlotId[1] == HayBale::getID() || craftSlotId[2] == HayBale::getID() || craftSlotId[3] == HayBale::getID())
            {
                craftItemId = Wheat::getID();
                craftItemSt = 1;
                craftItemAm = 9;
            }
        break;

        // coal block to coal items
        case 214:
            if(craftSlotId[0] == CoalBlock::getID() || craftSlotId[1] == CoalBlock::getID() || craftSlotId[2] == CoalBlock::getID() || craftSlotId[3] == CoalBlock::getID())
            {
                craftItemId = Coal::getID();
                craftItemSt = 1;
                craftItemAm = 9;
            }
        break;

        // redstone block to redstone items
        case 215:
            if(craftSlotId[0] == RedstoneBlock::getID() || craftSlotId[1] == RedstoneBlock::getID() || craftSlotId[2] == RedstoneBlock::getID() || craftSlotId[3] == RedstoneBlock::getID())
            {
                craftItemId = Redstone::getID();
                craftItemSt = 1;
                craftItemAm = 9;
            }
        break;

        // bricks to brick block
        case 1156:
            if(craftSlotId[0] == 289 && craftSlotId[1] == 289 && craftSlotId[2] == 289 && craftSlotId[3] == 289)
            {
                craftItemId = 27;
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;

        // clay pieces to clay block
        case 1132:
            if(craftSlotId[0] == 283 && craftSlotId[1] == 283 && craftSlotId[2] == 283 && craftSlotId[3] == 283)
            {
                craftItemId = 99;
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;

        // sand blocks to sandstone
        case 28:
            if(craftSlotId[0] == 7 && craftSlotId[1] == 7 && craftSlotId[2] == 7 && craftSlotId[3] == 7)
            {
                craftItemId = 26;
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;

        // stones to stone bricks
        case 12:
            if(craftSlotId[0] == 3 && craftSlotId[1] == 3 && craftSlotId[2] == 3 && craftSlotId[3] == 3)
            {
                craftItemId = 102;
                craftItemSt = 1;
                craftItemAm = 4;
            }
        break;


        // half to carved stone brick
        case 216:
            if((craftSlotId[0] == 108 && craftSlotId[2] == 108) || (craftSlotId[1] == 108 && craftSlotId[3] == 108))
            {
                craftItemId = 104;
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;


        // planks to crafting table
        case 136:
            if(craftSlotId[0] == 34)
            {
                craftItemId = 105;
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;
        case 620:
            if(craftSlotId[0] == 155)
            {
                craftItemId = 105;
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;
        case 624:
            if(craftSlotId[0] == 156)
            {
                craftItemId = 105;
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;

        // planks to sticks
        case 68: // oak
            if((craftSlotId[0] == 34 && craftSlotId[2] == 34)  || (craftSlotId[1] == 34 && craftSlotId[3] == 34))
            {
                craftItemId = 276;
                craftItemSt = 1;
                craftItemAm = 4;
            }
        break;
        case 310: // birch
            if((craftSlotId[0] == 155 && craftSlotId[2] == 155)  || (craftSlotId[1] == 155 && craftSlotId[3] == 155))
            {
                craftItemId = 276;
                craftItemSt = 1;
                craftItemAm = 4;
            }
        break;
        case 312: // spruce
            if((craftSlotId[0] == 156 && craftSlotId[2] == 156)  || (craftSlotId[1] == 156 && craftSlotId[3] == 156))
            {
                craftItemId = 276;
                craftItemSt = 1;
                craftItemAm = 4;
            }
        break;


        // torch
        case 553:
            if((craftSlotId[0] == 277 && craftSlotId[2] == 276) || (craftSlotId[1] == 277 && craftSlotId[3] == 276))
            {
                craftItemId = 308;
                craftItemSt = 1;
                craftItemAm = 4;
            }
        break;

        // jack o'lantern
        case 383:
            if((craftSlotId[0] == Pumpkin3::getID() && craftSlotId[2] == TorchItem::getID()) || (craftSlotId[1] == Pumpkin3::getID() && craftSlotId[3] == TorchItem::getID()))
            {
                craftItemId = JackOLantern3::getID();
                craftItemSt = 1;
                craftItemAm = 1;
            }
        break;

        case 556: //scissors
            if((craftSlotId[2] == 278 && craftSlotId[1] == 278))
            {
                craftItemId = 275;
                craftItemSt = 0;
                craftItemAm = 239;
            }
        break;

        case 629: //flint and steel
            if((craftSlotId[0] == IronBar::getID() && craftSlotId[3] == Flint::getID()))
            {
                craftItemId = FlintAndSteel::getID();
                craftItemSt = 0;
                craftItemAm = 65;
            }
        break;

        // pumpkin to pumpkin seeds
        case 75:
            if(craftSlotId[0] == 75 || craftSlotId[1] == 75 || craftSlotId[2] == 75 || craftSlotId[3] == 75)
            {
                craftItemId = PumpkinSeeds::getID();
                craftItemSt = 1;
                craftItemAm = 4;
            }
        break;

        // rose to dye
        case 294:
            if(craftSlotId[0] == Flower1Item::getID() || craftSlotId[1] == Flower1Item::getID() || craftSlotId[2] == Flower1Item::getID() || craftSlotId[3] == Flower1Item::getID())
            {
                craftItemId = RoseRed::getID();
                craftItemSt = 1;
                craftItemAm = 2;
            }
        break;

        // dandelion to dye
        case 295:
            if(craftSlotId[0] == Flower2Item::getID() || craftSlotId[1] == Flower2Item::getID() || craftSlotId[2] == Flower2Item::getID() || craftSlotId[3] == Flower2Item::getID())
            {
                craftItemId = DandelionYellow::getID();
                craftItemSt = 1;
                craftItemAm = 2;
            }
        break;

        default:
            craftItemId = -1;
            craftItemAm = -1;
            craftItemSt = 0;
        break;

        }
    }
}

/*///New crafting system :)
void StatePlay::NewCraftingSystem3x3()
{
	craftItemId3 = -1;
    craftItemAm3 = -1;
    craftItemSt3 = 0;

    int resultado = 0;
    bool MainCraftBody = true;
	
	std::vector<int> itemsIDs;
	std::string materialString;
	
	for(int i = 0; i <= 8; i ++)
    {
        if(craftSlotId3[i] != -1)
        {
            resultado += craftSlotId3[i];
            itemsIDs.push_back(craftSlotId3[i]);
        }
    }
	
	itemsIDs.clear();
	
	if(MainCraftBody)
    {
	switch(resultado)
	{
		
	}
	}
}*/


/// Need to create new crafting system. Too many recipes cause mistakes and errors due to this system
void StatePlay::CraftItem3x3()
{
    craftItemId3 = -1;
    craftItemAm3 = -1;
    craftItemSt3 = 0;

    int result = 0;
    bool enableMainCraftBody = true;
    std::string materialString;

    std::vector<int> itemsIDs;

    for(int i = 0; i <= 8; i ++)
    {
        if(craftSlotId3[i] != -1)
        {
            result += craftSlotId3[i];
            itemsIDs.push_back(craftSlotId3[i]);
        }

        if(craftSlotId3[i] >= BlackWoolBlock::getID() && craftSlotId3[i] <= WhiteWoolBlock::getID())
        {
            materialString += "W";
        }
        else if(craftSlotId3[i] == OakPlanks::getID() || craftSlotId3[i] == SprucePlanks::getID() || craftSlotId3[i] == BirchPlanks::getID())
        {
            materialString += "P";
        }
        else
        {
            materialString += "0";
        }
    }

    if(materialString == "000WWWPPP")
    {
        craftItemId3 = BedItem::getID();
        craftItemSt3 = 0;
        craftItemAm3 = 1;

        enableMainCraftBody = false;
    }

    if(materialString == "WWWPPP000")
    {
        craftItemId3 = BedItem::getID();
        craftItemSt3 = 0;
        craftItemAm3 = 1;

        enableMainCraftBody = false;
    }

    // carpets
    if(materialString == "WW0000000" || materialString == "0WW000000" ||
       materialString == "000WW0000" || materialString == "0000WW000" ||
       materialString == "000000WW0" || materialString == "0000000WW")
    {
        if(itemsIDs.size() == 2)
        {
            if(itemsIDs[0] == itemsIDs[1])
            {
                craftItemId3 = itemsIDs[0]+188;
                craftItemSt3 = 1;
                craftItemAm3 = 3;

                enableMainCraftBody = false;
            }
        }
    }


    if(itemsIDs.size() == 2) // dyes and colored wool crafting
    {
        if((itemsIDs[0] == RoseRed::getID() && itemsIDs[1] == DandelionYellow::getID()) ||
           (itemsIDs[1] == RoseRed::getID() && itemsIDs[0] == DandelionYellow::getID()))
        {
            craftItemId3 = OrangeDye::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == RoseRed::getID() && itemsIDs[1] == BoneMeal::getID()) ||
           (itemsIDs[1] == RoseRed::getID() && itemsIDs[0] == BoneMeal::getID()))
        {
            craftItemId3 = PinkDye::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == CactusGreen::getID() && itemsIDs[1] == BoneMeal::getID()) ||
           (itemsIDs[1] == CactusGreen::getID() && itemsIDs[0] == BoneMeal::getID()))
        {
            craftItemId3 = LimeDye::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == LapisLazuli::getID() && itemsIDs[1] == BoneMeal::getID()) ||
           (itemsIDs[1] == LapisLazuli::getID() && itemsIDs[0] == BoneMeal::getID()))
        {
            craftItemId3 = LightBlueDye::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == LapisLazuli::getID() && itemsIDs[1] == RoseRed::getID()) ||
           (itemsIDs[1] == LapisLazuli::getID() && itemsIDs[0] == RoseRed::getID()))
        {
            craftItemId3 = PurpleDye::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == PurpleDye::getID() && itemsIDs[1] == PinkDye::getID()) ||
           (itemsIDs[1] == PurpleDye::getID() && itemsIDs[0] == PinkDye::getID()))
        {
            craftItemId3 = MagnetaDye::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == LapisLazuli::getID() && itemsIDs[1] == RoseRed::getID()) ||
           (itemsIDs[1] == LapisLazuli::getID() && itemsIDs[0] == RoseRed::getID()))
        {
            craftItemId3 = PurpleDye::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == InkSac::getID() && itemsIDs[1] == BoneMeal::getID()) ||
           (itemsIDs[1] == InkSac::getID() && itemsIDs[0] == BoneMeal::getID()))
        {
            craftItemId3 = GrayDye::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == GrayDye::getID() && itemsIDs[1] == BoneMeal::getID()) ||
           (itemsIDs[1] == GrayDye::getID() && itemsIDs[0] == BoneMeal::getID()))
        {
            craftItemId3 = LightGrayDye::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 2;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == InkSac::getID() && itemsIDs[1] == OrangeDye::getID()) ||
           (itemsIDs[1] == InkSac::getID() && itemsIDs[0] == OrangeDye::getID()))
        {
            craftItemId3 = CocoaBeans::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }


        /// wool


        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == RoseRed::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == RoseRed::getID()))
        {
            craftItemId3 = RedWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == DandelionYellow::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == DandelionYellow::getID()))
        {
            craftItemId3 = YellowWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == OrangeDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == OrangeDye::getID()))
        {
            craftItemId3 = OrangeWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == CactusGreen::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == CactusGreen::getID()))
        {
            craftItemId3 = DarkGreenWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == LimeDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == LimeDye::getID()))
        {
            craftItemId3 = GreenWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == LapisLazuli::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == LapisLazuli::getID()))
        {
            craftItemId3 = BlueWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == LightBlueDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == LightBlueDye::getID()))
        {
            craftItemId3 = LightBlueWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == PurpleDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == PurpleDye::getID()))
        {
            craftItemId3 = VioletWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == MagnetaDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == MagnetaDye::getID()))
        {
            craftItemId3 = PastelWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == InkSac::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == InkSac::getID()))
        {
            craftItemId3 = BlackWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == GrayDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == GrayDye::getID()))
        {
            craftItemId3 = GrayWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }

        if((itemsIDs[0] == WhiteWoolBlock::getID() && itemsIDs[1] == LightGrayDye::getID()) ||
           (itemsIDs[1] == WhiteWoolBlock::getID() && itemsIDs[0] == LightGrayDye::getID()))
        {
            craftItemId3 = LightGrayWoolBlock::getID();
            craftItemSt3 = 1;
            craftItemAm3 = 1;

            enableMainCraftBody = false;
        }
    }

    if(itemsIDs.size() == 3)
    {
        if(result == RoseRed::getID() + PinkDye::getID() + LapisLazuli::getID())
        {
            if(itemsIDs[0] == RoseRed::getID() || itemsIDs[1] == RoseRed::getID() || itemsIDs[2] == RoseRed::getID())
            {
                craftItemId3 = MagnetaDye::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 3;

                enableMainCraftBody = false;
            }
        }
    }

    itemsIDs.clear();

    if(enableMainCraftBody)
    {
        switch(result)
        {
        case 321://bone to bone meals
            if(craftSlotId3[0] == 321 || craftSlotId3[1] == 321 || craftSlotId3[2] == 321 || craftSlotId3[3] == 321 || craftSlotId3[4] == 321 || craftSlotId3[5] == 321 || craftSlotId3[6] == 321 || craftSlotId3[7] == 321 || craftSlotId3[8] == 321)
            {
                craftItemId3 = 322;
                craftItemSt3 = 1;
                craftItemAm3 = 3;
            }
        break;

        case 294: //rose to dye
            if(craftSlotId3[0] == Flower1Item::getID() || craftSlotId3[1] == Flower1Item::getID() || craftSlotId3[2] == Flower1Item::getID() ||
               craftSlotId3[3] == Flower1Item::getID() || craftSlotId3[4] == Flower1Item::getID() || craftSlotId3[5] == Flower1Item::getID() ||
               craftSlotId3[6] == Flower1Item::getID() || craftSlotId3[7] == Flower1Item::getID() || craftSlotId3[8] == Flower1Item::getID())
            {
                craftItemId3 = RoseRed::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 2;
            }
        break;

        case 295: //rose to dye
            if(craftSlotId3[0] == Flower2Item::getID() || craftSlotId3[1] == Flower2Item::getID() || craftSlotId3[2] == Flower2Item::getID() ||
               craftSlotId3[3] == Flower2Item::getID() || craftSlotId3[4] == Flower2Item::getID() || craftSlotId3[5] == Flower2Item::getID() ||
               craftSlotId3[6] == Flower2Item::getID() || craftSlotId3[7] == Flower2Item::getID() || craftSlotId3[8] == Flower2Item::getID())
            {
                craftItemId3 = DandelionYellow::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 2;
            }
        break;

        case 625: //noteblock (oak)
            if(craftSlotId3[1] == 34 && craftSlotId3[2] == 34 && craftSlotId3[3] == 34 && craftSlotId3[5] == 34 && craftSlotId3[7] == 34 && craftSlotId3[0] == 34 && craftSlotId3[6] == 34 && craftSlotId3[8] == 34 && craftSlotId3[4] == Redstone::getID())
            {
                craftItemId3 = NoteBlock::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1593: //noteblock (birch)
            if(craftSlotId3[1] == 155 && craftSlotId3[2] == 155 && craftSlotId3[3] == 155 && craftSlotId3[5] == 155 && craftSlotId3[7] == 155 && craftSlotId3[0] == 155 && craftSlotId3[6] == 155 && craftSlotId3[8] == 155 && craftSlotId3[4] == Redstone::getID())
            {
                craftItemId3 = NoteBlock::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1601: //noteblock (spruce)
            if(craftSlotId3[1] == 156 && craftSlotId3[2] == 156 && craftSlotId3[3] == 156 && craftSlotId3[5] == 156 && craftSlotId3[7] == 156 && craftSlotId3[0] == 156 && craftSlotId3[6] == 156 && craftSlotId3[8] == 156 && craftSlotId3[4] == Redstone::getID())
            {
                craftItemId3 = NoteBlock::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;
		
		case 2049: //arco
            if(craftSlotId3[1] == 276 && craftSlotId3[3] == 276 && craftSlotId3[7] == 276 && craftSlotId3[2] == 407 && craftSlotId3[5] == 407 && craftSlotId3[8] == 407)
            {
                craftItemId3 = Bow0::getID();
                craftItemSt3 = 0;
                craftItemAm3 = 50;
            }
        break;
		
		case 1025: //flecha
		if(craftSlotId3[1] == 351 && craftSlotId3[4] == 276 && craftSlotId3[7] == 398)
		{
			craftItemId3 = Arrow::getID();
			craftItemSt3 = 1;
			craftItemAm3 = 3;
		}
		break;
		
        case 551: //noteblock (oak)
            if(craftSlotId3[1] == 34 && craftSlotId3[2] == 34 && craftSlotId3[3] == 34 && craftSlotId3[5] == 34 && craftSlotId3[7] == 34 && craftSlotId3[0] == 34 && craftSlotId3[6] == 34 && craftSlotId3[8] == 34 && craftSlotId3[4] == DiamondItem::getID())
            {
                craftItemId3 = JukeBox::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1519: //noteblock (birch)
            if(craftSlotId3[1] == 155 && craftSlotId3[2] == 155 && craftSlotId3[3] == 155 && craftSlotId3[5] == 155 && craftSlotId3[7] == 155 && craftSlotId3[0] == 155 && craftSlotId3[6] == 155 && craftSlotId3[8] == 155 && craftSlotId3[4] == DiamondItem::getID())
            {
                craftItemId3 = JukeBox::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1527: //noteblock (spruce)
            if(craftSlotId3[1] == 156 && craftSlotId3[2] == 156 && craftSlotId3[3] == 156 && craftSlotId3[5] == 156 && craftSlotId3[7] == 156 && craftSlotId3[0] == 156 && craftSlotId3[6] == 156 && craftSlotId3[8] == 156 && craftSlotId3[4] == DiamondItem::getID())
            {
                craftItemId3 = JukeBox::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 8: //wood to planks
            if(craftSlotId3[0] == 8 || craftSlotId3[1] == 8 || craftSlotId3[2] == 8 || craftSlotId3[3] == 8 || craftSlotId3[4] == 8 || craftSlotId3[5] == 8 || craftSlotId3[6] == 8 || craftSlotId3[7] == 8 || craftSlotId3[8] == 8)
            {
                craftItemId3 = 34;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;

        case 31://wood to birch planks
            if(craftSlotId3[0] == 31 || craftSlotId3[1] == 31 || craftSlotId3[2] == 31 || craftSlotId3[3] == 31 || craftSlotId3[4] == 31 || craftSlotId3[5] == 31 || craftSlotId3[6] == 31 || craftSlotId3[7] == 31 || craftSlotId3[8] == 31)
            {
                craftItemId3 = 155;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;
        case 30://wood to spurce planks
            if(craftSlotId3[0] == 30 || craftSlotId3[1] == 30 || craftSlotId3[2] == 30 || craftSlotId3[3] == 30 || craftSlotId3[4] == 30 || craftSlotId3[5] == 30 || craftSlotId3[6] == 30 || craftSlotId3[7] == 30 || craftSlotId3[8] == 30)
            {
                craftItemId3 = 156;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;
        case 311://melon to seeds
            if(craftSlotId3[0] == 311 || craftSlotId3[1] == 311 || craftSlotId3[2] == 311 || craftSlotId3[3] == 311 || craftSlotId3[4] == 311 || craftSlotId3[5] == 311 || craftSlotId3[6] == 311 || craftSlotId3[7] == 311 || craftSlotId3[8] == 311)
            {
                craftItemId3 = 312;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 293://cane to sugar
            if(craftSlotId3[0] == 293 || craftSlotId3[1] == 293 || craftSlotId3[2] == 293 || craftSlotId3[3] == 293 || craftSlotId3[4] == 293 || craftSlotId3[5] == 293 || craftSlotId3[6] == 293 || craftSlotId3[7] == 293 || craftSlotId3[8] == 293)
            {
                craftItemId3 = 313;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 68://sticks
            if( (craftSlotId3[0] == 34 && craftSlotId3[3] == 34)  || (craftSlotId3[1] == 34 && craftSlotId3[4] == 34) || (craftSlotId3[2] == 34 && craftSlotId3[5] == 34) || (craftSlotId3[3] == 34 && craftSlotId3[6] == 34) || (craftSlotId3[4] == 34 && craftSlotId3[7] == 34) || (craftSlotId3[5] == 34 && craftSlotId3[8] == 34))
            {
                craftItemId3 = 276;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;
        case 310: //birch
            if( (craftSlotId3[0] == 155 && craftSlotId3[3] == 155)  || (craftSlotId3[1] == 155 && craftSlotId3[4] == 155) || (craftSlotId3[2] == 155 && craftSlotId3[5] == 155) || (craftSlotId3[3] == 155 && craftSlotId3[6] == 155) || (craftSlotId3[4] == 155 && craftSlotId3[7] == 155) || (craftSlotId3[5] == 155 && craftSlotId3[8] == 155))
            {
                craftItemId3 = 276;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;
        case 312://spruce
            if( (craftSlotId3[0] == 156 && craftSlotId3[3] == 156)  || (craftSlotId3[1] == 156 && craftSlotId3[4] == 156) || (craftSlotId3[2] == 156 && craftSlotId3[5] == 156) || (craftSlotId3[3] == 156 && craftSlotId3[6] == 156) || (craftSlotId3[4] == 156 && craftSlotId3[7] == 156) || (craftSlotId3[5] == 156 && craftSlotId3[8] == 156))
            {
                craftItemId3 = 276;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;

        case 688: // oak fence
            if((craftSlotId3[0] == 34 && craftSlotId3[1] == 276 && craftSlotId3[2] == 34 && craftSlotId3[3] == 34 && craftSlotId3[4] == 276 && craftSlotId3[5] == 34) || (craftSlotId3[3] == 34 && craftSlotId3[4] == 276 && craftSlotId3[5] == 34 && craftSlotId3[6] == 34 && craftSlotId3[7] == 276 && craftSlotId3[8] == 34))
            {
                craftItemId3 = WoodenFence::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 2;
            }
        break;
        case 1172: // birch fence
            if((craftSlotId3[0] == 155 && craftSlotId3[1] == 276 && craftSlotId3[2] == 155 && craftSlotId3[3] == 155 && craftSlotId3[4] == 276 && craftSlotId3[5] == 155) || (craftSlotId3[3] == 155 && craftSlotId3[4] == 276 && craftSlotId3[5] == 155 && craftSlotId3[6] == 155 && craftSlotId3[7] == 276 && craftSlotId3[8] == 155))
            {
                craftItemId3 = BirchFence::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 2;
            }
        break;
        case 1176: // spruce fence
            if((craftSlotId3[0] == 156 && craftSlotId3[1] == 276 && craftSlotId3[2] == 156 && craftSlotId3[3] == 156 && craftSlotId3[4] == 276 && craftSlotId3[5] == 156) || (craftSlotId3[3] == 156 && craftSlotId3[4] == 276 && craftSlotId3[5] == 156 && craftSlotId3[6] == 156 && craftSlotId3[7] == 276 && craftSlotId3[8] == 156))
            {
                craftItemId3 = SpruceFence::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 2;
            }
        break;

        case 240: //glass panel
            if((craftSlotId3[0] == 40 && craftSlotId3[1] == 40 && craftSlotId3[2] == 40 && craftSlotId3[3] == 40 && craftSlotId3[4] == 40 && craftSlotId3[5] == 40)
            || (craftSlotId3[3] == 40 && craftSlotId3[4] == 40 && craftSlotId3[5] == 40 && craftSlotId3[6] == 40 && craftSlotId3[7] == 40 && craftSlotId3[8] == 40))
            {
                craftItemId3 = GlassPanelItem::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 16;
            }
        break;

        case 1668: //iron bars
            if((craftSlotId3[0] == 278 && craftSlotId3[1] == 278 && craftSlotId3[2] == 278 && craftSlotId3[3] == 278 && craftSlotId3[4] == 278 && craftSlotId3[5] == 278)
            || (craftSlotId3[3] == 278 && craftSlotId3[4] == 278 && craftSlotId3[5] == 278 && craftSlotId3[6] == 278 && craftSlotId3[7] == 278 && craftSlotId3[8] == 278))
            {
                craftItemId3 = IronBarsItem::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 16;
            }
        break;

        case 204: //door
            if((craftSlotId3[0] == 34 && craftSlotId3[1] == 34 && craftSlotId3[3] == 34 && craftSlotId3[4] == 34 && craftSlotId3[6] == 34 && craftSlotId3[7] == 34) || (craftSlotId3[1] == 34 && craftSlotId3[2] == 34 && craftSlotId3[4] == 34 && craftSlotId3[5] == 34 && craftSlotId3[7] == 34 && craftSlotId3[8] == 34))
            {
                craftItemId3 = 306;
                craftItemSt3 = 0;
                craftItemAm3 = 1;
            }
            //wooden stair
            if((craftSlotId3[0] == 34 && craftSlotId3[3] == 34 && craftSlotId3[4] == 34 && craftSlotId3[6] == 34 && craftSlotId3[7] == 34 && craftSlotId3[8] == 34) ||
               (craftSlotId3[2] == 34 && craftSlotId3[4] == 34 && craftSlotId3[5] == 34 && craftSlotId3[6] == 34 && craftSlotId3[7] == 34 && craftSlotId3[8] == 34))
            {
                craftItemId3 = 125;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
            //trapdoor
            if((craftSlotId3[0] == 34 && craftSlotId3[1] == 34 && craftSlotId3[2] == 34 && craftSlotId3[3] == 34 && craftSlotId3[4] == 34 && craftSlotId3[5] == 34) ||
               (craftSlotId3[3] == 34 && craftSlotId3[4] == 34 && craftSlotId3[5] == 34 && craftSlotId3[6] == 34 && craftSlotId3[7] == 34 && craftSlotId3[8] == 34))
            {
                craftItemId3 = TrapdoorClosed1::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 2;
            }
        break;

        case 930: //door (birch)
            if((craftSlotId3[0] == 155 && craftSlotId3[1] == 155 && craftSlotId3[3] == 155 && craftSlotId3[4] == 155 && craftSlotId3[6] == 155 && craftSlotId3[7] == 155) || (craftSlotId3[1] == 155 && craftSlotId3[2] == 155 && craftSlotId3[4] == 155 && craftSlotId3[5] == 155 && craftSlotId3[7] == 155 && craftSlotId3[8] == 155))
            {
                craftItemId3 = 306;
                craftItemSt3 = 0;
                craftItemAm3 = 1;
            }
            //wooden stair
            if((craftSlotId3[0] == 155 && craftSlotId3[3] == 155 && craftSlotId3[4] == 155 && craftSlotId3[6] == 155 && craftSlotId3[7] == 155 && craftSlotId3[8] == 155) ||
               (craftSlotId3[2] == 155 && craftSlotId3[4] == 155 && craftSlotId3[5] == 155 && craftSlotId3[6] == 155 && craftSlotId3[7] == 155 && craftSlotId3[8] == 155))
            {
                craftItemId3 = BirchStair1::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
            //trapdoor
            if((craftSlotId3[0] == 155 && craftSlotId3[1] == 155 && craftSlotId3[2] == 155 && craftSlotId3[3] == 155 && craftSlotId3[4] == 155 && craftSlotId3[5] == 155) ||
               (craftSlotId3[3] == 155 && craftSlotId3[4] == 155 && craftSlotId3[5] == 155 && craftSlotId3[6] == 155 && craftSlotId3[7] == 155 && craftSlotId3[8] == 155))
            {
                craftItemId3 = TrapdoorClosed1::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 2;
            }
        break;


        case 936: //door (spruce)
            if((craftSlotId3[0] == 156 && craftSlotId3[1] == 156 && craftSlotId3[3] == 156 && craftSlotId3[4] == 156 && craftSlotId3[6] == 156 && craftSlotId3[7] == 156) || (craftSlotId3[1] == 156 && craftSlotId3[2] == 156 && craftSlotId3[4] == 156 && craftSlotId3[5] == 156 && craftSlotId3[7] == 156 && craftSlotId3[8] == 156))
            {
                craftItemId3 = 306;
                craftItemSt3 = 0;
                craftItemAm3 = 1;
            }
            //wooden stair
            if((craftSlotId3[0] == 156 && craftSlotId3[3] == 156 && craftSlotId3[4] == 156 && craftSlotId3[6] == 156 && craftSlotId3[7] == 156 && craftSlotId3[8] == 156) ||
               (craftSlotId3[2] == 156 && craftSlotId3[4] == 156 && craftSlotId3[5] == 156 && craftSlotId3[6] == 156 && craftSlotId3[7] == 156 && craftSlotId3[8] == 156))
            {
                craftItemId3 = SpruceStair1::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
            //trapdoor
            if((craftSlotId3[0] == 156 && craftSlotId3[1] == 156 && craftSlotId3[2] == 156 && craftSlotId3[3] == 156 && craftSlotId3[4] == 156 && craftSlotId3[5] == 156) ||
               (craftSlotId3[3] == 156 && craftSlotId3[4] == 156 && craftSlotId3[5] == 156 && craftSlotId3[6] == 156 && craftSlotId3[7] == 156 && craftSlotId3[8] == 156))
            {
                craftItemId3 = TrapdoorClosed1::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 2;
            }
        break;


        case 216://stone stair
            if((craftSlotId3[0] == 36 && craftSlotId3[3] == 36 && craftSlotId3[4] == 36 && craftSlotId3[6] == 36 && craftSlotId3[7] == 36 && craftSlotId3[8] == 36) ||
               (craftSlotId3[2] == 36 && craftSlotId3[4] == 36 && craftSlotId3[5] == 36 && craftSlotId3[6] == 36 && craftSlotId3[7] == 36 && craftSlotId3[8] == 36))
            {
                craftItemId3 = 60;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
            // decorated stone brick
            if((craftSlotId3[0] == 108 && craftSlotId3[3] == 108) || (craftSlotId3[1] == 108 && craftSlotId3[4] == 108) || (craftSlotId3[2] == 108 && craftSlotId3[5] == 108) || (craftSlotId3[3] == 108 && craftSlotId3[6] == 108) || (craftSlotId3[4] == 108 && craftSlotId3[7] == 108) || (craftSlotId3[5] == 108 && craftSlotId3[8] == 108))
            {
                craftItemId3 = 104;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 162://brick stair
            if((craftSlotId3[0] == 27 && craftSlotId3[3] == 27 && craftSlotId3[4] == 27 && craftSlotId3[6] == 27 && craftSlotId3[7] == 27 && craftSlotId3[8] == 27) ||
               (craftSlotId3[2] == 27 && craftSlotId3[4] == 27 && craftSlotId3[5] == 27 && craftSlotId3[6] == 27 && craftSlotId3[7] == 27 && craftSlotId3[8] == 27))
            {
                craftItemId3 = 67;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;

        case 136://crafting
            if((craftSlotId3[0] == 34 && craftSlotId3[1] == 34 && craftSlotId3[3] == 34 && craftSlotId3[4] == 34) || (craftSlotId3[1] == 34 && craftSlotId3[2] == 34 && craftSlotId3[4] == 34 && craftSlotId3[5] == 34) || (craftSlotId3[3] == 34 && craftSlotId3[4] == 34 && craftSlotId3[6] == 34 && craftSlotId3[7] == 34) || (craftSlotId3[4] == 34 && craftSlotId3[5] == 34 && craftSlotId3[7] == 34 && craftSlotId3[8] == 34))
            {
                craftItemId3 = 105;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1196://snow block
            if((craftSlotId3[0] == 299 && craftSlotId3[1] == 299 && craftSlotId3[3] == 299 && craftSlotId3[4] == 299) || (craftSlotId3[1] == 299 && craftSlotId3[2] == 299 && craftSlotId3[4] == 299 && craftSlotId3[5] == 299) || (craftSlotId3[3] == 299 && craftSlotId3[4] == 299 && craftSlotId3[6] == 299 && craftSlotId3[7] == 299) || (craftSlotId3[4] == 299 && craftSlotId3[5] == 299 && craftSlotId3[7] == 299 && craftSlotId3[8] == 299))
            {
                craftItemId3 = 47;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 28: //stonesand from sand
            if((craftSlotId3[0] == 28 && craftSlotId3[1] == 28 && craftSlotId3[3] == 28 && craftSlotId3[4] == 28) || (craftSlotId3[1] == 28 && craftSlotId3[2] == 28 && craftSlotId3[4] == 28 && craftSlotId3[5] == 28) || (craftSlotId3[3] == 28 && craftSlotId3[4] == 28 && craftSlotId3[6] == 28 && craftSlotId3[7] == 28) || (craftSlotId3[4] == 28 && craftSlotId3[5] == 28 && craftSlotId3[7] == 28 && craftSlotId3[8] == 28))
            {
                craftItemId3 = 26;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1156: //brick block
            if((craftSlotId3[0] == 289 && craftSlotId3[1] == 289 && craftSlotId3[3] == 289 && craftSlotId3[4] == 289) || (craftSlotId3[1] == 289 && craftSlotId3[2] == 289 && craftSlotId3[4] == 289 && craftSlotId3[5] == 289) || (craftSlotId3[3] == 289 && craftSlotId3[4] == 289 && craftSlotId3[6] == 289 && craftSlotId3[7] == 289) || (craftSlotId3[4] == 289 && craftSlotId3[5] == 289 && craftSlotId3[7] == 289 && craftSlotId3[8] == 289))
            {
                craftItemId3 = 27;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 12: //stonebricks
            if((craftSlotId3[0] == 3 && craftSlotId3[1] == 3 && craftSlotId3[3] == 3 && craftSlotId3[4] == 3) || (craftSlotId3[1] == 3 && craftSlotId3[2] == 3 && craftSlotId3[4] == 3 && craftSlotId3[5] == 3) || (craftSlotId3[3] == 3 && craftSlotId3[4] == 3 && craftSlotId3[6] == 3 && craftSlotId3[7] == 3) || (craftSlotId3[4] == 3 && craftSlotId3[5] == 3 && craftSlotId3[7] == 3 && craftSlotId3[8] == 3))
            {
                craftItemId3 = 102;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;

        case 553://torches
            if((craftSlotId3[0] == 277 && craftSlotId3[3] == 276) || (craftSlotId3[1] == 277 && craftSlotId3[4] == 276) || (craftSlotId3[2] == 277 && craftSlotId3[5] == 276) || (craftSlotId3[3] == 277 && craftSlotId3[6] == 276) || (craftSlotId3[4] == 277 && craftSlotId3[7] == 276) || (craftSlotId3[5] == 277 && craftSlotId3[8] == 276))
            {
                craftItemId3 = 308;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;

        case 383://jack o'lantern
            if((craftSlotId3[0] == Pumpkin3::getID() && craftSlotId3[3] == TorchItem::getID()) || (craftSlotId3[1] == Pumpkin3::getID() && craftSlotId3[4] == TorchItem::getID()) || (craftSlotId3[2] == Pumpkin3::getID() && craftSlotId3[5] == TorchItem::getID()) || (craftSlotId3[3] == Pumpkin3::getID() && craftSlotId3[6] == TorchItem::getID()) || (craftSlotId3[4] == Pumpkin3::getID() && craftSlotId3[7] == TorchItem::getID()) || (craftSlotId3[5] == Pumpkin3::getID() && craftSlotId3[8] == TorchItem::getID()))
            {
                craftItemId3 = JackOLantern3::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 556://scissors
            if((craftSlotId3[1] == 278 && craftSlotId3[3] == 278) || (craftSlotId3[2] == 278 && craftSlotId3[4] == 278) || (craftSlotId3[4] == 278 && craftSlotId3[6] == 278) || (craftSlotId3[5] == 278 && craftSlotId3[7] == 278))
            {
                craftItemId3 = 275;
                craftItemSt3 = 0;
                craftItemAm3 = 239;
            }
        break;

        case 1132://clay
            if((craftSlotId3[0] == 283 && craftSlotId3[1] == 283 && craftSlotId3[3] == 283 && craftSlotId3[4] == 283) || (craftSlotId3[1] == 283 && craftSlotId3[2] == 283 && craftSlotId3[4] == 283 && craftSlotId3[5] == 283) || (craftSlotId3[3] == 283 && craftSlotId3[4] == 283 && craftSlotId3[6] == 283 && craftSlotId3[7] == 283) || (craftSlotId3[4] == 283 && craftSlotId3[5] == 283 && craftSlotId3[7] == 283 && craftSlotId3[8] == 283))
            {
                craftItemId3 = 99;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;
		
		case 1628 :// string to white wool
            if((craftSlotId3[0] == 407 && craftSlotId3[1] == 407 && craftSlotId3[3] == 407 && craftSlotId3[4] == 407) || (craftSlotId3[1] == 407 && craftSlotId3[2] == 407 && craftSlotId3[4] == 407 && craftSlotId3[5] == 407) || (craftSlotId3[3] == 407 && craftSlotId3[4] == 407 && craftSlotId3[6] == 407 && craftSlotId3[7] == 407) || (craftSlotId3[4] == 407 && craftSlotId3[5] == 407 && craftSlotId3[7] == 407 && craftSlotId3[8] == 407))
            {
                craftItemId3 = WhiteWoolBlock::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        /// HALF BLOCKS
        case 108://cobblestone
            if((craftSlotId3[0] == 36 && craftSlotId3[1] == 36 && craftSlotId3[2] == 36) || (craftSlotId3[3] == 36 && craftSlotId3[4] == 36 && craftSlotId3[5] == 36) || (craftSlotId3[6] == 36 && craftSlotId3[7] == 36 && craftSlotId3[8] == 36))
            {
                craftItemId3 = 83;
                craftItemSt3 = 1;
                craftItemAm3 = 6;
            }
        break;

        case 102://half planks
            if((craftSlotId3[0] == 34 && craftSlotId3[1] == 34 && craftSlotId3[2] == 34) ||
               (craftSlotId3[3] == 34 && craftSlotId3[4] == 34 && craftSlotId3[5] == 34) ||
               (craftSlotId3[6] == 34 && craftSlotId3[7] == 34 && craftSlotId3[8] == 34))
            {
                craftItemId3 = 82;
                craftItemSt3 = 1;
                craftItemAm3 = 6;
            }
            //4 bowls
            if((craftSlotId3[0] == 34 && craftSlotId3[4] == 34 && craftSlotId3[2] == 34) ||
               (craftSlotId3[3] == 34 && craftSlotId3[7] == 34 && craftSlotId3[5] == 34))
            {
                craftItemId3 = 302;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;

        case 465://half birch planks
            if((craftSlotId3[0] == 155 && craftSlotId3[1] == 155 && craftSlotId3[2] == 155) ||
               (craftSlotId3[3] == 155 && craftSlotId3[4] == 155 && craftSlotId3[5] == 155) ||
               (craftSlotId3[6] == 155 && craftSlotId3[7] == 155 && craftSlotId3[8] == 155))
            {
                craftItemId3 = BirchHalfBlock::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 6;
            }
            //4 bowls
            if((craftSlotId3[0] == 155 && craftSlotId3[4] == 155 && craftSlotId3[2] == 155) ||
               (craftSlotId3[3] == 155 && craftSlotId3[7] == 155 && craftSlotId3[5] == 155))
            {
                craftItemId3 = 302;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;

        case 468://half spruce planks
            if((craftSlotId3[0] == 156 && craftSlotId3[1] == 156 && craftSlotId3[2] == 156) ||
               (craftSlotId3[3] == 156 && craftSlotId3[4] == 156 && craftSlotId3[5] == 156) ||
               (craftSlotId3[6] == 156 && craftSlotId3[7] == 156 && craftSlotId3[8] == 156))
            {
                craftItemId3 = SpruceHalfBlock::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 6;
            }
            //4 bowls
            if((craftSlotId3[0] == 156 && craftSlotId3[4] == 156 && craftSlotId3[2] == 156) ||
               (craftSlotId3[3] == 156 && craftSlotId3[7] == 156 && craftSlotId3[5] == 156))
            {
                craftItemId3 = 302;
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;

        case 9://stone
            if((craftSlotId3[0] == 3 && craftSlotId3[1] == 3 && craftSlotId3[2] == 3) || (craftSlotId3[3] == 3 && craftSlotId3[4] == 3 && craftSlotId3[5] == 3) || (craftSlotId3[6] == 3 && craftSlotId3[7] == 3 && craftSlotId3[8] == 3))
            {
                craftItemId3 = 85;
                craftItemSt3 = 1;
                craftItemAm3 = 6;
            }
        break;

        case 78://stonesand half block
            if((craftSlotId3[0] == 26 && craftSlotId3[1] == 26 && craftSlotId3[2] == 26) || (craftSlotId3[3] == 26 && craftSlotId3[4] == 26 && craftSlotId3[5] == 26) || (craftSlotId3[6] == 26 && craftSlotId3[7] == 26 && craftSlotId3[8] == 26))
            {
                craftItemId3 = 86;
                craftItemSt3 = 1;
                craftItemAm3 = 6;
            }
        break;

        case 306: //stonebrick half block
            if((craftSlotId3[0] == 102 && craftSlotId3[1] == 102 && craftSlotId3[2] == 102) || (craftSlotId3[3] == 102 && craftSlotId3[4] == 102 && craftSlotId3[5] == 102) || (craftSlotId3[6] == 102 && craftSlotId3[7] == 102 && craftSlotId3[8] == 102))
            {
                craftItemId3 = 108;
                craftItemSt3 = 1;
                craftItemAm3 = 6;
            }
        break;


        // end
        case 903://mooshroom bowl
            if((craftSlotId3[1] == 300 && craftSlotId3[4] == 301 && craftSlotId3[7] == 302) || (craftSlotId3[1] == 301 && craftSlotId3[4] == 300 && craftSlotId3[7] == 302))
            {
                craftItemId3 = 303;
                craftItemSt3 = 0;
                craftItemAm3 = 1;
            }
        break;

        /// Tool crafts start
        //wooden tools (oak)
        case 654:
            if(craftSlotId3[1] == 34 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 34 && craftSlotId3[2] == 34) // pick
            {
                craftItemId3 = 250;
                craftItemSt3 = 0;
                craftItemAm3 = 60;

            }
            if(craftSlotId3[0] == 34 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[1] == 34 && craftSlotId3[3] == 34) // axe
            {
                craftItemId3 = 265;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
        break;

        case 620:
            if(craftSlotId3[1] == 34 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 34) // hoe
            {
                craftItemId3 = 270;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
            // crafting table (birch)
            if((craftSlotId3[0] == 155 && craftSlotId3[1] == 155 && craftSlotId3[3] == 155 && craftSlotId3[4] == 155) ||
               (craftSlotId3[1] == 155 && craftSlotId3[2] == 155 && craftSlotId3[4] == 155 && craftSlotId3[5] == 155) ||
               (craftSlotId3[3] == 155 && craftSlotId3[4] == 155 && craftSlotId3[6] == 155 && craftSlotId3[7] == 155) ||
               (craftSlotId3[4] == 155 && craftSlotId3[5] == 155 && craftSlotId3[7] == 155 && craftSlotId3[8] == 155))
            {
                craftItemId3 = 105;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 586:
            if(craftSlotId3[1] == 34 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276) // shovel
            {
                craftItemId3 = 260;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
            if(craftSlotId3[1] == 155 && craftSlotId3[4] == 155 && craftSlotId3[7] == 276) // sword
            {
                craftItemId3 = 255;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
        break;

        case 344:
            if(craftSlotId3[1] == 34 && craftSlotId3[4] == 34 && craftSlotId3[7] == 276) // sword
            {
                craftItemId3 = 255;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
        break;

        //wooden tools (birch)
        case 1017:
            if(craftSlotId3[1] == 155 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 155 && craftSlotId3[2] == 155) // pick
            {
                craftItemId3 = 250;
                craftItemSt3 = 0;
                craftItemAm3 = 60;

            }
            if(craftSlotId3[0] == 155 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[1] == 155 && craftSlotId3[3] == 155) // axe
            {
                craftItemId3 = 265;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
        break;

        case 862:
            if(craftSlotId3[1] == 155 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 155) // hoe
            {
                craftItemId3 = 270;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
        break;

        case 707:
            if(craftSlotId3[1] == 155 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276) // shovel
            {
                craftItemId3 = 260;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
        break;

        //wooden tools (spruce)
        case 1020:
            if(craftSlotId3[1] == 156 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 156 && craftSlotId3[2] == 156) // pick
            {
                craftItemId3 = 250;
                craftItemSt3 = 0;
                craftItemAm3 = 60;

            }
            if(craftSlotId3[0] == 156 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[1] == 156 && craftSlotId3[3] == 156) // axe
            {
                craftItemId3 = 265;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
        break;

        case 864:
            if(craftSlotId3[1] == 156 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 156) // hoe
            {
                craftItemId3 = 270;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
        break;

        case 708:
            if(craftSlotId3[1] == 156 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276) // shovel
            {
                craftItemId3 = 260;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
        break;

        //stone tools
        case 660:
            if(craftSlotId3[1] == 36 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 36 && craftSlotId3[2] == 36)
            {
                craftItemId3 = 251;
                craftItemSt3 = 0;
                craftItemAm3 = 132;

            }
            if(craftSlotId3[0] == 36 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[1] == 36 && craftSlotId3[3] == 36)
            {
                craftItemId3 = 266;
                craftItemSt3 = 0;
                craftItemAm3 = 132;

            }
            break;

        case 624:
            if(craftSlotId3[1] == 36 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 36)
            {
                craftItemId3 = 271;
                craftItemSt3 = 0;
                craftItemAm3 = 132;
            }
            if((craftSlotId3[0] == 156 && craftSlotId3[1] == 156 && craftSlotId3[3] == 156 && craftSlotId3[4] == 156) ||
               (craftSlotId3[1] == 156 && craftSlotId3[2] == 156 && craftSlotId3[4] == 156 && craftSlotId3[5] == 156) ||
               (craftSlotId3[3] == 156 && craftSlotId3[4] == 156 && craftSlotId3[6] == 156 && craftSlotId3[7] == 156) ||
               (craftSlotId3[4] == 156 && craftSlotId3[5] == 156 && craftSlotId3[7] == 156 && craftSlotId3[8] == 156))
            {
                craftItemId3 = 105;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 588:
            if(craftSlotId3[1] == 36 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276)
            {
                craftItemId3 = 261;
                craftItemSt3 = 0;
                craftItemAm3 = 132;
            }
            if(craftSlotId3[1] == 156 && craftSlotId3[4] == 156 && craftSlotId3[7] == 276) // sword
            {
                craftItemId3 = 255;
                craftItemSt3 = 0;
                craftItemAm3 = 60;
            }
        break;

        case 348:
            if(craftSlotId3[1] == 36 && craftSlotId3[4] == 36 && craftSlotId3[7] == 276)
            {
                craftItemId3 = 256;
                craftItemSt3 = 0;
                craftItemAm3 = 132;
            }
        break;

        //iron tools
        case 1386:
            if(craftSlotId3[1] == 278 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 278 && craftSlotId3[2] == 278)
            {
                craftItemId3 = 252;
                craftItemSt3 = 0;
                craftItemAm3 = 251;

            }
            if(craftSlotId3[0] == 278 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[1] == 278 && craftSlotId3[3] == 278)
            {
                craftItemId3 = 267;
                craftItemSt3 = 0;
                craftItemAm3 = 251;

            }
        break;

        case 1108:
            if(craftSlotId3[1] == 278 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 278)
            {
                craftItemId3 = 272;
                craftItemSt3 = 0;
                craftItemAm3 = 251;
            }
        break;

        case 830:
            if(craftSlotId3[1] == 278 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276)
            {
                craftItemId3 = 262;
                craftItemSt3 = 0;
                craftItemAm3 = 251;
            }
        break;

        case 832:
            if(craftSlotId3[1] == 278 && craftSlotId3[4] == 278 && craftSlotId3[7] == 276)
            {
                craftItemId3 = 257;
                craftItemSt3 = 0;
                craftItemAm3 = 251;
            }
            if(craftSlotId3[1] == 280 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276)
            {
                craftItemId3 = 264;
                craftItemSt3 = 0;
                craftItemAm3 = 1562;
            }
        break;

        //diamond tools
        case 1389:
            if(craftSlotId3[1] == 279 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 279 && craftSlotId3[2] == 279)
            {
                craftItemId3 = 253;
                craftItemSt3 = 0;
                craftItemAm3 = 1562;

            }
            if(craftSlotId3[0] == 279 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[1] == 279 && craftSlotId3[3] == 279)
            {
                craftItemId3 = 268;
                craftItemSt3 = 0;
                craftItemAm3 = 1562;

            }
            break;

        case 1110:
            if(craftSlotId3[1] == 279 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 279)
            {
                craftItemId3 = 273;
                craftItemSt3 = 0;
                craftItemAm3 = 1562;
            }
        break;

        case 831:
            if(craftSlotId3[1] == 279 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276)
            {
                craftItemId3 = 263;
                craftItemSt3 = 0;
                craftItemAm3 = 1562;
            }
        break;

        case 834:
            if(craftSlotId3[1] == 279 && craftSlotId3[4] == 279 && craftSlotId3[7] == 276)
            {
                craftItemId3 = 258;
                craftItemSt3 = 0;
                craftItemAm3 = 1562;
            }

            if((craftSlotId3[0] == 278 && craftSlotId3[4] == 278 && craftSlotId3[2] == 278) || (craftSlotId3[3] == 278 && craftSlotId3[7] == 278 && craftSlotId3[5] == 278))
            {
                craftItemId3 = 290;
                craftItemSt3 = 0;
                craftItemAm3 = 1;
            }
        break;

        //golden tools
        case 1392:
            if(craftSlotId3[1] == 280 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 280 && craftSlotId3[2] == 280)
            {
                craftItemId3 = 254;
                craftItemSt3 = 0;
                craftItemAm3 = 33;

            }
            if(craftSlotId3[0] == 280 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[1] == 280 && craftSlotId3[3] == 280)
            {
                craftItemId3 = 269;
                craftItemSt3 = 0;
                craftItemAm3 = 33;

            }
            break;

        case 1112:
            if(craftSlotId3[1] == 280 && craftSlotId3[4] == 276 && craftSlotId3[7] == 276 && craftSlotId3[0] == 280)
            {
                craftItemId3 = 274;
                craftItemSt3 = 0;
                craftItemAm3 = 33;
            }

            if((craftSlotId3[0] == 278 && craftSlotId3[2] == 278 && craftSlotId3[3] == 278 && craftSlotId3[5] == 278) || (craftSlotId3[3] == 278 && craftSlotId3[5] == 278 && craftSlotId3[6] == 278 && craftSlotId3[8] == 278))
            {
                craftItemId3 = 339;
                craftItemAm3 = 195;
                craftItemSt3 = false;
            }
        break;

        case 836:
            if(craftSlotId3[1] == 280 && craftSlotId3[4] == 280 && craftSlotId3[7] == 276)
            {
                craftItemId3 = 259;
                craftItemSt3 = 0;
                craftItemAm3 = 33;
            }
        break;
        /// Tool crafts end

        /// Armor crafts begin
        // leather

        // helmet
        case 1615:
            if((craftSlotId3[0] == 323 && craftSlotId3[1] == 323 && craftSlotId3[2] == 323 && craftSlotId3[3] == 323 && craftSlotId3[5] == 323) || (craftSlotId3[3] == 323 && craftSlotId3[4] == 323 && craftSlotId3[5] == 323 && craftSlotId3[6] == 323 && craftSlotId3[8] == 323))
            {
                craftItemId3 = 328;
                craftItemAm3 = 55;
                craftItemSt3 = false;
            }
        break;

        // chestplate
        case 2584:
            if(craftSlotId3[0] == 323 && craftSlotId3[2] == 323 && craftSlotId3[3] == 323 && craftSlotId3[4] == 323 && craftSlotId3[5] == 323 && craftSlotId3[6] == 323 && craftSlotId3[7] == 323 && craftSlotId3[8] == 323)
            {
                craftItemId3 = 329;
                craftItemAm3 = 80;
                craftItemSt3 = false;
            }
        break;

        // leggings
        case 2261:
            if(craftSlotId3[0] == 323 && craftSlotId3[1] == 323 && craftSlotId3[2] == 323 && craftSlotId3[3] == 323 && craftSlotId3[5] == 323 && craftSlotId3[6] == 323 && craftSlotId3[8] == 323)
            {
                craftItemId3 = 330;
                craftItemAm3 = 75;
                craftItemSt3 = false;
            }
        break;

        // boots
        case 1292:
            if((craftSlotId3[0] == 323 && craftSlotId3[2] == 323 && craftSlotId3[3] == 323 && craftSlotId3[5] == 323) || (craftSlotId3[3] == 323 && craftSlotId3[5] == 323 && craftSlotId3[6] == 323 && craftSlotId3[8] == 323))
            {
                craftItemId3 = 331;
                craftItemAm3 = 65;
                craftItemSt3 = false;
            }
        break;

        // end

        // iron

        // helmet
        case 1390:
            if((craftSlotId3[0] == 278 && craftSlotId3[1] == 278 && craftSlotId3[2] == 278 && craftSlotId3[3] == 278 && craftSlotId3[5] == 278) || (craftSlotId3[3] == 278 && craftSlotId3[4] == 278 && craftSlotId3[5] == 278 && craftSlotId3[6] == 278 && craftSlotId3[8] == 278))
            {
                craftItemId3 = 336;
                craftItemAm3 = 165;
                craftItemSt3 = false;
            }
        break;

        // chestplate
        case 2224:
            if(craftSlotId3[0] == 278 && craftSlotId3[2] == 278 && craftSlotId3[3] == 278 && craftSlotId3[4] == 278 && craftSlotId3[5] == 278 && craftSlotId3[6] == 278 && craftSlotId3[7] == 278 && craftSlotId3[8] == 278)
            {
                craftItemId3 = 337;
                craftItemAm3 = 240;
                craftItemSt3 = false;
            }
        break;

        // leggings
        case 1946:
            if(craftSlotId3[0] == 278 && craftSlotId3[1] == 278 && craftSlotId3[2] == 278 && craftSlotId3[3] == 278 && craftSlotId3[5] == 278 && craftSlotId3[6] == 278 && craftSlotId3[8] == 278)
            {
                craftItemId3 = 338;
                craftItemAm3 = 225;
                craftItemSt3 = false;
            }
        break;


        // end

        // diamond

        // helmet
        case 1395:
            if((craftSlotId3[0] == 279 && craftSlotId3[1] == 279 && craftSlotId3[2] == 279 && craftSlotId3[3] == 279 && craftSlotId3[5] == 279) || (craftSlotId3[3] == 279 && craftSlotId3[4] == 279 && craftSlotId3[5] == 279 && craftSlotId3[6] == 279 && craftSlotId3[8] == 279))
            {
                craftItemId3 = 340;
                craftItemAm3 = 363;
                craftItemSt3 = false;
            }
        break;

        // chestplate
        case 2232:
            if(craftSlotId3[0] == 279 && craftSlotId3[2] == 279 && craftSlotId3[3] == 279 && craftSlotId3[4] == 279 && craftSlotId3[5] == 279 && craftSlotId3[6] == 279 && craftSlotId3[7] == 279 && craftSlotId3[8] == 279)
            {
                craftItemId3 = 341;
                craftItemAm3 = 528;
                craftItemSt3 = false;
            }
        break;

        // leggings
        case 1953:
            if(craftSlotId3[0] == 279 && craftSlotId3[1] == 279 && craftSlotId3[2] == 279 && craftSlotId3[3] == 279 && craftSlotId3[5] == 279 && craftSlotId3[6] == 279 && craftSlotId3[8] == 279)
            {
                craftItemId3 = 342;
                craftItemAm3 = 495;
                craftItemSt3 = false;
            }
        break;

        // boots
        case 1116:
            if((craftSlotId3[0] == 279 && craftSlotId3[2] == 279 && craftSlotId3[3] == 279 && craftSlotId3[5] == 279) || (craftSlotId3[3] == 279 && craftSlotId3[5] == 279 && craftSlotId3[6] == 279 && craftSlotId3[8] == 279))
            {
                craftItemId3 = 343;
                craftItemAm3 = 429;
                craftItemSt3 = false;
            }
        break;

        // end


        // golden

        // helmet
        case 1400:
            if((craftSlotId3[0] == 280 && craftSlotId3[1] == 280 && craftSlotId3[2] == 280 && craftSlotId3[3] == 280 && craftSlotId3[5] == 280) || (craftSlotId3[3] == 280 && craftSlotId3[4] == 280 && craftSlotId3[5] == 280 && craftSlotId3[6] == 280 && craftSlotId3[8] == 280))
            {
                craftItemId3 = 344;
                craftItemAm3 = 77;
                craftItemSt3 = false;
            }
        break;

        // chestplate
        case 2240:
            if(craftSlotId3[0] == 280 && craftSlotId3[2] == 280 && craftSlotId3[3] == 280 && craftSlotId3[4] == 280 && craftSlotId3[5] == 280 && craftSlotId3[6] == 280 && craftSlotId3[7] == 280 && craftSlotId3[8] == 280)
            {
                craftItemId3 = 345;
                craftItemAm3 = 112;
                craftItemSt3 = false;
            }
        break;

        // leggings
        case 1960:
            if(craftSlotId3[0] == 280 && craftSlotId3[1] == 280 && craftSlotId3[2] == 280 && craftSlotId3[3] == 280 && craftSlotId3[5] == 280 && craftSlotId3[6] == 280 && craftSlotId3[8] == 280)
            {
                craftItemId3 = 346;
                craftItemAm3 = 105;
                craftItemSt3 = false;
            }
        break;

        // boots
        case 1120:
            if((craftSlotId3[0] == 280 && craftSlotId3[2] == 280 && craftSlotId3[3] == 280 && craftSlotId3[5] == 280) || (craftSlotId3[3] == 280 && craftSlotId3[5] == 280 && craftSlotId3[6] == 280 && craftSlotId3[8] == 280))
            {
                craftItemId3 = 347;
                craftItemAm3 = 91;
                craftItemSt3 = false;
            }
        break;

        // end
        /// Armor crafts end

        case 2799: //melon block
            if(craftSlotId3[0] == 311 && craftSlotId3[1] == 311 && craftSlotId3[2] == 311 && craftSlotId3[3] == 311 && craftSlotId3[4] == 311 && craftSlotId3[5] == 311 && craftSlotId3[6] == 311 && craftSlotId3[7] == 311 && craftSlotId3[8] == 311)
            {
                craftItemId3 = 81;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;


        case 2862: //lapis block
            if(craftSlotId3[0] == 318 && craftSlotId3[1] == 318 && craftSlotId3[2] == 318 && craftSlotId3[3] == 318 && craftSlotId3[4] == 318 && craftSlotId3[5] == 318 && craftSlotId3[6] == 318 && craftSlotId3[7] == 318 && craftSlotId3[8] == 318)
            {
                craftItemId3 = 101;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;


        case 101: //lapis block to items
            if(craftSlotId3[0] == 101 || craftSlotId3[1] == 101 || craftSlotId3[2] == 101 || craftSlotId3[3] == 101 || craftSlotId3[4] == 101 || craftSlotId3[5] == 101 || craftSlotId3[6] == 101 || craftSlotId3[7] == 101 || craftSlotId3[8] == 101)
            {
                craftItemId3 = 318;
                craftItemSt3 = 1;
                craftItemAm3 = 9;
            }
        break;

        case 2502: //iron block
            if(craftSlotId3[0] == 278 && craftSlotId3[1] == 278 && craftSlotId3[2] == 278 && craftSlotId3[3] == 278 && craftSlotId3[4] == 278 && craftSlotId3[5] == 278 && craftSlotId3[6] == 278 && craftSlotId3[7] == 278 && craftSlotId3[8] == 278)
            {
                craftItemId3 = 66;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;


        case 66: //iron block to bars
            if(craftSlotId3[0] == 66 || craftSlotId3[1] == 66 || craftSlotId3[2] == 66 || craftSlotId3[3] == 66 || craftSlotId3[4] == 66 || craftSlotId3[5] == 66 || craftSlotId3[6] == 66 || craftSlotId3[7] == 66 || craftSlotId3[8] == 66)
            {
                craftItemId3 = 278;
                craftItemSt3 = 1;
                craftItemAm3 = 9;
            }
        break;

        case 2520: //gold block
            if(craftSlotId3[0] == 280 && craftSlotId3[1] == 280 && craftSlotId3[2] == 280 && craftSlotId3[3] == 280 && craftSlotId3[4] == 280 && craftSlotId3[5] == 280 && craftSlotId3[6] == 280 && craftSlotId3[7] == 280 && craftSlotId3[8] == 280)
            {
                craftItemId3 = 65;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 65: //gold block to bars
            if(craftSlotId3[0] == 65 || craftSlotId3[1] == 65 || craftSlotId3[2] == 65 || craftSlotId3[3] == 65 || craftSlotId3[4] == 65 || craftSlotId3[5] == 65 || craftSlotId3[6] == 65 || craftSlotId3[7] == 65 || craftSlotId3[8] == 65)
            {
                craftItemId3 = 280;
                craftItemSt3 = 1;
                craftItemAm3 = 9;
            }
        break;

        case 2511: //diamond block
            if(craftSlotId3[0] == 279 && craftSlotId3[1] == 279 && craftSlotId3[2] == 279 && craftSlotId3[3] == 279 && craftSlotId3[4] == 279 && craftSlotId3[5] == 279 && craftSlotId3[6] == 279 && craftSlotId3[7] == 279 && craftSlotId3[8] == 279)
            {
                craftItemId3 = 64;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 64: //diamond block to diamonds
            if(craftSlotId3[0] == 64 || craftSlotId3[1] == 64 || craftSlotId3[2] == 64 || craftSlotId3[3] == 64 || craftSlotId3[4] == 64 || craftSlotId3[5] == 64 || craftSlotId3[6] == 64 || craftSlotId3[7] == 64 || craftSlotId3[8] == 64)
            {
                craftItemId3 = 279;
                craftItemSt3 = 1;
                craftItemAm3 = 9;
            }
        break;

        case 2583: //hay bale
            if(craftSlotId3[0] == Wheat::getID() && craftSlotId3[1] == Wheat::getID() && craftSlotId3[2] == Wheat::getID() && craftSlotId3[3] == Wheat::getID() && craftSlotId3[4] == Wheat::getID() &&
               craftSlotId3[5] == Wheat::getID() && craftSlotId3[6] == Wheat::getID() && craftSlotId3[7] == Wheat::getID() && craftSlotId3[8] == Wheat::getID())
            {
                craftItemId3 = HayBale::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 213: //hay bale to wheat
            if(craftSlotId3[0] == HayBale::getID() || craftSlotId3[1] == HayBale::getID() || craftSlotId3[2] == HayBale::getID() || craftSlotId3[3] == HayBale::getID() || craftSlotId3[4] == HayBale::getID() ||
               craftSlotId3[5] == HayBale::getID() || craftSlotId3[6] == HayBale::getID() || craftSlotId3[7] == HayBale::getID() || craftSlotId3[8] == HayBale::getID())
            {
                craftItemId3 = Wheat::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 9;
            }
        break;

        case 2493: //coal to coal block
            if(craftSlotId3[0] == Coal::getID() && craftSlotId3[1] == Coal::getID() && craftSlotId3[2] == Coal::getID() && craftSlotId3[3] == Coal::getID() && craftSlotId3[4] == Coal::getID() &&
               craftSlotId3[5] == Coal::getID() && craftSlotId3[6] == Coal::getID() && craftSlotId3[7] == Coal::getID() && craftSlotId3[8] == Coal::getID())
            {
                craftItemId3 = CoalBlock::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 214: //coal block to coal
            if(craftSlotId3[0] == CoalBlock::getID() || craftSlotId3[1] == CoalBlock::getID() || craftSlotId3[2] == CoalBlock::getID() || craftSlotId3[3] == CoalBlock::getID() || craftSlotId3[4] == CoalBlock::getID() ||
               craftSlotId3[5] == CoalBlock::getID() || craftSlotId3[6] == CoalBlock::getID() || craftSlotId3[7] == CoalBlock::getID() || craftSlotId3[8] == CoalBlock::getID())
            {
                craftItemId3 = Coal::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 9;
            }
        break;

        case 3177: //redstone to redstone block
            if(craftSlotId3[0] == Redstone::getID() && craftSlotId3[1] == Redstone::getID() && craftSlotId3[2] == Redstone::getID() && craftSlotId3[3] == Redstone::getID() && craftSlotId3[4] == Redstone::getID() &&
               craftSlotId3[5] == Redstone::getID() && craftSlotId3[6] == Redstone::getID() && craftSlotId3[7] == Redstone::getID() && craftSlotId3[8] == Redstone::getID())
            {
                craftItemId3 = RedstoneBlock::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 215: //redstonee block to redstone
            if(craftSlotId3[0] == RedstoneBlock::getID() || craftSlotId3[1] == RedstoneBlock::getID() || craftSlotId3[2] == RedstoneBlock::getID() || craftSlotId3[3] == RedstoneBlock::getID() || craftSlotId3[4] == RedstoneBlock::getID() ||
               craftSlotId3[5] == RedstoneBlock::getID() || craftSlotId3[6] == RedstoneBlock::getID() || craftSlotId3[7] == RedstoneBlock::getID() || craftSlotId3[8] == RedstoneBlock::getID())
            {
                craftItemId3 = Redstone::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 9;
            }
        break;

        case 1932: //ladder
            if(craftSlotId3[0] == 276 && craftSlotId3[2] == 276 && craftSlotId3[3] == 276 && craftSlotId3[4] == 276 && craftSlotId3[5] == 276 && craftSlotId3[6] == 276 && craftSlotId3[8] == 276)
            {
                craftItemId3 = 307;
                craftItemSt3 = 1;
                craftItemAm3 = 3;
            }
        break;

        case 272: //chest (oak)
            if(craftSlotId3[1] == 34 && craftSlotId3[2] == 34 && craftSlotId3[3] == 34 && craftSlotId3[5] == 34 && craftSlotId3[7] == 34 && craftSlotId3[0] == 34 && craftSlotId3[6] == 34 && craftSlotId3[8] == 34)
            {
                craftItemId3 = ChestBlock3::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1240: //chest (birch)
            if(craftSlotId3[1] == 155 && craftSlotId3[2] == 155 && craftSlotId3[3] == 155 && craftSlotId3[5] == 155 && craftSlotId3[7] == 155 && craftSlotId3[0] == 155 && craftSlotId3[6] == 155 && craftSlotId3[8] == 155)
            {
                craftItemId3 = ChestBlock3::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1248: //chest (spruce)
            if(craftSlotId3[1] == 156 && craftSlotId3[2] == 156 && craftSlotId3[3] == 156 && craftSlotId3[5] == 156 && craftSlotId3[7] == 156 && craftSlotId3[0] == 156 && craftSlotId3[6] == 156 && craftSlotId3[8] == 156)
            {
                craftItemId3 = ChestBlock3::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 288: //furnace
            if(craftSlotId3[1] == 36 && craftSlotId3[2] == 36 && craftSlotId3[3] == 36 && craftSlotId3[5] == 36 && craftSlotId3[7] == 36 && craftSlotId3[0] == 36 && craftSlotId3[6] == 36 && craftSlotId3[8] == 36)
            {
                craftItemId3 = FurnaceOff::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 861: //bread
            if((craftSlotId3[0] == 287 && craftSlotId3[1] == 287 && craftSlotId3[2] == 287) || (craftSlotId3[3] == 287 && craftSlotId3[4] == 287 && craftSlotId3[5] == 287) || (craftSlotId3[6] == 287 && craftSlotId3[7] == 287 && craftSlotId3[8] == 287))
            {
                craftItemId3 = 288;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 888: //cookies
            if((craftSlotId3[0] == 287 && craftSlotId3[1] == 314 && craftSlotId3[2] == 287) || (craftSlotId3[3] == 287 && craftSlotId3[4] == 314 && craftSlotId3[5] == 287) || (craftSlotId3[6] == 287 && craftSlotId3[7] == 314 && craftSlotId3[8] == 287))
            {
                craftItemId3 = 315;
                craftItemSt3 = 1;
                craftItemAm3 = 8;
            }
        break;

        case 2524: //golden apple
            if(craftSlotId3[0] == 280 && craftSlotId3[1] == 280 && craftSlotId3[2] == 280 && craftSlotId3[3] == 280 && craftSlotId3[4] == 284 && craftSlotId3[5] == 280 && craftSlotId3[6] == 280 && craftSlotId3[7] == 280 && craftSlotId3[8] == 280)
            {
                craftItemId3 = 285;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 879: //paper
            if((craftSlotId3[0] == 293 && craftSlotId3[1] == 293 && craftSlotId3[2] == 293) || (craftSlotId3[3] == 293 && craftSlotId3[4] == 293 && craftSlotId3[5] == 293) || (craftSlotId3[6] == 293 && craftSlotId3[7] == 293 && craftSlotId3[8] == 293))
            {
                craftItemId3 = 297;
                craftItemSt3 = 1;
                craftItemAm3 = 3;
            }
        break;

        case 891: //book
            if((craftSlotId3[0] == 297 && craftSlotId3[3] == 297 && craftSlotId3[6] == 297) || (craftSlotId3[1] == 297 && craftSlotId3[4] == 297 && craftSlotId3[7] == 297) || (craftSlotId3[2] == 297 && craftSlotId3[5] == 297 && craftSlotId3[8] == 297))
            {
                craftItemId3 = 298;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1098: //book shelf
            if(craftSlotId3[0] == 34 && craftSlotId3[1] == 34 && craftSlotId3[2] == 34 && craftSlotId3[3] == 298 && craftSlotId3[4] == 298 && craftSlotId3[5] == 298 && craftSlotId3[6] == 34 && craftSlotId3[7] == 34 && craftSlotId3[8] == 34)
            {
                craftItemId3 = 35;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1734: //book shelf (birch)
            if(craftSlotId3[0] == 155 && craftSlotId3[1] == 155 && craftSlotId3[2] == 155 && craftSlotId3[3] == 298 && craftSlotId3[4] == 298 && craftSlotId3[5] == 298 && craftSlotId3[6] == 155 && craftSlotId3[7] == 155 && craftSlotId3[8] == 155)
            {
                craftItemId3 = 35;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1740: //book shelf (birch)
            if(craftSlotId3[0] == 156 && craftSlotId3[1] == 156 && craftSlotId3[2] == 156 && craftSlotId3[3] == 298 && craftSlotId3[4] == 298 && craftSlotId3[5] == 298 && craftSlotId3[6] == 156 && craftSlotId3[7] == 156 && craftSlotId3[8] == 156)
            {
                craftItemId3 = 35;
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 1473: //clock
            if(craftSlotId3[1] == 280 && craftSlotId3[3] == 280 && craftSlotId3[4] == Redstone::getID() && craftSlotId3[5] == 280 && craftSlotId3[7] == 280)
            {
                craftItemId3 = Clock::getID();
                craftItemSt3 = 0;
                craftItemAm3 = 1;
            }
        break;

        case 1465: //compass
            if(craftSlotId3[1] == IronBar::getID() && craftSlotId3[3] == IronBar::getID() && craftSlotId3[4] == Redstone::getID() && craftSlotId3[5] == IronBar::getID() && craftSlotId3[7] == IronBar::getID())
            {
                craftItemId3 = Compass::getID();
                craftItemSt3 = 0;
                craftItemAm3 = 1;
            }
        break;

        case 846: //watering can
            if(craftSlotId3[8] == 290 && craftSlotId3[4] == 278 && craftSlotId3[0] == 278)
            {
                craftItemId3 = 304;
                craftItemSt3 = 0;
                craftItemAm3 = 0;
            }
        break;

        case 2531: //item frame
            if(craftSlotId3[1] == Stick::getID() && craftSlotId3[2] == Stick::getID() && craftSlotId3[3] == Stick::getID() &&
               craftSlotId3[5] == Stick::getID() && craftSlotId3[7] == Stick::getID() && craftSlotId3[0] == Stick::getID() &&
               craftSlotId3[6] == Stick::getID() && craftSlotId3[8] == Stick::getID() && craftSlotId3[4] == Leather::getID())
            {
                craftItemId3 = ItemFrameItem::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 629: //flint and steel
            if((craftSlotId3[0] == IronBar::getID() && craftSlotId3[4] == Flint::getID()) ||
               (craftSlotId3[1] == IronBar::getID() && craftSlotId3[5] == Flint::getID()) ||
               (craftSlotId3[3] == IronBar::getID() && craftSlotId3[7] == Flint::getID()) ||
               (craftSlotId3[4] == IronBar::getID() && craftSlotId3[8] == Flint::getID()))
            {
                craftItemId3 = FlintAndSteel::getID();
                craftItemSt3 = 0;
                craftItemAm3 = 65;
            }
        break;

        case 1788: //TNT
            if(craftSlotId3[1] == SandBlock::getID() && craftSlotId3[2] == Gunpowder::getID() && craftSlotId3[3] == SandBlock::getID() &&
               craftSlotId3[5] == SandBlock::getID() && craftSlotId3[7] == SandBlock::getID() && craftSlotId3[0] == Gunpowder::getID() &&
               craftSlotId3[6] == Gunpowder::getID() && craftSlotId3[8] == Gunpowder::getID() && craftSlotId3[4] == Gunpowder::getID())
            {
                craftItemId3 = TNTBlock::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 1;
            }
        break;

        case 75: //pumpkin to pumpkin seeds
            if(craftSlotId3[0] == 75 || craftSlotId3[1] == 75 || craftSlotId3[2] == 75 && craftSlotId3[3] == 75 || craftSlotId3[4] == 75 || craftSlotId3[5] == 75 || craftSlotId3[6] == 75 || craftSlotId3[7] == 75 || craftSlotId3[8] == 75)
            {
                craftItemId3 = PumpkinSeeds::getID();
                craftItemSt3 = 1;
                craftItemAm3 = 4;
            }
        break;

        default:
            craftItemId3 = -1;
            craftItemAm3 = -1;
            craftItemSt3 = 0;
        break;

        }
    }
}

void StatePlay::HandleEvents(StateManager* sManager)
{
    //update delta time
    dt = mTimer.GetDeltaTime();

    cameraSpeed = 2.0f * dt;
    cameraMoveSpeed = 4.00f * dt;

    //free memory counter
    freeMemoryTimer += dt;
    if(menuState != 0)
    {
        if(freeMemoryTimer > 3.0f)
        {
            freeMemory = mSystemMgr->ramAvailable();
            freeMemoryTimer = 0.0f;
        }
    }
    else
    {
        if(freeMemoryTimer > 15.0f)
        {
            freeMemory = mSystemMgr->ramAvailable();
            freeMemoryTimer = 0.0f;
        }
    }

    //update input //ui
    mSystemMgr->InputUpdate();

    if(mWorld->HP > 0 && sleepTime <= 0.0f)
    {
        mWorld->playerGrowth = 1.65f;//altura del jugador
        if(menuState == 0)//game state
        {
            if(invEn == false && craft3xEn == false && chestEn == false && furnaceEn == false)
            {
                if(tickShowSlotName > -0.1f)
                {
                    tickShowSlotName -= dt;
                }

                //switch right
                if(keyPressed(InputHelper::Instance()->getButtonToAction(9)))
                {
                    if (mWorld->invId[27+barPosition] != -1)
                    {
                        changeY = -0.16f;
                        anim[0] = 1;
                    }
                    barPosition != 8 ? barPosition ++ : barPosition = 0;

                    selectSprite->SetPosition(160 + (barPosition * 20),230);

                    tickShowSlotName = 1.25f;

                    startDt = 0;
                    dT = 0;
                    dET = 0;
                    dStd = -1;
                }

                //switch left
                if(keyPressed(InputHelper::Instance()->getButtonToAction(8)))
                {
                    if (mWorld->invId[27+barPosition] != -1)
                    {
                        changeY = -0.16f;
                        anim[0] = 1;
                    }
                    barPosition != 0 ? barPosition -- : barPosition = 8;

                    selectSprite->SetPosition(160 + (barPosition * 20),230);

                    tickShowSlotName = 1.25f;

                    startDt = 0;
                    dT = 0;
                    dET = 0;
                    dStd = -1;
                }

                // drop 1 item
                if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
                {
                    if(keyPressed(InputHelper::Instance()->getButtonToAction(14)))
                    {
                        if(mWorld->invId[27+barPosition] > 0)
                        {
                            Vector3 dropVelocity = Vector3(0,0,0);

                            dropVelocity.x = (cosf((fppCam->horAngle/180.0f)*PI))*4.0f;
                            dropVelocity.y = 2.0f;
                            dropVelocity.z = (sinf((fppCam->horAngle/180.0f)*PI))*4.0f;

                            if(mWorld->invSt[27+barPosition] == true)
                            {
                                if(mWorld->invAm[27+barPosition] > 0)
                                {
                                    mWorld->invAm[27+barPosition] -= 1;
                                    mWorld->DropThisNoAlign(mWorld->invId[27+barPosition],1,mWorld->invSt[27+barPosition],Vector3(fppCam->m_vPosition.x,fppCam->m_vPosition.y-0.25f,fppCam->m_vPosition.z),dropVelocity);

                                    if(mWorld->invAm[27+barPosition] == 0)
                                    {
                                        mWorld->invAm[27+barPosition] = -1;
                                        mWorld->invSt[27+barPosition] = 0;
                                        mWorld->invId[27+barPosition] = -1;
                                    }
                                }
                            }
                            else
                            {
                                mWorld->DropThisNoAlign(mWorld->invId[27+barPosition],mWorld->invAm[27+barPosition],mWorld->invSt[27+barPosition],Vector3(fppCam->m_vPosition.x,fppCam->m_vPosition.y-0.25f,fppCam->m_vPosition.z),dropVelocity);

                                mWorld->invAm[27+barPosition] = -1;
                                mWorld->invSt[27+barPosition] = 0;
                                mWorld->invId[27+barPosition] = -1;
                            }
                        }
                        return;
                    }
                }

                //open menu
                if(sleepTime == 0.0f)
                {
                    if(keyPressed(InputHelper::Instance()->getButtonToAction(15)))
                    {
                        menuState = 1;
                        menuOptions = false;
                        optionsMenuPos = 0;
                    }
                }
				
				//idle animation
				if(jumping == 0)
				{
					float animSpeed = 0.3*PI*dt;
					
					if(bobTipo == 0)
					{
						if (bobCiclo < PI)
						{
							bobCiclo += animSpeed;
						}
						else
						{
							bobTipo = 1;
						}
					}
					else
					{
						if (bobCiclo > 0)
						{
							bobCiclo -= animSpeed;
						}
						else
						{
							bobTipo = 0;
						}
					}
					
					float idleLost = sinf(bobCiclo - (3.14/2) + 3.14)/200;
					idle_y = idleLost;
				}
				
                //camera rotate up
                if(keyHold(InputHelper::Instance()->getButtonToAction(4)))
                {
                    if (startDt == true)	//start destroy
                    {
                        startDt = false;
                        dStd = -1;
                    }
                    fppCam->PitchView(1.0f*dt);
                    fppCam->PitchView(1.0f*dt);
                    shift_y > -0.06f ? shift_y -= 0.24*dt : shift_y = -0.06f;
                }
                else
                {
                    if (shift_y < 0.0f)
                    {
                        shift_y += 0.24*dt;
                    }
                    if(abs(shift_y) <= 0.001f)
                    {
                        shift_y = 0.0f;
                    }
                }

                //camera rotate down
                if(keyHold(InputHelper::Instance()->getButtonToAction(5)))
                {
                    if (startDt == true)
                    {
                        startDt = false;
                        dStd = -1;
                    }
                    fppCam->PitchView(-1.0f*dt);
                    fppCam->PitchView(-1.0f*dt);
                    shift_y < 0.06f ? shift_y += 0.24*dt : shift_y = 0.06f;
                }
                else
                {
                    if (shift_y > 0.0f)
                    {
                        shift_y -= 0.24*dt;
                    }
                    if(abs(shift_y) <= 0.001f)
                    {
                        shift_y = 0.0f;
                    }
                }

                //camera rotate right
                if(keyHold(InputHelper::Instance()->getButtonToAction(7)))
                {
                    if (startDt == true)
                    {
                        startDt = false;
                        dStd = -1;
                    }
                    fppCam->RotateView(-2.0f*dt,0,1,0);
                    shift_x > -0.06f ? shift_x -= 0.24*dt : shift_x = -0.06f;
                }
                else
                {
                    if (shift_x < 0.0f)
                    {
                        shift_x += 0.24*dt;
                    }
                    if(abs(shift_x) <= 0.001f)
                    {
                        shift_x = 0.0f;
                    }
                }

                //camera rotate left
                if(keyHold(InputHelper::Instance()->getButtonToAction(6)))
                {
                    if (startDt == true)
                    {
                        startDt = false;
                        dStd = -1;
                    }
                    fppCam->RotateView(2.0f*dt,0,1,0);
                    shift_x < 0.06f ? shift_x += 0.24*dt : shift_x = 0.06f;
                }
                else
                {
                    if (shift_x > 0.0f)
                    {
                        shift_x -= 0.24*dt;
                    }
                    if(abs(shift_x) <= 0.001f)
                    {
                        shift_x = 0.0f;
                    }
                }

                //camera moving
                moving = false;
                moveForward = false;
                bool diagonleMoving = false;

                if(sleepTime == 0.0f)
                {
                    //move forward
                    if(keyHold(InputHelper::Instance()->getButtonToAction(0)))
                    {
                        float slowdown = 0.8f;
                        float slowdown2 = 1.0f;
                        if (startDt == true)
                        {
                            startDt = false;
                            dStd = -1;
                        }
                        moving = true;
                        moveForward = true;

                        if(walkingOnGround == false)
                        {
                            slowdown -= 0.2f;
                            slowdown2 -= 0.2f;
                        }
                        if(slowdown < 0.72)
                        {
                            slowdown = 0.72f;
                        }

                        //move right at the same time
                        if(keyHold(InputHelper::Instance()->getButtonToAction(3)))
                        {
                            diagonleMoving = true;
                            fppCam->StrafePhysic(cameraMoveSpeed*slowdown);

                            if(canFly)
                                fppCam->MovePhysic(cameraMoveSpeed*slowdown);
                            else
                                fppCam->MovePhysicNoY(cameraMoveSpeed*slowdown);
                        }

                        //move left at the same time
                        if(keyHold(InputHelper::Instance()->getButtonToAction(2)))
                        {
                            diagonleMoving = true;
                            fppCam->StrafePhysic(-cameraMoveSpeed*slowdown);

                            if(canFly)
                                fppCam->MovePhysic(cameraMoveSpeed*slowdown);
                            else
                                fppCam->MovePhysicNoY(cameraMoveSpeed*slowdown);
                        }

                        if(!diagonleMoving)
                        {
                            if(canFly)
                                fppCam->MovePhysic(cameraMoveSpeed*slowdown2);
                            else
                                fppCam->MovePhysicNoY(cameraMoveSpeed*slowdown2);
                        }

                        if(mWorld->HG > 0)
                        {
                            mWorld->HG -= dt/75.0f;
                        }
                    }

                    //move back
                    if(keyHold(InputHelper::Instance()->getButtonToAction(1)))
                    {
                        float slowdown = 0.8f;
                        float slowdown2 = 1.0f;
                        if (startDt == true)
                        {
                            startDt = false;
                            dStd = -1;
                        }
                        moving = true;
                        moveForward = false;

                        if(walkingOnGround == false)
                        {
                            slowdown -= 0.2f;
                            slowdown2 -= 0.2f;
                        }
                        if(slowdown < 0.72)
                        {
                            slowdown = 0.72f;
                        }
                        //move right at the same time
                        if(keyHold(InputHelper::Instance()->getButtonToAction(3)))
                        {
                            diagonleMoving = true;
                            fppCam->StrafePhysic(cameraMoveSpeed*slowdown);

                            if(canFly)
                                fppCam->MovePhysic(-cameraMoveSpeed*slowdown);
                            else
                                fppCam->MovePhysicNoY(-cameraMoveSpeed*slowdown);
                        }

                        //move left at the same time
                        if(keyHold(InputHelper::Instance()->getButtonToAction(2)))
                        {
                            diagonleMoving = true;
                            fppCam->StrafePhysic(-cameraMoveSpeed*slowdown);

                            if(canFly)
                                fppCam->MovePhysic(-cameraMoveSpeed*slowdown);
                            else
                                fppCam->MovePhysicNoY(-cameraMoveSpeed*slowdown);
                        }

                        if(!diagonleMoving)
                        {
                            if(canFly)
                                fppCam->MovePhysic(-cameraMoveSpeed*slowdown2);
                            else
                                fppCam->MovePhysicNoY(-cameraMoveSpeed*slowdown2);
                        }

                        if(mWorld->HG > 0)
                        {
                            mWorld->HG -= dt/75.0f;
                        }
                    }

                    //move right
                    if(keyHold(InputHelper::Instance()->getButtonToAction(3)))
                    {
                        float slowdown = 1.0f;

                        if(walkingOnGround == false)
                        {
                            slowdown -= 0.2f;
                        }

                        if (startDt == true)
                        {
                            startDt = false;
                            dStd = -1;
                        }
                        if(!diagonleMoving)
                        {
                            fppCam->StrafePhysic(cameraMoveSpeed*slowdown);
                            moving = true;
                        }

                        if(mWorld->HG > 0)
                        {
                            mWorld->HG -= dt/75.0f;
                        }
                    }
					
					//Crouch
					if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
					{
						crouch == false ? crouch = true : crouch = false;
					}
					if(crouch)
					{
						sprint = false;
						mWorld->playerGrowth = 1.55f;
					}
					else
					{
						mWorld->playerGrowth = 1.65f;
					}
					
					//Sprint
					if(keyHold(InputHelper::Instance()->getButtonToAction(0)) && mWorld->HG >= 6)
					{
						if(tickSprintTime > 0.75f && tickSprintTime < 0.9f && crouch == false)
						{
							sprint = true;
							tickSprintTime = 1.0f;
						}
						else
						{
							tickSprintTime = 1.0f;
						}							
					}
					else{
						sprint = false;
					}							
					tickSprintTime -= dt;						

					//Sprint
					if(sprint)
					{							
						float slowdown = 0.3f;
						float slowdown2 = 0.5f;

						if(walkingOnGround == false)
						{
							slowdown -= 0.2f;
							slowdown2 -= 0.2f;
						}

						if(slowdown < 0.72)
						{
							slowdown = 0.32f;
						}
						
						moving = true;
						
						//move right at the same time
						if(keyHold(InputHelper::Instance()->getButtonToAction(3)))
						{
							diagonleMoving = true;
							fppCam->StrafePhysic(cameraMoveSpeed*slowdown);

							if(canFly)
								fppCam->MovePhysic(cameraMoveSpeed*slowdown);
							else
								fppCam->MovePhysicNoY(cameraMoveSpeed*slowdown);
						}
						
						//move left at the same time
						if(keyHold(InputHelper::Instance()->getButtonToAction(2)))
						{
							diagonleMoving = true;
							fppCam->StrafePhysic(-cameraMoveSpeed*slowdown);
	
							if(canFly)
							fppCam->MovePhysic(cameraMoveSpeed*slowdown);
							else
							fppCam->MovePhysicNoY(cameraMoveSpeed*slowdown);
						}
						
						if(!diagonleMoving)
						{
							fppCam->MovePhysic(cameraMoveSpeed*slowdown2);
						}

						if(mWorld->HG > 0)
						{
							mWorld->HG -= dt/9.0f;
						}
					}

                    //move left
                    if(keyHold(InputHelper::Instance()->getButtonToAction(2)))
                    {
                        float slowdown = 1.0f;

                        if(walkingOnGround == false)
                        {
                            slowdown -= 0.2f;
                        }

                        if (startDt == true)
                        {
                            startDt = false;
                            dStd = -1;
                        }
                        if(!diagonleMoving)
                        {
                            fppCam->StrafePhysic(-cameraMoveSpeed*slowdown);
                            moving = true;
                        }

                        if(mWorld->HG > 0)
                        {
                            mWorld->HG -= dt/75.0f;
                        }
                    }

                    // jumping/swiming
                    if(keyHold(InputHelper::Instance()->getButtonToAction(12)) && playerInCobweb == false)
                    {
                        dStd = -1;
                        if (headInWater || headInLava || mWorld->BlockAtPoint(Vector3(playerPosition.x,playerPosition.y-0.65f,playerPosition.z)) == WaterBlock::getID() || mWorld->BlockAtPoint(Vector3(playerPosition.x,playerPosition.y-0.65f,playerPosition.z)) == Lava::getID())
                        {
                            if(walkingOnGround == true)
                            {
                                jumping = true;
                            }
                            else
                            {
                                playerVelocity.y = 0.525 * JUMPVELOCITY;
                            }
                        }
                        if (headInWater == false && headInLava == false && walkingOnGround == false && (footInWater == true || footInLava == true))	//Your above he water, so Jump out
                        {
                            playerVelocity.y = 1.2 * JUMPVELOCITY;
                            walkingOnGround = false;
                            if(mWorld->HG > 0.0001)
                            {
                                mWorld->HG -= 0.00001;
                            }
                        }
                        if(walkingOnGround == true)
                        {
                            if(headInWater == false && headInLava == false)
                            {
                                jumping = true;
                            }
                        }
                        else
                        {
                            jumping = false;
                        }
                    }
                }

            //add cube
            if(keyPressed(InputHelper::Instance()->getButtonToAction(14)))
            {
                if(sleepTime == 0.0f)
                {
                    if(keyPressed(InputHelper::Instance()->getButtonToAction(13))) //open inventory
                    {
                        if(craft3xEn == false && chestEn == false && furnaceEn == false)
                        {
                            invEn = true;
                            dT = 0;
                            dET = 0;
                            chestId = -1;
                            dStd = -1;
                            return;
                        }
                    }
                }
                if (startDt == true)
                {
                    startDt = false; //stop destroying
                }


                if (mWorld->invId[27+barPosition] == 299) ///SNOWBALL THROWING
                {
                    changeY = -0.20f;
                    anim[0] = 1;

                    mSoundMgr->PlayBowSound();

                    SnowBall2* NewSB = new SnowBall2(playerPosition.x,playerPosition.y,playerPosition.z);
                    NewSB->SetVeloc(fppCam->upDownAngle,(fppCam->horAngle/(float)180)*PI);
                    mSnowBalls.push_back(NewSB);

                    mWorld->invAm[27+barPosition] -= 1;
                    if (mWorld->invAm[27+barPosition] == 0) // [27+barPosition] - selected item/block
                    {
                        mWorld->invAm[27+barPosition] = -1;
                        mWorld->invId[27+barPosition] = -1;
                        mWorld->invSt[27+barPosition] = 0;
                    }
                    return;
                }
				
				if (mWorld->invId[27+barPosition] == Bow0::getID()) //arco
                {
					for(int i = 0; i <= 27+barPosition; i++)
					{
						if(mWorld->invId[i] == Arrow::getID())
						{
							//changeY = -0.20f;
							//anim[0] = 1;

							mSoundMgr->PlayBowSound();

							Arrow2* NewARR = new Arrow2(playerPosition.x,playerPosition.y,playerPosition.z);
							NewARR->SetVeloc(fppCam->upDownAngle,(fppCam->horAngle/(float)180)*PI);
							mArrows.push_back(NewARR);
							
							mWorld->invAm[27+barPosition] -= 1;
							if (mWorld->invAm[27+barPosition] == 0) // [27+barPosition] - selected item/block
							{
								mWorld->invAm[27+barPosition] = -1;
								mWorld->invId[27+barPosition] = -1;
								mWorld->invSt[27+barPosition] = 0;
							}
							return;
						}
					}
                }

                //add cube
                Vector3 rayDir = fppCam->m_vView - fppCam->m_vPosition;
                rayDir.normalize();

                //get position and view vector
                Vector3 testPos;

                BoundingBox collider = mWorld->blockTypes[mWorld->GetBlock(testPos.x,testPos.y,testPos.z)].collideBox;
                BoundingBox testBox = BoundingBox(testPos.x+collider.min.x, testPos.y+collider.min.y, testPos.z+collider.min.z, testPos.x+collider.max.x, testPos.y+collider.max.y, testPos.z+collider.max.z);

					/// ALIMENTACION DE ANIMALES
					/// OVEJA Y VACA
					if(mWorld->mSheeps.empty() == false)
                    {
						for(float i = 0; i < 5.25f; i+=0.1f)
                        {
							testPos = fppCam->m_vPosition + (rayDir * i);
							for(int f = 0; f < mWorld->mSheeps.size(); f++)
							{
								Sheep *TestSheep = mWorld->mSheeps[f];
								if(TestSheep->DistanceToPlayer() < 2.85 && TestSheep->kicked == false && TestSheep->damaged == false)
								{
									if(TestSheep->bBox.contains(testPos))
									{
										/// ITEM	
										if(mWorld->invId[27+barPosition] == Wheat::getID())
										{
											for(float verAngle = 0.0f; verAngle <= PI; verAngle += PI/5.0f)
											{
												for(float horAngle = 0.0f; horAngle <= PI; horAngle += PI/5.0f)
												{
													if(mParticles.size() > 52)
													{
														continue;
													}
													float radius = 1.0f+(rand()%3)/10.0f;
													float radius2 = 0.3f+(rand()%1)/10.0f;

													Particle* Heart = new Particle(mWorld,"heart",Vector3(TestSheep->position.x,TestSheep->position.y,TestSheep->position.z));
													Heart->SetVelocity(Vector3(mWorld->LengthDirX(horAngle,verAngle)*radius2,mWorld->LengthDirY(horAngle,verAngle)*radius,mWorld->LengthDirZ(horAngle,verAngle)*radius2));

													mParticles.push_back(Heart);
												}
											}
											
											if(TestSheep->sheared == true)
											{
												if(rand()%100 <= 30)
												{
													TestSheep->sheared = false;
												}
											}
											
											mWorld->invAm[27+barPosition] -= 1;
											if (mWorld->invAm[27+barPosition] == 0)
											{
												mWorld->invAm[27+barPosition] = -1;
												mWorld->invId[27+barPosition] = -1;
												mWorld->invSt[27+barPosition] = 0;
											}

											changeY = -0.20f;
											anim[0] = 1;
											return;
										}
									}
								}
							}
						}
					}
					
					if(mWorld->mCows.empty() == false)
                    {
						for(float i = 0; i < 5.25f; i+=0.1f)
                        {
							testPos = fppCam->m_vPosition + (rayDir * i);
							for(int f = 0; f < mWorld->mCows.size(); f++)
							{
								Cow *TestCow = mWorld->mCows[f];
								if(TestCow->DistanceToPlayer() < 2.85 && TestCow->kicked == false && TestCow->damaged == false)
								{
									if(TestCow->bBox.contains(testPos))
									{
										/// ITEM	
										if(mWorld->invId[27+barPosition] == Wheat::getID())
										{
											for(float verAngle = 0.0f; verAngle <= PI; verAngle += PI/5.0f)
											{
												for(float horAngle = 0.0f; horAngle <= PI; horAngle += PI/5.0f)
												{
													if(mParticles.size() > 52)
													{
														continue;
													}
													float radius = 1.0f+(rand()%3)/10.0f;
													float radius2 = 0.3f+(rand()%1)/10.0f;

													Particle* Heart = new Particle(mWorld,"heart",Vector3(TestCow->position.x,TestCow->position.y,TestCow->position.z));
													Heart->SetVelocity(Vector3(mWorld->LengthDirX(horAngle,verAngle)*radius2,mWorld->LengthDirY(horAngle,verAngle)*radius,mWorld->LengthDirZ(horAngle,verAngle)*radius2));

													mParticles.push_back(Heart);
												}
											}
												
											mWorld->invAm[27+barPosition] -= 1;
											if (mWorld->invAm[27+barPosition] == 0)
											{
												mWorld->invAm[27+barPosition] = -1;
												mWorld->invId[27+barPosition] = -1;
												mWorld->invSt[27+barPosition] = 0;
											}

											changeY = -0.20f;
											anim[0] = 1;
											return;
										}
									}
								}
							}
						}
					}
				
					/// CERDO
					if(mWorld->mPigs.empty() == false)
                    {
						for(float i = 0; i < 5.25f; i+=0.1f)
                        {
							testPos = fppCam->m_vPosition + (rayDir * i);
							for(int f = 0; f < mWorld->mPigs.size(); f++)
							{
								Pig *TestPig = mWorld->mPigs[f];
								if(TestPig->DistanceToPlayer() < 2.85 && TestPig->kicked == false && TestPig->damaged == false)
								{
									if(TestPig->bBox.contains(testPos))
									{
										/// ITEM	
										if(mWorld->invId[27+barPosition] == Carrot::getID())
										{
											for(float verAngle = 0.0f; verAngle <= PI; verAngle += PI/5.0f)
											{
												for(float horAngle = 0.0f; horAngle <= PI; horAngle += PI/5.0f)
												{
													if(mParticles.size() > 52)
													{
														continue;
													}
													float radius = 1.0f+(rand()%3)/10.0f;
													float radius2 = 0.3f+(rand()%1)/10.0f;

													Particle* Heart = new Particle(mWorld,"heart",Vector3(TestPig->position.x,TestPig->position.y,TestPig->position.z));
													Heart->SetVelocity(Vector3(mWorld->LengthDirX(horAngle,verAngle)*radius2,mWorld->LengthDirY(horAngle,verAngle)*radius,mWorld->LengthDirZ(horAngle,verAngle)*radius2));

													mParticles.push_back(Heart);
												}
											}
												
											mWorld->invAm[27+barPosition] -= 1;
											if (mWorld->invAm[27+barPosition] == 0)
											{
												mWorld->invAm[27+barPosition] = -1;
												mWorld->invId[27+barPosition] = -1;
												mWorld->invSt[27+barPosition] = 0;
											}

											changeY = -0.20f;
											anim[0] = 1;
											return;
										}
									}
								}
							}
						}
					}
				
					/// POLLO
					if(mWorld->mChickens.empty() == false)
                    {
						for(float i = 0; i < 5.25f; i+=0.1f)
                        {
							testPos = fppCam->m_vPosition + (rayDir * i);
							for(int f = 0; f < mWorld->mChickens.size(); f++)
							{
								Chicken *TestChicken = mWorld->mChickens[f];
								if(TestChicken->DistanceToPlayer() < 2.85 && TestChicken->kicked == false && TestChicken->damaged == false)
								{
									if(TestChicken->bBox.contains(testPos))
									{
										/// ITEM	
										if(mWorld->invId[27+barPosition] == WheatSeeds::getID())
										{
											for(float verAngle = 0.0f; verAngle <= PI; verAngle += PI/5.0f)
											{
												for(float horAngle = 0.0f; horAngle <= PI; horAngle += PI/5.0f)
												{
													if(mParticles.size() > 52)
													{
														continue;
													}
													float radius = 1.0f+(rand()%3)/10.0f;
													float radius2 = 0.3f+(rand()%1)/10.0f;

													Particle* Heart = new Particle(mWorld,"heart",Vector3(TestChicken->position.x,TestChicken->position.y,TestChicken->position.z));
													Heart->SetVelocity(Vector3(mWorld->LengthDirX(horAngle,verAngle)*radius2,mWorld->LengthDirY(horAngle,verAngle)*radius,mWorld->LengthDirZ(horAngle,verAngle)*radius2));

													mParticles.push_back(Heart);
												}
											}
												
											mWorld->invAm[27+barPosition] -= 1;
											if (mWorld->invAm[27+barPosition] == 0)
											{
												mWorld->invAm[27+barPosition] = -1;
												mWorld->invId[27+barPosition] = -1;
												mWorld->invSt[27+barPosition] = 0;
											}

											changeY = -0.20f;
											anim[0] = 1;
											return;
										}
									}
								}
							}
						}
					}
					///FIN DE ALIMENTACION

                if(mWorld->invId[27+barPosition] == Scissors::getID() || (mWorld->ItemType(mWorld->invId[27+barPosition]) == 'Y' && mWorld->invId[27+barPosition] != BoneMeal::getID())) // shear sheep or dye it
                {
                    if(mWorld->mSheeps.empty() == false)
                    {
                        for(float i = 0; i < 5.25f; i+=0.1f)
                        {
                            testPos = fppCam->m_vPosition + (rayDir * i);
                            for(int f = 0; f < mWorld->mSheeps.size(); f++)
                            {
                                Sheep *TestSheep = mWorld->mSheeps[f];
                                if(TestSheep->DistanceToPlayer() < 2.85 && TestSheep->kicked == false && TestSheep->damaged == false)
                                {
                                    if(TestSheep->bBox.contains(testPos))
                                    {
                                        if(mWorld->ItemType(mWorld->invId[27+barPosition]) == 'Y') // dye item
                                        {
                                            int newColor = 14;
                                            switch(mWorld->invId[27+barPosition])
                                            {
                                                case 314: // brown
                                                newColor = 6;
                                                break;

                                                case 318: // blue
                                                newColor = 8;
                                                break;

                                                case 322: // white
                                                newColor = 14;
                                                break;

                                                case 365: // lt gray
                                                newColor = 13;
                                                break;

                                                case 366: // gray
                                                newColor = 1;
                                                break;

                                                case 367: // black
                                                newColor = 0;
                                                break;

                                                case 368: // pink
                                                newColor = 3;
                                                break;

                                                case 369: // red
                                                newColor = 2;
                                                break;

                                                case 370: // orange
                                                newColor = 12;
                                                break;

                                                case 371: // yellow
                                                newColor = 7;
                                                break;

                                                case 372: // lime
                                                newColor = 5;
                                                break;

                                                case 373: // lime
                                                newColor = 4;
                                                break;

                                                case 374: // lt blue
                                                newColor = 9;
                                                break;

                                                case 375: // purple
                                                newColor = 10;
                                                break;

                                                case 376: // magneta
                                                newColor = 11;
                                                break;
                                            }

                                            if(TestSheep->GetFurColor() != newColor)
                                            {
                                                TestSheep->SetFurColor(newColor);

                                                mWorld->invAm[27+barPosition] -= 1;
                                                if (mWorld->invAm[27+barPosition] == 0)
                                                {
                                                    mWorld->invAm[27+barPosition] = -1;
                                                    mWorld->invId[27+barPosition] = -1;
                                                    mWorld->invSt[27+barPosition] = 0;
                                                }

                                                animGo = true;
                                                animDest = 0.0f;
                                            }
                                            return;
                                        }

                                        if(mWorld->invId[27+barPosition] == Scissors::getID())
                                        {
                                            if(TestSheep->sheared == false)
                                            {
                                                int woolNum = 1+rand()%3;
                                                for(int i = 0; i < woolNum; i++)
                                                {
                                                    mWorld->DropThis(BlackWoolBlock::getID()+TestSheep->GetFurColor(),1,true,Vector3(TestSheep->position.x,TestSheep->position.y+0.3f,TestSheep->position.z));
                                                }

                                                TestSheep->sheared = true;
                                                mSoundMgr->PlayShearSound();

                                                mWorld->invAm[27+barPosition] -= 1;
                                                if (mWorld->invAm[27+barPosition] == 0)
                                                {
                                                    mWorld->invAm[27+barPosition] = -1;
                                                    mWorld->invId[27+barPosition] = -1;
                                                    mWorld->invSt[27+barPosition] = 0;
                                                }

                                                animGo = true;
                                                animDest = 0.0f;
                                                return;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    return;
                }

                for(float i = 0; i < 5.25f; i+=0.1f)
                {
                    testPos = fppCam->m_vPosition + (rayDir * i);

                    collider = mWorld->blockTypes[mWorld->GetBlock(testPos.x,testPos.y,testPos.z)].collideBox;
                    testBox = BoundingBox(floorf(testPos.x)+0.5f+collider.min.x, floorf(testPos.y)+0.5f+collider.min.y, floorf(testPos.z)+0.5f+collider.min.z, floorf(testPos.x)+0.5f+collider.max.x, floorf(testPos.y)+0.5f+collider.max.y, floorf(testPos.z)+0.5f+collider.max.z);

                    int selectedBlock = mWorld->GetBlock(testPos.x, testPos.y, testPos.z);
					

                    if(mWorld->invId[27+barPosition] == WaterBusket::getID())
                    {
                        if(selectedBlock == Lava::getID())
                        {
                            int x = (int)testPos.x;
                            int y = (int)testPos.y;
                            int z = (int)testPos.z;
                            BoundingBox blockBox = BoundingBox(Vector3(x,y,z),Vector3(x + 1,y + 1,z + 1));
                            BoundingBox playerBox;
                            if(walkingOnGround == false)
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.63f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            else
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.59f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            if(!blockBox.intersect(playerBox))
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = Obsidian::getID();

                                mSoundMgr->PlayFissSound();

                                int	curchunkTarget = mWorld->getChunkId(testPos);
                                mWorld->RebuildFullMeshChunk(curchunkTarget);
                                fppCam->needUpdate = true;

                                mWorld->invId[27+barPosition] = Busket::getID();

                                changeY = -0.16f;
                                anim[0] = 1;
                                return;
                            }
                        }
                    }

                    if(mWorld->invId[27+barPosition] == LavaBusket::getID())
                    {
                        if(selectedBlock == WaterBlock::getID())
                        {
                            int x = (int)testPos.x;
                            int y = (int)testPos.y;
                            int z = (int)testPos.z;
                            BoundingBox blockBox = BoundingBox(Vector3(x,y,z),Vector3(x + 1,y + 1,z + 1));
                            BoundingBox playerBox;
                            if(walkingOnGround == false)
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.63f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            else
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.59f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            if(!blockBox.intersect(playerBox))
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = CobbleStone::getID();

                                mSoundMgr->PlayFissSound();

                                int	curchunkTarget = mWorld->getChunkId(testPos);
                                mWorld->RebuildFullMeshChunk(curchunkTarget);
                                fppCam->needUpdate = true;

                                mWorld->invId[27+barPosition] = Busket::getID();

                                changeY = -0.16f;
                                anim[0] = 1;
                                return;
                            }
                        }
                    }

                    if(mWorld->invId[27+barPosition] == 290) //busket
                    {
                        if (selectedBlock == 4) //if it is WATER
                        {
                            // well simulation
                            if((mWorld->GetBlock(testPos.x-1, testPos.y, testPos.z) == 4 && mWorld->GetBlock(testPos.x-1, testPos.y, testPos.z-1) == 4 && mWorld->GetBlock(testPos.x, testPos.y, testPos.z-1) == 4) ||
                               (mWorld->GetBlock(testPos.x+1, testPos.y, testPos.z) == 4 && mWorld->GetBlock(testPos.x+1, testPos.y, testPos.z-1) == 4 && mWorld->GetBlock(testPos.x, testPos.y, testPos.z-1) == 4) ||
                               (mWorld->GetBlock(testPos.x-1, testPos.y, testPos.z) == 4 && mWorld->GetBlock(testPos.x-1, testPos.y, testPos.z+1) == 4 && mWorld->GetBlock(testPos.x, testPos.y, testPos.z+1) == 4) ||
                               (mWorld->GetBlock(testPos.x+1, testPos.y, testPos.z) == 4 && mWorld->GetBlock(testPos.x+1, testPos.y, testPos.z+1) == 4 && mWorld->GetBlock(testPos.x, testPos.y, testPos.z+1) == 4))
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = 4;
                            }
                            else
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = 0;

                                int	curchunkTarget = mWorld->getChunkId(testPos);
                                mWorld->RebuildTransparentMeshChunk(curchunkTarget);
                                fppCam->needUpdate = true;
                            }

                            mWorld->invId[27+barPosition] = 291;

                            changeY = -0.16f;
                            anim[0] = 1;
                            return;
                        }

                        if (selectedBlock == 112) //if it is lava
                        {
                            // well simulation
                            if((mWorld->GetBlock(testPos.x-1, testPos.y, testPos.z) == Lava::getID() && mWorld->GetBlock(testPos.x-1, testPos.y, testPos.z-1) == Lava::getID() && mWorld->GetBlock(testPos.x, testPos.y, testPos.z-1) == Lava::getID()) ||
                               (mWorld->GetBlock(testPos.x+1, testPos.y, testPos.z) == Lava::getID() && mWorld->GetBlock(testPos.x+1, testPos.y, testPos.z-1) == Lava::getID() && mWorld->GetBlock(testPos.x, testPos.y, testPos.z-1) == Lava::getID()) ||
                               (mWorld->GetBlock(testPos.x-1, testPos.y, testPos.z) == Lava::getID() && mWorld->GetBlock(testPos.x-1, testPos.y, testPos.z+1) == Lava::getID() && mWorld->GetBlock(testPos.x, testPos.y, testPos.z+1) == Lava::getID()) ||
                               (mWorld->GetBlock(testPos.x+1, testPos.y, testPos.z) == Lava::getID() && mWorld->GetBlock(testPos.x+1, testPos.y, testPos.z+1) == Lava::getID() && mWorld->GetBlock(testPos.x, testPos.y, testPos.z+1) == Lava::getID()))
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = Lava::getID();
                            }
                            else
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = 0;

                                mWorld->RemoveLigtSourceAtPosition(testPos.x,testPos.y,testPos.z,112);
                                mWorld->UpdateLightAreaIn(testPos);

                                int	curchunkTarget = mWorld->getChunkId(testPos);
                                mWorld->RebuildFullMeshChunk(curchunkTarget);
                                fppCam->needUpdate = true;
                            }
                            mWorld->invId[27+barPosition] = 292;

                            changeY = -0.16f;
                            anim[0] = 1;
                            return;
                        }
                    }

                    //check if we are touch something
                    if(mWorld->BlockEditable(testPos.x,testPos.y,testPos.z) && testBox.contains(testPos))
                    {
                        // we use hoes
                        if(mWorld->invId[27+barPosition] >= 270 && mWorld->invId[27+barPosition] <= 274)
                        {
                            if (selectedBlock == 1) //if it is grass
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = 2;
                                if(rand() % 11 == 0)
                                {
                                    mWorld->DropThis(286,1,true,testPos); //put seed in inventory
                                }

                                mWorld->invAm[27+barPosition] -= 1; // bit crashing to our hoe
                                if(mWorld->invAm[27+barPosition] == 0) //destroy tool if it has 0 durability points
                                {
                                    mWorld->invAm[27+barPosition] = -1;
                                    mWorld->invId[27+barPosition] = -1;
                                    mWorld->invSt[27+barPosition] = 0;

                                    mSoundMgr->PlayBreakSound();
                                }
                                int	curchunkTarget = mWorld->getChunkId(testPos);

                                //Rebuild nearby world
                                mWorld->RebuildOpaqueMeshChunk(curchunkTarget);
                                fppCam->needUpdate = true;

                                mSoundMgr->PlayWalkSound(1);
                                changeY = -0.16f;
                                anim[0] = 1;
                                return;
                            }

                            if (selectedBlock == 2) //if it is dirt
                            {
                                mWorld->mainStatistics.soilPlowed += 1;
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = 79;

                                mWorld->invAm[27+barPosition] -= 1;
                                if(mWorld->invAm[27+barPosition] == 0) //destroy tool if it has 0 durability points
                                {
                                    mWorld->invAm[27+barPosition] = -1;
                                    mWorld->invId[27+barPosition] = -1;
                                    mWorld->invSt[27+barPosition] = 0;

                                    mSoundMgr->PlayBreakSound();
                                }

                                int	curchunkTarget = mWorld->getChunkId(testPos);

                                //Rebuild nearby world
                                mWorld->RebuildOpaqueMeshChunk(curchunkTarget);
                                fppCam->needUpdate = true;

                                mSoundMgr->PlayWalkSound(1); // play dirt sound
                                changeY = -0.16f;
                                anim[0] = 1;
                                return;
                            }
                        }

                        if (selectedBlock == NoteBlock::getID())
                        {
                            int noteBlockId = mWorld->FindNoteBlockID(testPos);

                            if(noteBlockId != -1)
                            {
                                if(mWorld->GetBlock(testPos.x,testPos.y+1,testPos.z) == 0)
                                {
                                    NoteBlockEntity* TestNoteBlock = mWorld->mNoteBlocks[noteBlockId];

                                    TestNoteBlock->IncreaseNote();
                                    mSoundMgr->PlayNoteSound(!(mWorld->BlockMaterial(testPos.x,testPos.y-1,testPos.z) == 1 || mWorld->BlockMaterial(testPos.x,testPos.y-1,testPos.z) == 8),TestNoteBlock->GetPitch());

                                    float red, blue, green;
                                    if(TestNoteBlock->GetNote() <= 12)
                                    {
                                        blue = 1.0 - (TestNoteBlock->GetNote()/12.0f);
                                        green = (TestNoteBlock->GetNote()/12.0f);
                                        red = 0.0f;
                                    }
                                    else
                                    {
                                        blue = 0.0f;
                                        green = 1.0 - ((TestNoteBlock->GetNote()-12)/12.0f);
                                        red = ((TestNoteBlock->GetNote()-12)/12.0f);
                                    }
                                    Particle* Note = new Particle(mWorld,"note",Vector3((int)testPos.x+0.5f,(int)testPos.y+1.1f,(int)testPos.z+0.5f));
                                    Note->SetColor(Vector3(red,green,blue));
                                    mParticles.push_back(Note);
                                }

                                animGo = true;
                                animDest = 0.0f;

                                changeY = -0.1f;
                                anim[0] = 1;

                                return;
                            }
                        }

                        if(selectedBlock >= ItemFrame1::getID() && selectedBlock <= ItemFrame4::getID())
                        {
                            int itemFrameId = mWorld->FindItemFrameID(testPos);

                            if(itemFrameId != -1)  // set item in item frame
                            {
                                ItemFrameEntity* TestItemFrame = mWorld->mItemFrames[itemFrameId];

                                if(mWorld->invId[27+barPosition] != -1)
                                {
                                    if(TestItemFrame->GetItem() == -1)
                                    {
                                        TestItemFrame->SetItem(mWorld->invId[27+barPosition]);

                                        if(mWorld->invSt[27+barPosition] == true)
                                        {
                                            mWorld->invAm[27+barPosition] -= 1;
                                            if(mWorld->invAm[27+barPosition] == 0)
                                            {
                                                mWorld->invAm[27+barPosition] = -1;
                                                mWorld->invSt[27+barPosition] = 0;
                                                mWorld->invId[27+barPosition] = -1;
                                            }
                                        }
                                        else
                                        {
                                            TestItemFrame->SetAmount(mWorld->invAm[27+barPosition]);

                                            mWorld->invAm[27+barPosition] = -1;
                                            mWorld->invSt[27+barPosition] = 0;
                                            mWorld->invId[27+barPosition] = -1;
                                        }

                                        animGo = true;
                                        animDest = 0.0f;

                                        changeY = -0.1f;
                                        anim[0] = 1;

                                        return;
                                    }
                                }

                                if(TestItemFrame->GetItem() != -1)  // remove item in item frame
                                {
                                    if(TestItemFrame->GetItem() < 250)
                                    {
                                        mWorld->DropThis(TestItemFrame->GetItem(), 1, true, Vector3((int)testPos.x+0.5f,(int)testPos.y+0.5f,(int)testPos.z+0.5f));
                                    }
                                    else
                                    {
                                        if(TestItemFrame->GetAmount() != -1)
                                        {
                                            mWorld->DropThis(TestItemFrame->GetItem(), TestItemFrame->GetAmount(), mWorld->StackableItem(TestItemFrame->GetItem()), Vector3((int)testPos.x+0.5f,(int)testPos.y+0.5f,(int)testPos.z+0.5f));
                                        }
                                        else
                                        {
                                            if(TestItemFrame->GetItem() >= 250)
                                            {
                                                mWorld->DropThis(TestItemFrame->GetItem(), 1, mWorld->StackableItem(TestItemFrame->GetItem()), Vector3((int)testPos.x+0.5f,(int)testPos.y+0.5f,(int)testPos.z+0.5f));
                                            }
                                            else
                                            {
                                                mWorld->DropThis(TestItemFrame->GetItem(), 1, true, Vector3((int)testPos.x+0.5f,(int)testPos.y+0.5f,(int)testPos.z+0.5f));
                                            }
                                        }
                                    }

                                    TestItemFrame->ResetItem();
                                    TestItemFrame->SetAmount(-1);

                                    return;
                                }
                            }
                        }

                        if (selectedBlock == JukeBox::getID())
                        {
                            int jukeboxId = mWorld->FindJukeboxID(testPos);

                            if(jukeboxId != -1)
                            {
                                if(mWorld->GetBlock(testPos.x,testPos.y+1,testPos.z) == 0)
                                {
                                    JukeboxEntity* TestJukebox = mWorld->mJukeboxes[jukeboxId];

                                    if(TestJukebox->GetRecord() == -1)
                                    {
                                        if((mWorld->invId[27+barPosition] >= Disk1::getID() && mWorld->invId[27+barPosition] <= Disk3::getID()) || (mWorld->invId[27+barPosition] >= Disk4::getID() && mWorld->invId[27+barPosition] <= Disk12::getID()))
                                        {
                                            TestJukebox->SetRecord(mWorld->invId[27+barPosition]);

                                            int music_number = 0;
                                            if(mWorld->invId[27+barPosition] >= Disk1::getID() && mWorld->invId[27+barPosition] <= Disk3::getID())
                                            {
                                                music_number = mWorld->invId[27+barPosition]-Disk1::getID();
                                            }
                                            if(mWorld->invId[27+barPosition] >= Disk4::getID() && mWorld->invId[27+barPosition] <= Disk12::getID())
                                            {
                                                music_number = mWorld->invId[27+barPosition]-Disk4::getID()+3;
                                            }

                                            mSoundMgr->StopAmbient();
											mSoundMgr->StopCreative();
                                            mSoundMgr->StopDiskTrack();
                                            mSoundMgr->PlayDiskTrack(music_number);

                                            switch(music_number)
                                            {
                                                case 0: inputDiskName = "C418 - 13"; break;
                                                case 1: inputDiskName = "C418 - cat"; break;
                                                case 2: inputDiskName = "C418 - blocks"; break;
                                                case 3: inputDiskName = "C418 - chirp"; break;
                                                case 4: inputDiskName = "C418 - far"; break;
                                                case 5: inputDiskName = "C418 - mall"; break;
                                                case 6: inputDiskName = "C418 - mellohi"; break;
                                                case 7: inputDiskName = "C418 - stal"; break;
                                                case 8: inputDiskName = "C418 - strad"; break;
                                                case 9: inputDiskName = "C418 - ward"; break;
                                                case 10: inputDiskName = "C418 - 11"; break;
                                                case 11: inputDiskName = "C418 - wait"; break;
                                            }

                                            inputDiskNameTimer = 2.4f;

                                            mWorld->invId[27+barPosition] = -1;
                                            mWorld->invAm[27+barPosition] = -1;
                                            mWorld->invSt[27+barPosition] = 0;

                                            musicTime = 360.0f;
                                        }
                                    }
                                    else
                                    {
                                        inputDiskName = "";
                                        inputDiskNameTimer = 0.0f;

                                        mWorld->DropThis(TestJukebox->GetRecord(), 1, false, Vector3((int)testPos.x+0.5f,(int)testPos.y+1.0f,(int)testPos.z+0.5f) );
                                        TestJukebox->SetRecord(-1);

                                        mSoundMgr->StopAmbient();
                                        mSoundMgr->StopCreative();
                                        mSoundMgr->StopDiskTrack();

                                        musicTime = 60.0f+rand()%60;
                                    }

                                    animGo = true;
                                    animDest = 0.0f;

                                    changeY = -0.1f;
                                    anim[0] = 1;
                                }
                                return;
                            }
                        }

                        if (selectedBlock == CraftingTable::getID()) // open crafting table menu
                        {
                            craft3xEn = true;
                            return;
                        }

                        if(mWorld->invId[27+barPosition] == BoneMeal::getID())
                        {
                            bool used = false;

                            if (selectedBlock == GrassBlock::getID())
                            {
                                for(int xx = testPos.x-2; xx <= testPos.x+2; xx ++)
                                {
                                    for(int zz = testPos.z-2; zz <= testPos.z+2; zz ++)
                                    {
                                        for(int yy = testPos.y; yy > 0; yy--)
                                        {
                                            if(mWorld->GetBlock(xx,yy,zz) != 0)
                                            {
                                                if(mWorld->GetBlock(xx,yy,zz) == GrassBlock::getID())
                                                {
                                                    if(mWorld->GetBlock(xx,yy+1,zz) == 0)
                                                    {
                                                        if(rand()%10<=5)
                                                        {
                                                            rand() % 2 == 0 ? mWorld->GetBlock(xx,yy+1,zz) = Flower1::getID() : mWorld->GetBlock(xx,yy+1,zz) = Flower2::getID();
                                                            if(rand()%2 == 0)
                                                            {
                                                                mWorld->GetBlock(xx,yy+1,zz) = TallGrass::getID();
                                                            }
                                                        }
                                                        break;
                                                    }
                                                    else
                                                    {
                                                        break;
                                                    }
                                                }
                                                else
                                                {
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                                used = true;
                            }
                            if (selectedBlock >= WheatBlock1::getID() && selectedBlock <= WheatBlock5::getID())
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = WheatBlock6::getID();
                                used = true;
                            }
                            if (selectedBlock >= MelonPlant1::getID() && selectedBlock <= MelonPlant3::getID())
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = MelonPlant4::getID();
                                used = true;
                            }
                            if (selectedBlock >= PumpkinPlant1::getID() && selectedBlock <= PumpkinPlant3::getID())
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = PumpkinPlant4::getID();
                                used = true;
                            }
                            if (selectedBlock >= PotatoPlant1::getID() && selectedBlock <= PotatoPlant3::getID())
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = PotatoPlant4::getID();
                                used = true;
                            }
                            if (selectedBlock >= CarrotPlant1::getID() && selectedBlock <= CarrotPlant3::getID())
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = CarrotPlant4::getID();
                                used = true;
                            }

                            if(used)
                            {
                                animGo = true;
                                animDest = 0.0f;

                                changeY = -0.1f;
                                anim[0] = 1;

                                mWorld->invAm[27+barPosition] -= 1;
                                if(mWorld->invAm[27+barPosition] == 0)
                                {
                                    mWorld->invAm[27+barPosition] = -1;
                                    mWorld->invSt[27+barPosition] = 0;
                                    mWorld->invId[27+barPosition] = -1;
                                }
                                int	curchunkTarget = mWorld->getChunkId(testPos);
                                mWorld->RebuildTransparentMeshChunk(curchunkTarget);

                                fppCam->needUpdate = true;
                                return;
                            }
                        }

                        // half block + half block = full block
                        if (mWorld->blockTypes[mWorld->GetBlock(testPos.x, testPos.y, testPos.z)].blockModel == 1 &&
                            mWorld->blockTypes[mWorld->invId[27+barPosition]].blockModel == 1  &&
                            mWorld->GetBlock(testPos.x, testPos.y, testPos.z) == mWorld->invId[27+barPosition]
                           )
                        {
                            int x = (int)testPos.x;
                            int y = (int)testPos.y;
                            int z = (int)testPos.z;

                            if(testPos.y - y > 0.4f)
                            {
                                BoundingBox blockBox = BoundingBox(Vector3(x,y,z),Vector3(x + 1,y + 1,z + 1));
                                BoundingBox playerBox;
                                if(walkingOnGround == false)
                                {
                                    playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.63f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                                }
                                else
                                {
                                    playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.59f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                                }

                                if(!blockBox.intersect(playerBox))
                                {
                                    switch(mWorld->invId[27+barPosition])
                                    {
                                        case 82 : mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = OakPlanks::getID(); break;
                                        case 83 : mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = CobbleStone::getID(); break;
                                        case 84 : mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = BrickBlock::getID(); break;
                                        case 85 : mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = DoubleSlab::getID(); break;
                                        case 86 : mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = SandStone::getID(); break;
                                        case 108 : mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = StoneBrick::getID(); break;
                                        case 157 : mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = BirchPlanks::getID(); break;
                                        case 158 : mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = SprucePlanks::getID(); break;
                                    }

                                    mSoundMgr->PlayWalkSound(mWorld->blockTypes[mWorld->invId[27+barPosition]].soundType);

                                    mWorld->invAm[27+barPosition] -= 1;
                                    if(mWorld->invAm[27+barPosition] == 0)
                                    {
                                        mWorld->invAm[27+barPosition] = -1;
                                        mWorld->invId[27+barPosition] = -1;
                                        mWorld->invSt[27+barPosition] = 0;
                                    }

                                    animGo = true;
                                    animDest = 0.0f;

                                    int	curchunkTarget = mWorld->getChunkId(testPos);
                                    fppCam->needUpdate = true;
                                    mWorld->RebuildOpaqueMeshChunk(curchunkTarget);
                                    mWorld->rebuildNearestChunks(curchunkTarget,testPos);
                                    return;
                                }
                            }
                        }


                        if (selectedBlock >= TrapdoorClosed1::getID() && selectedBlock <= TrapdoorClosed4::getID()) //door
                        {
                            //check if player dont stand inside the door block
                            int x = (int)testPos.x;
                            int y = (int)testPos.y;
                            int z = (int)testPos.z;
                            BoundingBox doorBox = BoundingBox(Vector3(x,y,z),Vector3(x + 1,y + 1,z + 1));
                            BoundingBox playerBox;
                            if(walkingOnGround == false)
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.63f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            else
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.59f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            if(!doorBox.intersect(playerBox))
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = mWorld->GetBlock(testPos.x, testPos.y, testPos.z) - 4;
                                mSoundMgr->PlayDoorSound(true);

                                int	curchunkTarget = mWorld->getChunkId(testPos);
                                fppCam->needUpdate = true;
                                mWorld->RebuildTransparentMeshChunk(curchunkTarget);
                                return;
                            }
                        }

                        if (selectedBlock >= TrapdoorOpen1::getID() && selectedBlock <= TrapdoorOpen4::getID()) //door
                        {
                            //check if player dont stand inside the door block
                            int x = (int)testPos.x;
                            int y = (int)testPos.y;
                            int z = (int)testPos.z;
                            BoundingBox doorBox = BoundingBox(Vector3(x,y,z),Vector3(x + 1,y + 1,z + 1));
                            BoundingBox playerBox;
                            if(walkingOnGround == false)
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.63f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            else
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.59f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }

                            if(!doorBox.intersect(playerBox))
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = mWorld->GetBlock(testPos.x, testPos.y, testPos.z) + 4;
                                mSoundMgr->PlayDoorSound(false);

                                int	curchunkTarget = mWorld->getChunkId(testPos);
                                fppCam->needUpdate = true;
                                mWorld->RebuildTransparentMeshChunk(curchunkTarget);
                                return;
                            }
                        }

                        if (selectedBlock >= 49 && selectedBlock <= 52) //door
                        {
                            //check if player dont stand inside the door block
                            int x = (int)testPos.x;
                            int y = (int)testPos.y;
                            int z = (int)testPos.z;
                            BoundingBox doorBox = BoundingBox(Vector3(x,y,z),Vector3(x + 1,y + 2,z + 1));
                            BoundingBox playerBox;
                            if(walkingOnGround == false)
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.63f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            else
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.59f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            if(!doorBox.intersect(playerBox))
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = mWorld->GetBlock(testPos.x, testPos.y, testPos.z) + 4;
                                mWorld->GetBlock(testPos.x, testPos.y+1, testPos.z) = 58;
                                mSoundMgr->PlayDoorSound(true);

                                int	curchunkTarget = mWorld->getChunkId(testPos);
                                fppCam->needUpdate = true;
                                mWorld->RebuildTransparentMeshChunk(curchunkTarget);
                                return;
                            }
                        }

                        if (selectedBlock >= 53 && selectedBlock <= 56) //door
                        {
                            //check if player dont stand inside the door block
                            int x = (int)testPos.x;
                            int y = (int)testPos.y;
                            int z = (int)testPos.z;
                            BoundingBox doorBox = BoundingBox(Vector3(x,y,z),Vector3(x + 1,y + 2,z + 1));
                            BoundingBox playerBox;
                            if(walkingOnGround == false)
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.63f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            else
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.59f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            if(!doorBox.intersect(playerBox))
                            {
                                mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = mWorld->GetBlock(testPos.x, testPos.y, testPos.z) - 4;
                                mWorld->GetBlock(testPos.x, testPos.y+1, testPos.z) = 57;
                                mSoundMgr->PlayDoorSound(false);

                                int	curchunkTarget = mWorld->getChunkId(testPos);
                                fppCam->needUpdate = true;
                                mWorld->RebuildTransparentMeshChunk(curchunkTarget);
                                return;
                            }
                        }

                        if (selectedBlock == 57) //door
                        {
                            //check if player dont stand inside the door block
                            int x = (int)testPos.x;
                            int y = (int)testPos.y;
                            int z = (int)testPos.z;
                            BoundingBox doorBox = BoundingBox(Vector3(x, y-2, z),Vector3(x + 1,y,z + 1));
                            BoundingBox playerBox;
                            if(walkingOnGround == false)
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.63f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            else
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.59f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            if(!doorBox.intersect(playerBox))
                            {
                                if (mWorld->GetBlock(testPos.x, testPos.y-1, testPos.z) >= 49 && mWorld->GetBlock(testPos.x, testPos.y-1, testPos.z) <= 52) //door
                                {
                                    mWorld->GetBlock(testPos.x, testPos.y-1, testPos.z) = mWorld->GetBlock(testPos.x, testPos.y-1, testPos.z) + 4;
                                    mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = 58;
                                    mSoundMgr->PlayDoorSound(true);

                                    int	curchunkTarget = mWorld->getChunkId(Vector3(testPos.x,testPos.y-1,testPos.z));
                                    fppCam->needUpdate = true;
                                    mWorld->RebuildTransparentMeshChunk(curchunkTarget);
                                    return;
                                }
                            }
                        }

                        if (selectedBlock == 58) //door
                        {
                            //check if player dont stand inside the door block
                            int x = (int)testPos.x;
                            int y = (int)testPos.y;
                            int z = (int)testPos.z;
                            BoundingBox doorBox = BoundingBox(Vector3(x, y-2, z),Vector3(x + 1,y,z + 1));
                            BoundingBox playerBox;
                            if(walkingOnGround == false)
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.63f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            else
                            {
                                playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.59f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                            }
                            if(!doorBox.intersect(playerBox))
                            {
                                if (mWorld->GetBlock(testPos.x, testPos.y-1, testPos.z) >= 53 && mWorld->GetBlock(testPos.x, testPos.y-1, testPos.z) <= 56) //door
                                {
                                    mWorld->GetBlock(testPos.x, testPos.y-1, testPos.z) = mWorld->GetBlock(testPos.x, testPos.y-1, testPos.z) - 4;
                                    mWorld->GetBlock(testPos.x, testPos.y, testPos.z) = 57;
                                    mSoundMgr->PlayDoorSound(false);

                                    int	curchunkTarget = mWorld->getChunkId(Vector3(testPos.x,testPos.y-1,testPos.z));
                                    fppCam->needUpdate = true;
                                    mWorld->RebuildTransparentMeshChunk(curchunkTarget);
                                    return;
                                }
                            }
                        }

                        if (selectedBlock == TNTBlock::getID() && mWorld->invId[27+barPosition] == FlintAndSteel::getID())//TNT explosion
                        {
                            mWorld->SpawnTNTentity(((int)testPos.x)+0.5f,((int)testPos.y)+0.5f,((int)testPos.z)+0.5f);
                            mWorld->GetBlock(testPos.x,testPos.y,testPos.z) = 0;
                            mSoundMgr->PlayFuseSound();
                            mWorld->RebuildOpaqueMeshChunk(mWorld->getChunkId(testPos));
                            fppCam->needUpdate = true;

                            mWorld->invAm[27+barPosition] -= 1;
                            if(mWorld->invAm[27+barPosition] <= 0)
                            {
                                mWorld->invId[27+barPosition] = -1;
                                mWorld->invAm[27+barPosition] = -1;
                                mWorld->invSt[27+barPosition] = 0;
                            }

                            animGo = true;
                            animDest = 0.0f;
                            return;
                        }
						
						//chest open function
                        if (selectedBlock == ChestBlock1::getID() || selectedBlock == ChestBlock2::getID() || selectedBlock == ChestBlock3::getID() || selectedBlock == ChestBlock4::getID())
                        {
							mSoundMgr->PlayChestSound();
                            chestId = FindChestId(testPos.x,testPos.y,testPos.z);
                            if(chestId != -1)
                            {
                                craft3xEn = false;
                                chestEn = true;
                                UseChest = mWorld->mChests[chestId];
                                return;
                            }
							
                        }

                        if (selectedBlock == FurnaceOff::getID() || selectedBlock == FurnaceOn::getID())
                        {
                            furnaceId = FindFurnaceId(testPos.x,testPos.y,testPos.z);
                            if(furnaceId != -1)
                            {
                                craft3xEn = false;
                                chestEn = false;
                                furnaceEn = true;
                                UseFurnace = mWorld->mFurnaces[furnaceId];
                                return;
                            }
                        }

                        if(selectedBlock >= BedBlock1::getID() && selectedBlock <= BedBlock8::getID() && sleepMessageTime <= 0.0f)
                        {
                            if(mWorld->worldDayTime >= 6 && mWorld->worldDayTime <= 21)
                            {
                                sleepMessageTime = 3.0f;
                                sleepMessageType = 1;
                                return;
                            }
							
                            if(mWorld->mZombies.empty() == false)
                            {
                                for(int f = 0; f < mWorld->mZombies.size(); f++)
                                {
                                    Zombie *TestZombie = mWorld->mZombies[f];
                                    if(abs(playerPosition.x-TestZombie->position.x) <= 7 && abs(playerPosition.z-TestZombie->position.z) <= 7 && abs(playerPosition.y-TestZombie->position.y) <= 4)
                                    {
                                        sleepMessageTime = 3.0f;
                                        sleepMessageType = 2;
                                        break;
                                    }
                                }
                                if(sleepMessageTime == 3.0f)
                                {
                                    return;
                                }
                            }
							
							if(mWorld->mSpiders.empty() == false)
                            {
                                for(int f = 0; f < mWorld->mSpiders.size(); f++)
                                {
                                    Spider *TestSpider = mWorld->mSpiders[f];
                                    if(abs(playerPosition.x-TestSpider->position.x) <= 7 && abs(playerPosition.z-TestSpider->position.z) <= 7 && abs(playerPosition.y-TestSpider->position.y) <= 4)
                                    {
                                        sleepMessageTime = 3.0f;
                                        sleepMessageType = 2;
                                        break;
                                    }
                                }
                                if(sleepMessageTime == 3.0f)
                                {
                                    return;
                                }
                            }

                            mWorld->playerSpawnPointPosition = Vector3((int)mWorld->playerPos.x+0.5f,mWorld->playerPos.y,(int)mWorld->playerPos.z+0.5f);
                            sleepTime = 4.6f;

                            if(selectedBlock == BedBlock1::getID() || selectedBlock == BedBlock5::getID())
                            {
                                Vector3 pillowPosition;
                                if(selectedBlock == BedBlock5::getID())
                                {
                                    pillowPosition.x = (int)testPos.x+0.75f;
                                    pillowPosition.y = (int)testPos.y+0.8f;
                                    pillowPosition.z = (int)testPos.z+0.5f;
                                }
                                if(selectedBlock == BedBlock1::getID())
                                {
                                    pillowPosition.x = (int)testPos.x+1.75f;
                                    pillowPosition.y = (int)testPos.y+0.8f;
                                    pillowPosition.z = (int)testPos.z+0.5f;
                                }

                                playerPosition = pillowPosition;

                                fppCam->PositionCamera(playerPosition.x,playerPosition.y,playerPosition.z, playerPosition.x-5.0f,playerPosition.y,playerPosition.z, 0.0f,1.0f,0.0f);
                                fppCam->RotateView(0,0,0,0);
                                fppCam->upDownAngle = 0;
                                fppCam->horAngle = 180;
                                return;
                            }
                            if(selectedBlock == BedBlock2::getID() || selectedBlock == BedBlock6::getID())
                            {
                                Vector3 pillowPosition;
                                if(selectedBlock == BedBlock6::getID())
                                {
                                    pillowPosition.x = (int)testPos.x+0.25f;
                                    pillowPosition.y = (int)testPos.y+0.8f;
                                    pillowPosition.z = (int)testPos.z+0.5f;
                                }
                                if(selectedBlock == BedBlock2::getID())
                                {
                                    pillowPosition.x = (int)testPos.x-0.75f;
                                    pillowPosition.y = (int)testPos.y+0.8f;
                                    pillowPosition.z = (int)testPos.z+0.5f;
                                }

                                playerPosition = pillowPosition;

                                fppCam->PositionCamera(playerPosition.x,playerPosition.y,playerPosition.z, playerPosition.x+5.0f,playerPosition.y,playerPosition.z, 0.0f,1.0f,0.0f);
                                fppCam->RotateView(0,0,180,0);
                                fppCam->upDownAngle = 0;
                                fppCam->horAngle = 0;
                                return;
                            }
                            if(selectedBlock == BedBlock3::getID() || selectedBlock == BedBlock7::getID())
                            {
                                Vector3 pillowPosition;
                                if(selectedBlock == BedBlock7::getID())
                                {
                                    pillowPosition.x = (int)testPos.x+0.5f;
                                    pillowPosition.y = (int)testPos.y+0.8f;
                                    pillowPosition.z = (int)testPos.z+0.75f;
                                }
                                if(selectedBlock == BedBlock3::getID())
                                {
                                    pillowPosition.x = (int)testPos.x+0.5f;
                                    pillowPosition.y = (int)testPos.y+0.8f;
                                    pillowPosition.z = (int)testPos.z+1.75f;
                                }

                                playerPosition = pillowPosition;

                                fppCam->PositionCamera(playerPosition.x,playerPosition.y,playerPosition.z, playerPosition.x,playerPosition.y,playerPosition.z-5.0f, 0.0f,1.0f,0.0f);
                                fppCam->RotateView(0,0,90,0);
                                fppCam->upDownAngle = 0;
                                fppCam->horAngle = 270;
                                return;
                            }
                            if(selectedBlock == BedBlock4::getID() || selectedBlock == BedBlock8::getID())
                            {
                                Vector3 pillowPosition;
                                if(selectedBlock == BedBlock8::getID())
                                {
                                    pillowPosition.x = (int)testPos.x+0.5f;
                                    pillowPosition.y = (int)testPos.y+0.8f;
                                    pillowPosition.z = (int)testPos.z+0.25f;
                                }
                                if(selectedBlock == BedBlock4::getID())
                                {
                                    pillowPosition.x = (int)testPos.x+0.5f;
                                    pillowPosition.y = (int)testPos.y+0.8f;
                                    pillowPosition.z = (int)testPos.z-0.75f;
                                }

                                playerPosition = pillowPosition;

                                fppCam->PositionCamera(playerPosition.x,playerPosition.y,playerPosition.z, playerPosition.x,playerPosition.y,playerPosition.z+5.0f, 0.0f,1.0f,0.0f);
                                fppCam->RotateView(0,0,270,0);
                                fppCam->upDownAngle = 0;
                                fppCam->horAngle = 90;
                                return;
                            }
                        }

                        if(ceilf(mWorld->HG) < 20.0f && selectedBlock != Soil::getID() && selectedBlock != WateredSoil::getID())
                        {
                            bool foodEaten = false;

                            if (mWorld->invId[27+barPosition] == 288) ///BREAD
                            {
                                foodEaten = true;
                                mWorld->HG += 5;
                            }

                            if (mWorld->invId[27+barPosition] == 315) ///COOKIE
                            {
                                foodEaten = true;
                                mWorld->HG += 1;
                                mWorld->HP += 1;
                            }

                            if (mWorld->invId[27+barPosition] == RottenFlesh::getID()) ///ROTTEN FLESH
                            {
                                foodEaten = true;
                                mWorld->HG += 1;

                                if(rand() % 100 < 45)
								{
									HGP = 20;
									HungerPoisoned = true;
								}
                            }

                            if (mWorld->invId[27+barPosition] == Potato::getID()) ///POTATO
                            {
                                foodEaten = true;
                                mWorld->HG += 1;
                            }

                            if (mWorld->invId[27+barPosition] == PoisonedPotato::getID()) ///POSIONED POTATO
                            {
                                foodEaten = true;
                                mWorld->HG += 1;
								HurtPlayer(1);
								HPE = 5;
								HPPoisoned = true;
                            }

                            if (mWorld->invId[27+barPosition] == BakedPotato::getID()) ///BAKED POTATO
                            {
                                foodEaten = true;
                                mWorld->HG += 5;
                            }

                            if (mWorld->invId[27+barPosition] == Carrot::getID()) ///CARROT
                            {
                                foodEaten = true;
                                mWorld->HG += 3;
                            }

                            if (mWorld->invId[27+barPosition] == 311) ///MELON
                            {
                                foodEaten = true;
                                mWorld->HG += 2;
                            }

                            if (mWorld->invId[27+barPosition] == 317) ///STEAK
                            {
                                foodEaten = true;
                                mWorld->HG += 8;
                            }

							if (mWorld->invId[27+barPosition] == 393) ///COOKEDPORCHOP
							{
							foodEaten = true;
							mWorld->HG += 8;
							}

							if (mWorld->invId[27+barPosition] == 395) ///COOKEDMUTTON
							{
							foodEaten = true;
							mWorld->HG += 8;
							}
							
							if (mWorld->invId[27+barPosition] == 397) ///COOKEDCHICKEN
							{
							foodEaten = true;
							mWorld->HG += 8;
							}
							
                            if (mWorld->invId[27+barPosition] == 316) ///RAWBEEF
                            {
                                foodEaten = true;
                                mWorld->HG += 3;
                            }
							
							if (mWorld->invId[27+barPosition] == 406) ///SPIDEREYE
                            {
                                foodEaten = true;
                                mWorld->HG += 2;
								HurtPlayer(1);
								HPE = 5;
								HPPoisoned = true;
                            }
							
							if (mWorld->invId[27+barPosition] == 396) ///RAWCHICKEN
                            {
                                foodEaten = true;
                                mWorld->HG += 2;
								
								if(rand() % 100 < 20)
								{
									HGP = 13;
									HungerPoisoned = true;
								}
                            }
							
							if (mWorld->invId[27+barPosition] == 394) ///RAWMUTTON
                            {
                                foodEaten = true;
                                mWorld->HG += 2;
                            }

                            if (mWorld->invId[27+barPosition] == 392) ///RAWPORKCHOP
                            {
                                foodEaten = true;
                                mWorld->HG += 3;
                            }

                            if (mWorld->invId[27+barPosition] == 284) ///APPLE
                            {
                                foodEaten = true;
                                mWorld->HG += 4;
                            }

                            if (mWorld->invId[27+barPosition] == 285) ///GOLDEN APPLE
                            {
                                foodEaten = true;
                                mWorld->HG = 20;
                                mWorld->HP = 20;
                            }

                            if (mWorld->invId[27+barPosition] == 303) ///MOOSHROOM BOWL
                            {
                                foodEaten = true;
                                mWorld->HG += 6;
                            }

                            if(foodEaten)
                            {
                                mSoundMgr->PlayEatSound();
                                mWorld->mainStatistics.foodEaten += 1;

                                changeY = -0.20f;
                                anim[0] = 1;

                                Vector3 rayParticleDir = fppCam->m_vView - fppCam->m_vPosition;
                                rayParticleDir.normalize();

                                Vector3 particlePos = fppCam->m_vPosition + (rayParticleDir * 0.38f);

                                if(mParticles.size() <= 52)
                                {
                                    for(int k = 0; k <= 5; k++) // 5 particles
                                    {
                                        Particle* FoodPart = new Particle(mWorld,"item crack",Vector3(particlePos.x,particlePos.y-0.18f,particlePos.z));
                                        FoodPart->var1 = mWorld->invId[27+barPosition]-250;
                                        mParticles.push_back(FoodPart);
                                    }
                                }

                                if(mWorld->HG >= 20)
                                {
                                    mWorld->HG = 19.999f;
                                }

                                if(mWorld->invId[27+barPosition] == MooshroomBowl::getID())
                                {
                                    mWorld->invId[27+barPosition] = 302; // turn it into simple stackable bowl
                                    mWorld->invSt[27+barPosition] = 1;
                                }
                                else
                                {
                                    mWorld->invAm[27+barPosition] -= 1;
                                    if (mWorld->invAm[27+barPosition] == 0)
                                    {
                                        mWorld->invAm[27+barPosition] = -1;
                                        mWorld->invId[27+barPosition] = -1;
                                        mWorld->invSt[27+barPosition] = 0;
                                    }
                                }
                                return;
                            }
                        }

                        BoundingBox testBox2 = BoundingBox(Vector3(cubePos.x - 0.5f,cubePos.y - 0.5f,cubePos.z - 0.5f),Vector3(cubePos.x + 0.5f,cubePos.y + 0.5f,cubePos.z + 0.5f));
                        Ray tesRay = Ray(fppCam->m_vPosition,rayDir);

                        float distance = -1.0f;
                        tesRay.hasIntersected(testBox2,distance);

                        bool wasUpdated = false;

                        if(distance != -1.0f)
                        {
                            Vector3 hitPos = fppCam->m_vPosition + (rayDir * distance);
                            Vector3 normal = (hitPos - cubePos);
                            normal.normalize();

                            Vector3 testPos2;

                            if(normal.x < -0.66f)
                                testPos2.x = -1.0f;
                            else if(normal.x > 0.66f)
                                testPos2.x = 1.0f;
                            else
                                testPos2.x = 0.0f;

                            if(normal.y < -0.66f)
                                testPos2.y = -1.0f;
                            else if(normal.y > 0.66f)
                                testPos2.y = 1.0f;
                            else
                                testPos2.y = 0.0f;

                            if(normal.z < -0.66f)
                                testPos2.z = -1.0f;
                            else if(normal.z > 0.66f)
                                testPos2.z = 1.0f;
                            else
                                testPos2.z = 0.0f;

                            testPos2 += testPos;

                            //check if this block is empty
                            if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == Lava::getID() ||
                               mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 4 ||
                               mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 0 ||
                               mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 93)
                            {
                                //now check if player is not there ;) we don't want to place cubes where we stand
                                int boxX = (int)testPos2.x;
                                int boxY = (int)testPos2.y;
                                int boxZ = (int)testPos2.z;

                                BoundingBox blockBox = BoundingBox(Vector3(boxX,boxY,boxZ),Vector3(boxX + 1,boxY + 1,boxZ + 1));
                                BoundingBox playerBox;
                                if(walkingOnGround == false)
                                {
                                    playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.63f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                                }
                                else
                                {
                                    playerBox = BoundingBox(Vector3(playerPosition.x - (mWorld->playerCollisionSize-0.03f),playerPosition.y - 1.59f,playerPosition.z - (mWorld->playerCollisionSize-0.03)),Vector3(playerPosition.x + (mWorld->playerCollisionSize-0.03),playerPosition.y + 0.2f,playerPosition.z + (mWorld->playerCollisionSize-0.03)));
                                }

                                if(!blockBox.intersect(playerBox) ||
                                   mWorld->invId[27+barPosition] == TorchItem::getID()    ||
                                   mWorld->invId[27+barPosition] == LadderItem::getID()   ||
                                   mWorld->invId[27+barPosition] == WheatSeeds::getID()   ||
                                   mWorld->invId[27+barPosition] == MelonSeeds::getID()   ||
                                   mWorld->invId[27+barPosition] == PumpkinSeeds::getID() ||
                                   mWorld->invId[27+barPosition] == Carrot::getID()       ||
                                   mWorld->invId[27+barPosition] == Potato::getID())
                                {
                                    //check if you want put light source or normal block
                                    if(mWorld->CanPutBlockHere(testPos2.x,testPos2.y,testPos2.z,mWorld->invId[27+barPosition]))
                                    {
                                        bool wasLight = false;
                                        int oldBlock = mWorld->invId[27+barPosition];

                                        if(mWorld->LightSourceBlock(mWorld->invId[27+barPosition]))
                                        {
                                            wasLight = true;
                                            mWorld->SetLigtSourcePosition(testPos2.x,testPos2.y,testPos2.z,mWorld->invId[27+barPosition]);
                                        }

                                        if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == Lava::getID())
                                        {
                                            wasLight = true;
                                            mWorld->RemoveLigtSourceAtPosition(testPos2.x,testPos2.y,testPos2.z,112);
                                            mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 0;
                                        }

                                        if (mWorld->invId[27+barPosition]<250) // if block in our hands
                                        {
                                            if (mWorld->invId[27+barPosition] >= BlackWoolCarpet::getID() && mWorld->invId[27+barPosition] <= WhiteWoolCarpet::getID()) // if carpet in our hands
                                            {
                                                if(mWorld->BlockSpecial(testPos2.x,testPos2.y-1,testPos2.z) != 2)
                                                {
                                                    if(mWorld->BlockSpecial(testPos2.x,testPos2.y-1,testPos2.z) != 0 || mWorld->BlockSolid(testPos2.x,testPos2.y-1,testPos2.z) == false)
                                                    {
                                                        return;
                                                    }
                                                }
                                            }

                                            if(mWorld->invId[27+barPosition] == NoteBlock::getID()) // if we are putting note block
                                            {
                                                NoteBlockEntity* NewNoteBlockEntity = new NoteBlockEntity(testPos2.x,testPos2.y,testPos2.z,12);
                                                mWorld->mNoteBlocks.push_back(NewNoteBlockEntity);
                                            }

                                            if(mWorld->invId[27+barPosition] == JukeBox::getID()) // if we are putting jukebox
                                            {
                                                JukeboxEntity* NewJukeboxEntity = new JukeboxEntity(testPos2.x,testPos2.y,testPos2.z,-1);
                                                mWorld->mJukeboxes.push_back(NewJukeboxEntity);
                                            }

                                            if(mWorld->invId[27+barPosition] == ChestBlock3::getID()) // if we are putting chest
                                            {
                                                Chest* NewChest =  new Chest(testPos2.x,testPos2.y,testPos2.z);
                                                mWorld->mChests.push_back(NewChest);
                                            }

                                            if(mWorld->invId[27+barPosition] == FurnaceOff::getID()) // if we are putting chest
                                            {
                                                Furnace* NewFurnace =  new Furnace(testPos2.x,testPos2.y,testPos2.z);

                                                NewFurnace->SetSide(0);
                                                if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                {
                                                    NewFurnace->SetSide(3);
                                                }
                                                if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                {
                                                    NewFurnace->SetSide(0);
                                                }
                                                if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                {
                                                    NewFurnace->SetSide(2);
                                                }
                                                if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                {
                                                    NewFurnace->SetSide(1);
                                                }

                                                mWorld->mFurnaces.push_back(NewFurnace);
                                            }

                                            if(mWorld->invId[27+barPosition]!=-1) //multi-texturing blocks
                                            {
                                                bool placed = true;

                                                switch(mWorld->invId[27+barPosition])
                                                {
                                                    case 75:
                                                    if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 75;
                                                    }
                                                    if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 76;
                                                    }
                                                    if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 73;
                                                    }
                                                    if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 74;
                                                    }
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 73;
                                                    }
                                                    break;

                                                    case 145:
                                                    if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 145;
                                                    }
                                                    if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 146;
                                                    }
                                                    if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 143;
                                                    }
                                                    if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 144;
                                                    }
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 143;
                                                    }
                                                    break;

                                                    case 96:
                                                    if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 96; // 3
                                                    }
                                                    if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 97; // 4
                                                    }
                                                    if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 94; // 1
                                                    }
                                                    if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 95; // 2
                                                    }
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 94;
                                                    }
                                                    break;

                                                    case 125:
                                                    if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 125; // 3
                                                    }
                                                    if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 126; // 4
                                                    }
                                                    if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 127; // 1
                                                    }
                                                    if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 128; // 2
                                                    }
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 125;
                                                    }
                                                    break;

                                                    case 161: // birch stair
                                                    if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 161; // 3
                                                    }
                                                    if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 162; // 4
                                                    }
                                                    if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 163; // 1
                                                    }
                                                    if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 164; // 2
                                                    }
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 161;
                                                    }
                                                    break;

                                                    case 165: // spruce stair
                                                    if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 165; // 3
                                                    }
                                                    if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 166; // 4
                                                    }
                                                    if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 167; // 1
                                                    }
                                                    if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 168; // 2
                                                    }
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 165;
                                                    }
                                                    break;

                                                    case 60:
                                                    if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 60; // 3
                                                    }
                                                    if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 61; // 4
                                                    }
                                                    if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 62; // 1
                                                    }
                                                    if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 63; // 2
                                                    }
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 60;
                                                    }
                                                    break;

                                                    case 67:
                                                    if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 67; // 3
                                                    }
                                                    if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 68; // 4
                                                    }
                                                    if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 69; // 1
                                                    }
                                                    if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 70; // 2
                                                    }
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 67;
                                                    }
                                                    break;

                                                    case 151:
                                                    if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                    {
                                                        if(mWorld->BlockSolid(testPos2.x,testPos2.y,testPos2.z+1))
                                                        {
                                                            mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 154; // 3
                                                        }
                                                        else
                                                        {
                                                            placed = false;
                                                        }
                                                    }
                                                    if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                    {
                                                        if(mWorld->BlockSolid(testPos2.x,testPos2.y,testPos2.z-1))
                                                        {
                                                            mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 153; // 4
                                                        }
                                                        else
                                                        {
                                                            placed = false;
                                                        }
                                                    }
                                                    if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                    {
                                                        if(mWorld->BlockSolid(testPos2.x+1,testPos2.y,testPos2.z))
                                                        {
                                                            mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 152; // 1
                                                        }
                                                        else
                                                        {
                                                            placed = false;
                                                        }
                                                    }
                                                    if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                    {
                                                        if(mWorld->BlockSolid(testPos2.x-1,testPos2.y,testPos2.z))
                                                        {
                                                            mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 151; // 2
                                                        }
                                                        else
                                                        {
                                                            placed = false;
                                                        }
                                                    }
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        if(mWorld->BlockSolid(testPos2.x-1,testPos2.y,testPos2.z))
                                                        {
                                                            mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 151;
                                                        }
                                                        else
                                                        {
                                                            placed = false;
                                                        }
                                                    }
                                                    break;


                                                    default:
                                                    mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = mWorld->invId[27+barPosition];
                                                    break;
                                                }

                                                if(placed)
                                                {
                                                    if(mWorld->invAm[27+barPosition]==1)
                                                    {
                                                        mWorld->invAm[27+barPosition]= -1;
                                                        mWorld->invId[27+barPosition]= -1;
                                                        mWorld->invSt[27+barPosition]= 0;
                                                    }
                                                    else
                                                    {
                                                        mWorld->invAm[27+barPosition]-= 1;
                                                    }
                                                }
                                                else
                                                {
                                                    return;
                                                }
                                            }
                                        }
                                        else //if it is item in our hand
                                        {
                                            if(mWorld->invId[27+barPosition] == TorchItem::getID()) // if it is torch
                                            {
                                                if(mWorld->BlockSolid(testPos2.x+1,testPos2.y,testPos2.z) == true || mWorld->BlockSolid(testPos2.x-1,testPos2.y,testPos2.z) == true || mWorld->BlockSolid(testPos2.x,testPos2.y,testPos2.z-1) == true || mWorld->BlockSolid(testPos2.x,testPos2.y,testPos2.z+1) == true || mWorld->BlockSolid(testPos2.x,testPos2.y-1,testPos2.z) == true)
                                                {
                                                    mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 48;
                                                    fppCam->needUpdate = true;

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                       mWorld->invAm[27+barPosition] = -1;
                                                       mWorld->invSt[27+barPosition] = 0;
                                                       mWorld->invId[27+barPosition] = -1;
                                                    }
                                                    wasLight = true;
                                                    mWorld->SetLigtSourcePosition(testPos2.x,testPos2.y,testPos2.z,mWorld->invId[27+barPosition]);
                                                }
                                                else
                                                {
                                                    return;
                                                }
                                            }

                                            if(mWorld->invId[27+barPosition] == WheatSeeds::getID()) // if it is seeds
                                            {
                                                if( mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == 79 || mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == 80) // planting seeds
                                                {
                                                    mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 134;
                                                    fppCam->needUpdate = true;

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                       mWorld->invAm[27+barPosition] = -1;
                                                       mWorld->invSt[27+barPosition] = 0;
                                                       mWorld->invId[27+barPosition] = -1;
                                                    }
                                                }
                                                else
                                                {
                                                    return;
                                                }
                                            }

                                            if(mWorld->invId[27+barPosition] == MelonSeeds::getID()) // if it is melon seeds
                                            {
                                                if( mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == Soil::getID() || mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == WateredSoil::getID()) // planting seeds
                                                {
                                                    mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = MelonPlant1::getID();
                                                    fppCam->needUpdate = true;

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                       mWorld->invAm[27+barPosition] = -1;
                                                       mWorld->invSt[27+barPosition] = 0;
                                                       mWorld->invId[27+barPosition] = -1;
                                                    }
                                                }
                                                else
                                                {
                                                    return;
                                                }
                                            }

                                            if(mWorld->invId[27+barPosition] == PumpkinSeeds::getID()) // if it is pumpkin seeds
                                            {
                                                if( mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == Soil::getID() || mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == WateredSoil::getID()) // planting seeds
                                                {
                                                    mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = PumpkinPlant1::getID();
                                                    fppCam->needUpdate = true;

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                       mWorld->invAm[27+barPosition] = -1;
                                                       mWorld->invSt[27+barPosition] = 0;
                                                       mWorld->invId[27+barPosition] = -1;
                                                    }
                                                }
                                                else
                                                {
                                                    return;
                                                }
                                            }

                                            if(mWorld->invId[27+barPosition] == Potato::getID())
                                            {
                                                if( mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == Soil::getID() || mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == WateredSoil::getID()) // planting seeds
                                                {
                                                    mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = PotatoPlant1::getID();
                                                    fppCam->needUpdate = true;

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                       mWorld->invAm[27+barPosition] = -1;
                                                       mWorld->invSt[27+barPosition] = 0;
                                                       mWorld->invId[27+barPosition] = -1;
                                                    }
                                                    mWorld->RebuildTransparentMeshChunk(mWorld->getChunkId(testPos2));

                                                    animGo = true;
                                                    animDest = 0.0f;

                                                    changeY = -0.1f;
                                                    anim[0] = 1;
                                                }
                                                return;
                                            }

                                            if(mWorld->invId[27+barPosition] == Carrot::getID())
                                            {
                                                if( mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == Soil::getID() || mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == WateredSoil::getID()) // planting seeds
                                                {
                                                    mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = CarrotPlant1::getID();
                                                    fppCam->needUpdate = true;

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                       mWorld->invAm[27+barPosition] = -1;
                                                       mWorld->invSt[27+barPosition] = 0;
                                                       mWorld->invId[27+barPosition] = -1;
                                                    }
                                                    mWorld->RebuildTransparentMeshChunk(mWorld->getChunkId(testPos2));

                                                    animGo = true;
                                                    animDest = 0.0f;

                                                    changeY = -0.1f;
                                                    anim[0] = 1;
                                                }
                                                return;
                                            }

                                            if(mWorld->invId[27+barPosition] == GlassPanelItem::getID()) // if it is glass panel
                                            {
                                                mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = GlassPanel::getID();

                                                fppCam->needUpdate = true;
                                                int chunkTarget = mWorld->getChunkId(testPos2);
                                                mWorld->RebuildTransparentMeshChunk(chunkTarget);
                                                mWorld->rebuildNearestChunks(chunkTarget,testPos2);

                                                mWorld->invAm[27+barPosition] -= 1;
                                                if(mWorld->invAm[27+barPosition] == 0)
                                                {
                                                   mWorld->invAm[27+barPosition] = -1;
                                                   mWorld->invSt[27+barPosition] = 0;
                                                   mWorld->invId[27+barPosition] = -1;
                                                }

                                                wasUpdated = true;
                                            }

                                            if(mWorld->invId[27+barPosition] == IronBarsItem::getID()) // if it is iron bars,kw
                                            {
                                                mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = IronBars::getID();

                                                fppCam->needUpdate = true;
                                                int chunkTarget = mWorld->getChunkId(testPos2);
                                                mWorld->RebuildTransparentMeshChunk(chunkTarget);
                                                mWorld->rebuildNearestChunks(chunkTarget,testPos2);

                                                mWorld->invAm[27+barPosition] -= 1;
                                                if(mWorld->invAm[27+barPosition] == 0)
                                                {
                                                   mWorld->invAm[27+barPosition] = -1;
                                                   mWorld->invSt[27+barPosition] = 0;
                                                   mWorld->invId[27+barPosition] = -1;
                                                }

                                                wasUpdated = true;
                                            }

                                            if(mWorld->invId[27+barPosition] >= 300 && mWorld->invId[27+barPosition] <= 301) // if it is moshrooms
                                            {
                                                mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 77 + (mWorld->invId[27+barPosition] - 300);
                                                fppCam->needUpdate = true;

                                                mWorld->invAm[27+barPosition] -= 1;
                                                if(mWorld->invAm[27+barPosition] == 0)
                                                {
                                                    mWorld->invAm[27+barPosition] = -1;
                                                    mWorld->invSt[27+barPosition] = 0;
                                                    mWorld->invId[27+barPosition] = -1;
                                                }
                                            }

                                            if(mWorld->invId[27+barPosition] >= 294 && mWorld->invId[27+barPosition] <= 296) // if it is flowers or saplings
                                            {
                                                if(mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == 1 || mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == 2) // if under is dirt or grass
                                                {
                                                    mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 119 + (mWorld->invId[27+barPosition] - 294); //very difficult expression >:
                                                    fppCam->needUpdate = true;

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                       mWorld->invAm[27+barPosition] = -1;
                                                       mWorld->invSt[27+barPosition] = 0;
                                                       mWorld->invId[27+barPosition] = -1;
                                                    }
                                                }
                                                else
                                                {
                                                    return;
                                                }
                                            }
											
											/*if(mWorld->invId[27+barPosition] == 378) //nether
											{
												int created1 = false;
												if(mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == 46)
												{
													if(mWorld->GetBlock(testPos2.x-1,testPos2.y-1,testPos2.z) == 46)
													{
														if(mWorld->GetBlock(testPos2.x+1,testPos2.y,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x+1,testPos2.y+1,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x+1,testPos2.y+2,testPos2.z) == 46)
														{
															if(mWorld->GetBlock(testPos2.x-2,testPos2.y,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x-2,testPos2.y+1,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x-2,testPos2.y+2,testPos2.z) == 46)
															{
																if(mWorld->GetBlock(testPos2.x,testPos2.y+3,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x-1,testPos2.y+3,testPos2.z) == 46)
																{
																	mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+2,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x-1,testPos2.y,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x-1,testPos2.y+1,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x-1,testPos2.y+2,testPos2.z) = 222;
																	created1 = true;
																}
															}
														}
													}
													if(mWorld->GetBlock(testPos2.x+1,testPos2.y-1,testPos2.z) == 46)
													{
														if(mWorld->GetBlock(testPos2.x-1,testPos2.y,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x-1,testPos2.y+1,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x-1,testPos2.y+2,testPos2.z) == 46)
														{
															if(mWorld->GetBlock(testPos2.x+2,testPos2.y,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x+2,testPos2.y+1,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x+2,testPos2.y+2,testPos2.z) == 46)
															{
																if(mWorld->GetBlock(testPos2.x,testPos2.y+3,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x+1,testPos2.y+3,testPos2.z) == 46)
																{
																	mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+2,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x+1,testPos2.y,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x+1,testPos2.y+1,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x+1,testPos2.y+2,testPos2.z) = 222;
																	created1 = true;
																}
															}
														}
													}

													if(mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z-1) == 46)
													{
														if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z+1) == 46 && mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z+1) == 46 && mWorld->GetBlock(testPos2.x,testPos2.y+2,testPos2.z+1) == 46)
														{
															if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z-2) == 46 && mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z-2) == 46 && mWorld->GetBlock(testPos2.x,testPos2.y+2,testPos2.z-2) == 46)
															{
																if(mWorld->GetBlock(testPos2.x,testPos2.y+3,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x,testPos2.y+3,testPos2.z-1) == 46)
																{
																	mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+2,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z-1) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z-1) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+2,testPos2.z-1) = 222;
																	created1 = true;
																}
															}
														}
													}

													if(mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z+1) == 46)
													{
														if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z-1) == 46 && mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z-1) == 46 && mWorld->GetBlock(testPos2.x,testPos2.y+2,testPos2.z-1) == 46)
														{
															if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z+2) == 46 && mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z+2) == 46 && mWorld->GetBlock(testPos2.x,testPos2.y+2,testPos2.z+2) == 46)
															{
																if(mWorld->GetBlock(testPos2.x,testPos2.y+3,testPos2.z) == 46 && mWorld->GetBlock(testPos2.x,testPos2.y+3,testPos2.z+1) == 46)
																{
																	mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+2,testPos2.z) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z+1) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z+1) = 222;
																	mWorld->GetBlock(testPos2.x,testPos2.y+2,testPos2.z+1) = 222;
																	created1 = true;
																}
															}
														}
													}
												}
												else
												{
													return;
												}
												if(created1 == true)
												{
													fppCam->needUpdate = true;

													mWorld->invAm[27+barPosition] -= 1;
													if(mWorld->invAm[27+barPosition] <= 0)
													{
														mWorld->invAm[27+barPosition] = -1;
														mWorld->invSt[27+barPosition] = 0;
														mWorld->invId[27+barPosition] = -1;
													}
												}
											}*/
														
											/// FIRE
                                            if(mWorld->invId[27+barPosition] == FlintAndSteel::getID())
                                            {
												if(mWorld->BlockSolid(testPos2.x,testPos2.y-1,testPos2.z) == true)
												{
													mSoundMgr->PlayIgniteSound();
													mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 219;
													fppCam->needUpdate = true;
													
													mWorld->invAm[27+barPosition] -= 1;
													if(mWorld->invAm[27+barPosition] == 0)
													{
													   mWorld->invAm[27+barPosition] = -1;
													   mWorld->invSt[27+barPosition] = 0;
													   mWorld->invId[27+barPosition] = -1;
													}
													wasLight = true;
													mWorld->SetLigtSourcePosition(testPos2.x,testPos2.y,testPos2.z,mWorld->invId[27+barPosition]);
												}
												else
												{
													return;
												}

                                            }
											
											/// SPAWN EGGS
											
											/*if(mWorld->invId[27+barPosition] == Stick::getID())
											{
													mWorld->SpawnSpider(testPos2.x,testPos2.y,testPos2.z);
													
													fppCam->needUpdate = true;
													int chunkTarget = mWorld->getChunkId(testPos2);
													mWorld->RebuildTransparentMeshChunk(chunkTarget);
													mWorld->rebuildNearestChunks(chunkTarget,testPos2);
											}
											
											if(mWorld->invId[27+barPosition] == Feather::getID())
											{
													mWorld->SpawnSpidercave(testPos2.x,testPos2.y,testPos2.z);
													
													fppCam->needUpdate = true;
													int chunkTarget = mWorld->getChunkId(testPos2);
													mWorld->RebuildTransparentMeshChunk(chunkTarget);
													mWorld->rebuildNearestChunks(chunkTarget,testPos2);
											}*/
											
											if(mWorld->invId[27+barPosition] == CowEGG::getID())
											{
													mWorld->SpawnCow(testPos2.x,testPos2.y,testPos2.z);
													
													fppCam->needUpdate = true;
													int chunkTarget = mWorld->getChunkId(testPos2);
													mWorld->RebuildTransparentMeshChunk(chunkTarget);
													mWorld->rebuildNearestChunks(chunkTarget,testPos2);
											}
											
											if(mWorld->invId[27+barPosition] == SpiderEGG::getID())
											{
													mWorld->SpawnSpider(testPos2.x,testPos2.y,testPos2.z);
													
													fppCam->needUpdate = true;
													int chunkTarget = mWorld->getChunkId(testPos2);
													mWorld->RebuildTransparentMeshChunk(chunkTarget);
													mWorld->rebuildNearestChunks(chunkTarget,testPos2);
											}
											
											if(mWorld->invId[27+barPosition] == SpidercaveEGG::getID())
											{
													mWorld->SpawnSpidercave(testPos2.x,testPos2.y,testPos2.z);
													
													fppCam->needUpdate = true;
													int chunkTarget = mWorld->getChunkId(testPos2);
													mWorld->RebuildTransparentMeshChunk(chunkTarget);
													mWorld->rebuildNearestChunks(chunkTarget,testPos2);
											}
											
											if(mWorld->invId[27+barPosition] == SheepEGG::getID())
											{
													mWorld->SpawnSheep(testPos2.x,testPos2.y,testPos2.z);
													
													fppCam->needUpdate = true;
													int chunkTarget = mWorld->getChunkId(testPos2);
													mWorld->RebuildTransparentMeshChunk(chunkTarget);
													mWorld->rebuildNearestChunks(chunkTarget,testPos2);
											}
											
											if(mWorld->invId[27+barPosition] == ChickenEGG::getID())
											{
													mWorld->SpawnChicken(testPos2.x,testPos2.y,testPos2.z);
													
													fppCam->needUpdate = true;
													int chunkTarget = mWorld->getChunkId(testPos2);
													mWorld->RebuildTransparentMeshChunk(chunkTarget);
													mWorld->rebuildNearestChunks(chunkTarget,testPos2);
											}
											
											if(mWorld->invId[27+barPosition] == PigEGG::getID())
											{
													mWorld->SpawnPig(testPos2.x,testPos2.y,testPos2.z);
													
													fppCam->needUpdate = true;
													int chunkTarget = mWorld->getChunkId(testPos2);
													mWorld->RebuildTransparentMeshChunk(chunkTarget);
													mWorld->rebuildNearestChunks(chunkTarget,testPos2);
											}
											
											if(mWorld->invId[27+barPosition] == ZombieEGG::getID())
											{
													mWorld->SpawnZombie(testPos2.x,testPos2.y,testPos2.z);
													
													fppCam->needUpdate = true;
													int chunkTarget = mWorld->getChunkId(testPos2);
													mWorld->RebuildTransparentMeshChunk(chunkTarget);
													mWorld->rebuildNearestChunks(chunkTarget,testPos2);
											}
											
											if(mWorld->invId[27+barPosition] == CreeperEGG::getID())
											{
													mWorld->SpawnCreeper(testPos2.x,testPos2.y,testPos2.z);
													
													fppCam->needUpdate = true;
													int chunkTarget = mWorld->getChunkId(testPos2);
													mWorld->RebuildTransparentMeshChunk(chunkTarget);
													mWorld->rebuildNearestChunks(chunkTarget,testPos2);
											}
											
											/// END

                                            if(mWorld->invId[27+barPosition] == 309 || mWorld->invId[27+barPosition] == 310) // if it is another sapling
                                            {
                                                if(mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == 1 || mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == 2 || mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == 29) // if under is dirt or grass
                                                {
                                                    mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 122 + (mWorld->invId[27+barPosition] - 309); //very difficult expression >:
                                                    fppCam->needUpdate = true;

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                       mWorld->invAm[27+barPosition] = -1;
                                                       mWorld->invSt[27+barPosition] = 0;
                                                       mWorld->invId[27+barPosition] = -1;
                                                    }
                                                }
                                                else
                                                {
                                                    return;
                                                }
                                            }

                                            if(mWorld->invId[27+barPosition] == 306) // if it is door item
                                            {
                                                if(mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z) == 0)
                                                {
                                                    if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 49; // 3
                                                        mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z) = 57;
                                                    }
                                                    if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 50; // 4
                                                        mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z) = 57;
                                                    }
                                                    if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 51; // 1
                                                        mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z) = 57;
                                                    }
                                                    if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 52; // 2
                                                        mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z) = 57;
                                                    }
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 49;
                                                        mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z) = 57;
                                                    }

                                                    fppCam->needUpdate = true;
                                                    int chunkTarget = mWorld->getChunkId(testPos2);
                                                    mWorld->RebuildTransparentMeshChunk(chunkTarget);
                                                    mWorld->rebuildNearestChunks(chunkTarget,testPos2);

                                                    wasUpdated = true;

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                        mWorld->invAm[27+barPosition] = -1;
                                                        mWorld->invSt[27+barPosition] = 0;
                                                        mWorld->invId[27+barPosition] = -1;
                                                    }

                                                    animGo = true;
                                                    animDest = 0.0f;
                                                }
                                                else
                                                {
                                                    return;
                                                }
                                            }


                                            if(mWorld->invId[27+barPosition] == BedItem::getID()) // if it is door item
                                            {
                                                bool placed = false;
                                                if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                {

                                                    if(mWorld->GetBlock(testPos2.x+1,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = BedBlock1::getID(); // 3
                                                        mWorld->GetBlock(testPos2.x+1,testPos2.y,testPos2.z) = BedBlock5::getID();

                                                        placed = true;
                                                    }
                                                }
                                                if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                {

                                                    if(mWorld->GetBlock(testPos2.x-1,testPos2.y,testPos2.z) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = BedBlock2::getID(); // 3
                                                        mWorld->GetBlock(testPos2.x-1,testPos2.y,testPos2.z) = BedBlock6::getID();

                                                        placed = true;
                                                    }
                                                }
                                                if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                {
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z+1) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = BedBlock3::getID(); // 3
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z+1) = BedBlock7::getID();

                                                        placed = true;
                                                    }
                                                }
                                                if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                {
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z-1) == 0)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = BedBlock4::getID(); // 3
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z-1) = BedBlock8::getID();

                                                        placed = true;
                                                    }
                                                }


                                                if(placed)
                                                {
                                                    fppCam->needUpdate = true;
                                                    int chunkTarget = mWorld->getChunkId(testPos2);
                                                    mWorld->RebuildTransparentMeshChunk(chunkTarget);
                                                    mWorld->rebuildNearestChunks(chunkTarget,testPos2);

                                                    wasUpdated = true;

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                        mWorld->invAm[27+barPosition] = -1;
                                                        mWorld->invSt[27+barPosition] = 0;
                                                        mWorld->invId[27+barPosition] = -1;
                                                    }

                                                    animGo = true;
                                                    animDest = 0.0f;

                                                    return;
                                                }
                                            }


                                            if(mWorld->invId[27+barPosition] == ItemFrameItem::getID()) // if it is door item
                                            {
                                                bool placed = false;
                                                unsigned int facing = 0;

                                                if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                {
                                                    if(mWorld->BlockSolid(testPos2.x,testPos2.y,testPos2.z+1))
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = ItemFrame4::getID();

                                                        facing = 4;
                                                        placed = true;
                                                    }
                                                }
                                                if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                {
                                                    if(mWorld->BlockSolid(testPos2.x,testPos2.y,testPos2.z-1))
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = ItemFrame3::getID();

                                                        facing = 3;
                                                        placed = true;
                                                    }
                                                }
                                                if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                {
                                                    if(mWorld->BlockSolid(testPos2.x+1,testPos2.y,testPos2.z))
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = ItemFrame2::getID();

                                                        facing = 2;
                                                        placed = true;
                                                    }
                                                }
                                                if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                {
                                                    if(mWorld->BlockSolid(testPos2.x-1,testPos2.y,testPos2.z))
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = ItemFrame1::getID();

                                                        facing = 1;
                                                        placed = true;
                                                    }
                                                }

                                                if(placed)
                                                {
                                                    ItemFrameEntity* NewItemFrameEntity = new ItemFrameEntity(testPos2.x,testPos2.y,testPos2.z,facing);
                                                    mWorld->mItemFrames.push_back(NewItemFrameEntity);

                                                    fppCam->needUpdate = true;
                                                    int chunkTarget = mWorld->getChunkId(testPos2);

                                                    mWorld->RebuildTransparentMeshChunk(chunkTarget);

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                        mWorld->invAm[27+barPosition] = -1;
                                                        mWorld->invSt[27+barPosition] = 0;
                                                        mWorld->invId[27+barPosition] = -1;
                                                    }

                                                    wasUpdated = true;
                                                }
                                                else
                                                {
                                                    return;
                                                }
                                            }


                                            if(mWorld->invId[27+barPosition] == 307) // if it is ladder item
                                            {
                                                bool placed = false;
                                                if(fppCam->horAngle > 45 && fppCam->horAngle < 135)
                                                {
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z+1) != 0 && mWorld->BlockSolid(testPos2.x,testPos2.y,testPos2.z+1) == true)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 90; // 3
                                                        placed = true;
                                                    }
                                                }
                                                if(fppCam->horAngle > 225 && fppCam->horAngle < 315)
                                                {
                                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z-1) != 0 && mWorld->BlockSolid(testPos2.x,testPos2.y,testPos2.z-1) == true)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 89;
                                                        placed = true;
                                                    }
                                                }
                                                if(fppCam->horAngle < 45 || fppCam->horAngle > 315)
                                                {
                                                    if(mWorld->GetBlock(testPos2.x+1,testPos2.y,testPos2.z) != 0 && mWorld->BlockSolid(testPos2.x+1,testPos2.y,testPos2.z) == true)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 88;
                                                        placed = true;
                                                    }
                                                }
                                                if(fppCam->horAngle > 135 && fppCam->horAngle < 225)
                                                {
                                                    if(mWorld->GetBlock(testPos2.x-1,testPos2.y,testPos2.z) != 0 && mWorld->BlockSolid(testPos2.x-1,testPos2.y,testPos2.z) == true)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 87;
                                                        placed = true;
                                                    }
                                                }

                                                if(placed == true)
                                                {
                                                    fppCam->needUpdate = true;
                                                    int chunkTarget = mWorld->getChunkId(testPos2);

                                                    mWorld->RebuildTransparentMeshChunk(chunkTarget);
                                                    mWorld->rebuildNearestChunks(chunkTarget,testPos2);

                                                    wasUpdated = true;

                                                    mWorld->invAm[27+barPosition] -= 1;
                                                    if(mWorld->invAm[27+barPosition] == 0)
                                                    {
                                                        mWorld->invAm[27+barPosition] = -1;
                                                        mWorld->invSt[27+barPosition] = 0;
                                                        mWorld->invId[27+barPosition] = -1;
                                                    }
                                                }
                                                else
                                                {
                                                    return;
                                                }
                                            }


                                            if(mWorld->invId[27+barPosition] == 293) // if it is cane item
                                            {
                                                if(mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z) == 7) //check if under is sand
                                                {
                                                    if(mWorld->GetBlock(testPos2.x-1,testPos2.y-1,testPos2.z) == 4 || mWorld->GetBlock(testPos2.x+1,testPos2.y-1,testPos2.z) == 4 || mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z+1) == 4 || mWorld->GetBlock(testPos2.x,testPos2.y-1,testPos2.z-1) == 4)
                                                    {
                                                        mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 33;
                                                        fppCam->needUpdate = true;

                                                        mWorld->invAm[27+barPosition] -= 1;
                                                        if(mWorld->invAm[27+barPosition] == 0)
                                                        {
                                                            mWorld->invAm[27+barPosition] = -1;
                                                            mWorld->invSt[27+barPosition] = 0;
                                                            mWorld->invId[27+barPosition] = -1;
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    return;
                                                }
                                            }

                                            if(mWorld->invId[27+barPosition] == 291) // if it is water busket
                                            {
                                                int chunkTarget = mWorld->getChunkId(testPos2);
                                                mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 4;
                                                mWorld->invId[27+barPosition] = 290;

                                                //mWorld->rebuildChunk(chunkTarget);
                                                //mWorld->rebuildTransparentChunk(chunkTarget);
                                                fppCam->needUpdate = true;
                                            }
                                            if(mWorld->invId[27+barPosition] == 292) // if it is lava busket//lava
                                            {
                                                int chunkTarget = mWorld->getChunkId(testPos2);

                                                mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 112;
                                                mWorld->invId[27+barPosition] = 290;

                                                if(mWorld->mChunks[chunkTarget]->periodicallyUpadted == false)
                                                {
                                                    mWorld->updatingChunks.push_back(chunkTarget);
                                                    mWorld->mChunks[chunkTarget]->periodicallyUpadted = true;
                                                }

                                                wasLight = true;
                                                mWorld->SetLigtSourcePosition(testPos2.x,testPos2.y,testPos2.z,mWorld->invId[27+barPosition]);
                                                fppCam->needUpdate = true;
                                            }
                                        }

                                        // some animation stuff
                                        animGo = true;
                                        animDest = 0.0f;

                                        changeY = -0.1f;
                                        anim[0] = 1;

                                        mWorld->mainStatistics.blockPlaced += 1;

                                        int chunkTarget = mWorld->getChunkId(testPos2);

                                        if(oldBlock < 250 && oldBlock != -1)
                                        {
                                            mSoundMgr->PlayWalkSound(mWorld->blockTypes[oldBlock].soundType);
                                        }

                                        //rebuild
                                        if(wasLight == true)
                                        {
                                            mWorld->UpdateLightAreaIn(testPos2);
                                            mWorld->RebuildFullMeshChunk(chunkTarget);
                                            mWorld->rebuildNearestChunksForLight(chunkTarget,testPos2);
                                        }
                                        else
                                        {
                                            mWorld->initPutBlocksLight(testPos2.x, testPos2.y+2, testPos2.z);

                                            if(!wasUpdated)
                                            {
                                                if(mWorld->GetLightLevel(testPos2.x,testPos2.y,testPos2.z) != 0)
                                                {
                                                    mWorld->UpdateLightAreaIn(testPos2);
                                                }

                                                mWorld->RebuildFullMeshChunk(chunkTarget);

                                                if(mWorld->BlockTransparent(testPos2.x,testPos2.y,testPos2.z) || mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == WoodenFence::getID())
                                                {
                                                    mWorld->rebuildNearestChunks(chunkTarget,testPos2);
                                                }
                                            }
                                        }
                                    }
                                    fppCam->needUpdate = true;
                                }
                            }
                        }
                        break;
                    }
                }

                if(ceilf(mWorld->HG) < 20.0f)
                {
                    bool foodEaten = false;

                    if (mWorld->invId[27+barPosition] == 288) ///BREAD
                    {
                        foodEaten = true;
                        mWorld->HG += 5;
                    }

                    if (mWorld->invId[27+barPosition] == 315) ///COOKIE
                    {
                        foodEaten = true;
                        mWorld->HG += 1;
                        mWorld->HP += 1;
                    }

                    if (mWorld->invId[27+barPosition] == RottenFlesh::getID()) ///ROTTEN FLESH
                    {
                        foodEaten = true;
                        mWorld->HG += 1;

                        if(rand() % 100 < 45)
                        {
                            HGP = 20;
							HungerPoisoned = true;
                        }
                    }

                    if (mWorld->invId[27+barPosition] == Potato::getID()) ///POTATO
                    {
                        foodEaten = true;
                        mWorld->HG += 1;
                    }

                    if (mWorld->invId[27+barPosition] == PoisonedPotato::getID()) ///POSIONED POTATO
                    {
                        foodEaten = true;
                        mWorld->HG += 1;
						HurtPlayer(1);
						HPE = 5;
						HPPoisoned = true;
                    }

                    if (mWorld->invId[27+barPosition] == BakedPotato::getID()) ///BAKED POTATO
                    {
                        foodEaten = true;
                        mWorld->HG += 5;
                    }

                    if (mWorld->invId[27+barPosition] == Carrot::getID()) ///CARROT
                    {
                        foodEaten = true;
                        mWorld->HG += 3;
                    }

                    if (mWorld->invId[27+barPosition] == 311) ///MELON
                    {
                        foodEaten = true;
                        mWorld->HG += 2;
                    }

                    if (mWorld->invId[27+barPosition] == 317) ///STEAK
                    {
                        foodEaten = true;
                        mWorld->HG += 8;
                    }
					
					if (mWorld->invId[27+barPosition] == 393) ///COOKEDPORCHOP
                    {
                        foodEaten = true;
                        mWorld->HG += 8;
                    }

					if (mWorld->invId[27+barPosition] == 395) ///COOKEDMUTTON
                    {
                        foodEaten = true;
                        mWorld->HG += 8;
                    }
					
					if (mWorld->invId[27+barPosition] == 397) ///COOKEDCHICKEN
                    {
                        foodEaten = true;
                        mWorld->HG += 8;
                    }

                    if (mWorld->invId[27+barPosition] == 316) ///RAWBEEF
                    {
                        foodEaten = true;
                        mWorld->HG += 3;
                    }
					
					if (mWorld->invId[27+barPosition] == 406) ///SPIDEREYE
					{
						foodEaten = true;
						mWorld->HG += 2;
						HurtPlayer(1);
						HPE = 5;
						HPPoisoned = true;
					}
					
					if (mWorld->invId[27+barPosition] == 396) ///RAWCHICKEN
                    {
                        foodEaten = true;
                        mWorld->HG += 2;
						
						if(rand() % 100 < 20)
                        {
                            HGP = 13;
							HungerPoisoned = true;
                        }
                    }

                    if (mWorld->invId[27+barPosition] == 394) ///RAWMUTTON
                    {
                        foodEaten = true;
                        mWorld->HG += 2;
                    }
					
                    if (mWorld->invId[27+barPosition] == 392) ///RAWPORKCHOP
                    {
                        foodEaten = true;
                        mWorld->HG += 3;
                    }

                    if (mWorld->invId[27+barPosition] == 284) ///APPLE
                    {
                        foodEaten = true;
                        mWorld->HG += 4;
                    }

                    if (mWorld->invId[27+barPosition] == 285) ///GOLDEN APPLE
                    {
                        foodEaten = true;
                        mWorld->HG = 20;
                        mWorld->HP = 20;
                    }

                    if (mWorld->invId[27+barPosition] == 303) ///MOOSHROOM BOWL
                    {
                        foodEaten = true;
                        mWorld->HG += 6;
                    }

                    if(foodEaten)
                    {
                        mSoundMgr->PlayEatSound();
                        mWorld->mainStatistics.foodEaten += 1;

                        changeY = -0.20f;
                        anim[0] = 1;

                        Vector3 rayParticleDir = fppCam->m_vView - fppCam->m_vPosition;
                        rayParticleDir.normalize();

                        Vector3 particlePos = fppCam->m_vPosition + (rayParticleDir * 0.38f);

                        if(mParticles.size() <= 52)
                        {
                            for(int k = 0; k <= 5; k++) // 5 particles
                            {
                                Particle* FoodPart = new Particle(mWorld,"item crack",Vector3(particlePos.x,particlePos.y-0.18f,particlePos.z));
                                FoodPart->var1 = mWorld->invId[27+barPosition]-250;
                                mParticles.push_back(FoodPart);
                            }
                        }

                        if(mWorld->HG >= 20)
                        {
                            mWorld->HG = 19.999f;
                        }

                        if(mWorld->invId[27+barPosition] == MooshroomBowl::getID())
                        {
                            mWorld->invId[27+barPosition] = 302; // turn it into simple stackable bowl
                            mWorld->invSt[27+barPosition] = 1;
                        }
                        else
                        {
                            mWorld->invAm[27+barPosition] -= 1;
                            if (mWorld->invAm[27+barPosition] == 0)
                            {
                                mWorld->invAm[27+barPosition] = -1;
                                mWorld->invId[27+barPosition] = -1;
                                mWorld->invSt[27+barPosition] = 0;
                            }
                        }
                        return;
                    }
                }
            }


            if(keyPressed(InputHelper::Instance()->getButtonToAction(13)) || tryToDt == true)
            {
                tryToDt = false;

                //remove cube
                Vector3 rayDir = fppCam->m_vView - fppCam->m_vPosition;
                rayDir.normalize();

                //we are takin our positiona and view vector
                Vector3 testPos2;
                testPos2 = Vector3(-1,-1,-1);

                BoundingBox collider = mWorld->blockTypes[mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z)].collideBox;
                BoundingBox testBox = BoundingBox(testPos2.x+collider.min.x, testPos2.y+collider.min.y, testPos2.z+collider.min.z, testPos2.x+collider.max.x, testPos2.y+collider.max.y, testPos2.z+collider.max.z);

                //we are moving slowly to the target +=0.5
                for(float i = 0; i < 5.25f; i+=0.1f)
                {
                    testPos2 = fppCam->m_vPosition + (rayDir * i);

                    collider = mWorld->blockTypes[mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z)].collideBox;
                    testBox = BoundingBox(floorf(testPos2.x)+0.5f+collider.min.x, floorf(testPos2.y)+0.5f+collider.min.y, floorf(testPos2.z)+0.5f+collider.min.z, floorf(testPos2.x)+0.5f+collider.max.x, floorf(testPos2.y)+0.5f+collider.max.y, floorf(testPos2.z)+0.5f+collider.max.z);

                    bool kick = false;
                    if(mWorld->mZombies.empty() == false)
                    {
                        for(int f = 0; f < mWorld->mZombies.size(); f++)
                        {
                            Zombie *TestZombie = mWorld->mZombies[f];
                            if(TestZombie->DistanceToPlayer() < 3.16f && TestZombie->kicked == false && TestZombie->damaged == false)
                            {
                                if(TestZombie->bBox.contains(testPos2))
                                {
                                    float damage = 1.0f;
                                    bool weapon = false;

                                    damage = mWorld->FetchDamagePoints(mWorld->invId[27+barPosition], weapon);

                                    if(weapon == true)
                                    {
                                        mWorld->invAm[27+barPosition] -= 1+rand()%2;

                                        if(mWorld->invAm[27+barPosition] <= 0)
                                        {
                                            mWorld->invId[27+barPosition] = -1;
                                            mWorld->invAm[27+barPosition] = -1;
                                            mWorld->invSt[27+barPosition] = false;

                                            mSoundMgr->PlayBreakSound();
                                        }
                                    }

                                    TestZombie->TakeDamage(damage,0.75f,dt);
                                    kick = true;

                                    animGo = true;
                                    animDest = 0.0f;

                                    changeY = -0.1f;
                                    anim[0] = 1;
                                    break;
                                }
                            }
                        }
                        if(kick == true)
                        {
                            return;
                        }
                    }
					
					/// SPIDER
					
					if(mWorld->mSpiders.empty() == false)
                    {
                        for(int f = 0; f < mWorld->mSpiders.size(); f++)
                        {
                            Spider *TestSpider = mWorld->mSpiders[f];
                            if(TestSpider->DistanceToPlayer() < 4.16f && TestSpider->kicked == false && TestSpider->damaged == false)
                            {
                                if(TestSpider->bBox.contains(testPos2))
                                {
                                    float damage = 1.0f;
                                    bool weapon = false;

                                    damage = mWorld->FetchDamagePoints(mWorld->invId[27+barPosition], weapon);

                                    if(weapon == true)
                                    {
                                        mWorld->invAm[27+barPosition] -= 1+rand()%2;

                                        if(mWorld->invAm[27+barPosition] <= 0)
                                        {
                                            mWorld->invId[27+barPosition] = -1;
                                            mWorld->invAm[27+barPosition] = -1;
                                            mWorld->invSt[27+barPosition] = false;

                                            mSoundMgr->PlayBreakSound();
                                        }
                                    }

                                    TestSpider->TakeDamage(damage,0.75f,dt);
                                    kick = true;

                                    animGo = true;
                                    animDest = 0.0f;

                                    changeY = -0.1f;
                                    anim[0] = 1;
                                    break;
                                }
                            }
                        }
                        if(kick == true)
                        {
                            return;
                        }
                    }
					
					/// SPIDERCAVE
					
					if(mWorld->mSpidercaves.empty() == false)
                    {
                        for(int f = 0; f < mWorld->mSpidercaves.size(); f++)
                        {
                            Spidercave *TestSpidercave = mWorld->mSpidercaves[f];
                            if(TestSpidercave->DistanceToPlayer() < 4.16f && TestSpidercave->kicked == false && TestSpidercave->damaged == false)
                            {
                                if(TestSpidercave->bBox.contains(testPos2))
                                {
                                    float damage = 1.0f;
                                    bool weapon = false;

                                    damage = mWorld->FetchDamagePoints(mWorld->invId[27+barPosition], weapon);

                                    if(weapon == true)
                                    {
                                        mWorld->invAm[27+barPosition] -= 1+rand()%2;

                                        if(mWorld->invAm[27+barPosition] <= 0)
                                        {
                                            mWorld->invId[27+barPosition] = -1;
                                            mWorld->invAm[27+barPosition] = -1;
                                            mWorld->invSt[27+barPosition] = false;

                                            mSoundMgr->PlayBreakSound();
                                        }
                                    }

                                    TestSpidercave->TakeDamage(damage,0.75f,dt);
                                    kick = true;

                                    animGo = true;
                                    animDest = 0.0f;

                                    changeY = -0.1f;
                                    anim[0] = 1;
                                    break;
                                }
                            }
                        }
                        if(kick == true)
                        {
                            return;
                        }
                    }

                    if(mWorld->mCows.empty() == false)
                    {
                        for(int f = 0; f < mWorld->mCows.size(); f++)
                        {
                            Cow *TestCow = mWorld->mCows[f];
                            if(TestCow->DistanceToPlayer() < 2.75 && TestCow->kicked == false && TestCow->damaged == false)
                            {
                                if(TestCow->bBox.contains(testPos2))
                                {
                                    float damage = 1.0f;
                                    bool weapon = false;

                                    damage = mWorld->FetchDamagePoints(mWorld->invId[27+barPosition], weapon);

                                    if(weapon == true)
                                    {
                                        mWorld->invAm[27+barPosition] -= 1+rand()%2;

                                        if(mWorld->invAm[27+barPosition] <= 0)
                                        {
                                            mWorld->invId[27+barPosition] = -1;
                                            mWorld->invAm[27+barPosition] = -1;
                                            mWorld->invSt[27+barPosition] = false;

                                            mSoundMgr->PlayBreakSound();
                                        }
                                    }

                                    TestCow->TakeDamage(damage,0.7f,dt);
                                    kick = true;

                                    animGo = true;
                                    animDest = 0.0f;
                                    break;
                                }
                            }
                        }
                        if(kick == true)
                        {
                            return;
                        }
                    }
					
					if(mWorld->mChickens.empty() == false)
                    {
                        for(int f = 0; f < mWorld->mChickens.size(); f++)
                        {
                            Chicken *TestChicken = mWorld->mChickens[f];
                            if(TestChicken->DistanceToPlayer() < 2.75 && TestChicken->kicked == false && TestChicken->damaged == false)
                            {
                                if(TestChicken->bBox.contains(testPos2))
                                {
                                    float damage = 1.0f;
                                    bool weapon = false;

                                    damage = mWorld->FetchDamagePoints(mWorld->invId[27+barPosition], weapon);

                                    if(weapon == true)
                                    {
                                        mWorld->invAm[27+barPosition] -= 1+rand()%2;

                                        if(mWorld->invAm[27+barPosition] <= 0)
                                        {
                                            mWorld->invId[27+barPosition] = -1;
                                            mWorld->invAm[27+barPosition] = -1;
                                            mWorld->invSt[27+barPosition] = false;

                                            mSoundMgr->PlayBreakSound();
                                        }
                                    }

                                    TestChicken->TakeDamage(damage,0.7f,dt);
                                    kick = true;

                                    animGo = true;
                                    animDest = 0.0f;
                                    break;
                                }
                            }
                        }
                        if(kick == true)
                        {
                            return;
                        }
                    }//end//
					
                    if(mWorld->mPigs.empty() == false)
                    {
                        for(int f = 0; f < mWorld->mPigs.size(); f++)
                        {
                            Pig *TestPig = mWorld->mPigs[f];
                            if(TestPig->DistanceToPlayer() < 2.75 && TestPig->kicked == false && TestPig->damaged == false)
                            {
                                if(TestPig->bBox.contains(testPos2))
                                {
                                    float damage = 1.0f;
                                    bool weapon = false;

                                    damage = mWorld->FetchDamagePoints(mWorld->invId[27+barPosition], weapon);

                                    if(weapon == true)
                                    {
                                        mWorld->invAm[27+barPosition] -= 1+rand()%2;

                                        if(mWorld->invAm[27+barPosition] <= 0)
                                        {
                                            mWorld->invId[27+barPosition] = -1;
                                            mWorld->invAm[27+barPosition] = -1;
                                            mWorld->invSt[27+barPosition] = false;

                                            mSoundMgr->PlayBreakSound();
                                        }
                                    }

                                    TestPig->TakeDamage(damage,0.7f,dt);
                                    kick = true;

                                    animGo = true;
                                    animDest = 0.0f;
                                    break;
                                }
                            }
                        }
                        if(kick == true)
                        {
                            return;
                        }
                    }


                    if(mWorld->mSheeps.empty() == false)
                    {
                        for(int f = 0; f < mWorld->mSheeps.size(); f++)
                        {
                            Sheep *TestSheep = mWorld->mSheeps[f];
                            if(TestSheep->DistanceToPlayer() < 2.75 && TestSheep->kicked == false && TestSheep->damaged == false)
                            {
                                if(TestSheep->bBox.contains(testPos2))
                                {
                                    float damage = 1.0f;
                                    bool weapon = false;

                                    damage = mWorld->FetchDamagePoints(mWorld->invId[27+barPosition], weapon);

                                    if(weapon == true)
                                    {
                                        mWorld->invAm[27+barPosition] -= 1+rand()%2;

                                        if(mWorld->invAm[27+barPosition] <= 0)
                                        {
                                            mWorld->invId[27+barPosition] = -1;
                                            mWorld->invAm[27+barPosition] = -1;
                                            mWorld->invSt[27+barPosition] = false;

                                            mSoundMgr->PlayBreakSound();
                                        }
                                    }

                                    TestSheep->TakeDamage(damage,0.7f,dt);
                                    kick = true;

                                    animGo = true;
                                    animDest = 0.0f;
                                    break;
                                }
                            }
                        }
                        if(kick == true)
                        {
                            return;
                        }
                    }


                    if(mWorld->mCreepers.empty() == false)
                    {
                        for(int f = 0; f < mWorld->mCreepers.size(); f++)
                        {
                            Creeper *TestCreeper = mWorld->mCreepers[f];
                            if(mWorld->FastDistance2d(abs(TestCreeper->position.x-playerPosition.x)*10,abs(TestCreeper->position.z-playerPosition.z)*10)/10.0f + abs(TestCreeper->position.y-(playerPosition.y-0.7))/2.0f < 2.18 && TestCreeper->kicked == false && TestCreeper->damaged == false)
                            {
                                if(TestCreeper->bBox.contains(testPos2))
                                {
                                    float damage = 1.0f;
                                    bool weapon = false;

                                    damage = mWorld->FetchDamagePoints(mWorld->invId[27+barPosition], weapon);

                                    if(weapon == true)
                                    {
                                        mWorld->invAm[27+barPosition] -= 1+rand()%2;

                                        if(mWorld->invAm[27+barPosition] <= 0)
                                        {
                                            mWorld->invId[27+barPosition] = -1;
                                            mWorld->invAm[27+barPosition] = -1;
                                            mWorld->invSt[27+barPosition] = false;

                                            mSoundMgr->PlayBreakSound();
                                        }
                                    }

                                    TestCreeper->TakeDamage(damage,0.7f,dt);
                                    kick = true;

                                    animGo = true;
                                    animDest = 0.0f;
                                    break;
                                }
                            }
                        }
                        if(kick == true)
                        {
                            return;
                        }
                    }


                    //check if we touch something
                    if(mWorld->BlockEditable(testPos2.x,testPos2.y,testPos2.z) && testBox.contains(testPos2))
                    {
                        if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == NoteBlock::getID())
                        {
                            unsigned int noteBlockId = mWorld->FindNoteBlockID(testPos2);

                            if(noteBlockId != -1)
                            {
                                if(mWorld->GetBlock(testPos2.x,testPos2.y+1,testPos2.z) == 0)
                                {
                                    NoteBlockEntity* TestNoteBlock = mWorld->mNoteBlocks[noteBlockId];
                                    mSoundMgr->PlayNoteSound(!(mWorld->BlockMaterial(testPos2.x,testPos2.y-1,testPos2.z) == 1 || mWorld->BlockMaterial(testPos2.x,testPos2.y-1,testPos2.z) == 8),TestNoteBlock->GetPitch());

                                    float red, blue, green;
                                    if(TestNoteBlock->GetNote() <= 12)
                                    {
                                        blue = 1.0 - (TestNoteBlock->GetNote()/12.0f);
                                        green = (TestNoteBlock->GetNote()/12.0f);
                                        red = 0.0f;
                                    }
                                    else
                                    {
                                        blue = 0.0f;
                                        green = 1.0 - ((TestNoteBlock->GetNote()-12)/12.0f);
                                        red = ((TestNoteBlock->GetNote()-12)/12.0f);
                                    }
                                    Particle* Note = new Particle(mWorld,"note",Vector3((int)testPos2.x+0.5f,(int)testPos2.y+1.1f,(int)testPos2.z+0.5f));
                                    Note->SetColor(Vector3(red,green,blue));
                                    mParticles.push_back(Note);
                                }
                            }
                        }

                        if (startDt == false)
                        {
                            testPos1.x  = testPos2.x;
                            testPos1.y  = testPos2.y;
                            testPos1.z  = testPos2.z;
                            dET = 1.0f;
                            dT = 0.0f;
                            switch(mWorld->BlockMaterial(testPos1.x,testPos1.y,testPos1.z)) // fetch time of block breaking
                            {
                                case 1: // wood
                                if(mWorld->invId[27+barPosition] == WoodenAxe::getID()) // wooden axe
                                {
                                    dET = 1.5f; // 1.5 СЃРµРєСѓРЅРґС‹
                                }
                                else if(mWorld->invId[27+barPosition] == StoneAxe::getID()) // stone axe
                                {
                                    dET = 0.75f;
                                }
                                else if(mWorld->invId[27+barPosition] == IronAxe::getID()) // iron axe
                                {
                                    dET = 0.5f;
                                }
                                else if(mWorld->invId[27+barPosition] == DiamondAxe::getID()) // diamond axe
                                {
                                    dET = 0.4f;
                                }
                                else if(mWorld->invId[27+barPosition] == GoldenAxe::getID()) // golden axe
                                {
                                    dET = 0.25f;
                                }
                                else
                                {
                                    dET = 3.0f;
                                }
                                break;

                                case 2: // stone
                                if(mWorld->invId[27+barPosition] == WoodenPickaxe::getID())
                                {
                                    dET = 1.5f;
                                }
                                else if(mWorld->invId[27+barPosition] == StonePickaxe::getID())
                                {
                                    dET = 0.75f;
                                }
                                else if(mWorld->invId[27+barPosition] == IronPickaxe::getID())
                                {
                                    dET = 0.5f;
                                }
                                else if(mWorld->invId[27+barPosition] == DiamondPickaxe::getID())
                                {
                                    dET = 0.4f;
                                }
                                else if(mWorld->invId[27+barPosition] == GoldenPickaxe::getID())
                                {
                                    dET = 0.25f;
                                }
                                else
                                {
                                    dET = 10.0f;
                                }
                                break;

                                case 3: // soil
                                if(mWorld->invId[27+barPosition] == 260)
                                {
                                    dET = 0.45f;
                                }
                                else if(mWorld->invId[27+barPosition] == 261)
                                {
                                    dET = 0.25f;
                                }
                                else if(mWorld->invId[27+barPosition] == 262)
                                {
                                    dET = 0.15f;
                                }
                                else if(mWorld->invId[27+barPosition] == 263)
                                {
                                    dET = 0.15f;
                                }
                                else if(mWorld->invId[27+barPosition] == 264)
                                {
                                    dET = 0.1f;
                                }
                                else
                                {
                                    dET = 0.9f;
                                }
                                break;

                                case 4: // leaves
                                if(mWorld->invId[27+barPosition] == 275)
                                {
                                    dET = 0.1f;
                                }
                                else
                                {
                                    dET = 0.4f;
                                }
                                break;

                                case 5: // obsidian
                                if(mWorld->invId[27+barPosition] == 253)
                                {
                                    dET = 15.0f;
                                }
                                else
                                {
                                    dET = 250.0f;
                                }
                                break;

                                case 6: // bedrock
                                dET = 36000.0f;
                                break;

                                break;

                                case 7: // ores
                                if(mWorld->invId[27+barPosition] == 250)
                                {
                                    dET = 2.25f;
                                }
                                else if(mWorld->invId[27+barPosition] == 251)
                                {
                                    dET = 1.15f;
                                }
                                else if(mWorld->invId[27+barPosition] == 252)
                                {
                                    dET = 0.75f;
                                }
                                else if(mWorld->invId[27+barPosition] == 253)
                                {
                                    dET = 0.6f;
                                }
                                else if(mWorld->invId[27+barPosition] == 254)
                                {
                                    dET = 0.4f;
                                }
                                else
                                {
                                    dET = 15.0f;
                                }
                                break;

                                case 8: // strong wood
                                if(mWorld->invId[27+barPosition] == 265) // wooden axe
                                {
                                    dET = 1.9f; //
                                }
                                else if(mWorld->invId[27+barPosition] == 266) // stone axe
                                {
                                    dET = 0.95f;
                                }
                                else if(mWorld->invId[27+barPosition] == 267) // iron axe
                                {
                                    dET = 0.65f;
                                }
                                else if(mWorld->invId[27+barPosition] == 268) // diamond axe
                                {
                                    dET = 0.5f;
                                }
                                else if(mWorld->invId[27+barPosition] == 269) // golden axe
                                {
                                    dET = 0.35f;
                                }
                                else
                                {
                                    dET = 3.75f;
                                }
                                break;

                                case 9: // strong stone
                                if(mWorld->invId[27+barPosition] == 250)
                                {
                                    dET = 2.65f;
                                }
                                else if(mWorld->invId[27+barPosition] == 251)
                                {
                                    dET = 1.35f;
                                }
                                else if(mWorld->invId[27+barPosition] == 252)
                                {
                                    dET = 0.9f;
                                }
                                else if(mWorld->invId[27+barPosition] == 253)
                                {
                                    dET = 0.7f;
                                }
                                else if(mWorld->invId[27+barPosition] == 254)
                                {
                                    dET = 0.45f;
                                }
                                else
                                {
                                    dET = 17.5f;
                                }
                                break;

                                case 10: // metal
                                if(mWorld->invId[27+barPosition] == 250)
                                {
                                    dET = 25.0f;
                                }
                                else if(mWorld->invId[27+barPosition] == 251)
                                {
                                    dET = 10.0f;
                                }
                                else if(mWorld->invId[27+barPosition] == 252)
                                {
                                    dET = 5.0f;
                                }
                                else if(mWorld->invId[27+barPosition] == 253)
                                {
                                    dET = 2.5f;
                                }
                                else if(mWorld->invId[27+barPosition] == 254)
                                {
                                    dET = 25.0f;
                                }
                                else
                                {
                                    dET = 25.0f;
                                }
                                break;

                                default:
                                dET = 0.3f;
                                break;

                            }
                            dT = 0;
                            startDt = true;
                        }
                        break;
                    }
                }
            }

            //remove cube
            if(keyHold(InputHelper::Instance()->getButtonToAction(13)))
            {
                if (startDt == true)
                {
                    if(animGo == false)
                    {
                        mSoundMgr->PlayDigSound(mWorld->BlockSoundAtPos(Vector3(testPos1.x,testPos1.y,testPos1.z)));

                        animGo = true;
                        animDest = 0.0f;
                    }

                    if(headInWater) // slow removing under water
                    {
                        dT < dET ? dT += dt*0.2 : dT = 0;
                    }
                    else
                    {
                        dT < dET ? dT += dt : dT = 0;
                    }

                    dStd = ceilf(dT*(100/(float)dET))/10.0f;
                    //remove block
                    if (dT >= dET)
                    {
                        bool wasLight = false;
                        int oldBlock = 0;
                        oldBlock = mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z);
                        if(oldBlock != -1)//if it's light block
                        {
                            if(mWorld->LightSourceBlock(oldBlock))
                            {
                                wasLight = true;
                            }
                        }

                        if(oldBlock >= ChestBlock1::getID() && oldBlock <= ChestBlock4::getID()) // chest destroying
                        {
                            chestId = FindChestId(testPos1.x,testPos1.y,testPos1.z);
                            if(chestId != -1)
                            {
                                // drop all items in chest
                                Chest* TestChest = mWorld->mChests[chestId];
                                for(int i = 0; i <= 27; i++)
                                {
                                    if(TestChest->chestSlotId[i] > 0)
                                    {
                                        Vector3 dropVelocity = Vector3(0,0,0);

                                        dropVelocity.x = (-15+rand()%30)*0.1f;
                                        dropVelocity.y = 3.0f;
                                        dropVelocity.z = (-15+rand()%30)*0.1f;

                                        if(TestChest->chestSlotAm[i] > 0)
                                        {
                                            mWorld->DropThisNoAlign(TestChest->chestSlotId[i],TestChest->chestSlotAm[i],TestChest->chestSlotSt[i],Vector3(testPos1.x+0.5f,testPos1.y+0.25f,testPos1.z+0.5f),dropVelocity);

                                            TestChest->chestSlotAm[i] = -1;
                                            TestChest->chestSlotSt[i] = 0;
                                            TestChest->chestSlotId[i] = -1;
                                        }
                                    }
                                }

                                delete mWorld->mChests[chestId];
                                mWorld->mChests.erase(mWorld->mChests.begin()+chestId);
                            }
                        }

                        if(oldBlock == NoteBlock::getID())
                        {
                            int noteBlockId = mWorld->FindNoteBlockID(testPos1);

                            if(noteBlockId != -1)
                            {
                                delete mWorld->mNoteBlocks[noteBlockId];
                                mWorld->mNoteBlocks.erase(mWorld->mNoteBlocks.begin()+noteBlockId);
                            }
                        }

                        if(oldBlock == JukeBox::getID())
                        {
                            int jukeboxId = mWorld->FindJukeboxID(testPos1);

                            if(jukeboxId != -1)
                            {
                                if(mWorld->mJukeboxes[jukeboxId]->GetRecord() != -1)
                                {
                                    mWorld->DropThis(mWorld->mJukeboxes[jukeboxId]->GetRecord(), 1, false, Vector3((int)testPos1.x+0.5f,(int)testPos1.y+0.5f,(int)testPos1.z+0.5f) );
                                }

                                delete mWorld->mJukeboxes[jukeboxId];
                                mWorld->mJukeboxes.erase(mWorld->mJukeboxes.begin()+jukeboxId);
                                mSoundMgr->StopDiskTrack();
                            }
                        }

                        if(oldBlock == Spawner::getID())
                        {
                            int MonsterSpawnerId = mWorld->FindMonsterSpawnerID(testPos1);

                            if(MonsterSpawnerId != -1)
                            {
								int Acumulado = mWorld->EXP+1;
								mWorld->EXPcount += (35+rand()%40)*15/Acumulado;
								mSoundMgr->PlayOrbSound();
								
                                delete mWorld->mMonsterSpawners[MonsterSpawnerId];
                                mWorld->mMonsterSpawners.erase(mWorld->mMonsterSpawners.begin()+MonsterSpawnerId);
                            }
                        }

                        if(oldBlock >= ItemFrame1::getID() && oldBlock <= ItemFrame4::getID())
                        {
                            mWorld->DestroyItemFrameEntity(testPos1);
                        }

                        if(oldBlock == FurnaceOff::getID() || oldBlock == FurnaceOn::getID()) // furnace destroying
                        {
                            furnaceId = FindFurnaceId(testPos1.x,testPos1.y,testPos1.z);
                            if(furnaceId != -1)
                            {
                                // drop all items in furnace
                                Furnace* TestFurnace = mWorld->mFurnaces[furnaceId];
                                for(int i = 0; i <= 2; i++)
                                {
                                    if(TestFurnace->furnaceSlotId[i] > 0)
                                    {
                                        Vector3 dropVelocity = Vector3(0,0,0);

                                        dropVelocity.x = (-15+rand()%30)*0.1f;
                                        dropVelocity.y = 3.0f;
                                        dropVelocity.z = (-15+rand()%30)*0.1f;

                                        if(TestFurnace->furnaceSlotAm[i] > 0)
                                        {
                                            mWorld->DropThisNoAlign(TestFurnace->furnaceSlotId[i],TestFurnace->furnaceSlotAm[i],TestFurnace->furnaceSlotSt[i],Vector3(testPos1.x+0.5f,testPos1.y+0.25f,testPos1.z+0.5f),dropVelocity);

                                            TestFurnace->furnaceSlotAm[i] = -1;
                                            TestFurnace->furnaceSlotSt[i] = 0;
                                            TestFurnace->furnaceSlotId[i] = -1;
                                        }
                                    }
                                }
                                delete mWorld->mFurnaces[furnaceId];
                                mWorld->mFurnaces.erase(mWorld->mFurnaces.begin()+furnaceId);
                            }
                        }

                        if(oldBlock >= 57 && oldBlock <= 58)
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = 0;
                            mWorld->GetBlock(testPos1.x,testPos1.y-1,testPos1.z) = 0;
                        }

                        if(oldBlock >= 49 && oldBlock <= 56)
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = 0;
                            mWorld->GetBlock(testPos1.x,testPos1.y+1,testPos1.z) = 0;
                        }

                        if(oldBlock == BedBlock1::getID())
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = 0;
                            mWorld->GetBlock(testPos1.x+1,testPos1.y,testPos1.z) = 0;
                        }
                        if(oldBlock == BedBlock2::getID())
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = 0;
                            mWorld->GetBlock(testPos1.x-1,testPos1.y,testPos1.z) = 0;
                        }
                        if(oldBlock == BedBlock3::getID())
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = 0;
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z+1) = 0;
                        }
                        if(oldBlock == BedBlock4::getID())
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = 0;
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z-1) = 0;
                        }

                        if(oldBlock == BedBlock5::getID())
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = 0;
                            mWorld->GetBlock(testPos1.x-1,testPos1.y,testPos1.z) = 0;
                        }
                        if(oldBlock == BedBlock6::getID())
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = 0;
                            mWorld->GetBlock(testPos1.x+1,testPos1.y,testPos1.z) = 0;
                        }
                        if(oldBlock == BedBlock7::getID())
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = 0;
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z-1) = 0;
                        }
                        if(oldBlock == BedBlock8::getID())
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = 0;
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z+1) = 0;
                        }

                        if(oldBlock == IceBlock::getID())
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = WaterBlock::getID();
                        }
                        else
                        {
                            mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z) = 0;

                            int upper_block = mWorld->GetBlock(testPos1.x,testPos1.y+1,testPos1.z);

                            if(upper_block != 0)
                            {
                                if(mWorld->BlockSolid(testPos1.x,testPos1.y+1,testPos1.z) == false &&
                                   !(upper_block >= Door1::getID() && upper_block <= Door8::getID()) &&
                                   !(upper_block >= ItemFrame1::getID() && upper_block <= ItemFrame4::getID()) &&
                                   upper_block != Torch::getID() &&
                                   upper_block != WaterBlock::getID() &&
                                   upper_block != Lava::getID() &&
                                   upper_block != CaneBlock::getID() &&
                                   !(upper_block >= Ladder1::getID() && upper_block <= Ladder4::getID()) &&
                                   !(upper_block >= TrapdoorOpen1::getID() && upper_block <= TrapdoorClosed4::getID()) &&
								   upper_block != HellPortal::getID())
                                {
                                    Vector3 testPos2 = Vector3(testPos1.x,testPos1.y+1,testPos1.z);

                                    mWorld->DropThis(mWorld->BlockLoot(testPos2.x,testPos2.y,testPos2.z),testPos2);
                                    mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 0;

                                    if(mWorld->ChunksEqual(testPos1,testPos2) == false)
                                    {
                                        int chunkTarget = mWorld->getChunkId(testPos2);
                                        mWorld->RebuildTransparentMeshChunk(chunkTarget);
                                    }
                                }

                                for(int checkY = testPos1.y+1; checkY <= testPos1.y+5; checkY++)
                                {
                                    Vector3 testPos2 = Vector3(testPos1.x,checkY,testPos1.z);
                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) != CaneBlock::getID() && mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) != CactusBlock::getID())
                                    {
                                        if(mWorld->ChunksEqual(testPos1, testPos2) == false)
                                        {
                                            int chunkTarget = mWorld->getChunkId(testPos2);
                                            mWorld->RebuildTransparentMeshChunk(chunkTarget);
                                        }
                                        break;
                                    }

                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == CaneBlock::getID())
                                    {
                                        mWorld->DropThis(CaneItem::getID(),testPos2);
                                    }
                                    if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) == CactusBlock::getID())
                                    {
                                        mWorld->DropThis(CactusBlock::getID(),testPos2);
                                    }
                                    mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 0;
                                }
                            }
                        }

                        if(mWorld->invId[27+barPosition] >= 250 && mWorld->invId[27+barPosition] <= 275) // tool breaking
                        {
                            mWorld->invAm[27+barPosition] -= 1;
                            if(mWorld->invAm[27+barPosition] <= 0)
                            {
                                Vector3 rayParticleDir = fppCam->m_vView - fppCam->m_vPosition;
                                rayParticleDir.normalize();

                                Vector3 particlePos = fppCam->m_vPosition + (rayParticleDir * 0.38f);

                                if(mParticles.size() <= 52)
                                {
                                    for(int k = 0; k <= 5; k++) // 5 particles
                                    {
                                        Particle* ToolPart = new Particle(mWorld,"item crack",Vector3(particlePos.x,particlePos.y-0.18f,particlePos.z));
                                        ToolPart->var1 = mWorld->invId[27+barPosition]-250;
                                        mParticles.push_back(ToolPart);
                                    }
                                }

                                mWorld->invId[27+barPosition] = -1;
                                mWorld->invAm[27+barPosition] = -1;
                                mWorld->invSt[27+barPosition] = 0;
                                mSoundMgr->PlayBreakSound();
                            }
                        }

						int Acumulado = mWorld->EXP+1;
                        switch(oldBlock)
                        {
                        case 9: // oak leaves
                            if(mWorld->invId[27+barPosition] != 275)
                            {
                                if ((rand() % 50) == 0)
                                {
                                    mWorld->DropThis(284,testPos1);
                                }

                                if ((rand() % 15) == 0)
                                {
                                    mWorld->DropThis(296,testPos1);
                                }
                            }
                            else
                            {
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                            }
                        break;

                        case 37: // spruce leaves
                            if(mWorld->invId[27+barPosition] != 275)
                            {
                                if (rand() % 10 == 1)
                                {
                                    mWorld->DropThis(309,testPos1);
                                }
                            }
                            else
                            {
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                            }
                        break;

                        case 38: // birch leaves
                            if(mWorld->invId[27+barPosition] != 275)
                            {
                                if (rand() % 16 == 1)
                                {
                                    mWorld->DropThis(310,testPos1);
                                }
                            }
                            else
                            {
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                            }
                        break;

                        case 139: // wheat
                            mWorld->DropThis(Wheat::getID(),testPos1);
                            mWorld->DropThis(WheatSeeds::getID(),1+rand()%2,testPos1);
                        break;

                        case 93: // snow cover
                            if(mWorld->invId[27+barPosition] >= 260 && mWorld->invId[27+barPosition] <= 264)
                            {
                                mWorld->DropThis(299,4,testPos1);
                            }
                        break;

                        // ore digging with special tools
                        case 3: // stone
                            if(mWorld->invId[27+barPosition] >= WoodenPickaxe::getID() && mWorld->invId[27+barPosition] <= GoldenPickaxe::getID())
                            {
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                            }
                        break;

                        case 36: // cobblestone
                            if(mWorld->invId[27+barPosition] >= WoodenPickaxe::getID() && mWorld->invId[27+barPosition] <= GoldenPickaxe::getID())
                            {
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                            }
                        break;

                        case 41: // iron ore
                            if(mWorld->invId[27+barPosition] >= StonePickaxe::getID() && mWorld->invId[27+barPosition] <= GoldenPickaxe::getID())
                            {
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                            }
                        break;

                        case 42: // coal ore
                            if(mWorld->invId[27+barPosition] >= WoodenPickaxe::getID() && mWorld->invId[27+barPosition] <= GoldenPickaxe::getID())
                            {
								if(rand() % 10 < 8)
								{
									mWorld->EXPcount += (6+rand()%8)*15/Acumulado;	
									mSoundMgr->PlayOrbSound();
								}
								
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                            }
                        break;

                        case 43: // diamond ore
                            if(mWorld->invId[27+barPosition] >= IronPickaxe::getID() && mWorld->invId[27+barPosition] <= GoldenPickaxe::getID())
                            {
								
								mWorld->EXPcount += (13+rand()%17)*15/Acumulado;	
								mSoundMgr->PlayOrbSound();
								
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                            }
                        break;

                        case 44: // redstone ore
                            if(mWorld->invId[27+barPosition] >= IronPickaxe::getID() && mWorld->invId[27+barPosition] <= GoldenPickaxe::getID())
                            {
								mWorld->EXPcount += (8+rand()%13)*15/Acumulado;	
								mSoundMgr->PlayOrbSound();
								
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),3,testPos1);
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),1+rand()%2,testPos1);
                            }
                        break;

                        case 45: // lapis ore
                            if(mWorld->invId[27+barPosition] >= StonePickaxe::getID() && mWorld->invId[27+barPosition] <= GoldenPickaxe::getID())
                            {
								
								mWorld->EXPcount += (8+rand()%11)*15/Acumulado;	
								mSoundMgr->PlayOrbSound();
								
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),3,testPos1);
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),1+rand()%5,testPos1);
                            }
                        break;

                        case 46: // obsidian
                            if(mWorld->invId[27+barPosition] == DiamondPickaxe::getID() )
                            {
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),3,testPos1);
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),1+rand()%5,testPos1);
                            }
                        break;

                        case 5: // golden ore
                            if(mWorld->invId[27+barPosition] >= IronPickaxe::getID() && mWorld->invId[27+barPosition] <= GoldenPickaxe::getID())
                            {
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                            }
                        break;
                        //

                        case 99: // clay block
                            mWorld->DropThis(mWorld->LootBlock(oldBlock),2,testPos1);
                            mWorld->DropThis(mWorld->LootBlock(oldBlock),2,testPos1);
                        break;

                        case 81: // melon block
                            mWorld->DropThis(mWorld->LootBlock(oldBlock),2,testPos1);

                            mWorld->DropThis(mWorld->LootBlock(oldBlock),1+rand()%5,testPos1);
                        break;

                        case 133: // final melon plant
                            mWorld->DropThis(MelonSeeds::getID(),1+rand()%3,testPos1);
                        break;

                        case 181: // final pumpkin plant
                            mWorld->DropThis(PumpkinSeeds::getID(),1+rand()%3,testPos1);
                        break;

                        case 185: // matrue potato
                            mWorld->DropThis(Potato::getID(),1,testPos1);
                            mWorld->DropThis(Potato::getID(),rand() % 4,testPos1);

                            if(rand() % 100 <= 1)
                            {
                                mWorld->DropThis(PoisonedPotato::getID(),1,testPos1);
                            }
                        break;

                        case 189: // matrue carrot
                            mWorld->DropThis(Carrot::getID(),1,testPos1);
                            mWorld->DropThis(Carrot::getID(),rand() % 4,testPos1);
                        break;

                        case 33: // cane
                            mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                            for(int checkY = testPos1.y+1; checkY <= testPos1.y+5; checkY++)
                            {
                                Vector3 testPos2 = Vector3(testPos1.x,checkY,testPos1.z);
                                if(mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) != 33)
                                {
                                    if(mWorld->ChunksEqual(testPos1, testPos2) == false)
                                    {
                                        int chunkTarget = mWorld->getChunkId(testPos2);
                                        mWorld->RebuildTransparentMeshChunk(chunkTarget);
                                    }
                                    break;
                                }

                                mWorld->GetBlock(testPos2.x,testPos2.y,testPos2.z) = 0;
                                mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos2);
                            }
                        break;

                        case 116: // tall grass
                        if (rand() % 8 == 1)
                        {
                            mWorld->DropThis(WheatSeeds::getID(),testPos1);
                        }
                        break;

                        case 113: // gravel
                        if (rand() % 12 == 0)
                        {
                            mWorld->DropThis(Flint::getID(),testPos1);
                        }
                        else
                        {
                            mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                        }
                        break;

                        default: // all other blocks
                            mWorld->DropThis(mWorld->LootBlock(oldBlock),testPos1);
                        break;
                        }

                        mWorld->mainStatistics.blockDestroyed += 1;
                        if(mWorld->mainStatistics.blockDestroyed == 1)
                        {
                            tutorialTimer = 10.0f;
                        }

                        if(oldBlock == GlassBlock::getID() || oldBlock == IceBlock::getID())
                        {
                            mSoundMgr->PlayGlassSound();
                        }
                        else
                        {
                            if(oldBlock >= 0 && oldBlock < mWorld->blockTypes.size())
                            {
                                mSoundMgr->PlayEndDigSound(mWorld->blockTypes[oldBlock].soundType);
                            }
                        }

                        if(mParticles.size() <= 52 && oldBlock >= 0)
                        {
                            for(int k = 0; k <= 8; k++) // 9 particles
                            {
                                Particle* Smoke2 = new Particle(mWorld,"block crack",Vector3((int)testPos1.x + 0.5f + (-0.3+(rand() % 7)*0.1f),(int)testPos1.y + 0.5f + (-0.2+(rand() % 5)*0.1f),(int)testPos1.z + 0.5f + (-0.3+(rand() % 7)*0.1f)));
                                Smoke2->var1 = oldBlock;
                                mParticles.push_back(Smoke2);
                            }
                        }

                        //check if this block is a support for light block or ladder
                        mWorld->CheckForTorchSupport(testPos1.x,testPos1.y,testPos1.z,mWorld->GetBlock(testPos1.x,testPos1.y,testPos1.z));
                        mWorld->CheckForLadderSupport(testPos1.x,testPos1.y,testPos1.z);
                        mWorld->DestroyAroundTrapdoors(testPos1.x,testPos1.y,testPos1.z);
                        mWorld->DestroyAroundItemFrames(testPos1.x,testPos1.y,testPos1.z);

                        int chunkTarget = mWorld->getChunkId(testPos1);

                        if(chunkTarget != -1)
                        {
                            if(wasLight == true)
                            {
                                mWorld->RemoveLigtSourceAtPosition(testPos1.x,testPos1.y,testPos1.z,Torch::getID());
                                mWorld->UpdateLightAreaIn(testPos1);

                                mWorld->RebuildFullMeshChunk(chunkTarget);
                                mWorld->rebuildNearestChunksForLight(chunkTarget,testPos1);
                            }
                            else
                            {
                                mWorld->initPutBlocksLight(testPos1.x, testPos1.y+2, testPos1.z);
                                if(mWorld->GetLightLevel(testPos1.x-1,testPos1.y,testPos1.z) != 0 ||
                                   mWorld->GetLightLevel(testPos1.x+1,testPos1.y,testPos1.z) != 0 ||
                                   mWorld->GetLightLevel(testPos1.x,testPos1.y-1,testPos1.z) != 0 ||
                                   mWorld->GetLightLevel(testPos1.x,testPos1.y+1,testPos1.z) != 0 ||
                                   mWorld->GetLightLevel(testPos1.x,testPos1.y,testPos1.z-1) != 0 ||
                                   mWorld->GetLightLevel(testPos1.x,testPos1.y,testPos1.z+1) != 0)
                                {
                                    mWorld->UpdateLightAreaIn(testPos1);
                                }

                                mWorld->RebuildFullMeshChunk(chunkTarget);
                                mWorld->rebuildNearestChunks(chunkTarget,testPos1);
                            }
                        }

                        if(mWorld->HG > 0.01)
                        {
                            mWorld->HG -= (1.0f)/64.0f;
                        }

                        fppCam->needUpdate = true;
                        testPos1 = Vector3(-1,-1,-1);
                        startDt = false;
                        tryToDt = true;
                        dT = 0;
                        dET = 0;
                        chestId = -1;
                        dStd = -1;
                    }
                }
            }
            else
            {
                if (startDt != false)
                {
                    dT = 0;
                    dStd = -1;
                    startDt = false;
                    chestId = -1;
                }

            }
            }

            /// INVENTORY

            if(invEn == true || craft3xEn == true || chestEn == true || furnaceEn == true)
            {
                //Craft item
                if(mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE))
                {
                    if(invEn == true)
                    {
                        for(int i = 0; i <= 3; i++)
                        {
                            if(craftSlotId[i] > 0)
                            {
                                Vector3 dropVelocity = Vector3(0,0,0);

                                dropVelocity.x = (cosf((fppCam->horAngle/180.0f)*PI))*4.0f;
                                dropVelocity.y = 2.0f;
                                dropVelocity.z = (sinf((fppCam->horAngle/180.0f)*PI))*4.0f;

                                if(craftSlotAm[i] > 0)
                                {
                                    mWorld->DropThisNoAlign(craftSlotId[i],craftSlotAm[i],craftSlotSt[i],Vector3(fppCam->m_vPosition.x,fppCam->m_vPosition.y-0.25f,fppCam->m_vPosition.z),dropVelocity);

                                    craftSlotAm[i] = -1;
                                    craftSlotSt[i] = 0;
                                    craftSlotId[i] = -1;
                                }
                            }
                        }
                        craftItemId = -1;
                        craftItemAm = -1;
                        craftItemSt = false;
                    }

                    if(craft3xEn == true)
                    {
                        for(int i = 0; i <= 8; i++)
                        {
                            if(craftSlotId3[i] > 0)
                            {
                                Vector3 dropVelocity = Vector3(0,0,0);

                                dropVelocity.x = (cosf((fppCam->horAngle/180.0f)*PI))*4.0f;
                                dropVelocity.y = 2.0f;
                                dropVelocity.z = (sinf((fppCam->horAngle/180.0f)*PI))*4.0f;

                                if(craftSlotAm3[i] > 0)
                                {
                                    mWorld->DropThisNoAlign(craftSlotId3[i],craftSlotAm3[i],craftSlotSt3[i],Vector3(fppCam->m_vPosition.x,fppCam->m_vPosition.y-0.25f,fppCam->m_vPosition.z),dropVelocity);

                                    craftSlotAm3[i] = -1;
                                    craftSlotSt3[i] = 0;
                                    craftSlotId3[i] = -1;
                                }
                            }
                        }
                        craftItemId3 = -1;
                        craftItemAm3 = -1;
                        craftItemSt3 = false;
                    }

                    if(upEn == true)
                    {
                        invXPosition = 0;
                        invYPosition = 0;
                    }
                    selectInvSprite->SetPosition(168+(invXPosition * 18),149+(invYPosition * 18));

                    invEn = false;
                    craft3xEn = false;
                    furnaceEn = false;
					
					if(chestEn == true)
					{
						mSoundMgr->PlayChestClosedSound();
					}
                    chestEn = false;
                    upEn = false;
                    furnaceEn = false;
                    chestId = -1;
                    armorEn = false;

                    UseChest = 0;
                    UseFurnace = 0;

                    mWorld->haveCompass = mWorld->HaveItemInBarSlots(Compass::getID());
                    return;
                }

                if(keyPressed(InputHelper::Instance()->getButtonToAction(14))) // if you press R
                {
                    if(invEn == true) // if 2x craft menu is open
                    {
                        if(craftItemId != -1) // if craft cell is not empty
                        {
                            if(mWorld->mId == craftItemId) // if item in craft cell = item in your mouse
                            {
                                if(craftItemSt == 1) // if item in craft cell is stackble
                                {
                                    if(mWorld->mAm+craftItemAm <= 64) //if sum of item in you mouse and items in craft cell is lower or equal than 64
                                    {
                                        for(int i = 0; i <= 3; i++) // check items in craft menu
                                        {
                                            craftSlotAm[i] -= 1;
                                            if(craftSlotAm[i] == 0) // delete if 0
                                            {
                                                craftSlotId[i] = -1;
                                                craftSlotAm[i] = -1;
                                                craftSlotSt[i] = 0;
                                            }
                                        }

                                        mWorld->mAm += craftItemAm; // item in mouse ++
                                        mWorld->mainStatistics.itemsCrafted += craftItemAm;

                                        CraftItem2x2(); // repeat function because in craft menu we can lost items
                                    }
                                }
                            }

                            if(mWorld->mId == -1) //if you haven't mouse item
                            {
                                for(int i = 0; i <= 3; i++)
                                {
                                    craftSlotAm[i] -= 1;
                                    if(craftSlotAm[i] == 0)
                                    {
                                        craftSlotId[i] = -1;
                                        craftSlotAm[i] = -1;
                                        craftSlotSt[i] = 0;
                                    }
                                }
                                mWorld->mId = craftItemId; // mouse item id = id of craft cell item
                                mWorld->mAm = craftItemAm;
                                mWorld->mSt = craftItemSt;
                                mWorld->mainStatistics.itemsCrafted += craftItemAm;

                                CraftItem2x2();

                            }
                        }
                    }

					int Acumulado = mWorld->EXP+1;
                    if(furnaceEn == true)
                    {
                        if(UseFurnace->furnaceSlotId[2] != -1)
                        {
                            if(mWorld->mId == UseFurnace->furnaceSlotId[2])
                            {
                                if(UseFurnace->furnaceSlotSt[2] == 1)
                                {
                                    if(mWorld->mAm+UseFurnace->furnaceSlotAm[2] <= 64)
                                    {
										
										/// CONTADORES DE EXPERIENCIA POR FUNDIR ITEMS U BLOQUES
										
										int ExpId = UseFurnace->furnaceSlotId[2];
										int ExpAm = UseFurnace->furnaceSlotAm[2];
										
										if(ExpId == IronBar::getID() || ExpId == GoldenBar::getID() || ExpId == RockBlock::getID() || ExpId == Steak::getID() || ExpId == CookedChicken::getID() || ExpId == CookedMutton::getID() || ExpId == Brick::getID() || ExpId == BakedPotato::getID() || ExpId == CactusGreen::getID())
										{
											if(ExpAm <= 19)
											{
												int Divisor = ExpAm/2;
												int Multiplicador = 10 * Divisor;
												
												if(rand()%100 <= Multiplicador)
												{
													mWorld->EXPcount += (3+rand()%5)*15/Acumulado;
													mSoundMgr->PlayOrbSound();	
												}
											}
											else if(ExpAm >=20 && ExpAm <= 30)
											{
												mWorld->EXPcount += (5+rand()%7)*15/Acumulado;
												mSoundMgr->PlayOrbSound();
											}
											else
											{
												mWorld->EXPcount += (7+rand()%9)*15/Acumulado;
												mSoundMgr->PlayOrbSound();
											}
											
										}
										/// FIN-END
										
                                        mWorld->mAm += UseFurnace->furnaceSlotAm[2];

                                        UseFurnace->furnaceSlotAm[2] = -1;
                                        UseFurnace->furnaceSlotId[2] = -1;
                                        UseFurnace->furnaceSlotSt[2] = 0;
                                    }
                                }
                            }

                            if(mWorld->mId == -1) //if you haven't mouse item
                            {
								/// CONTADORES DE EXPERIENCIA POR FUNDIR ITEMS U BLOQUES
								
								int ExpId = UseFurnace->furnaceSlotId[2];
								int ExpAm = UseFurnace->furnaceSlotAm[2];
								
								if(ExpId == IronBar::getID() || ExpId == GoldenBar::getID() || ExpId == RockBlock::getID() || ExpId == Steak::getID() || ExpId == CookedChicken::getID() || ExpId == CookedMutton::getID() || ExpId == Brick::getID() || ExpId == BakedPotato::getID() || ExpId == CactusGreen::getID())
								{
									if(ExpAm <= 19)
									{
										int Divisor = ExpAm/2;
										int Multiplicador = 10 * Divisor;
										
										if(rand()%100 <= Multiplicador)
										{
											mWorld->EXPcount += (3+rand()%5)*15/Acumulado;
											mSoundMgr->PlayOrbSound();	
										}
									}
									else if(ExpAm >=20 && ExpAm <= 30)
									{
										mWorld->EXPcount += (5+rand()%7)*15/Acumulado;
										mSoundMgr->PlayOrbSound();
									}
									else
									{
										mWorld->EXPcount += (7+rand()%9)*15/Acumulado;
										mSoundMgr->PlayOrbSound();
									}
									
								}
								/// FIN-END

                                mWorld->mId = UseFurnace->furnaceSlotId[2];
                                mWorld->mAm = UseFurnace->furnaceSlotAm[2];
                                mWorld->mSt = UseFurnace->furnaceSlotSt[2];

                                UseFurnace->furnaceSlotId[2] = -1;
                                UseFurnace->furnaceSlotAm[2] = -1;
                                UseFurnace->furnaceSlotSt[2] = 0;
                            }
                        }
                    }


                    if(craft3xEn == true)
                    {
                        if(craftItemId3 != -1)
                        {

                            if(mWorld->mId == craftItemId3)
                            {
                                if(craftItemSt3 == 1)
                                {
                                    if(mWorld->mAm+craftItemAm3 <= 64)
                                    {
                                        for(int i = 0; i <= 8; i++)
                                        {
                                            craftSlotAm3[i] -= 1;
                                            if(craftSlotAm3[i] == 0)
                                            {
                                                craftSlotId3[i] = -1;
                                                craftSlotAm3[i] = -1;
                                                craftSlotSt3[i] = 0;
                                            }
                                        }

                                        mWorld->mAm += craftItemAm3;
                                        mWorld->mainStatistics.itemsCrafted += craftItemAm3;

                                        CraftItem3x3();
                                    }
                                }
                            }

                            if(mWorld->mId == -1)
                            {
                                for(int i = 0; i <= 8; i++)
                                {
                                    craftSlotAm3[i] -= 1;
                                    if(craftSlotAm3[i] == 0)
                                    {
                                        craftSlotId3[i] = -1;
                                        craftSlotAm3[i] = -1;
                                        craftSlotSt3[i] = 0;
                                    }
                                }
                                mWorld->mId = craftItemId3;
                                mWorld->mAm = craftItemAm3;
                                mWorld->mSt = craftItemSt3;
                                mWorld->mainStatistics.itemsCrafted += craftItemAm3;

                                CraftItem3x3();
                            }
                        }
                    }
                }

                //switch right
                if(keyPressed(InputHelper::Instance()->getButtonToAction(9)))
                {
                    if (upEn == 0) // if your mouse in neither of craft menus
                    {
						invXPosition != 8 ? invXPosition ++ : invXPosition = 8;
						selectInvSprite->SetPosition(168+(invXPosition * 18),149+(invYPosition * 18));
                    }
                    else
                    {
                        if(invEn == 1)
                        {
                            invXPosition != 1 ? invXPosition ++ : invXPosition = 1;
                            selectInvSprite->SetPosition(248 + (invXPosition * 18),87+(invYPosition * 18));
                        }
                        if(craft3xEn == 1)
                        {
                            invXPosition != 2 ? invXPosition ++ : invXPosition = 2;
                            selectInvSprite->SetPosition(204 + (invXPosition * 18),26+(invYPosition * 18));
                        }
                        if(chestEn == 1) // if it is in chest
                        {
                            invXPosition != 8 ? invXPosition ++ : invXPosition = 8;
                            selectInvSprite->SetPosition(96 + (invXPosition * 18),26+(invYPosition * 18));
                        }
                    }
                }

                //switch left
                if(keyPressed(InputHelper::Instance()->getButtonToAction(8)))
                {
                    if (upEn == 0) // not only craft but chest too
                    {
                        invXPosition != 0 ? invXPosition -- : invXPosition = 0;
                        selectInvSprite->SetPosition(168+(invXPosition * 18),149+(invYPosition * 18));
                    }
                    else
                    {
                        invXPosition != 0 ? invXPosition -- : invXPosition = 0;
                        if(invEn == 1)
                        {
                            selectInvSprite->SetPosition(248 + (invXPosition * 18),87+(invYPosition * 18));
                        }
                        if(craft3xEn == 1)
                        {
                            selectInvSprite->SetPosition(204 + (invXPosition * 36),26+(invYPosition * 36));
                        }
                        if(chestEn == 1)
                        {
                            selectInvSprite->SetPosition(96 + (invXPosition * 36),26+(invYPosition * 36));
                        }
                    }
                }

                //switch down
                if(keyPressed(InputHelper::Instance()->getButtonToAction(11)))
                {
                    if (upEn == 0)
                    {
                        if(armorEn == true)
                        {
                            //invYPosition ++;
                            selectInvSprite->SetPosition(168+(invXPosition * 0),69+(invYPosition * 18));
							
							if(invYPosition == 0)
							{
								if(invXPosition == 0)
								{
									invYPosition = 0;
									invXPosition = 1;
								}
								
								if(invXPosition == 1)
								{
									invYPosition = 1;
									invXPosition = 0;
								}
								
							}
							
							if(invYPosition == 1)
							{
								if(invXPosition == 0)
								{
									invYPosition = 1;
									invXPosition = 1;
								}
								
								if(invXPosition == 1)
								{
									invXPosition = 0;
									invYPosition = 0;
									armorEn = false;

									selectInvSprite->SetPosition(168+(invXPosition * 18),149+(invYPosition * 18));
								}
							}
                        }
                        else
                        {
                            invYPosition != 3 ? invYPosition ++ : invYPosition = 3;
                            selectInvSprite->SetPosition(168+(invXPosition * 18),149+(invYPosition * 18));
                        }
                    }
                    else
                    {
                        if (invEn == 1) //if mouse in the down of menu
                        {
                            if (invYPosition == 1)
                            {
                                upEn = 0; // we go to standart inv menu
                                invYPosition = 0;
                                invXPosition = 4 + invXPosition;
                                selectInvSprite->SetPosition(168+(invXPosition * 18),149+(invYPosition * 18));
                                return;
                            }
                            invYPosition != 1 ? invYPosition ++ : invYPosition = 1;
                            selectInvSprite->SetPosition(248 + (invXPosition * 18),87+(invYPosition * 18));
                        }

                        if (craft3xEn == 1)
                        {
                            if (invYPosition == 2)
                            {
                                upEn = 0;
                                invYPosition = 0;
                                invXPosition = 3 + invXPosition;
                                selectInvSprite->SetPosition(96+(invXPosition * 36),138+(invYPosition * 36));
                                return;
                            }
                            invYPosition != 2 ? invYPosition ++ : invYPosition = 2;
                            selectInvSprite->SetPosition(204 + (invXPosition * 36),26+(invYPosition * 36));
                        }

                        if (chestEn == 1)
                        {
                            if (invYPosition == 2)
                            {
                                upEn = 0;
                                invYPosition = 0;
                                selectInvSprite->SetPosition(96+(invXPosition * 36),138+(invYPosition * 36));
                                return;
                            }
                            invYPosition != 1 ? invYPosition ++ : invYPosition = 2;
                            selectInvSprite->SetPosition(96 + (invXPosition * 36),26+(invYPosition * 36));
                        }

                        if (furnaceEn == 1)
                        {
                            if (invYPosition == 1)
                            {
                                upEn = 0;
                                invYPosition = 0;
                                invXPosition = 3;
                                selectInvSprite->SetPosition(96+(invXPosition * 36),138+(invYPosition * 36));
                                return;
                            }
                            invYPosition != 1 ? invYPosition ++ : invYPosition = 1;
                            selectInvSprite->SetPosition(204 ,26+(invYPosition * 72));
                        }
                    }
                }

                //switch up
                if(keyPressed(InputHelper::Instance()->getButtonToAction(10)))
                {
                    if (upEn == 0)
                    {
                        if (invYPosition == 0)
                        {
                            if(invEn == 1)
                            {
                                if((invXPosition == 4 || invXPosition == 5) && armorEn == false)
                                {
                                    invXPosition -= 4;
                                    invYPosition = 1;
                                    selectInvSprite->SetPosition(248 + (invXPosition * 18),87+(invYPosition * 18));
                                    upEn = true;
                                }
                                if((invXPosition == 0 || invXPosition == 3) && armorEn == false && upEn == false)
                                {
                                    if(invXPosition == 3)
                                    {
                                        invXPosition = 1;
                                    }
                                    invYPosition = 1;
                                    selectInvSprite->SetPosition(168 + (invXPosition * 0),69+(invYPosition * 18));
                                    armorEn = true;
                                }
                            }
                            if(craft3xEn == 1)
                            {
                                if(invXPosition >= 3 && invXPosition <= 5)
                                {
                                    invXPosition -= 3;
                                    invYPosition = 2;
                                    selectInvSprite->SetPosition(204 + (invXPosition * 36),26+(invYPosition * 36));
                                    upEn = 1;
                                }
                            }
                            if(chestEn == 1)
                            {
                                invYPosition = 2;
                                selectInvSprite->SetPosition(96 + (invXPosition * 36),26+(invYPosition * 36));
                                upEn = 1;
                            }
                            if(furnaceEn == 1)
                            {
                                if(invXPosition == 3)
                                {
                                    invYPosition = 1;
                                    selectInvSprite->SetPosition(204,26+(invYPosition * 72));
                                    upEn = 1;
                                }
                            }
                            return;
                        }
                        invYPosition != 0 ? invYPosition -- : invYPosition = 0;
                        selectInvSprite->SetPosition(168+(invXPosition * 18),149+(invYPosition * 18));

                        if(armorEn == true)
                        {
                            selectInvSprite->SetPosition(168 + (invXPosition * 0),69+(invYPosition * 18));
                        }
                    }
                    else
                    {
                        invYPosition != 0 ? invYPosition -- : invYPosition = 0;

                        if(invEn == 1)
                        {
                            selectInvSprite->SetPosition(248 + (invXPosition * 18),87+(invYPosition * 18));
                        }
                        if(craft3xEn == 1)
                        {
                            selectInvSprite->SetPosition(204 + (invXPosition * 36),26+(invYPosition * 36));
                        }
                        if(chestEn == 1)
                        {
                            selectInvSprite->SetPosition(96 + (invXPosition * 36),26+(invYPosition * 36));
                        }
                        if(furnaceEn == 1)
                        {
                            selectInvSprite->SetPosition(204,26+(invYPosition * 72));
                        }
                    }
                }


                if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
                {
                    if (upEn == 0)
                    {
                        if(armorEn == false)
                        {
                            if (mWorld->mId == -1)
                            {
                                if (mWorld->invId[invYPosition*9 + invXPosition] != -1)
                                {
                                    mWorld->mId = mWorld->invId[invYPosition*9 + invXPosition];
                                    mWorld->mAm = mWorld->invAm[invYPosition*9 + invXPosition];
                                    mWorld->mSt = mWorld->invSt[invYPosition*9 + invXPosition];
                                    mWorld->invId[invYPosition*9 + invXPosition] = -1;
                                    mWorld->invAm[invYPosition*9 + invXPosition] = -1;
                                    mWorld->invSt[invYPosition*9 + invXPosition] = 0;
                                }
                            }
                            else
                            {
                                if (mWorld->invId[invYPosition*9 + invXPosition] == -1)
                                {
                                    mWorld->invId[invYPosition*9 + invXPosition]=mWorld->mId;
                                    mWorld->invAm[invYPosition*9 + invXPosition]=mWorld->mAm;
                                    mWorld->invSt[invYPosition*9 + invXPosition]=mWorld->mSt;
                                    mWorld->mId = -1;
                                    mWorld->mAm = -1;
                                    mWorld->mSt = 0;
                                }

                                if (mWorld->invId[invYPosition*9 + invXPosition] == mWorld->mId)
                                {
                                    if(mWorld->invSt[invYPosition*9 + invXPosition] == 1)
                                    {
                                        if (mWorld->invAm[invYPosition*9 + invXPosition]+mWorld->mAm < 64)
                                        {
                                            mWorld->invAm[invYPosition*9 + invXPosition]+=mWorld->mAm;
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                            mWorld->mSt = 0;
                                        }
                                        else
                                        {
                                            mWorld->mAm = (mWorld->invAm[invYPosition*9 + invXPosition] + mWorld->mAm) - 64;
                                            mWorld->invAm[invYPosition*9 + invXPosition]=64;

                                            if(mWorld->mAm == 0)
                                            {
                                                mWorld->mAm = -1;
                                                mWorld->mId = -1;
                                                mWorld->mSt = 0;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (mWorld->mId == -1)
                            {
                                if (mWorld->armorId[invYPosition*2 + invXPosition] != 324+invYPosition*2+invXPosition) // 324+invYPosition*2+invXPosition is an id of sample armor item
                                {
                                    mWorld->mId = mWorld->armorId[invYPosition*2 + invXPosition];
                                    mWorld->mAm = mWorld->armorAm[invYPosition*2 + invXPosition];
                                    mWorld->mSt = mWorld->armorSt[invYPosition*2 + invXPosition];
                                    mWorld->armorId[invYPosition*2 + invXPosition] = 324+invYPosition*2+invXPosition;
                                    mWorld->armorAm[invYPosition*2 + invXPosition] = -1;
                                    mWorld->armorSt[invYPosition*2 + invXPosition] = false;
                                }
                            }
                            else
                            {
                                if (mWorld->armorId[invYPosition*2 + invXPosition] == 324+invYPosition*2+invXPosition)
                                {
                                    if(mWorld->mId == 324+invYPosition*2+invXPosition+4 || /* all leather armor */
                                       mWorld->mId == 324+invYPosition*2+invXPosition+8 || /* all chain armor */
                                       mWorld->mId == 324+invYPosition*2+invXPosition+12|| /* all iron armor */
                                       mWorld->mId == 324+invYPosition*2+invXPosition+16|| /* all diamond armor */
                                       mWorld->mId == 324+invYPosition*2+invXPosition+20|| /* all golde armor */
                                       (invXPosition == 0 && invYPosition == 0 && mWorld->mId == Pumpkin3::getID()) /** it is pumpkin and cursor on helmet slot **/
                                      ) // crazy code monkey skill
                                    {
                                        mWorld->armorId[invYPosition*2 + invXPosition]=mWorld->mId;
                                        mWorld->armorAm[invYPosition*2 + invXPosition]=mWorld->mAm;
                                        mWorld->armorSt[invYPosition*2 + invXPosition]=mWorld->mSt;
                                        mWorld->mId = -1;
                                        mWorld->mAm = -1;
                                        mWorld->mSt = false;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (invEn == 1)
                        {
                            if (mWorld->mId == -1)
                            {
                                if (craftSlotId[invYPosition*2 + invXPosition] != -1)
                                {
                                    mWorld->mId = craftSlotId[invYPosition*2 + invXPosition];
                                    mWorld->mAm = craftSlotAm[invYPosition*2 + invXPosition];
                                    mWorld->mSt = craftSlotSt[invYPosition*2 + invXPosition];
                                    craftSlotId[invYPosition*2 + invXPosition] = -1;
                                    craftSlotAm[invYPosition*2 + invXPosition] = -1;
                                    craftSlotSt[invYPosition*2 + invXPosition] = 0;
                                }
                            }
                            else
                            {
                                if (craftSlotId[invYPosition*2 + invXPosition] == -1)
                                {
                                    craftSlotId[invYPosition*2 + invXPosition]=mWorld->mId;
                                    craftSlotAm[invYPosition*2 + invXPosition]=mWorld->mAm;
                                    craftSlotSt[invYPosition*2 + invXPosition]=mWorld->mSt;
                                    mWorld->mId = -1;
                                    mWorld->mAm = -1;
                                    mWorld->mSt = 0;
                                }

                                if (craftSlotId[invYPosition*2 + invXPosition] == mWorld->mId)
                                {
                                    if(craftSlotSt[invYPosition*2 + invXPosition] == 1)
                                    {
                                    if (craftSlotAm[invYPosition*2 + invXPosition]+mWorld->mAm < 64)
                                    {
                                        craftSlotAm[invYPosition*2 + invXPosition]+=mWorld->mAm;
                                        mWorld->mId = -1;
                                        mWorld->mAm = -1;
                                        mWorld->mSt = 0;
                                    }
                                    else
                                    {
                                        mWorld->mAm = (craftSlotAm[invYPosition*2 + invXPosition] + mWorld->mAm) - 64;
                                        craftSlotAm[invYPosition*2 + invXPosition]=64;

                                        if(mWorld->mAm == 0)
                                        {
                                            mWorld->mAm = -1;
                                            mWorld->mId = -1;
                                            mWorld->mSt = 0;
                                        }
                                    }
                                    }
                                }
                            }
                            CraftItem2x2();
                        }

                        if (craft3xEn == 1)
                        {
                            if (mWorld->mId == -1)
                            {
                                if (craftSlotId3[invYPosition*3 + invXPosition] != -1)
                                {
                                    mWorld->mId = craftSlotId3[invYPosition*3 + invXPosition];
                                    mWorld->mAm = craftSlotAm3[invYPosition*3 + invXPosition];
                                    mWorld->mSt = craftSlotSt3[invYPosition*3 + invXPosition];
                                    craftSlotId3[invYPosition*3 + invXPosition] = -1;
                                    craftSlotAm3[invYPosition*3 + invXPosition] = -1;
                                    craftSlotSt3[invYPosition*3 + invXPosition] = 0;
                                }
                            }
                            else
                            {
                                if (craftSlotId3[invYPosition*3 + invXPosition] == -1)
                                {
                                    craftSlotId3[invYPosition*3 + invXPosition]=mWorld->mId;
                                    craftSlotAm3[invYPosition*3 + invXPosition]=mWorld->mAm;
                                    craftSlotSt3[invYPosition*3 + invXPosition]=mWorld->mSt;
                                    mWorld->mId = -1;
                                    mWorld->mAm = -1;
                                    mWorld->mSt = 0;
                                }

                                if (craftSlotId3[invYPosition*3 + invXPosition] == mWorld->mId)
                                {
                                    if(craftSlotSt3[invYPosition*3 + invXPosition] == 1)
                                    {
                                        if (craftSlotAm3[invYPosition*3 + invXPosition]+mWorld->mAm < 64)
                                        {
                                            craftSlotAm3[invYPosition*3 + invXPosition]+=mWorld->mAm;
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                            mWorld->mSt = 0;
                                        }
                                        else
                                        {
                                            mWorld->mAm = (craftSlotAm3[invYPosition*3 + invXPosition] + mWorld->mAm) - 64;
                                            craftSlotAm3[invYPosition*3 + invXPosition]=64;

                                            if(mWorld->mAm == 0)
                                            {
                                                mWorld->mAm = -1;
                                                mWorld->mId = -1;
                                                mWorld->mSt = 0;
                                            }
                                        }
                                    }
                                }
                            }
                            CraftItem3x3();
                        }
                        //Chest => action when put a item into a chest
                        if (chestEn == 1)
                        {
							
                            if (mWorld->mId == -1)
                            {
                                if (UseChest->chestSlotId[invYPosition*9 + invXPosition] != -1)
                                {
                                    mWorld->mId = UseChest->chestSlotId[invYPosition*9 + invXPosition];
                                    mWorld->mAm = UseChest->chestSlotAm[invYPosition*9 + invXPosition];
                                    mWorld->mSt = UseChest->chestSlotSt[invYPosition*9 + invXPosition];
                                    UseChest->chestSlotId[invYPosition*9 + invXPosition] = -1;
                                    UseChest->chestSlotAm[invYPosition*9 + invXPosition] = -1;
                                    UseChest->chestSlotSt[invYPosition*9 + invXPosition] = 0;
                                }
                            }
                            else
                            {
                                if (UseChest->chestSlotId[invYPosition*9 + invXPosition] == -1)
                                {
                                    UseChest->chestSlotId[invYPosition*9 + invXPosition]=mWorld->mId;
                                    UseChest->chestSlotAm[invYPosition*9 + invXPosition]=mWorld->mAm;
                                    UseChest->chestSlotSt[invYPosition*9 + invXPosition]=mWorld->mSt;
                                    mWorld->mId = -1;
                                    mWorld->mAm = -1;
                                    mWorld->mSt = 0;
                                }

                                if (UseChest->chestSlotId[invYPosition*9 + invXPosition] == mWorld->mId)
                                {
                                    if(UseChest->chestSlotSt[invYPosition*9 + invXPosition] == 1)
                                    {
                                        if (UseChest->chestSlotAm[+invYPosition*9 + invXPosition]+mWorld->mAm < 64)
                                        {
                                            UseChest->chestSlotAm[invYPosition*9 + invXPosition]+=mWorld->mAm;
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                            mWorld->mSt = 0;
                                        }
                                        else
                                        {
                                            mWorld->mAm = (UseChest->chestSlotAm[invYPosition*9 + invXPosition] + mWorld->mAm) - 64;
                                            UseChest->chestSlotAm[invYPosition*9 + invXPosition]=64;

                                            if(mWorld->mAm == 0)
                                            {
                                                mWorld->mAm = -1;
                                                mWorld->mId = -1;
                                                mWorld->mSt = 0;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        //furnace
                        if(furnaceEn == true)
                        {
                            if (mWorld->mId == -1)
                            {
                                if (UseFurnace->furnaceSlotId[invYPosition] != -1)
                                {
                                    mWorld->mId = UseFurnace->furnaceSlotId[invYPosition];
                                    mWorld->mAm = UseFurnace->furnaceSlotAm[invYPosition];
                                    mWorld->mSt = UseFurnace->furnaceSlotSt[invYPosition];
                                    UseFurnace->furnaceSlotId[invYPosition] = -1;
                                    UseFurnace->furnaceSlotAm[invYPosition] = -1;
                                    UseFurnace->furnaceSlotSt[invYPosition] = 0;
                                }
                            }
                            else
                            {
                                if (UseFurnace->furnaceSlotId[invYPosition] == -1)
                                {
                                    UseFurnace->furnaceSlotId[invYPosition]=mWorld->mId;
                                    UseFurnace->furnaceSlotAm[invYPosition]=mWorld->mAm;
                                    UseFurnace->furnaceSlotSt[invYPosition]=mWorld->mSt;
                                    mWorld->mId = -1;
                                    mWorld->mAm = -1;
                                    mWorld->mSt = 0;
                                }

                                if (UseFurnace->furnaceSlotId[invYPosition] == mWorld->mId)
                                {
                                    if(UseFurnace->furnaceSlotSt[invYPosition] == 1)
                                    {
                                        if (UseFurnace->furnaceSlotAm[invYPosition]+mWorld->mAm < 64)
                                        {
                                            UseFurnace->furnaceSlotAm[invYPosition]+=mWorld->mAm;
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                            mWorld->mSt = 0;
                                        }
                                        else
                                        {
                                            mWorld->mAm = (UseFurnace->furnaceSlotAm[invYPosition] + mWorld->mAm) - 64;
                                            UseFurnace->furnaceSlotAm[invYPosition]=64;

                                            if(mWorld->mAm == 0)
                                            {
                                                mWorld->mAm = -1;
                                                mWorld->mId = -1;
                                                mWorld->mSt = 0;
                                            }
                                        }
                                    }
                                }
                            }
                            CheckForFurnFuel(UseFurnace);
                            CheckForFurnWorking(UseFurnace);
                        }
                    }
                }

                if(mSystemMgr->KeyPressed(PSP_CTRL_SQUARE))
                {
                    if (upEn == 0)
                    {
                        if (mWorld->invId[invYPosition*9 + invXPosition] == mWorld->mId)
                        {
                            if(mWorld->mSt == true)
                            {
                                if (mWorld->invAm[invYPosition*9 + invXPosition] != 64)
                                {
                                    mWorld->invAm[invYPosition*9 + invXPosition] += 1;

                                    mWorld->mAm -= 1;

                                    if(mWorld->mAm == 0)
                                    {
                                        mWorld->mId = -1;
                                        mWorld->mAm = -1;
                                    }
                                    return;
                                }
                            }
                        }

                        if (mWorld->mId != -1)
                        {
                            if(mWorld->mSt == true)
                            {
                               if (mWorld->invId[invYPosition*9 + invXPosition] == -1)
                               {
                                    mWorld->invId[invYPosition*9 + invXPosition] = mWorld->mId;
                                    mWorld->invAm[invYPosition*9 + invXPosition] = 1;
                                    mWorld->invSt[invYPosition*9 + invXPosition] = 1;

                                    mWorld->mAm -= 1;

                                    if(mWorld->mAm == 0)
                                    {
                                        mWorld->mId = -1;
                                        mWorld->mAm = -1;
                                        mWorld->mSt = 0;
                                    }
                                    return;
                                }
                            }
                        }
                        else
                        {
                           if (mWorld->invId[invYPosition*9 + invXPosition] != -1 && mWorld->invSt[invYPosition*9 + invXPosition] == 1)
                           {
                               if(mWorld->invAm[invYPosition*9 + invXPosition] == 1)
                               {
                                   mWorld->mId = mWorld->invId[invYPosition*9 + invXPosition];
                                   mWorld->mAm = 1;
                                   mWorld->mSt = mWorld->invSt[invYPosition*9 + invXPosition];

                                   mWorld->invId[invYPosition*9 + invXPosition] = -1;
                                   mWorld->invAm[invYPosition*9 + invXPosition] = -1;
                                   mWorld->invSt[invYPosition*9 + invXPosition] = 0;
                               }
                               if(mWorld->invAm[invYPosition*9 + invXPosition] > 1)
                               {
                                   mWorld->mId = mWorld->invId[invYPosition*9 + invXPosition];
                                   mWorld->mAm = ceilf((float)(mWorld->invAm[invYPosition*9 + invXPosition]/2.0f));
                                   mWorld->mSt = mWorld->invSt[invYPosition*9 + invXPosition];

                                   mWorld->invAm[invYPosition*9 + invXPosition] = mWorld->invAm[invYPosition*9 + invXPosition]-mWorld->mAm;
                               }
                            }
                        }
                    }
                    else
                    {
                        ///2x CRAFT
                        if (invEn == 1)
                        {
                            if (craftSlotId[invYPosition*2 + invXPosition] == mWorld->mId)
                            {
                                if(mWorld->mSt == true)
                                {
                                    if (craftSlotAm[invYPosition*2 + invXPosition] != 64)
                                    {
                                        craftSlotAm[invYPosition*2 + invXPosition] += 1;

                                        mWorld->mAm -= 1;

                                        if(mWorld->mAm == 0)
                                        {
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                        }
                                        CraftItem2x2();
                                        return;
                                    }
                                }
                            }

                            if (mWorld->mId != -1)
                            {
                                if(mWorld->mSt == true)
                                {
                                   if (craftSlotId[invYPosition*2 + invXPosition] == -1)
                                   {
                                       craftSlotId[invYPosition*2 + invXPosition] = mWorld->mId;
                                       craftSlotAm[invYPosition*2 + invXPosition] = 1;
                                       craftSlotSt[invYPosition*2 + invXPosition] = 1;

                                       mWorld->mAm -= 1;

                                       if(mWorld->mAm == 0)
                                       {
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                            mWorld->mSt = 0;
                                        }
                                        CraftItem2x2();
                                        return;
                                    }
                                }
                            }
                            else
                            {
                               if (craftSlotId[invYPosition*2 + invXPosition] != -1 && craftSlotSt[invYPosition*2 + invXPosition] == 1)
                               {
                                   if(craftSlotAm[invYPosition*2 + invXPosition] == 1)
                                   {
                                       mWorld->mId = craftSlotId[invYPosition*2 + invXPosition];
                                       mWorld->mAm = 1;
                                       mWorld->mSt = craftSlotSt[invYPosition*2 + invXPosition];

                                       craftSlotId[invYPosition*2 + invXPosition] = -1;
                                       craftSlotAm[invYPosition*2 + invXPosition] = -1;
                                       craftSlotSt[invYPosition*2 + invXPosition] = 0;
                                   }
                                   if(craftSlotAm[invYPosition*2 + invXPosition] > 1)
                                   {
                                       mWorld->mId = craftSlotId[invYPosition*2 + invXPosition];
                                       mWorld->mAm = ceilf((float)(craftSlotAm[invYPosition*2 + invXPosition]/2.0f));
                                       mWorld->mSt = craftSlotSt[invYPosition*2 + invXPosition];

                                       craftSlotAm[invYPosition*2 + invXPosition] = craftSlotAm[invYPosition*2 + invXPosition]-mWorld->mAm;
                                   }
                                }
                            }
                            CraftItem2x2();
                        }

                        ///CRAFT TABLE
                        if (craft3xEn == 1)
                        {
                            if (craftSlotId3[invYPosition*3 + invXPosition] == mWorld->mId)
                            {
                                if(mWorld->mSt == true)
                                {
                                    if (craftSlotAm3[invYPosition*3 + invXPosition] != 64)
                                    {
                                        craftSlotAm3[invYPosition*3 + invXPosition] += 1;

                                        mWorld->mAm -= 1;

                                        if(mWorld->mAm == 0)
                                        {
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                        }
                                        CraftItem3x3();
                                        return;
                                    }
                                }
                            }

                            if (mWorld->mId != -1)
                            {
                                if(mWorld->mSt == true)
                                {
                                   if (craftSlotId3[invYPosition*3 + invXPosition] == -1)
                                   {
                                       craftSlotId3[invYPosition*3 + invXPosition] = mWorld->mId;
                                       craftSlotAm3[invYPosition*3 + invXPosition] = 1;
                                       craftSlotSt3[invYPosition*3 + invXPosition] = 1;

                                       mWorld->mAm -= 1;

                                       if(mWorld->mAm == 0)
                                       {
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                            mWorld->mSt = 0;
                                       }
                                       CraftItem3x3();
                                       return;
                                   }
                                }
                           }
                            else
                            {
                               if (craftSlotId3[invYPosition*3 + invXPosition] != -1 && craftSlotSt3[invYPosition*3 + invXPosition] == 1)
                               {
                                   if(craftSlotAm3[invYPosition*3 + invXPosition] == 1)
                                   {
                                       mWorld->mId = craftSlotId3[invYPosition*3 + invXPosition];
                                       mWorld->mAm = 1;
                                       mWorld->mSt = craftSlotSt3[invYPosition*3 + invXPosition];

                                       craftSlotId3[invYPosition*3 + invXPosition] = -1;
                                       craftSlotAm3[invYPosition*3 + invXPosition] = -1;
                                       craftSlotSt3[invYPosition*3 + invXPosition] = 0;
                                   }
                                   if(craftSlotAm3[invYPosition*3 + invXPosition] > 1)
                                   {
                                       mWorld->mId = craftSlotId3[invYPosition*3 + invXPosition];
                                       mWorld->mAm = ceilf((float)(craftSlotAm3[invYPosition*3 + invXPosition]/2.0f));
                                       mWorld->mSt = craftSlotSt3[invYPosition*3 + invXPosition];

                                       craftSlotAm3[invYPosition*3 + invXPosition] = craftSlotAm3[invYPosition*3 + invXPosition]-mWorld->mAm;
                                   }
                                }
                            }
                           CraftItem3x3();
                        }

                        ///CHEST
                        if (chestEn == 1)
                        {
                            if (UseChest->chestSlotId[invYPosition*9 + invXPosition] == mWorld->mId)
                            {
                                if(mWorld->mSt == true)
                                {
                                    if (UseChest->chestSlotAm[invYPosition*9 + invXPosition] != 64)
                                    {
                                        UseChest->chestSlotAm[invYPosition*9 + invXPosition] += 1;

                                        mWorld->mAm -= 1;
                                        if(mWorld->mAm == 0)
                                        {
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                        }
                                        return;
                                    }
                                }
                            }

                            if (mWorld->mId != -1)
                            {
                                if(mWorld->mSt == true)
                                {
                                    if (UseChest->chestSlotId[invYPosition*9 + invXPosition] == -1)
                                    {
                                        UseChest->chestSlotId[invYPosition*9 + invXPosition] = mWorld->mId;
                                        UseChest->chestSlotAm[invYPosition*9 + invXPosition] = 1;
                                        UseChest->chestSlotSt[invYPosition*9 + invXPosition] = 1;

                                        mWorld->mAm -= 1;

                                        if(mWorld->mAm == 0)
                                        {
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                            mWorld->mSt = 0;
                                        }
                                        return;
                                    }
                                }
                            }
                            else
                            {
                               if (UseChest->chestSlotId[invYPosition*9 + invXPosition] != -1 && UseChest->chestSlotSt[invYPosition*9 + invXPosition] == 1)
                               {
                                   if(UseChest->chestSlotAm[invYPosition*9 + invXPosition] == 1)
                                   {
                                       mWorld->mId = UseChest->chestSlotId[invYPosition*9 + invXPosition];
                                       mWorld->mAm = 1;
                                       mWorld->mSt = UseChest->chestSlotSt[invYPosition*9 + invXPosition];

                                       UseChest->chestSlotId[invYPosition*9 + invXPosition] = -1;
                                       UseChest->chestSlotAm[invYPosition*9 + invXPosition] = -1;
                                       UseChest->chestSlotSt[invYPosition*9 + invXPosition] = 0;
                                   }
                                   if(UseChest->chestSlotAm[invYPosition*9 + invXPosition] > 1)
                                   {
                                       mWorld->mId = UseChest->chestSlotId[invYPosition*9 + invXPosition];
                                       mWorld->mAm = ceilf((float)(UseChest->chestSlotAm[invYPosition*9 + invXPosition]/2.0f));
                                       mWorld->mSt = UseChest->chestSlotSt[invYPosition*9 + invXPosition];

                                       UseChest->chestSlotAm[invYPosition*9 + invXPosition] = UseChest->chestSlotAm[invYPosition*9 + invXPosition]-mWorld->mAm;
                                   }
                                }
                            }
                        }
                        ///FURNACE
                        if (furnaceEn == 1)
                        {
                            if (UseFurnace->furnaceSlotId[invYPosition] == mWorld->mId)
                            {
                                if(mWorld->mSt == true)
                                {
                                    if (UseFurnace->furnaceSlotAm[invYPosition] != 64)
                                    {
                                        UseFurnace->furnaceSlotAm[invYPosition] += 1;

                                        mWorld->mAm -= 1;
                                        if(mWorld->mAm == 0)
                                        {
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                        }
                                        CheckForFurnFuel(UseFurnace);
                                        CheckForFurnWorking(UseFurnace);
                                        return;
                                    }
                                }
                            }

                            if (mWorld->mId != -1)
                            {
                                if(mWorld->mSt == true)
                                {
                                    if (UseFurnace->furnaceSlotId[invYPosition] == -1)
                                    {
                                        UseFurnace->furnaceSlotId[invYPosition] = mWorld->mId;
                                        UseFurnace->furnaceSlotAm[invYPosition] = 1;
                                        UseFurnace->furnaceSlotSt[invYPosition] = 1;

                                        mWorld->mAm -= 1;

                                        if(mWorld->mAm == 0)
                                        {
                                            mWorld->mId = -1;
                                            mWorld->mAm = -1;
                                            mWorld->mSt = 0;
                                        }
                                        CheckForFurnFuel(UseFurnace);
                                        CheckForFurnWorking(UseFurnace);
                                        return;
                                    }
                                }
                            }
                            else
                            {
                               if (UseFurnace->furnaceSlotId[invYPosition] != -1 && UseFurnace->furnaceSlotSt[invYPosition] == 1)
                               {
                                   if(UseFurnace->furnaceSlotAm[invYPosition] == 1)
                                   {
                                       mWorld->mId = UseFurnace->furnaceSlotId[invYPosition];
                                       mWorld->mAm = 1;
                                       mWorld->mSt = UseFurnace->furnaceSlotSt[invYPosition];

                                       UseFurnace->furnaceSlotId[invYPosition] = -1;
                                       UseFurnace->furnaceSlotAm[invYPosition] = -1;
                                       UseFurnace->furnaceSlotSt[invYPosition] = 0;
                                   }
                                   if(UseFurnace->furnaceSlotAm[invYPosition] > 1)
                                   {
                                       mWorld->mId = UseFurnace->furnaceSlotId[invYPosition];
                                       mWorld->mAm = ceilf((float)(UseFurnace->furnaceSlotAm[invYPosition]/2.0f));
                                       mWorld->mSt = UseFurnace->furnaceSlotSt[invYPosition];

                                       UseFurnace->furnaceSlotAm[invYPosition] = UseFurnace->furnaceSlotAm[invYPosition]-mWorld->mAm;
                                   }
                                }
                            }
                            CheckForFurnFuel(UseFurnace);
                            CheckForFurnWorking(UseFurnace);
                        }
                    }
                }
            }
        }
        else 
		if(menuState == 1) //menu state
        {
            //turn off menu with the same key
            if(keyPressed(InputHelper::Instance()->getButtonToAction(15)))
            {
                menuState = 0;
                menuOptions = false;
                optionsMenuPos = 0;
                selectPos = 0;
            }
			
			if(menuOptions)
            {
                //up, down
                if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
                {
                    if(optionsMenuPos == 8 || optionsMenuPos == 0)
                    {
                        return;
                    }
                    optionsMenuPos--;
                    mSoundMgr->PlayMenuSound();
                }

                if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
                {
                    if(optionsMenuPos == 7 || optionsMenuPos == 15)
                    {
                        return;
                    }
                    optionsMenuPos++;
                    mSoundMgr->PlayMenuSound();
                }

                if(mSystemMgr->KeyPressed(PSP_CTRL_RTRIGGER))
                {
                    if(optionsMenuPos + 8 > 15)
                    {
                        return;
                    }
                    optionsMenuPos += 8;
                    mSoundMgr->PlayMenuSound();
                }

                if(mSystemMgr->KeyPressed(PSP_CTRL_LTRIGGER))
                {
                    if(optionsMenuPos - 8 < 0)
                    {
                        return;
                    }
                    optionsMenuPos -= 8;
                    mSoundMgr->PlayMenuSound();
                }

                if(mSystemMgr->KeyPressed(PSP_CTRL_RIGHT))
                {
                    if(optionsMenuPos == 0)
                    {
                        if (mWorld->mainOptions.fov <= 95)
                        {
                            mWorld->mainOptions.fov += 5;
                            mSoundMgr->PlayMenuSound();
							mRender->fovv = mWorld->mainOptions.fov;
                            RenderManager::InstancePtr()->SetPerspective(0, 480.0f / 272.0f, 0.18f, 1000.f);
                            skyLight->UpdateLightSource(mWorld->skyTime);
                            skyMoonLight->UpdateLightSource(mWorld->skyTime);
                        }
                    }
                    if(optionsMenuPos == 1)
                    {
                        if (mWorld->mainOptions.horizontalViewDistance != 4)
                        {
                            mWorld->mainOptions.horizontalViewDistance += 1;
                            mSoundMgr->PlayMenuSound();
                            mWorld->playerZoneSize = Vector3(CHUNK_SIZE*mWorld->mainOptions.horizontalViewDistance,CHUNK_SIZE*mWorld->mainOptions.verticalViewDistance,CHUNK_SIZE*mWorld->mainOptions.horizontalViewDistance);
                            mWorld->UpdatePlayerZoneBB(fppCam->m_vPosition);
                            fppCam->needUpdate = true;
                        }
                    }
                    if(optionsMenuPos == 8)
                    {
                        if (mWorld->mainOptions.verticalViewDistance != 2)
                        {
                            mWorld->mainOptions.verticalViewDistance += 1;
                            mSoundMgr->PlayMenuSound();
                            mWorld->playerZoneSize = Vector3(CHUNK_SIZE*mWorld->mainOptions.horizontalViewDistance,CHUNK_SIZE*mWorld->mainOptions.verticalViewDistance,CHUNK_SIZE*mWorld->mainOptions.horizontalViewDistance);
                            mWorld->UpdatePlayerZoneBB(fppCam->m_vPosition);
                            fppCam->needUpdate = true;
                        }
                    }
                    if(optionsMenuPos == 9)
                    {
                        mWorld->mainOptions.difficult += 1;
                        mSoundMgr->PlayMenuSound();

                        if(mWorld->mainOptions.difficult > 3)
                        {
                            mWorld->mainOptions.difficult = 3;
                        }
                    }
                }

                if(mSystemMgr->KeyPressed(PSP_CTRL_LEFT))
                {
                    if(optionsMenuPos == 0)
                    {
                        if (mWorld->mainOptions.fov >= 45)
                        {
                            mWorld->mainOptions.fov -= 5;
                            mSoundMgr->PlayMenuSound();
                            mRender->fovv = mWorld->mainOptions.fov;
							RenderManager::InstancePtr()->SetPerspective(0, 480.0f / 272.0f, 0.18f, 1000.f);
                            skyLight->UpdateLightSource(mWorld->skyTime);
                            skyMoonLight->UpdateLightSource(mWorld->skyTime);
                        }
                    }
                    if(optionsMenuPos == 1)
                    {
                        if (mWorld->mainOptions.horizontalViewDistance != 1)
                        {
                            mWorld->mainOptions.horizontalViewDistance -= 1;
                            mSoundMgr->PlayMenuSound();
                            mWorld->playerZoneSize = Vector3(CHUNK_SIZE*mWorld->mainOptions.horizontalViewDistance,CHUNK_SIZE*mWorld->mainOptions.verticalViewDistance,CHUNK_SIZE*mWorld->mainOptions.horizontalViewDistance);
                            fppCam->needUpdate = true;
                        }
                    }
                    if(optionsMenuPos == 8)
                    {
                        if (mWorld->mainOptions.verticalViewDistance != 1)
                        {
                            mWorld->mainOptions.verticalViewDistance -= 1;
                            mSoundMgr->PlayMenuSound();
                            mWorld->playerZoneSize = Vector3(CHUNK_SIZE*mWorld->mainOptions.horizontalViewDistance,CHUNK_SIZE*mWorld->mainOptions.verticalViewDistance,CHUNK_SIZE*mWorld->mainOptions.horizontalViewDistance);
                            fppCam->needUpdate = true;
                        }
                    }
                    if(optionsMenuPos == 9)
                    {
                        mWorld->mainOptions.difficult -= 1;
                        mSoundMgr->PlayMenuSound();

                        if(mWorld->mainOptions.difficult < 0)
                        {
                            mWorld->mainOptions.difficult = 0;
                        }
                    }
                }

                //back
                if(mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE))
                {
                    if (mWorld->mainOptions.difficult == 0)
                    {
                        mWorld->DestroyAllZombies();
                        mWorld->DestroyAllSpiders();
                        mWorld->DestroyAllSpidercaves();
                        mWorld->DestroyAllCreepers();
                    }
                    selectPos = 1;
                    menuOptions = false;
                }

                if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
                {
                    //fog rendering
                    if(optionsMenuPos == 2)
                    {
                         mWorld->mainOptions.fogRendering = !mWorld->mainOptions.fogRendering;
                         mSoundMgr->PlayMenuSound();
                    }

                    //clouds rendering
                    if(optionsMenuPos == 3)
                    {
                         mWorld->mainOptions.detailedSky = !mWorld->mainOptions.detailedSky;
                         mSoundMgr->PlayMenuSound();
                    }

                    //fast rendering
                    if(optionsMenuPos == 4)
                    {
                        mWorld->mainOptions.newSprintOption = !mWorld->mainOptions.newSprintOption;
                        mSoundMgr->PlayMenuSound();

                        //mWorld->RebuildVisibleChunks();
                    }

                    //smooth lighting
                    if(optionsMenuPos == 5)
                    {
                        mWorld->mainOptions.smoothLighting = !mWorld->mainOptions.smoothLighting;
                        mSoundMgr->PlayMenuSound();

                        mWorld->RebuildVisibleChunks();
                    }

                    //block animation
                    if(optionsMenuPos == 6)
                    {
                        mWorld->mainOptions.worldBlockAnimation = !mWorld->mainOptions.worldBlockAnimation;
                        mSoundMgr->PlayMenuSound();
                    }

                    //auto jump
                    if(optionsMenuPos == 7)
                    {
                        mWorld->mainOptions.autoJump = !mWorld->mainOptions.autoJump;
                        bobCycle = 0.0f;
                        mSoundMgr->PlayMenuSound();
                    }

                    if(optionsMenuPos == 10)
                    {
						mWorld->mainOptions.buttonsDrawing = !mWorld->mainOptions.buttonsDrawing;
                        mSoundMgr->PlayMenuSound();
                    }

                    if(optionsMenuPos == 11)
                    {
                        mWorld->mainOptions.guiDrawing = !mWorld->mainOptions.guiDrawing;
                        mSoundMgr->PlayMenuSound();
                    }

                    if(optionsMenuPos == 12)
                    {
                        mWorld->mainOptions.sounds = !mWorld->mainOptions.sounds;
                        mSoundMgr->PlayMenuSound();
                        mSoundMgr->playerSounds = mWorld->mainOptions.sounds;
                    }

                    //music
                    if(optionsMenuPos == 13)
                    {
                        mWorld->mainOptions.music = !mWorld->mainOptions.music;
                        mSoundMgr->PlayMenuSound();
                        mSoundMgr->StopAmbient();
                        mSoundMgr->StopCreative();
                    }


                    //take screen
                    if(optionsMenuPos == 14)
                    {
                        makeScreen = true;
                        /*unsigned short test[128];
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

                            sprintf(mWorld->worldName,"%s",newWorldName.c_str());
                        }*/
                    }

                    if(optionsMenuPos == 15)
                    {
                        devMode = !devMode;
                    }
                }
            }
            else
            {
                //up, down
                if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
                {
                    selectPos--;
                    if(selectPos < 0)
                        selectPos = 5;

                    mSoundMgr->PlayMenuSound();
                }

                if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
                {
                    selectPos++;
                    if(selectPos > 5)
                        selectPos = 0;

                    mSoundMgr->PlayMenuSound();
                }

                if(mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE))
                {
                    menuState = 0;
                    selectPos = 0;
                    optionsMenuPos = 0;
                    menuOptions = false;
                }

                if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
                {
                    if(selectPos == 0)//resume
                    {
                        menuState = 0;
                    }
                    if(selectPos == 1)//options
                    {
                        optionsMenuPos = 0;
                        menuOptions = true;
                    }
                    if(selectPos == 2)//stats
                    {
                        menuState = 3;
                        selectPos = 0;
                    }
                    if(selectPos == 3)//save
                    {
                        mWorld->SaveCompressedWorld(saveFileName.c_str());
                        menuState = 0;
                    }
                    if(selectPos == 4)//save end exit
                    {
						
						mSoundMgr->PlayRandomMenu(); //menu
                        mSoundMgr->StopAmbient();
                        mSoundMgr->StopCreative();
                        mSoundMgr->StopDiskTrack();
                        mWorld->SaveCompressedWorld(saveFileName.c_str());
                        sManager->PopState();
                    }
                    if(selectPos == 5)//exit
                    {
						
						mSoundMgr->PlayRandomMenu(); //menu
                        mSoundMgr->StopAmbient();
                        mSoundMgr->StopCreative();
                        mSoundMgr->StopDiskTrack();
                        sManager->PopState();
                    }
                }
            }
        }
        if (menuState == 3)
        {
            if(mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE))
            {
                selectPos = 2;
                menuState = 1;
            }
            if(mSystemMgr->KeyPressed(PSP_CTRL_RTRIGGER))
            {
                statisticsPage += 1;
                if(statisticsPage == 2)
                {
                    statisticsPage = 0;
                }
            }
            if(mSystemMgr->KeyPressed(PSP_CTRL_LTRIGGER))
            {
                statisticsPage -= 1;
                if(statisticsPage == -1)
                {
                    statisticsPage = 1;
                }
            }
        }
    }
	
		//New Die Screen!!! //muerte
		if(mWorld->HP <=0)
		{
			if(mWorld->gameModeWorld == 2)
			{
				if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
				{
					if(diePos == 0);
					{
						mWorld->gameLocked = true;
						mWorld->SaveCompressedWorld(saveFileName.c_str());

						std::string fileName = saveFileName;
						remove(fileName.c_str());
						fileName = fileName + "c";
						remove(fileName.c_str());
							
						mSoundMgr->PlayRandomMenu();
						mSoundMgr->StopAmbient();
						mSoundMgr->StopCreative();
						mSoundMgr->StopDiskTrack();
						sManager->PopState();
					}
				}
			}
			else
			{
				if(mSystemMgr->KeyPressed(PSP_CTRL_UP))
					{
						diePos--;
						if(diePos < 0)
						diePos = 1;
						mSoundMgr->PlayMenuSound();
					}
					
					if(mSystemMgr->KeyPressed(PSP_CTRL_DOWN))
					{
						diePos++;
						if(diePos > 1)
						diePos = 0;

						mSoundMgr->PlayMenuSound();
					}
				
				if(mSystemMgr->KeyPressed(PSP_CTRL_CROSS))
				{
					if(diePos == 0)
					{
						mWorld->playerGrowth = 1.65f;

						Vector3 newPosition = mWorld->playerSpawnPointPosition;
						if(mWorld->BlockSolid(newPosition.x,newPosition.y-1.2f,newPosition.z) || mWorld->BlockSolid(newPosition.x,newPosition.y,newPosition.z))
						{
							newPosition.y = mWorld->groundHeight(newPosition.x,newPosition.z)+mWorld->playerGrowth-0.05f+1;
						}

						playerPosition.x = newPosition.x;
						playerPosition.y = newPosition.y;
						playerPosition.z = newPosition.z;

						mWorld->UpdatePlayerZoneBB(playerPosition);	//Move player back to original spawn point
						fppCam->PositionCamera(playerPosition.x,playerPosition.y,playerPosition.z, playerPosition.x,playerPosition.y,playerPosition.z-5.0f, 0.0f,1.0f,0.0f);
						fppCam->RotateView(0,0,90,0);
						fppCam->upDownAngle = 0;
						fppCam->horAngle = 270;
						mWorld->HP = 20;
						mWorld->HG = 20;
						mWorld->OS = 10;
						mWorld->FR = 0;
						HGP = 0;
						HPE = 0;
						mWorld->EXPcount = 0;
						mWorld->EXP = 0;
						
						HungerPoisoned = false;
						HPPoisoned = false;
						crouch = false;
						burning = false;
						invEn = false;
						craft3xEn = false;
						furnaceEn = false;
						chestEn = false;
						upEn = false;
						furnaceEn = false;
						chestId = -1;
						armorEn = false;

						UseChest = NULL;
						UseFurnace = NULL;
						
						int chunkTarget = mWorld->getChunkId(playerPosition-Vector3(0, -1.75, 0));

						//Rebuild the world around
						mWorld->RebuildFullMeshChunk(chunkTarget);
						mWorld->rebuildNearestChunks(chunkTarget,playerPosition);
						//mWorld->rebuildVisibleChunks();
					   // mWorld->rebuildVisibleTransperentChunks();

						dieFactor = 1;
						
					}
					
					if(diePos == 1)
					{
						mSoundMgr->PlayRandomMenu();
						mSoundMgr->StopAmbient();
						mSoundMgr->StopCreative();
						mSoundMgr->StopDiskTrack();
						sManager->PopState();
					}
				}
			}
		}
	
    if(mWorld->HP <= 0)
    {
        mWorld->playerGrowth = 0.65f;
        if(dieFactor == 1) // drop your loot
        {
            for(int i = 0; i < 36; i++)
            {
                if(i <= 3) // drop armor items
                {
                    if(mWorld->armorId[i] != 324 + i)
                    {
                        Vector3 dropVelocity = Vector3(0,0,0);

                        dropVelocity.x = (-15+rand()%30)*0.1f;
                        dropVelocity.y = 3.0f;
                        dropVelocity.z = (-15+rand()%30)*0.1f;

                        mWorld->DropThisNoAlign(mWorld->armorId[i],mWorld->armorAm[i],mWorld->armorSt[i],Vector3(fppCam->m_vPosition.x,fppCam->m_vPosition.y-0.25f,fppCam->m_vPosition.z),dropVelocity);

                        mWorld->armorAm[i] = -1;
                        mWorld->armorSt[i] = 0;
                        mWorld->armorId[i] = -1;
                    }
                }
                if(mWorld->invId[i] > 0)
                {
                    Vector3 dropVelocity = Vector3(0,0,0);

                    dropVelocity.x = (-15+rand()%30)*0.1f;
                    dropVelocity.y = 3.0f;
                    dropVelocity.z = (-15+rand()%30)*0.1f;

                    if(mWorld->invAm[i] > 0)
                    {
                        mWorld->DropThisNoAlign(mWorld->invId[i],mWorld->invAm[i],mWorld->invSt[i],Vector3(fppCam->m_vPosition.x,fppCam->m_vPosition.y-0.25f,fppCam->m_vPosition.z),dropVelocity);

                        mWorld->invAm[i] = -1;
                        mWorld->invSt[i] = 0;
                        mWorld->invId[i] = -1;
                    }
                }
            }

            mWorld->mainStatistics.dies += 1;
            dieFactor = 0;
        }
    }
}


void StatePlay::Update(StateManager* sManager)
{
    if(mWorld->armorAm[0] <= 0)
    {
        mWorld->armorId[0] = 324;
        mWorld->armorAm[0] = -1;
        mWorld->armorSt[0] = false;
    }
    if(mWorld->armorAm[1] <= 0)
    {
        mWorld->armorId[1] = 325;
        mWorld->armorAm[1] = -1;
        mWorld->armorSt[1] = false;
    }
    if(mWorld->armorAm[2] <= 0)
    {
        mWorld->armorId[2] = 326;
        mWorld->armorAm[2] = -1;
        mWorld->armorSt[2] = false;
    }
    if(mWorld->armorAm[3] <= 0)
    {
        mWorld->armorId[3] = 327;
        mWorld->armorAm[3] = -1;
        mWorld->armorSt[3] = false;
    }
    mWorld->AP = mWorld->FetchArmorPoints(mWorld->armorId[3],mWorld->armorId[2],mWorld->armorId[1],mWorld->armorId[0]);

    if(menuState == 0)//game state
    {
        cubeLight = mWorld->BlockFinalLight(playerPosition.x,playerPosition.y-1,playerPosition.z) * 1.2f;
        if(cubeLight > 1.0f)
        {
            cubeLight = 1.0f;
        }
		
		///-----*EXPSYSTEM*-----///
		
		/*if(mWorld->EXPcount == 179)
		{
			break;
		}*/
		
		if(mWorld->EXPcount > 178)
		{
			mWorld->EXPcount = 0;
			mWorld->EXP += 1;
			
			if(mWorld->EXP == 5 || (mWorld->EXP == 10 || mWorld->EXP == 15 || mWorld->EXP == 20 || mWorld->EXP == 25 || mWorld->EXP == 30 
			|| mWorld->EXP == 35 || mWorld->EXP == 40 || mWorld->EXP == 45 || mWorld->EXP == 50 || mWorld->EXP == 55 || mWorld->EXP == 60 
			|| mWorld->EXP == 65 || mWorld->EXP == 70 || mWorld->EXP == 75 || mWorld->EXP == 80 || mWorld->EXP == 85 || mWorld->EXP == 90))
			{
				mSoundMgr->PlayExpSound();
			}
		}
		///-------*END*-------///

        furnaceTimes += dt;
        if(furnaceTimes >= 0.5f)
        {
            if(mWorld->mFurnaces.size() > 0)
            {
                vector<Furnace*>::iterator it;
                for(it = mWorld->mFurnaces.begin(); it != mWorld->mFurnaces.end(); ++it)
                {
                    if((*it)->fuelTime > 0)
                    {
                        (*it)->fuelTime -= 0.5f;
                        if(mWorld->GetBlock((*it)->furnaceX, (*it)->furnaceY, (*it)->furnaceZ) == FurnaceOff::getID())
                        {
                            Vector3 testPos5 = Vector3(-1,-1,-1);
                            int chunkTarget2 = -1;

                            testPos5.x = (*it)->furnaceX;
                            testPos5.y = (*it)->furnaceY;
                            testPos5.z = (*it)->furnaceZ;
                            chunkTarget2 = mWorld->getChunkId(testPos5);

                            mWorld->GetBlock((*it)->furnaceX, (*it)->furnaceY, (*it)->furnaceZ) = FurnaceOn::getID();

                            //mWorld->SetLigtSourcePosition(WorkingFurance->furnaceX,WorkingFurance->furnaceY,WorkingFurance->furnaceZ,107);
                            mWorld->UpdateLightAreaIn(testPos5);

                            mWorld->RebuildFullMeshChunk(chunkTarget2);
                            mWorld->rebuildNearestChunksForLight(chunkTarget2,testPos5);

                            fppCam->needUpdate = true;
                        }
                    }

                    if((*it)->fuelTime <= 0)
                    {
                        (*it)->fuelTime = 0.0f;
                        CheckForFurnFuel((*it));
                    }

                    if((*it)->fuelTime <= 0)
                    {
                        if(mWorld->GetBlock((*it)->furnaceX,(*it)->furnaceY,(*it)->furnaceZ) == 107)
                        {
                            Vector3 testPos5 = Vector3(-1,-1,-1);
                            int chunkTarget2 = -1;

                            testPos5.x = (*it)->furnaceX;
                            testPos5.y = (*it)->furnaceY;
                            testPos5.z = (*it)->furnaceZ;
                            chunkTarget2 = mWorld->getChunkId(testPos5);

                            mWorld->GetBlock((*it)->furnaceX,(*it)->furnaceY,(*it)->furnaceZ) = 106;

                            mWorld->RemoveLigtSourceAtPosition((*it)->furnaceX,(*it)->furnaceY,(*it)->furnaceZ,107);

                            mWorld->UpdateLightAreaIn(testPos5);

                            mWorld->RebuildFullMeshChunk(chunkTarget2);
                            mWorld->rebuildNearestChunksForLight(chunkTarget2,testPos5);

                            fppCam->needUpdate = true;
                        }
                    }

                    if((*it)->fuelTime > 0 && (*it)->working == 1)
                    {
                        (*it)->meltingTime += 0.5f;
                        if((*it)->meltingTime >= 10)
                        {
                            (*it)->meltingTime = 0;
                            (*it)->working = 0;

                            ReadyFurnSmelting((*it));
                            CheckForFurnWorking((*it));
                        }
                    }
                }
            }
            furnaceTimes -= 0.5f;
        }

        if(mWorld->mMonsterSpawners.size() > 0)
        {
            BoundingBox playerBox = BoundingBox(Vector3(playerPosition.x - 0.15f,playerPosition.y - 1.60f,playerPosition.z - 0.15f),Vector3(playerPosition.x + 0.15f,playerPosition.y + 0.2f,playerPosition.z + 0.15f));

            vector<MonsterSpawnerEntity*>::iterator it;
            for(it = mWorld->mMonsterSpawners.begin(); it != mWorld->mMonsterSpawners.end(); ++it)
            {
                if(playerBox.intersect((*it)->actArea))
                {
                    (*it)->Update(dt);

                    if((*it)->particleDelay1 <= 0)
                    {
                         (*it)->particleDelay1 = 0.5f+(rand() % 500)/1000.0f;

                         for(int i = 0; i < 4; i++)
                         {
                            float particle_x = (*it)->GetX()+(rand()%(11)/10.0f);
                            float particle_y = (*it)->GetY()+(rand()%(11)/10.0f);
                            float particle_z = (*it)->GetZ()+(rand()%(11)/10.0f);

                            Particle* Smoke = new Particle(mWorld, "black smoke",Vector3(particle_x,particle_y,particle_z));
                            Particle* Flame = new Particle(mWorld, "flame",Vector3(particle_x,particle_y,particle_z));
                            mParticles.push_back(Smoke);
                            mParticles.push_back(Flame);
                         }
                    }
                    if((*it)->particleDelay2 <= 0)
                    {
                         (*it)->particleDelay2 = 0.5f+(rand() % 500)/1000.0f;

                         for(int i = 0; i < 6; i++)
                         {
                            float particle_x = (*it)->GetX()+(rand()%(11)/10.0f);
                            float particle_y = (*it)->GetY()+(rand()%(11)/10.0f);
                            float particle_z = (*it)->GetZ()+(rand()%(11)/10.0f);

                            Particle* Smoke = new Particle(mWorld, "black smoke",Vector3(particle_x,particle_y,particle_z));
                            Particle* Flame = new Particle(mWorld, "flame",Vector3(particle_x,particle_y,particle_z));
                            mParticles.push_back(Smoke);
                            mParticles.push_back(Flame);
                         }
                    }

                    if((*it)->ItIsTimeToSpawn() && mWorld->BlockFinalLight((*it)->GetX(),(*it)->GetY()+1,(*it)->GetZ()) < 0.6f && mWorld->mainOptions.difficult != 0)
                    {
                        (*it)->forceSpawn = false;
                        int nearbyEntityAmount = 0;

                        if((*it)->MobToSpawn() == "zombie")
                        {
                            if(mWorld->mZombies.empty() == false)
                            {
                                for(int i = 0; i < mWorld->mZombies.size(); i++)
                                {
                                    if(nearbyEntityAmount == 4)
                                    {
                                        break;
                                    }
                                    Zombie* TestMob = mWorld->mZombies[i];
                                    if(TestMob->bBox.intersect((*it)->actArea))
                                    {
                                        nearbyEntityAmount ++;
                                    }
                                }
                            }

                            if(nearbyEntityAmount < 4)
                            {
                                int count0 = (*it)->MobCountToSpawn();
                                for(int i = 0; i < count0; i++)
                                {
                                    float spawn_x = (*it)->GetX()-2+(rand()%2)*4;
                                    float spawn_z = (*it)->GetZ()-2+(rand()%2)*4;
                                    spawn_x += 0.5f;
                                    spawn_z += 0.5f;

                                    float spawn_y = mWorld->groundHeight(spawn_x+0.5f,(*it)->GetY()+1,spawn_z+0.5f)+1.2f;

                                    mWorld->SpawnZombie((*it)->GetX()-1+rand()%3,(*it)->GetY()+1,(*it)->GetZ()-1+rand()%3);
                                }
                            }
                        }
						
						/*if((*it)->MobToSpawn() == "cavespider")
                        {
                            if(mWorld->mSpidercaves.empty() == false)
                            {
                                for(int i = 0; i < mWorld->mSpidercaves.size(); i++)
                                {
                                    if(nearbyEntityAmount == 4)
                                    {
                                        break;
                                    }
                                    Spidercave* TestMob = mWorld->mSpidercaves[i];
                                    if(TestMob->bBox.intersect((*it)->actArea))
                                    {
                                        nearbyEntityAmount ++;
                                    }
                                }
                            }

                            if(nearbyEntityAmount < 4)
                            {
                                int count0 = (*it)->MobCountToSpawn();
                                for(int i = 0; i < count0; i++)
                                {
                                    float spawn_x = (*it)->GetX()+1;
                                    float spawn_z = (*it)->GetZ()+1;
                                    spawn_x += 0.5f;
                                    spawn_z += 0.5f;

                                    float spawn_y = mWorld->groundHeight(spawn_x+0.5f,(*it)->GetY()+1,spawn_z+0.5f)+1.2f;

                                    mWorld->SpawnSpidercave((*it)->GetX()+1,(*it)->GetY()+1,(*it)->GetZ()+1);
                                }
                            }
                        }*/
                    }
                }
            }
        }

        if(sleepTime > 0.0f)
        {
            sleepTime -= dt;
        }
        if(sleepTime < 0.1f && sleepTime > 0.0f)
        {
            Vector3 previousPosition = mWorld->playerSpawnPointPosition;

            playerPosition.x = previousPosition.x;
            playerPosition.y = previousPosition.y;
            playerPosition.z = previousPosition.z;

            mWorld->UpdatePlayerZoneBB(playerPosition);
            fppCam->PositionCamera(playerPosition.x,playerPosition.y,playerPosition.z, playerPosition.x,playerPosition.y,playerPosition.z-5.0f, 0.0f,1.0f,0.0f);
            fppCam->RotateView(0,0,90,0);
            fppCam->upDownAngle = 0;
            fppCam->horAngle = 270;
			
            sleepTime = 0.0f;
            sleepMessageType = 0;
            mWorld->SetWolrdTime(9);
            mWorld->brightFactor = 0.0f;
            mWorld->mainStatistics.daysInGame++;
            mWorld->RebuildVisibleChunks();
			
			if(mWorld->worldWeather->GetWeatherType() == 1)
			{
				mWorld->worldWeather->SetWeatherType(0);
				mWorld->worldWeather->SetWeatherDuration(500+rand()%5500);
			}
        }

        if (mWorld->skyTime >= 100 && mWorld->skyTime <= 150) // morning
        {
            mWorld->brightFactor = (mWorld->skyTime - 100.0f)/50.0f;
        }
        if (mWorld->skyTime >= 146 && mWorld->skyTime <= 249) // noon
        {
            mWorld->brightFactor = 1.0f;
        }
        if (mWorld->skyTime >= 250 && mWorld->skyTime <= 300) // sunset
        {
            mWorld->brightFactor = (300.0f - mWorld->skyTime)/50.0f;
        }
        if (mWorld->skyTime >= 300 && mWorld->skyTime <= 460) // night
        {
            mWorld->brightFactor = 0.0f;
        }

        if (mWorld->skyTime >= 101 && mWorld->skyTime <= 116)
        {
            mWorld->dawnSunsetFactor = (mWorld->skyTime - 101)/18.0f;
        }
        if (mWorld->skyTime >= 125 && mWorld->skyTime <= 139)
        {
            mWorld->dawnSunsetFactor = (139 - mWorld->skyTime)/18.0f;
        }
        if (mWorld->skyTime > 134 && mWorld->skyTime < 240)
        {
            mWorld->dawnSunsetFactor = 0.0f;
        }

        if (mWorld->skyTime >= 265 && mWorld->skyTime <= 288)
        {
            mWorld->dawnSunsetFactor = (mWorld->skyTime - 265)/18.0f;
        }
        if (mWorld->skyTime >= 291 && mWorld->skyTime <= 306)
        {
            mWorld->dawnSunsetFactor = (306 - mWorld->skyTime)/18.0f;
        }
        if (mWorld->skyTime > 306)
        {
            mWorld->dawnSunsetFactor = 0.0f;
        }

        if(mWorld->dawnSunsetFactor > 1.0f)
        {
            mWorld->dawnSunsetFactor = 1.0f;
        }

        if (anim[0] == 1)
        {
            if (changeY < 0.2f)
            {
                changeY += dt*0.9f;
            }
            if (changeY >= 0.0f)
            {
                anim[0] = 0;
                changeY = 0.0f;
            }
        }
		
        animSpeed = dt*3.7*PI;
        if(animGo == true)
        {
            if(animDest < PI)
            {
                animDest += animSpeed;
            }
            if(animDest >= PI)
            {
                animGo = false;
                animDest = 0;
            }
        }

        if(playerVelocity.y != 0.0f)
        {
            fppCam->needUpdate = true;
        }

        if(fppCam->needUpdate)
        {
            showCube = false;

            //show pick cube
            Vector3 rayDir = fppCam->m_vView - fppCam->m_vPosition;
            rayDir.normalize();

            //we take position and view vector
            Vector3 testPos ;

            // collider
            BoundingBox collider = mWorld->blockTypes[mWorld->GetBlock(testPos.x,testPos.y,testPos.z)].collideBox;
            BoundingBox testBox = BoundingBox(testPos.x+collider.min.x, testPos.y+collider.min.y, testPos.z+collider.min.z, testPos.x+collider.max.x, testPos.y+collider.max.y, testPos.z+collider.max.z);

            //moving slowly to the target
            for(float i = 0; i < 5.25f; i+=0.1f)
            {
                testPos = fppCam->m_vPosition + (rayDir * i);

                collider = mWorld->blockTypes[mWorld->GetBlock(testPos.x,testPos.y,testPos.z)].collideBox;
                testBox = BoundingBox(floorf(testPos.x)+0.5f+collider.min.x, floorf(testPos.y)+0.5f+collider.min.y, floorf(testPos.z)+0.5f+collider.min.z, floorf(testPos.x)+0.5f+collider.max.x, floorf(testPos.y)+0.5f+collider.max.y, floorf(testPos.z)+0.5f+collider.max.z);

                //check if we touching something
                if(mWorld->BlockEditable(testPos.x,testPos.y,testPos.z) && testBox.contains(testPos) == true)
                {
                    //if yes then show pick cube
                    cubePos.x = (int)testPos.x + 0.5f;
                    cubePos.y = (int)testPos.y + 0.5f;
                    cubePos.z = (int)testPos.z + 0.5f;

                    showCube = true;
                    break;
                }
            }
        }

        int soundBlockType = -1;

        //RenderManager::InstancePtr()

        //update player position
        if(dt < 0.1f)
        {
            /// player timers
            musicTime -= dt;
            if (musicTime < 0.0f)
            {
                musicTime = 300.0f;
                if(mWorld->mainOptions.music == true)
                {
					switch(rand() % 2)
					{
						case 0: musicTime = mSoundMgr->PlayRandomAmbient(); break;
						case 1: musicTime = mSoundMgr->PlayRandomCreative(); break;
					}
                }
            }

            cloudsOffset += dt/1500.0f;
            if(cloudsOffset >= 2.0f)
            {
                cloudsOffset -= 2.0f;
            }

            { // nearest chunk update
                tickChunk += dt;

                if (tickChunk >= 10.0f)
                {
                    chunkId = mWorld->getChunkId(playerPosition);
                    mWorld->UpdateChunkBlocks(chunkId);

                    mWorld->RebuildFullMeshChunk(chunkId);
                    tickChunk -= 10.0f;
                }
            }

            {
                tickHunger += dt;
                if(tickHunger >= 75)
                {
                    HungerTime();
                    tickHunger -= 75;
                }

                tickHealth += dt;
                if(tickHealth >= 5.0f)
                {
                    HealthTime();
                    tickHealth -= 5.0f;
                }
				
				if(HPPoisoned == true)
				{
					tickHPveneno += dt;
					if(tickHPveneno >= 1.0f)
					{
						if(HPE <= 0)
						{
							HPPoisoned = false;
						}
						if(HPE > 0)
						{
							HPE -= 1;
						}
						if(mWorld->HP > 1)
						{
							HurtPlayer(1);
						}
						tickHPveneno -= 1.0f;
					}
				}
				else
				{
					tickHPveneno = 0.0f;
					HPPoisoned = false;
				}
				
				
				if(HungerPoisoned == true)
				{
					mWorld->HG -= dt/4.0f;
				}
				
				if(HungerPoisoned == true)
				{
					tickHungerPoisoned += dt;
					if(tickHungerPoisoned >= 0.0f)
					{
						if(HGP <= 0)
						{
							HungerPoisoned = false;
						}
							
						if(HGP > 0)
						{
							HGP -= 1;
						}
						tickHungerPoisoned -= 1.0f;
					}
				}
				else
				{
					HungerPoisoned = false;
				}

                tickLavaDamage += dt;
                if(tickLavaDamage >= 0.5f)
                {
                    if(headInLava || footInLava)
                    {
                        HurtPlayer(4.0f);
                        if(mWorld->armorId[0] != 324)
                        {
                            mWorld->armorAm[0] -= 8+rand()%8;
                        }
                        if(mWorld->armorId[1] != 325)
                        {
                            mWorld->armorAm[1] -= 8+rand()%8;
                        }
                        if(mWorld->armorId[2] != 326)
                        {
                            mWorld->armorAm[2] -= 8+rand()%8;
                        }
                        if(mWorld->armorId[3] != 327)
                        {
                            mWorld->armorAm[3] -= 8+rand()%8;
                        }
                    }
                    tickLavaDamage -= 0.5f;
                }

				if(footInFire)
				{
					burning = true;
					mWorld->FR = 2;
				}
				
				tickFireDamage += dt;
				if(tickFireDamage >= 0.0f)
				{
					if(burning == true)
					{	
						HurtPlayer(2);
						if(mWorld->armorId[0] != 324)
                        {
                            mWorld->armorAm[0] -= 4+rand()%4;
                        }
                        if(mWorld->armorId[1] != 325)
                        {
                            mWorld->armorAm[1] -= 4+rand()%4;
                        }
                        if(mWorld->armorId[2] != 326)
                        {
                            mWorld->armorAm[2] -= 4+rand()%4;
                        }
                        if(mWorld->armorId[3] != 327)
                        {
                            mWorld->armorAm[3] -= 4+rand()%4;
                        }
					}
					tickFireDamage -= 0.8f;
				}
				
				if(headInWater || footInWater)
				{
					burning = false;
					mWorld->FR = 0;
				}
				
				if(burning == true)
                {
                    FireBurning();
                }

				//end

                if(touchingCactus)
                {
                    tickCactusDamage += dt;
                }
                else
                {
                    tickCactusDamage = 0.4f;
                }
                if(tickCactusDamage >= 0.5f)
                {
                    HurtPlayer(1);

                    tickCactusDamage -= 0.5f;
                }

                if (hurt_time > 0)
                {
                    hurt_time -= dt;
                }
                if (hurt_time <= 0)
                {
                    hurt = false;
                }


                if(tickOS >= 1.0f && headInWater == 1)
                {
                    OxygenTime();
                    tickOS = 0;
                }

                if (headInWater == 0)
                {
                    if (mWorld->OS != 10)
                    {
                        mWorld->OS = 10;
                    }
                    tickOS = 0;
                }
                else
                {
                    tickOS += dt;
                }
            }

            /// player physics
            playerPosition = fppCam->m_vPosition;

            //gravity
            Vector3 footPosition = playerPosition + Vector3(0.0f, -mWorld->playerGrowth, 0.0f);
            Vector3 legsPosition = playerPosition + Vector3(0.0f, -mWorld->playerGrowth+0.65f, 0.0f);
            Vector3 headPosition = playerPosition + Vector3(0.0f, 0.2f, 0.0f);

            if(sleepTime <= 0.0f)
            {
				
				/*//player in portals
				if(mWorld->PlayerInHellPortal(legsPosition) || mWorld->PlayerInHellPortal(headPosition))
				{
					if(!mWorld->teleported)
					{
						InHellPortal = true;
					}
					else
					{
						InHellPortal = false;
					}
				}
				else
				{
					InHellPortal = false;
					mWorld->teleported = false;
				}
				
				if(mWorld->PlayerInEndPortal(legsPosition) || mWorld->PlayerInEndPortal(headPosition))
				{
					InEndPortal = true;
				}
				else
				{
					InEndPortal = false;
				}
				
				if(InHellPortal)
				{
					if(mWorld->currentWorld == 0 && mWorld->createdHell && !mWorld->createdHell)//save and load
					{
						std::string fileName1 = saveFileName;
						remove(fileName1.c_str());
						fileName1 = fileName1 + "c";
						remove(fileName1.c_str());

						mWorld->thisWorld = false;
						mWorld->SaveCompressedWorld(saveFileName);//save normal world
						
						short armorId1[4];
						short armorAm1[4];
						bool armorSt1[4];
						short invId1[36];
						short invAm1[36];
						bool invSt1[36];
						float HP1 = mWorld->HP;
						float HG1 = mWorld->HG;
						float AP1 = mWorld->AP;
						char OS1 = mWorld->OS;
						
						for(int i = 0; i < 36; i++)
						{
							if(i <= 3)
							{
								armorAm1[i] = mWorld->armorAm[i];
								armorSt1[i] = mWorld->armorSt[i];
								armorId1[i] = mWorld->armorId[i];
							}
							invAm1[i] = mWorld->invAm[i];
							invSt1[i] = mWorld->invSt[i];
							invId1[i] = mWorld->invId[i];
						}
						
						saveFileName.erase(saveFileName.length()-4);//- ".lms"
						saveFileName += "_hell_world.lms";
						mWorld->LoadCompressedWorld(saveFileName);
						
						std::string fileName2 = saveFileName;
						remove(fileName2.c_str());
						fileName2 = fileName2 + "c";
						remove(fileName2.c_str());
						
						mWorld->thisWorld = true;
						
						mWorld->HP = HP1;
						mWorld->HG = HG1;
						mWorld->AP = AP1;
						mWorld->OS = OS1;
						for(int i = 0; i < 36; i++)
						{
							if(i <= 3)
							{
								mWorld->armorAm[i] = armorAm1[i];
								mWorld->armorSt[i] = armorSt1[i];
								mWorld->armorId[i] = armorId1[i];
							}
							mWorld->invAm[i] = invAm1[i];
							mWorld->invSt[i] = invSt1[i];
							mWorld->invId[i] = invId1[i];
						}
						
						mWorld->SaveCompressedWorld(saveFileName);
					
						mWorld->GetSkyColor();
						mWorld->InitLightValues();
					}
					else if(mWorld->currentWorld == 0)//generate
					{
						std::string fileName1 = saveFileName;
						remove(fileName1.c_str());
						fileName1 = fileName1 + "c";
						remove(fileName1.c_str());
						
						mWorld->thisWorld = false;
						mWorld->createdHell = true;
						mWorld->SaveCompressedWorld(saveFileName);
						
						mWorld->DestroyAllMobs();
						
						for(int xx = 0; xx < 252; xx++)
						{
							for(int yy = 0; yy < 112; yy++)
							{
								for(int zz = 0; zz < 252; zz++)
								{
									int cBlock = mWorld->GetBlock(xx,yy,zz);
									if(mWorld->GetBlock(xx-1,yy,zz) == 0 && mWorld->GetBlock(xx+1,yy,zz) == 0 && mWorld->GetBlock(xx,yy,zz-1) == 0 && mWorld->GetBlock(xx,yy,zz+1) == 0 && cBlock != 0)
									{
										mWorld->GetBlock(xx,yy,zz)=0;
									}
									if(yy > 97 && yy <= 100)
									{
										mWorld->GetBlock(xx,yy,zz)=142;//bedrock up
										continue;
									}
									if(cBlock == 46 || cBlock == 216)//obsidian and portal
									{
										continue;
									}
									if(yy > 86 && yy<98)
									{
										mWorld->GetBlock(xx,yy,zz)=222;//two lvl world
										continue;
									}
									if(yy < 50 && cBlock != 142)
									{
										mWorld->GetBlock(xx,yy,zz)=222;//netherrack
										continue;
									}
									if(cBlock == 4)
									{
										mWorld->GetBlock(xx,yy,zz)=112;//lava
										continue;
									}
									if(cBlock == 7)
									{
										mWorld->GetBlock(xx,yy,zz)=221;//soul sand
										continue;
									}
									if(cBlock == 8 || cBlock == 30 || cBlock == 31 || cBlock == 9 || cBlock == 37 || cBlock == 38 || cBlock == 116 || cBlock == 48 || cBlock == 93)
									{
										mWorld->GetBlock(xx,yy,zz)=0;
										continue;
									}
									if(cBlock != 0)
									{
										mWorld->GetBlock(xx,yy,zz)=222;//netherrack
									}
								}
							}
						}
						
						int sv = 4+rand()%4;
						for(int ggg = 0; ggg <= sv; ggg++)//glowstone generate
						{
							int x = rand()%252;
							int z = rand()%252;
							int y =  86;
							int glgen = 15 + rand()%16;
							for(int ggg2 = 0 ; ggg2 <= glgen; ggg2++)
							{
								mWorld->GetBlock(x + rand()%4,y - rand()%4,z + rand()%4) = 223;
							}
						}
						
						mWorld->playerSpawnPointPosition = playerPosition;
						saveFileName.erase(saveFileName.length()-4);//- ".lms"
						saveFileName += "_hell_world.lms";
						mWorld->currentWorld = 1;
						mWorld->thisWorld = true;
						mWorld->SaveCompressedWorld(saveFileName);// show as "worldName_hell_world.lms"
					
						mWorld->GetSkyColor();
						mWorld->InitLightValues();
					}
					else if(mWorld->currentWorld == 1)//save and load
					{
						std::string fileName1 = saveFileName;
						remove(fileName1.c_str());
						fileName1 = fileName1 + "c";
						remove(fileName1.c_str());
						
						mWorld->thisWorld = false;
						mWorld->SaveCompressedWorld(saveFileName);// save hell
						
						short armorId1[4];
						short armorAm1[4];
						bool armorSt1[4];
						short invId1[36];
						short invAm1[36];
						bool invSt1[36];
						float HP1 = mWorld->HP;
						float HG1 = mWorld->HG;
						float AP1 = mWorld->AP;
						char OS1 = mWorld->OS;
						
						for(int i = 0; i < 36; i++)
						{
							if(i <= 3)
							{
								armorAm1[i] = mWorld->armorAm[i];
								armorSt1[i] = mWorld->armorSt[i];
								armorId1[i] = mWorld->armorId[i];
							}
							invAm1[i] = mWorld->invAm[i];
							invSt1[i] = mWorld->invSt[i];
							invId1[i] = mWorld->invId[i];
						}

						saveFileName.erase(saveFileName.length()-15);//- "_hell_world.lms"
						saveFileName += ".lms";
						mWorld->LoadCompressedWorld(saveFileName);//load normal

						std::string fileName2 = saveFileName;
						remove(fileName2.c_str());
						fileName2 = fileName2 + "c";
						remove(fileName2.c_str());
						
						mWorld->thisWorld = true;
						
						mWorld->HP = HP1;
						mWorld->HG = HG1;
						mWorld->AP = AP1;
						mWorld->OS = OS1;
						for(int i = 0; i < 36; i++)
						{
							if(i <= 3)
							{
								mWorld->armorAm[i] = armorAm1[i];
								mWorld->armorSt[i] = armorSt1[i];
								mWorld->armorId[i] = armorId1[i];
							}
							mWorld->invAm[i] = invAm1[i];
							mWorld->invSt[i] = invSt1[i];
							mWorld->invId[i] = invId1[i];
						}
						
						mWorld->SaveCompressedWorld(saveFileName);
					
						mWorld->GetSkyColor();
						mWorld->InitLightValues();
					}
					mWorld->teleported = true;
				}*/
				
                //foot in water
                if(mWorld->PlayerInWater(legsPosition))
                {
                    if(footInWater == false)
                    {
                        mSoundMgr->PlaySplashSound();
                        footInWater = true;
                    }

                }
                else
                {
                    footInWater = false;
                }

                //check if head is is in the water
                if(mWorld->PlayerInWater(headPosition))
                {
                    //change gravity
                    if (playerVelocity.y > -3)
                    {
                        playerVelocity.y += (GRAVITY/3.0f) * dt;
                    }
                    else
                    {
                        playerVelocity.y = -3;
                    }
                    /*if(headInWater == false)
                    {
                        mSoundMgr->PlaySplashSound();
                        headInWater = true;
                    }*/
                    headInWater = true;
                }
                else
                {
                    headInWater = false;
                }

				//fire
                if(mWorld->PlayerInFire(legsPosition))
                {
                    footInFire = true;
                }
                else
                {
                    footInFire = false;
                }
				//CobWeb
                if(mWorld->PlayerInCobweb(legsPosition) || mWorld->PlayerInCobweb(headPosition))
                {
                    playerInCobweb = true;
                }
                else
                {
                    playerInCobweb = false;
                }
				
				if(mWorld->PlayerInLava(legsPosition))
                {
                    footInLava = true;
                }
                else
                {
                    footInLava = false;
                }
                //check if head is is in the Lava
                if(mWorld->PlayerInLava(headPosition))
                {
                    //change gravity
                    playerVelocity.y += (GRAVITY/3.0f) * dt;
                    headInLava = true;
                }
                else
                {
                    headInLava = false;
                }

                if(!headInLava && !headInWater)
                {
                    playerVelocity.y += GRAVITY * 3.4f * dt;
                }

                if(mWorld->PlayerCollisionWithLadder(playerPosition) && moving == false)
                {
                    playerVelocity.y = -CLIMBVELOCITY;
                }

                touchingCactus = false;
                if(mWorld->PlayerCollisionWithCactus(playerPosition) || mWorld->PlayerCollisionWithCactus(Vector3(playerPosition.x,playerPosition.y-1.5f,playerPosition.z)))
                {
                    touchingCactus = true;
                }

                //check if we will stand on ground
                Vector3 futureFootPosition = Vector3(footPosition.x,footPosition.y+playerVelocity.y*dt,footPosition.z);
                if(mWorld->SolidAtPointForPlayer(futureFootPosition))
                {
                    walkingOnGround = true;
                    soundBlockType = mWorld->BlockSoundAtPos(futureFootPosition);

                    // If the player is stuck in the ground, bring them out.
                    // This happens because we're standing on a block at -1.5, but stuck in it at -1.4, so -1.45 is the sweet spot.
                    if (mWorld->SolidAtPointForPlayer(futureFootPosition))
                    {
                        int blockOn = (int)(futureFootPosition.y);

                        if(mWorld->GetBlock(futureFootPosition.x,futureFootPosition.y,futureFootPosition.z) == CactusBlock::getID())
                        {
                            touchingCactus = true;
                        }

                        if(mWorld->PlayerOnHalfBlock(futureFootPosition))
                        {
                            playerPosition.y = (float)(blockOn + 1 + mWorld->playerGrowth-0.55f);
                        }
                        else if(mWorld->PlayerOnStairsBlock(futureFootPosition))
                        {
                            if(mWorld->PlayerOnHighStair(futureFootPosition))
                            {
                                playerPosition.y = (float)(blockOn + 1 + mWorld->playerGrowth-0.05f);
                            }
                            else
                            {
                                playerPosition.y = (float)(blockOn + 1 + mWorld->playerGrowth-0.55f);
                            }
                        }
                        else
                        {
                            playerPosition.y = (float)(blockOn + 1 + mWorld->playerGrowth-0.05f);
                        }
                    }

                    if(playerVelocity.y != 0.0f) /// FALL DAMAGE
                    {
                        if (!mWorld->PlayerInWater(Vector3(futureFootPosition.x,futureFootPosition.y+0.45f,futureFootPosition.z)) &&
                            !(mWorld->GetBlock(playerPosition.x,playerPosition.y-1.3f,playerPosition.z) >= Ladder1::getID() && mWorld->GetBlock(playerPosition.x,playerPosition.y-1.3f,playerPosition.z) <= Ladder4::getID()))
                        {
                            if(playerVelocity.y < -6 && rand() % 10 >= 7) // destroy soil
                            {
                                if(mWorld->GetBlock(futureFootPosition.x, futureFootPosition.y-0.25f, futureFootPosition.z) == Soil::getID() || mWorld->GetBlock(futureFootPosition.x, futureFootPosition.y-0.25f, futureFootPosition.z) == WateredSoil::getID())
                                {
                                    mWorld->GetBlock(futureFootPosition.x, futureFootPosition.y-0.25f, futureFootPosition.z) = DirtBlock::getID();
                                    if(mWorld->GetBlock(futureFootPosition.x, futureFootPosition.y+0.25f, futureFootPosition.z) != 0)
                                    {
                                        mWorld->DropThis(mWorld->BlockLoot(futureFootPosition.x, futureFootPosition.y+0.25f, futureFootPosition.z),1,true,Vector3(futureFootPosition.x, futureFootPosition.y+0.25f, futureFootPosition.z));
                                        mWorld->GetBlock(futureFootPosition.x, futureFootPosition.y+0.25f, futureFootPosition.z) = 0;
                                    }
                                    if(!mWorld->ChunksEqual(Vector3(futureFootPosition.x, futureFootPosition.y-0.25f, futureFootPosition.z), Vector3(futureFootPosition.x, futureFootPosition.y+0.25f, futureFootPosition.z)))
                                    {
                                        mWorld->RebuildFullMeshChunk(mWorld->getChunkId(Vector3(futureFootPosition.x, futureFootPosition.y-0.25f, futureFootPosition.z)));
                                        mWorld->RebuildFullMeshChunk(mWorld->getChunkId(Vector3(futureFootPosition.x, futureFootPosition.y+0.25f, futureFootPosition.z)));
                                    }
                                    else
                                    {
                                        mWorld->RebuildFullMeshChunk(mWorld->getChunkId(Vector3(futureFootPosition.x, futureFootPosition.y-0.25f, futureFootPosition.z)));
                                    }
                                }
                            }
                            if(playerVelocity.y < -12.8 && playerVelocity.y > -19)
                            {
                                mSoundMgr->PlayFallSound(playerVelocity.y);
                                if(mWorld->GetBlock(futureFootPosition.x, futureFootPosition.y-0.25f, futureFootPosition.z) == HayBale::getID())
                                {
                                    HurtPlayer((playerVelocity.y*-1 - 11) / 1.4 * 0.3f);
                                }
                                else
                                {
                                    HurtPlayer((playerVelocity.y*-1 - 11) / 1.4);
                                }

                                if(mWorld->HG > 0.2)
                                {
                                    mWorld->HG -= 0.2;
                                }
                                mWorld->mainStatistics.badlyFalls += 1;

                                if(mWorld->armorId[3] != 327) // if we have boots
                                {
                                    mWorld->armorAm[3] -= 2;
                                }
                            }
                            if(playerVelocity.y < -19)
                            {
                                mSoundMgr->PlayFallSound(playerVelocity.y);
                                if(mWorld->GetBlock(futureFootPosition.x, futureFootPosition.y-0.25f, futureFootPosition.z) == HayBale::getID())
                                {
                                    HurtPlayer((playerVelocity.y*-1 - 11) * 1.3f * 0.3f);
                                }
                                else
                                {
                                    HurtPlayer((playerVelocity.y*-1 - 11) * 1.3f);
                                }

                                if(mWorld->HG > 0.3)
                                {
                                    mWorld->HG -= 0.3;
                                }
                                mWorld->mainStatistics.badlyFalls += 1;

                                if(mWorld->armorId[3] != 327) // if we have boots
                                {
                                    mWorld->armorAm[3] -= 6;
                                }
                            }
                        }
                    }
                    playerVelocity.y = 0.0f;

                    //dodatkowa logika podczas stania na klocku
                    //jump
                    if(jumping)
                    {
                        if(!mWorld->SolidAtPointForPlayerHead(headPosition))
                        {
                            playerVelocity.y = 1.575f * JUMPVELOCITY;
                            walkingOnGround = false;

                            mWorld->mainStatistics.jumps += 1;
                            mWorld->HG -= (1.0f)/55.0f;
                        }
                        jumping = false;
                    }
                }
                else
                {
                    walkingOnGround = false;
                }

                // If the player has their head stuck in a block, push them down.
                Vector3 futureHeadPosition = Vector3(headPosition.x,headPosition.y+playerVelocity.y*dt,headPosition.z);
                if(mWorld->SolidAtPointForPlayerHead(futureHeadPosition))
                {
                    int blockIn = (int)(futureHeadPosition.y);
                    playerPosition.y = (float)(blockIn-0.2f); //0.15
                    playerVelocity.y = 0.0f;
                }
            }

            if(mWorld->kickedStart)
            {
                if(mWorld->kickedBy == 1) // if you damaged by zombie
                {
					if(mWorld->kickedVeneno == 1)
					{
						switch(mWorld->mainOptions.difficult)
						{
							case 0:
								HPPoisoned = false;
								HPE = 0;
							break;
							case 1:
								HPPoisoned = true;
								HPE = 7;
							break;
							case 2:
								HPPoisoned = true;
								HPE = 7;
							break;
							case 3:
								HPPoisoned = true;
								HPE = 15;
							break;
						}
					}
					
                    walkingOnGround = false;
                    playerVelocity.y = 0.9f * JUMPVELOCITY;
                    mWorld->kickedTimer = 0.5f;
                    fppCam->needUpdate = true;

                    switch(mWorld->mainOptions.difficult)
                    {
                    case 0: // peaceful
                        HurtPlayerConsiderArmor(0);
                    break;
                    case 1: // easy
                        HurtPlayerConsiderArmor(mWorld->kickedDamage-1);
                    break;
                    case 2: // normal
                        HurtPlayerConsiderArmor(mWorld->kickedDamage);
                    break;
                    case 3: // hard
                        HurtPlayerConsiderArmor(mWorld->kickedDamage+1);
                    break;
                    }
                    if(mWorld->armorId[0] != 324)
                    {
                        mWorld->armorAm[0] -= 1+rand()%3;
                    }
                    if(mWorld->armorId[1] != 325)
                    {
                        mWorld->armorAm[1] -= 1+rand()%4;
                    }
                    if(mWorld->armorId[2] != 326)
                    {
                        mWorld->armorAm[2] -= 1+rand()%3;
                    }
                    if(mWorld->armorId[3] != 327)
                    {
                        mWorld->armorAm[3] -= rand()%3;
                    }
                }
                if(mWorld->kickedBy == 2) // creeper or TNT
                {
                    walkingOnGround = false;
                    playerVelocity.y = 1.2f * JUMPVELOCITY;
                    mWorld->kickedTimer = 0.6f;
                    fppCam->needUpdate = true;

                    switch(mWorld->mainOptions.difficult)
                    {
                    case 0: // peaceful
                        HurtPlayerConsiderArmor(mWorld->kickedDamage*0.5f);
                    break;
                    case 1: // easy
                        HurtPlayerConsiderArmor(mWorld->kickedDamage*0.66f);
                    break;
                    case 2: // normal
                        HurtPlayerConsiderArmor(mWorld->kickedDamage);
                    break;
                    case 3: // hard
                        HurtPlayerConsiderArmor(mWorld->kickedDamage*1.33f);
                    break;
                    }

                    if(mWorld->armorId[0] != 324)
                    {
                        mWorld->armorAm[0] -= (mWorld->kickedDamage/2.0f)+rand()%6;
                    }
                    if(mWorld->armorId[1] != 325)
                    {
                        mWorld->armorAm[1] -= (mWorld->kickedDamage/1.5f)+rand()%6;
                    }
                    if(mWorld->armorId[2] != 326)
                    {
                        mWorld->armorAm[2] -= (mWorld->kickedDamage/2.0f)+rand()%4;
                    }
                    if(mWorld->armorId[3] != 327)
                    {
                        mWorld->armorAm[3] -= (mWorld->kickedDamage/2.0f)+rand()%3;
                    }
                }
                mWorld->kickedStart = false;
				mWorld->kickedVeneno = 0;
                mWorld->kickedBy = 0;
                mWorld->kickedDamage = 0;
            }

            //update position
            if(sleepTime <= 0.0f)
            {
                if(!canFly)
                {
                    playerPosition += playerVelocity * dt;
                }
                else
                {
                    playerVelocity.y = 0.0f;
                }
            }

            //collision with walls
            isWalking = false;
            Vector3 moveVector = fppCam->m_vVelocity;

            jumping = false;
            if(moveVector.x != 0.0f || moveVector.z != 0.0f)
            {
                if(TryToMove(Vector3(0,0,moveVector.z),dt))
                {
                    isWalking = true;
                }
                else
                {
                    if(mWorld->mainOptions.autoJump)
                    {
                        if(jumping == false)
                        {
                            jumping = true;
                        }
                    }
                }
                if(TryToMove(Vector3(moveVector.x,0,0),dt))
                {
                    isWalking = true;
                }
                else
                {
                    if(mWorld->mainOptions.autoJump)
                    {
                        if(jumping == false)
                        {
                            jumping = true;
                        }
                    }
                }
            }

            if(mWorld->kickedTimer > 0.0f)
            {
                mWorld->kickedTimer -= dt;
                TryToMove(Vector3(sinf(mWorld->kickedAngle+PI)*0.1,0,cosf(mWorld->kickedAngle+PI)*0.1),dt);
            }
            else
            {
                mWorld->kickedTimer = 0.0f;
                mWorld->kickedAngle = 0.0f;

                fppCam->needUpdate = true;
            }

            //update camera
            Vector3 delta = fppCam->m_vView - fppCam->m_vPosition;
            fppCam->m_vPosition = playerPosition;
            fppCam->m_vView = fppCam->m_vPosition + delta;
            fppCam->m_vVelocity = Vector3(0,0,0);
            fppCam->needUpdate = true;

            mWorld->UpdateWorldTime(dt);
            skyLight->UpdateLightSource(mWorld->skyTime);
            mWorld->UpdateWorldProcesses(dt);
        }

        if(fppCam->needUpdate)
        {
            mWorld->UpdatePlayerZoneBB(fppCam->m_vPosition);
        }
		
        //walking sound & walking animation
        if(isWalking && walkingOnGround && (!footInWater || !footInLava))
        {
            float walkSoundSpeed = dt;
			
			if(sprint)
			{
				walkSoundSpeed *= 1.2f;
			}			
            if(footInLava || footInWater)
            {
                walkSoundSpeed *= 0.7f;
            }
			if(headInLava || headInWater)
            {
                walkSoundSpeed *= 0.5f;
            }
			if(playerInCobweb)
			{
				walkSoundSpeed *= 0.2f;
			}
			if(crouch)
			{
				walkSoundSpeed *= 0.3f;
			}
		    walkSoundAccu += walkSoundSpeed;

			if(walkSoundAccu > 0.428f || walkSoundAccu == 0.0f)
			{
				mSoundMgr->PlayWalkSound(soundBlockType);
				walkSoundAccu = 0.0f;
			}

			float animSpeed = 2.621*PI*dt;
					
			if(sprint)
			{
				animSpeed *= 1.2f;
			}
			if(footInLava || footInWater)
            {
                animSpeed *= 0.7f;
            }
			if(headInLava || headInWater)
            {
                animSpeed *= 0.5f;
            }
			if(playerInCobweb)
			{
				animSpeed *= 0.2f;
			}
			if(crouch)
			{
				animSpeed *= 0.0f;
			}

            if(bobType == 0)
            {
                if (bobCycle < PI)
                {
                    bobCycle += animSpeed;
                }
                else
                {
                    bobType = 1;
                }
            }
            else
            {
                if (bobCycle > 0)
                {
                    bobCycle -= animSpeed;
                }
                else
                {
                    bobType = 0;
                }
            }
			
			
			//repetidor
            /*float bobSine = sinf(bobCycle/2.0f -PI/2.0f);
            float bobCose = cosf(bobCycle - (PI/2.0f) + PI)/3.0f;

            fppCam->m_vOffset = Vector3(0.0f,0.06f*bobSine+0.03f,0.0f);*/
		}
		else
		{
			walkSoundAccu = 0.45f;
            if(bobCycle > PI/2.0f)
            {
                bobCycle -= (bobCycle-(PI/2.0f))/3.0f;
            }
            else
            {
                bobCycle += ((PI/2.0f) - bobCycle)/3.0f;
            }
            fppCam->m_vOffset = Vector3(0.0f,0.0,0.0f);
		}

        if(invEn == true || craft3xEn == true || chestEn == true || furnaceEn == true)
        {
            mWorld->UpdateCompassTexture((fppCam->horAngle/180.0f)*PI);
        }
        else
        {
            if(mWorld->haveCompass) // only if you have compass in bar slots, compass texture should be updated
            {
                mWorld->UpdateCompassTexture((fppCam->horAngle/180.0f)*PI);
            }
        }
    }
}


void StatePlay::Draw(StateManager* sManager)
{
    mWorld->startSkyColor = mWorld->startClearSkyColor*(1.0-mWorld->rainyColorAlpha)+mWorld->startRainySkyColor*mWorld->rainyColorAlpha;
    mWorld->endSkyColor = mWorld->endClearSkyColor*(1.0-mWorld->rainyColorAlpha)+mWorld->endRainySkyColor*mWorld->rainyColorAlpha;
    mWorld->dawnSunsetSkyColor = mWorld->dawnSunsetClearSkyColor*(1.0-mWorld->rainyColorAlpha)+mWorld->dawnSunsetRainySkyColor*mWorld->rainyColorAlpha;
    (mWorld->startSkyColor).saturate();
    (mWorld->endSkyColor).saturate();
    (mWorld->dawnSunsetSkyColor).saturate();

    mRender->SetFontStyle(0.345f,0xFFFFFFFF,0,0x00000200);

    if (mWorld->skyTime < 180)
    {
        if(fppCam->horAngle < 360 && fppCam->horAngle >= 239)
        {
            angleFactor = (fppCam->horAngle - 239)/120.0f;
        }
        else if(fppCam->horAngle >= 0 && fppCam->horAngle <= 120)
        {
            angleFactor = (120 - fppCam->horAngle)/120.0f;
        }
        else
        {
            angleFactor = 0;
        }
    }
    else
    {
        if(fppCam->horAngle < 180 && fppCam->horAngle >= 60)
        {
            angleFactor = (fppCam->horAngle - 60)/120.0f;
        }
        else if(fppCam->horAngle >= 180 && fppCam->horAngle <= 300)
        {
            angleFactor = (300 - fppCam->horAngle)/120.0f;
        }
        else
        {
            angleFactor = 0;
        }
    }
    if(angleFactor < 0.0f)
    {
        angleFactor = 0.0f;
    }
    if(angleFactor > 1.0f)
    {
        angleFactor = 1.0f;
    }

	bool needUpdate = fppCam->needUpdate;
	float nullFactor = (1.0f-mWorld->dawnSunsetFactor)+(1.0f-angleFactor);
	if(nullFactor > 1.0f)
    {
        nullFactor = 1.0f;
    }

    if(playerPosition.y <= 48)
    {
        Vector3 color = ((mWorld->startSkyColor*mWorld->brightFactor)*nullFactor+(mWorld->dawnSunsetSkyColor*mWorld->dawnSunsetFactor*angleFactor))*(playerPosition.y/48.0f)*(playerPosition.y/48.0f);
        mRender->StartFrame(color.x,color.y,color.z);
    }
    else
    {
        Vector3 color = (mWorld->startSkyColor*mWorld->brightFactor)*nullFactor+(mWorld->dawnSunsetSkyColor*mWorld->dawnSunsetFactor*angleFactor);
        mRender->StartFrame(color.x,color.y,color.z);
    }

    if(mWorld->mainOptions.smoothLighting == 1)
    {
        sceGuShadeModel(GU_SMOOTH);
    }
    else
    {
        sceGuShadeModel(GU_FLAT);
    }
	
    if(mWorld->mainOptions.detailedSky == 1)
    {
        if(playerPosition.y <= 48)
        {
            Vector3 color = ((mWorld->startSkyColor*mWorld->brightFactor)*nullFactor+(mWorld->dawnSunsetSkyColor*mWorld->dawnSunsetFactor*angleFactor))*(playerPosition.y/48.0f)*(playerPosition.y/48.0f);
            sceGuFog(mWorld->mainOptions.horizontalViewDistance * 7, mWorld->mainOptions.horizontalViewDistance * 14 + 168.0f, GU_COLOR(color.x,color.y,color.z,1.0));
        }
        else
        {
            Vector3 color = (mWorld->startSkyColor*mWorld->brightFactor)*nullFactor+(mWorld->dawnSunsetSkyColor*mWorld->dawnSunsetFactor*angleFactor);
            sceGuFog(mWorld->mainOptions.horizontalViewDistance * 7, mWorld->mainOptions.horizontalViewDistance * 14 + 168.0f, GU_COLOR(color.x,color.y,color.z,1.0));
        }
        sceGuEnable(GU_FOG);

        /// Skybox rendering
        {
            MatrixPush();
            MatrixTranslation(Vector3(fppCam->m_vPosition.x,fppCam->m_vPosition.y+44,fppCam->m_vPosition.z));
            MatrixScale(Vector3(1.0f,1.0f,1.0f));

            if(playerPosition.y <= 45)
            {
                skyBox->Render(mWorld->endSkyColor*mWorld->brightFactor*(playerPosition.y/45.0f),playerPosition,fppCam->horAngle/180.0f*PI);
            }
            else
            {
                skyBox->Render(mWorld->endSkyColor*mWorld->brightFactor,playerPosition,fppCam->horAngle/180.0f*PI);
            }
            MatrixPop();
        }
        sceGuDisable(GU_FOG);

        /// StarsBox rendering
        if(mWorld->rainyColorAlpha != 1.0f)
        {
            if(mWorld->worldDayTime < 10 || mWorld->worldDayTime >= 21)
            {
                TextureManager::Instance()->SetTextureModeulate(stars);

                MatrixPush();
				
                MatrixTranslation(Vector3(fppCam->m_vPosition.x,fppCam->m_vPosition.y+fppCam->m_vOffset.y,fppCam->m_vPosition.z));
                MatrixScale(Vector3(1.0f,1.0f,1.0f));
				MatrixAngle(Vector3(0.0f, 0.0f, mWorld->skyTime * 1.0f));

                if(mWorld->brightFactor <= 0.05f)
                {
                    if(mWorld->starsFactor < 0.3f)
                    {
                        mWorld->starsFactor += dt*0.03f;
                    }
                    if(mWorld->starsFactor > 0.3f)
                    {
                        mWorld->starsFactor = 0.3f;
                    }
                }
                else
                {
                    if(mWorld->starsFactor > 0.0f)
                    {
                        mWorld->starsFactor -= dt*0.03f;
                    }
                    if(mWorld->starsFactor < 0.0f)
                    {
                        mWorld->starsFactor = 0.0f;
                    }
                }
				
                starsBox->Render((1.0f-mWorld->brightFactor-0.4+mWorld->starsFactor)*(1-mWorld->rainyColorAlpha));

                MatrixPop();
            }
        }


        /// Sun/moon rendering
        if(mWorld->rainyColorAlpha != 1)
        {
            MatrixColor(GU_COLOR(1.0f-mWorld->rainyColorAlpha,1.0f-mWorld->rainyColorAlpha,1.0f-mWorld->rainyColorAlpha,1.0f-mWorld->rainyColorAlpha));

            // sun
            MatrixPush();
            MatrixTranslation(Vector3(fppCam->m_vPosition.x,fppCam->m_vPosition.y,fppCam->m_vPosition.z));

            TextureManager::Instance()->SetTextureModeulate(suntex);

            skyLight->Render();
            MatrixPop();

            // moon
            MatrixPush();
            MatrixTranslation(Vector3(fppCam->m_vPosition.x,fppCam->m_vPosition.y,fppCam->m_vPosition.z));

            TextureManager::Instance()->SetTextureModeulate(moontex);

            skyMoonLight->UpdateLightSource(mWorld->skyTime+180.0f);
            skyMoonLight->Render();
            MatrixPop();
        }

        if(mWorld->rainyColorAlpha != 1.0f)
        {
            if(mWorld->mainOptions.fogRendering == 1) /// FOG FOR CLOUDS
            {
                if(headInWater == true)
                {
                    sceGuFog(0.0f, 15.0f, GU_COLOR(74.0f/255.0f*mWorld->brightFactor*0.6,130.0f/255.0f*mWorld->brightFactor*0.6,158.0f/255.0f*mWorld->brightFactor*0.6, 1.0f));
                }
                else
                {
                    if(playerPosition.y <= 48)
                    {
                        Vector3 color = (( mWorld->startSkyColor*mWorld->brightFactor)*nullFactor+(mWorld->dawnSunsetSkyColor*mWorld->dawnSunsetFactor*angleFactor))*(playerPosition.y/48.0f)*(playerPosition.y/48.0f);
                        sceGuFog(mWorld->mainOptions.horizontalViewDistance * 7 + 48.0f, mWorld->mainOptions.horizontalViewDistance * 14 + 122.0f, GU_COLOR(color.x,color.y,color.z,1.0));
                    }
                    else
                    {
                        Vector3 color = (mWorld->startSkyColor*mWorld->brightFactor)*nullFactor+(mWorld->dawnSunsetSkyColor*mWorld->dawnSunsetFactor*angleFactor);
                        sceGuFog(mWorld->mainOptions.horizontalViewDistance * 7 + 48.0f, mWorld->mainOptions.horizontalViewDistance * 14 + 122.0f, GU_COLOR(color.x,color.y,color.z,1.0));
                    }
                }
                sceGuEnable(GU_FOG);
            }

            /// Clouds rendering

            /*MatrixPush();

            if(playerPosition.y <= 48)
            {
                MatrixColor(GU_COLOR(mWorld->brightFactor*(playerPosition.y/48.0f),mWorld->brightFactor*(playerPosition.y/48.0f),mWorld->brightFactor*(playerPosition.y/48.0f),1-mWorld->rainyColorAlpha));
            }
            else
            {
                MatrixColor(GU_COLOR(mWorld->brightFactor,mWorld->brightFactor,mWorld->brightFactor,1-mWorld->rainyColorAlpha));
            }

            MatrixTranslation(Vector3(-114,116,-114));

            TextureManager::Instance()->SetTextureModeulate(cloudsTex);

            sceGuTexWrap(GU_REPEAT,GU_REPEAT);
            sceGuTexOffset(cloudsOffset,0.0f); // fake movement
            mWorld->drawClouds();
            sceGuTexWrap(GU_CLAMP,GU_CLAMP);

            MatrixPop();
            MatrixColor(0xFFFFFFFF);

            if(mWorld->mainOptions.fogRendering == 1)
            {
                sceGuDisable(GU_FOG);
            }*/
			// Clouds - Layer 1
			MatrixPush();
			if (playerPosition.y <= 48)
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), 1 - mWorld->rainyColorAlpha));
			}
			else
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor, mWorld->brightFactor, mWorld->brightFactor, 1 - mWorld->rainyColorAlpha));
			}

			MatrixTranslation(Vector3(-114, 115, -114));

			TextureManager::Instance()->SetTextureModeulate(clouds2Tex);

			sceGuTexWrap(GU_REPEAT, GU_REPEAT);
			sceGuTexOffset(cloudsOffset, 0.0f); // fake movement
			mWorld->drawClouds();
			sceGuTexWrap(GU_CLAMP, GU_CLAMP);

			MatrixPop();
			MatrixColor(0xFFFFFFFF);
			// Layer 2
			MatrixPush();
			if (playerPosition.y <= 48)
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), 1 - mWorld->rainyColorAlpha));
			}
			else
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor, mWorld->brightFactor, mWorld->brightFactor, 1 - mWorld->rainyColorAlpha));
			}
			MatrixTranslation(Vector3(-114, 114.75f, -114));

			TextureManager::Instance()->SetTextureModeulate(clouds2Tex);

			sceGuTexWrap(GU_REPEAT, GU_REPEAT);
			sceGuTexOffset(cloudsOffset, 0.0f); // fake movement
			mWorld->drawClouds();
			sceGuTexWrap(GU_CLAMP, GU_CLAMP);

			MatrixPop();
			MatrixColor(0xFFFFFFFF);
			// Layer 3
			MatrixPush();
			if (playerPosition.y <= 48)
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), 1 - mWorld->rainyColorAlpha));
			}
			else
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor, mWorld->brightFactor, mWorld->brightFactor, 1 - mWorld->rainyColorAlpha));
			}
			MatrixTranslation(Vector3(-114, 114.5f, -114));

			TextureManager::Instance()->SetTextureModeulate(clouds2Tex);

			sceGuTexWrap(GU_REPEAT, GU_REPEAT);
			sceGuTexOffset(cloudsOffset, 0.0f); // fake movement
			mWorld->drawClouds();
			sceGuTexWrap(GU_CLAMP, GU_CLAMP);

			MatrixPop();
			MatrixColor(0xFFFFFFFF);
			// Layer 4
			MatrixPush();
			if (playerPosition.y <= 48)
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), 1 - mWorld->rainyColorAlpha));
			}
			else
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor, mWorld->brightFactor, mWorld->brightFactor, 1 - mWorld->rainyColorAlpha));
			}
			MatrixTranslation(Vector3(-114, 114.25f, -114));

			TextureManager::Instance()->SetTextureModeulate(clouds2Tex);

			sceGuTexWrap(GU_REPEAT, GU_REPEAT);
			sceGuTexOffset(cloudsOffset, 0.0f); // fake movement
			mWorld->drawClouds();
			sceGuTexWrap(GU_CLAMP, GU_CLAMP);

			MatrixPop();
			MatrixColor(0xFFFFFFFF);
			// Layer 5
			MatrixPush();
			if (playerPosition.y <= 48)
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), 1 - mWorld->rainyColorAlpha));
			}
			else
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor, mWorld->brightFactor, mWorld->brightFactor, 1 - mWorld->rainyColorAlpha));
			}
			MatrixTranslation(Vector3(-114, 114, -114));

			TextureManager::Instance()->SetTextureModeulate(clouds2Tex);

			sceGuTexWrap(GU_REPEAT, GU_REPEAT);
			sceGuTexOffset(cloudsOffset, 0.0f); // fake movement
			mWorld->drawClouds();
			sceGuTexWrap(GU_CLAMP, GU_CLAMP);

			MatrixPop();
			MatrixColor(0xFFFFFFFF);
			// Layer 6
			MatrixPush();
			if (playerPosition.y <= 48)
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), 1 - mWorld->rainyColorAlpha));
			}
			else
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor, mWorld->brightFactor, mWorld->brightFactor, 1 - mWorld->rainyColorAlpha));
			}
			MatrixTranslation(Vector3(-114, 113.75f, -114));

			TextureManager::Instance()->SetTextureModeulate(clouds2Tex);

			sceGuTexWrap(GU_REPEAT, GU_REPEAT);
			sceGuTexOffset(cloudsOffset, 0.0f); // fake movement
			mWorld->drawClouds();
			sceGuTexWrap(GU_CLAMP, GU_CLAMP);

			MatrixPop();
			MatrixColor(0xFFFFFFFF);
			// Layer 7
			MatrixPush();
			if (playerPosition.y <= 48)
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), 1 - mWorld->rainyColorAlpha));
			}
			else
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor, mWorld->brightFactor, mWorld->brightFactor, 1 - mWorld->rainyColorAlpha));
			}
			MatrixTranslation(Vector3(-114, 113.5f, -114));

			TextureManager::Instance()->SetTextureModeulate(clouds2Tex);

			sceGuTexWrap(GU_REPEAT, GU_REPEAT);
			sceGuTexOffset(cloudsOffset, 0.0f); // fake movement
			mWorld->drawClouds();
			sceGuTexWrap(GU_CLAMP, GU_CLAMP);

			MatrixPop();
			MatrixColor(0xFFFFFFFF);
			// Layer 8
			MatrixPush();
			if (playerPosition.y <= 48)
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), 1 - mWorld->rainyColorAlpha));
			}
			else
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor, mWorld->brightFactor, mWorld->brightFactor, 1 - mWorld->rainyColorAlpha));
			}
			MatrixTranslation(Vector3(-114, 113.25f, -114));

			TextureManager::Instance()->SetTextureModeulate(clouds2Tex);

			sceGuTexWrap(GU_REPEAT, GU_REPEAT);
			sceGuTexOffset(cloudsOffset, 0.0f); // fake movement
			mWorld->drawClouds();
			sceGuTexWrap(GU_CLAMP, GU_CLAMP);

			MatrixPop();
			MatrixColor(0xFFFFFFFF);
			// Layer 9
			MatrixPush();
			if (playerPosition.y <= 48)
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), mWorld->brightFactor * (playerPosition.y / 48.0f), 1 - mWorld->rainyColorAlpha));
			}
			else
			{
				MatrixColor(GU_COLOR(mWorld->brightFactor, mWorld->brightFactor, mWorld->brightFactor, 1 - mWorld->rainyColorAlpha));
			}
			MatrixTranslation(Vector3(-114, 112.75, -114));

			TextureManager::Instance()->SetTextureModeulate(cloudsTex);

			sceGuTexWrap(GU_REPEAT, GU_REPEAT);
			sceGuTexOffset(cloudsOffset, 0.0f); // fake movement
			mWorld->drawClouds();
			sceGuTexWrap(GU_CLAMP, GU_CLAMP);

			MatrixPop();
			MatrixColor(0xFFFFFFFF);
        }
    }

    if(mWorld->mainOptions.fogRendering == 1) /// FOG FOR WORLD
    {
        if(headInWater == true)
        {
            sceGuFog(0.0f, 15.0f, GU_COLOR(74.0f/255.0f*mWorld->brightFactor*0.6,130.0f/255.0f*mWorld->brightFactor*0.6,158.0f/255.0f*mWorld->brightFactor*0.6, 1.0f));
        }
        else
        {
            float fog_start = 0.0f;
            float fog_end = mWorld->mainOptions.horizontalViewDistance * 14 + 54.75f;
            if(fog_start < 0.0f)
            {
                fog_start = 0.0f;
            }
            if(playerPosition.y <= 48)
            {
                Vector3 color = ((Vector3(0.8f-(mWorld->rainyColorAlpha/10.0f)*3,0.8f-(mWorld->rainyColorAlpha/10.0f)*3,0.8f-(mWorld->rainyColorAlpha/10.0f)*3)*mWorld->brightFactor)*nullFactor+(mWorld->dawnSunsetSkyColor*mWorld->dawnSunsetFactor*angleFactor))*(playerPosition.y/48.0f)*(playerPosition.y/48.0f);
                sceGuFog(fog_start, fog_end, GU_COLOR(color.x,color.y,color.z,1.0));
            }
            else
            {
                Vector3 color = (Vector3(0.8f-(mWorld->rainyColorAlpha/10.0f)*3,0.8f-(mWorld->rainyColorAlpha/10.0f)*3,0.8f-(mWorld->rainyColorAlpha/10.0f)*3)*mWorld->brightFactor)*nullFactor+(mWorld->dawnSunsetSkyColor*mWorld->dawnSunsetFactor*angleFactor);
                sceGuFog(fog_start, fog_end, GU_COLOR(color.x,color.y,color.z,1.0));
            }
        }
        sceGuEnable(GU_FOG);	// Enable fog
    }

    /// Draw world
    {
        sceGuEnable(GU_TEXTURE_2D);
        /*if(mWorld->mainOptions.newSprintOption)
        {
            TextureManager::Instance()->SetMipMaps2Textures(texture, texture_mips);
        }
        else
        {
            TextureManager::Instance()->SetTextureModeulate(texture);
        }*/	
		TextureManager::Instance()->SetTextureModeulate(texture);
        sceGumPushMatrix();
        mWorld->drawWorld(fppCam->mFrustum,needUpdate);
        sceGumPopMatrix();
        sceGuDisable(GU_TEXTURE_2D);
    }

    /// DROP
    if(mWorld->mDrops.empty() == false)
    {
        for(int d = 0; d < mWorld->mDrops.size(); d++)
        {
            if(d < mWorld->mDrops.size() && mWorld->mDrops.empty() == false)
            {
                Drop* TestDrop = mWorld->mDrops[d];

                if(menuState == 0)//game state
                {
                    TestDrop->Update(dt);
                }

                if(TestDrop->GetId() < 250)
                {
                    TextureManager::Instance()->SetTextureModeulate(texture);
                }
                else
                {
                    mWorld->ItemHaveTerrainTexture(TestDrop->GetId()) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);
                }

                TestDrop->Render(fppCam->mFrustum, playerPosition, fppCam->horAngle/180.0f*PI);

                if(TestDrop->toDestroy == true)
                {
                    mWorld->DestroyDrop(d);
                    d -= 1;
                    continue;
                }

                if(TestDrop->getMe == true)
                {
                    mWorld->PutInInventory(TestDrop->GetId(),TestDrop->GetAmount(),TestDrop->GetStackable(), slotForChangeScale);
                    mSoundMgr->PlayPlopSound();
                    mWorld->DestroyDrop(d);
                    d -= 1.0;

                    if(slotForChangeScale != -1)
                    {
                        changeScale = 1.0f;
                    }
                    continue;
                }
            }
        }
    }

    if(mWorld->mTNTs.empty() == false)
    {
        TextureManager::Instance()->SetTextureModeulate(texture);
        for(int k = mWorld->mTNTs.size()-1; k >= 0; k--)
        {
            if(mWorld->mTNTs.empty() == true)
            {
                return;
            }

            TNT* TestTNT = mWorld->mTNTs[k];

            if(TestTNT != NULL) // if this TNT exists
            {
                if(TestTNT->toDestroy == false)
                {
                    if(menuState == 0)//game state
                    {
                        TestTNT->Update(dt);
                        int time = TestTNT->timeToExplode*100;
                        if(time % 4 == 0 && rand()%10 <= 6 && time > 40 && TestTNT->ignitedByPlayer)
                        {

                            Particle* Smoke = new Particle(mWorld,"smoke",Vector3(TestTNT->position.x+(-0.1f+(rand()%10)/100.0f),TestTNT->position.y+0.60f,TestTNT->position.z+(-0.1f+(rand()%10)/100.0f)));
                            Smoke->SetVelocity(Vector3(0,1.5f+(rand()%10)/10.0f,0));
                            Smoke->SetScale(0.1f+(rand()%10)/100.0f);
                            Smoke->SetMaxLiveTime(1000);
                            Smoke->SetLiveTime(500,1250);
                            Smoke->SetBrightness(0.15,0.25);
                            Smoke->friction = 4.0f;

                            mParticles.push_back(Smoke);
                        }
                    }

                    TestTNT->Render();
                }

                if(TestTNT->toDestroy == true)
                {
                    for(float verAngle = 0.0f; verAngle <= PI; verAngle += PI/4.0f)
                    {
                        for(float horAngle = 0.0f; horAngle <= PI*2; horAngle += PI/2.0f)
                        {
                            if(mParticles.size() > 52)
                            {
                                continue;
                            }
                            float radius = 4.5f + (rand() % 100)/10.0f;

                            Particle* Smoke = new Particle(mWorld,"smoke",Vector3(TestTNT->position.x,TestTNT->position.y,TestTNT->position.z));
                            Smoke->SetVelocity(Vector3(mWorld->LengthDirX(horAngle,verAngle)*radius,mWorld->LengthDirY(horAngle,verAngle)*radius,mWorld->LengthDirZ(horAngle,verAngle)*radius));
                            Smoke->SetScale(0.3f+(rand()%25)/100.0f);
                            Smoke->friction = 4.25f;

                            mParticles.push_back(Smoke);
                        }
                    }
                    if(mParticles.size() < 64)
                    {
                        Particle* Explosion = new Particle(mWorld,"explosion",Vector3(TestTNT->position.x-1.2f,TestTNT->position.y-1.2f,TestTNT->position.z));
                        Particle* Explosion2 = new Particle(mWorld,"explosion",Vector3(TestTNT->position.x+1.2f,TestTNT->position.y+1.2f,TestTNT->position.z));
                        Particle* Explosion3 = new Particle(mWorld,"explosion",Vector3(TestTNT->position.x,TestTNT->position.y-1.2f,TestTNT->position.z-1.2f));
                        Particle* Explosion4 = new Particle(mWorld,"explosion",Vector3(TestTNT->position.x,TestTNT->position.y+1.2f,TestTNT->position.z+1.2f));
                        Particle* Explosion5 = new Particle(mWorld,"explosion",Vector3(TestTNT->position.x,TestTNT->position.y,TestTNT->position.z));
                        mParticles.push_back(Explosion);
                        mParticles.push_back(Explosion2);
                        mParticles.push_back(Explosion3);
                        mParticles.push_back(Explosion4);
                        mParticles.push_back(Explosion5);
                    }

                    mWorld->DestroyTNTentity(k);
                    if(mWorld->mTNTs.size() <= 16)
                    {
                        mSoundMgr->PlayTNTSound();
                    }
                }
            }
        }
    }

	int Acumulado = mWorld->EXP+1;

    /// ZOMBIE
    if(mWorld->mZombies.empty() == false)
    {
        for(int d = 0; d < mWorld->mZombies.size(); d++)
        {
            if(d < mWorld->mZombies.size() && mWorld->mZombies.empty() == false)
            {
                Zombie* TestZombie = mWorld->mZombies[d];

                if(menuState == 0)//game state
                {
                    TestZombie->Update(mWorld, playerPosition, dt);
                }
                TestZombie->Render(fppCam->mFrustum, dt);

                if(TestZombie->playHurtSound)
                {
                    mSoundMgr->PlayZombieHurtSound(TestZombie->FastDistanceToPlayer());
                    TestZombie->playHurtSound = false;
                }

                if(TestZombie->playSaySound)
                {
                    mSoundMgr->PlayZombieSaySound(TestZombie->FastDistanceToPlayer());
                    TestZombie->playSaySound = false;
                }

                if(TestZombie->killed == true)
                {
                    if(TestZombie->playDieSound)
                    {
                        mSoundMgr->PlayZombieDieSound(TestZombie->FastDistanceToPlayer());
                        TestZombie->playDieSound = false;
                    }
                }

                if(TestZombie->toDestroy == true || (TestZombie->killed == true && TestZombie->deathTimer <= 0.0f))
                {
                    if(TestZombie->killed == true)
                    {
                        if(TestZombie->lastKickType != 2) // kicked by player or other death
                        {
                            int rfAmount = rand() % 3;
                            if(rfAmount != 0)
                            {
                                mWorld->DropThis(RottenFlesh::getID(),rfAmount,true,Vector3(TestZombie->position.x,TestZombie->position.y+0.3f,TestZombie->position.z));
                            }

                            // rare drop
                            if(rand() % 100 <= 2)
                            {
                                mWorld->DropThis(IronBar::getID(),1,true,Vector3(TestZombie->position.x,TestZombie->position.y+0.3f,TestZombie->position.z));
                            }
                            if(rand() % 100 <= 3)
                            {
                                mWorld->DropThis(Potato::getID(),1,true,Vector3(TestZombie->position.x,TestZombie->position.y+0.3f,TestZombie->position.z));
                            }
                            if(rand() % 100 <= 3)
                            {
                                mWorld->DropThis(Carrot::getID(),1,true,Vector3(TestZombie->position.x,TestZombie->position.y+0.3f,TestZombie->position.z));
                            }
                            // end

                            if(TestZombie->helmetId != -1 && rand() % 4 <= 2)
                            {
                                mWorld->DropThis(TestZombie->helmetId,30+rand()%20,false,Vector3(TestZombie->position.x,TestZombie->position.y+0.3f,TestZombie->position.z));
                            }
                            if(TestZombie->chestplateId != -1 && rand() % 4 <= 2)
                            {
                                mWorld->DropThis(TestZombie->chestplateId,30+rand()%20,false,Vector3(TestZombie->position.x,TestZombie->position.y+0.3f,TestZombie->position.z));
                            }
                            if(TestZombie->leggingsId != -1 && rand() % 4 <= 2)
                            {
                                mWorld->DropThis(TestZombie->leggingsId,30+rand()%20,false,Vector3(TestZombie->position.x,TestZombie->position.y+0.3f,TestZombie->position.z));
                            }
                            if(TestZombie->bootsId != -1 && rand() % 4 <= 2)
                            {
                                mWorld->DropThis(TestZombie->bootsId,30+rand()%20,false,Vector3(TestZombie->position.x,TestZombie->position.y+0.3f,TestZombie->position.z));
                            }
                        }
                        else // killed by explosion
                        {
                            mWorld->DropThis(Bone::getID(),1,true,Vector3(TestZombie->position.x,TestZombie->position.y+0.3f,TestZombie->position.z));
                            mWorld->DropThis(Bone::getID(),1,true,Vector3(TestZombie->position.x,TestZombie->position.y+0.3f,TestZombie->position.z));
                            if(rand()%3 == 0)
                            {
                                mWorld->DropThis(Bone::getID(),1,true,Vector3(TestZombie->position.x,TestZombie->position.y+0.3f,TestZombie->position.z));
                            }
                        }
                    }

                    for(int k = 0; k <= 7; k++)
                    {
                        Particle* Smoke = new Particle(mWorld, "smoke",Vector3(TestZombie->position.x-0.4+(rand()%9/10.0f),TestZombie->position.y+0.2f+(rand()%7/5.0f),TestZombie->position.z-0.4+(rand()%9/10.0f)));
                        mParticles.push_back(Smoke);
                    }


                    mWorld->DestroyZombie(d);
					if(TestZombie->killed == true && TestZombie->lastKickType == 1)
					{
						if(TestZombie->helmetId != -1 || TestZombie->chestplateId != -1 || TestZombie->leggingsId != -1 || TestZombie->bootsId != -1)
						{
							mWorld->EXPcount += (5+rand()%8)*15/Acumulado;
							mSoundMgr->PlayOrbSound();
						}
						else
						{
							mWorld->EXPcount += (2+rand()%5)*15/Acumulado;
							mSoundMgr->PlayOrbSound();
						}
					}
                    d --;
                }
            }
        }
    }
	
	/// SPIDER
    if(mWorld->mSpiders.empty() == false)
    {
        for(int d = 0; d < mWorld->mSpiders.size(); d++)
        {
            if(d < mWorld->mSpiders.size() && mWorld->mSpiders.empty() == false)
            {
                Spider* TestSpider = mWorld->mSpiders[d];

                if(menuState == 0)//game state
                {
                    TestSpider->Update(mWorld, playerPosition, dt);
                }
                TestSpider->Render(fppCam->mFrustum, dt);

                if(TestSpider->playHurtSound)
                {
                    mSoundMgr->PlaySpiderHurtSound(TestSpider->FastDistanceToPlayer());
                    TestSpider->playHurtSound = false;
                }

                if(TestSpider->playSaySound)
                {
                    mSoundMgr->PlaySpiderHurtSound(TestSpider->FastDistanceToPlayer());
                    TestSpider->playSaySound = false;
                }

                if(TestSpider->killed == true)
                {
                    if(TestSpider->playDieSound)
                    {
                        mSoundMgr->PlaySpiderDieSound(TestSpider->FastDistanceToPlayer());
                        TestSpider->playDieSound = false;
                    }
                }

                if(TestSpider->toDestroy == true || (TestSpider->killed == true && TestSpider->deathTimer <= 0.0f))
                {
                    if(TestSpider->killed == true)
                    {
                        if(TestSpider->lastKickType != 2) // kicked by player or other death
                        {
							mWorld->DropThis(SpiderEye::getID(),1+rand()%2,true,Vector3(TestSpider->position.x,TestSpider->position.y+0.3f,TestSpider->position.z));
							mWorld->DropThis(String::getID(),1+rand()%2,true,Vector3(TestSpider->position.x,TestSpider->position.y+0.3f,TestSpider->position.z));
                        }
                        else // killed by explosion
                        {
                            mWorld->DropThis(Bone::getID(),1,true,Vector3(TestSpider->position.x,TestSpider->position.y+0.3f,TestSpider->position.z));
                            mWorld->DropThis(Bone::getID(),1,true,Vector3(TestSpider->position.x,TestSpider->position.y+0.3f,TestSpider->position.z));
                            if(rand()%3 == 0)
                            {
                                mWorld->DropThis(Bone::getID(),1,true,Vector3(TestSpider->position.x,TestSpider->position.y+0.3f,TestSpider->position.z));
                            }
                        }
                    }

                    for(int k = 0; k <= 7; k++)
                    {
                        Particle* Smoke = new Particle(mWorld, "smoke",Vector3(TestSpider->position.x-0.4+(rand()%9/10.0f),TestSpider->position.y+0.2f+(rand()%7/5.0f),TestSpider->position.z-0.4+(rand()%9/10.0f)));
                        mParticles.push_back(Smoke);
                    }


                    mWorld->DestroySpider(d);
					if(TestSpider->killed == true && TestSpider->lastKickType == 1)
					{
						mWorld->EXPcount += (2+rand()%5)*15/Acumulado;
						mSoundMgr->PlayOrbSound();
					}
                    d --;
                }
            }
        }
    }
	
	/// SPIDERCAVE
    if(mWorld->mSpidercaves.empty() == false)
    {
        for(int d = 0; d < mWorld->mSpidercaves.size(); d++)
        {
            if(d < mWorld->mSpidercaves.size() && mWorld->mSpidercaves.empty() == false)
            {
                Spidercave* TestSpidercave = mWorld->mSpidercaves[d];

                if(menuState == 0)//game state
                {
                    TestSpidercave->Update(mWorld, playerPosition, dt);
                }
                TestSpidercave->Render(fppCam->mFrustum, dt);

                if(TestSpidercave->playHurtSound)
                {
                    mSoundMgr->PlaySpiderHurtSound(TestSpidercave->FastDistanceToPlayer());
                    TestSpidercave->playHurtSound = false;
                }

                if(TestSpidercave->playSaySound)
                {
                    mSoundMgr->PlaySpiderHurtSound(TestSpidercave->FastDistanceToPlayer());
                    TestSpidercave->playSaySound = false;
                }

                if(TestSpidercave->killed == true)
                {
                    if(TestSpidercave->playDieSound)
                    {
                        mSoundMgr->PlaySpiderDieSound(TestSpidercave->FastDistanceToPlayer());
                        TestSpidercave->playDieSound = false;
                    }
                }

                if(TestSpidercave->toDestroy == true || (TestSpidercave->killed == true && TestSpidercave->deathTimer <= 0.0f))
                {
                    if(TestSpidercave->killed == true)
                    {
                        mWorld->DropThis(SpiderEye::getID(),1+rand()%2,true,Vector3(TestSpidercave->position.x,TestSpidercave->position.y+0.3f,TestSpidercave->position.z));
                        mWorld->DropThis(String::getID(),1+rand()%2,true,Vector3(TestSpidercave->position.x,TestSpidercave->position.y+0.3f,TestSpidercave->position.z));
                    }

                    for(int k = 0; k <= 7; k++)
                    {
                        Particle* Smoke = new Particle(mWorld, "smoke",Vector3(TestSpidercave->position.x-0.4+(rand()%9/10.0f),TestSpidercave->position.y+0.2f+(rand()%7/5.0f),TestSpidercave->position.z-0.4+(rand()%9/10.0f)));
                        mParticles.push_back(Smoke);
                    }


                    mWorld->DestroySpidercave(d);
					if(TestSpidercave->killed == true && TestSpidercave->lastKickType == 1)
					{
						mWorld->EXPcount += (2+rand()%5)*15/Acumulado;
						mSoundMgr->PlayOrbSound();
					}
                    d --;
                }
            }
        }
    }

    /// COWS
    if(mWorld->mCows.empty() == false)
    {			
        for(int d = 0; d < mWorld->mCows.size(); d++)
        {
            if(d < mWorld->mCows.size() && mWorld->mCows.empty() == false)
            {
                Cow* TestCow = mWorld->mCows[d];

                if(menuState == 0)//game state
                {
                    TestCow->handItemId = mWorld->invId[27+barPosition];
                    TestCow->Update(mWorld, playerPosition, dt);
                }
                TestCow->Render(fppCam->mFrustum, dt);

                if(TestCow->playHurtSound)
                {
                    mSoundMgr->PlayCowHurtSound(TestCow->FastDistanceToPlayer());
                    TestCow->playHurtSound = false;
                }

                if(TestCow->playSaySound)
                {
                    mSoundMgr->PlayCowSaySound(TestCow->FastDistanceToPlayer());
                    TestCow->playSaySound = false;
                }

                if(TestCow->toDestroy == true || (TestCow->killed == true && TestCow->deathTimer < 0.0f))
                {
					
                    if(TestCow->killed == true)
                    {
                        mWorld->DropThis(RawBeef::getID(),1+rand()%2,true,Vector3(TestCow->position.x,TestCow->position.y+0.3f,TestCow->position.z));
                        mWorld->DropThis(Leather::getID(),1+rand()%2,true,Vector3(TestCow->position.x,TestCow->position.y+0.3f,TestCow->position.z));
                    }

                    for(int k = 0; k <= 7; k++)
                    {
                        Particle* Smoke = new Particle(mWorld, "smoke",Vector3(TestCow->position.x-0.5+(rand()%11/10.0f),TestCow->position.y+0.2+(rand()%4/5.0f),TestCow->position.z-0.5+(rand()%11/10.0f)));
                        k % 2 == 0 ? Smoke->SetVelocity(Vector3(-3*rand()%2,0,-3*rand()%2)) : Smoke->SetVelocity(Vector3(3*rand()%2,0,3*rand()%2));
                        mParticles.push_back(Smoke);
                    }

                    mWorld->DestroyCow(d);
					if(TestCow->killed == true && TestCow->lastKickType == 1)
					{
							mWorld->EXPcount += (2+rand()%5)*15/Acumulado;
							mSoundMgr->PlayOrbSound();	
					}
                    d --;
                }			
            }
        }
    }
	
	/// CHICKENS
    if(mWorld->mChickens.empty() == false)
    {
        for(int d = 0; d < mWorld->mChickens.size(); d++)
        {
            if(d < mWorld->mChickens.size() && mWorld->mChickens.empty() == false)
            {
                Chicken* TestChicken = mWorld->mChickens[d];

                if(menuState == 0)//game state
                {
                    TestChicken->handItemId = mWorld->invId[27+barPosition];
                    TestChicken->Update(mWorld, playerPosition, dt);
                }
                TestChicken->Render(fppCam->mFrustum, dt);

                if(TestChicken->playHurtSound)
                {
                    mSoundMgr->PlayChickenHurtSound(TestChicken->FastDistanceToPlayer());
                    TestChicken->playHurtSound = false;
                }

                if(TestChicken->playSaySound)
                {
                    mSoundMgr->PlayChickenSaySound(TestChicken->FastDistanceToPlayer());
                    TestChicken->playSaySound = false;
                }
				
                if(TestChicken->toDestroy == true || (TestChicken->killed == true && TestChicken->deathTimer < 0.0f))
                {
                    if(TestChicken->killed == true)
                    {
                        mWorld->DropThis(RawChicken::getID(),1+rand()%2,true,Vector3(TestChicken->position.x,TestChicken->position.y+0.3f,TestChicken->position.z));
						mWorld->DropThis(Feather::getID(),rand() % 2,true,Vector3(TestChicken->position.x,TestChicken->position.y+0.3f,TestChicken->position.z));
                    }

                    for(int k = 0; k <= 7; k++)
                    {
                        Particle* Smoke = new Particle(mWorld, "smoke",Vector3(TestChicken->position.x-0.5+(rand()%11/10.0f),TestChicken->position.y+0.2+(rand()%4/5.0f),TestChicken->position.z-0.5+(rand()%11/10.0f)));
                        k % 2 == 0 ? Smoke->SetVelocity(Vector3(-3*rand()%2,0,-3*rand()%2)) : Smoke->SetVelocity(Vector3(3*rand()%2,0,3*rand()%2));
                        mParticles.push_back(Smoke);
                    }

                    mWorld->DestroyChicken(d);
					if(TestChicken->killed == true && TestChicken->lastKickType == 1)
					{
						mWorld->EXPcount += (2+rand()%5)*15/Acumulado;
						mSoundMgr->PlayOrbSound();
					}
                    d --;
                }
            }
        }
    }

    /// PIGS
    if(mWorld->mPigs.empty() == false)
    {
        for(int d = 0; d < mWorld->mPigs.size(); d++)
        {
            if(d < mWorld->mPigs.size() && mWorld->mPigs.empty() == false)
            {
                Pig* TestPig = mWorld->mPigs[d];

                if(menuState == 0)//game state
                {
                    TestPig->handItemId = mWorld->invId[27+barPosition];
                    TestPig->Update(mWorld, playerPosition, dt);
                }
                TestPig->Render(fppCam->mFrustum, dt);

                if(TestPig->playHurtSound)
                {
                    mSoundMgr->PlayPigHurtSound(TestPig->FastDistanceToPlayer());
                    TestPig->playHurtSound = false;
                }

                if(TestPig->playSaySound)
                {
                    mSoundMgr->PlayPigHurtSound(TestPig->FastDistanceToPlayer());
                    TestPig->playSaySound = false;
                }
				///PigDieSound///
				if(TestPig->killed == true)
                {
                    if(TestPig->playDieSound)
                    {
                        mSoundMgr->PlayPigDieSound(TestPig->FastDistanceToPlayer());
                        TestPig->playDieSound = false;
                    }
                }
				///end///
                if(TestPig->toDestroy == true || (TestPig->killed == true && TestPig->deathTimer < 0.0f))
                {
					
                    if(TestPig->killed == true)
                   {
                        mWorld->DropThis(RawPorkchop::getID(),1+rand() % 2,true,Vector3(TestPig->position.x,TestPig->position.y+0.3f,TestPig->position.z));
                    }

                    for(int k = 0; k <= 7; k++)
                    {
                        Particle* Smoke = new Particle(mWorld, "smoke",Vector3(TestPig->position.x-0.5+(rand()%11/10.0f),TestPig->position.y+0.2+(rand()%4/5.0f),TestPig->position.z-0.5+(rand()%11/10.0f)));
                        k % 2 == 0 ? Smoke->SetVelocity(Vector3(-3*rand()%2,0,-3*rand()%2)) : Smoke->SetVelocity(Vector3(3*rand()%2,0,3*rand()%2));
                        mParticles.push_back(Smoke);
                    }

                    mWorld->DestroyPig(d);
					if(TestPig->killed == true && TestPig->lastKickType == 1)
					{
						mWorld->EXPcount += (2+rand()%5)*15/Acumulado;
						mSoundMgr->PlayOrbSound();
					}
                    d --;
                }
            }
        }
    }

    /// SHEEP
    if(mWorld->mSheeps.empty() == false)
    {
        for(int d = 0; d < mWorld->mSheeps.size(); d++)
        {
            if(d < mWorld->mSheeps.size() && mWorld->mSheeps.empty() == false)
            {
                Sheep* TestSheep = mWorld->mSheeps[d];

                if(menuState == 0)//game state
                {
                    TestSheep->handItemId = mWorld->invId[27+barPosition];
                    TestSheep->Update(mWorld, playerPosition, dt);
                }
                TestSheep->Render(fppCam->mFrustum, dt);

                if(TestSheep->playHurtSound)
                {
                    mSoundMgr->PlaySheepHurtSound(TestSheep->FastDistanceToPlayer());
                    TestSheep->playHurtSound = false;
                }

                if(TestSheep->playSaySound)
                {
                    mSoundMgr->PlaySheepHurtSound(TestSheep->FastDistanceToPlayer());
                    TestSheep->playSaySound = false;
                }

                if(TestSheep->toDestroy == true || (TestSheep->killed == true && TestSheep->deathTimer < 0.0f))
                {
                    if(TestSheep->killed == true && TestSheep->sheared == false)
                    {
                        mWorld->DropThis(BlackWoolBlock::getID()+TestSheep->GetFurColor(),1,true,Vector3(TestSheep->position.x,TestSheep->position.y+0.3f,TestSheep->position.z));
						mWorld->DropThis(RawMutton::getID(),1+rand()%2,true,Vector3(TestSheep->position.x,TestSheep->position.y+0.3f,TestSheep->position.z));
                    }

                    for(int k = 0; k <= 7; k++)
                    {
                        Particle* Smoke = new Particle(mWorld, "smoke",Vector3(TestSheep->position.x-0.5+(rand()%11/10.0f),TestSheep->position.y+0.2+(rand()%4/5.0f),TestSheep->position.z-0.5+(rand()%11/10.0f)));
                        k % 2 == 0 ? Smoke->SetVelocity(Vector3(-3*rand()%2,0,-3*rand()%2)) : Smoke->SetVelocity(Vector3(3*rand()%2,0,3*rand()%2));
                        mParticles.push_back(Smoke);
                    }

                    mWorld->DestroySheep(d);
					if(TestSheep->killed == true && TestSheep->lastKickType == 1)
					{
						mWorld->EXPcount += (2+rand()%5)*15/Acumulado;
						mSoundMgr->PlayOrbSound();
					}
                    d --;
                }
            }
        }
    }

    /// CREEPER
    if(mWorld->mCreepers.empty() == false)
    {
        for(int d = 0; d < mWorld->mCreepers.size(); d++)
        {
            if(d < mWorld->mCreepers.size() && mWorld->mCreepers.empty() == false)
            {
                Creeper* TestCreeper = mWorld->mCreepers[d];

                if(menuState == 0)//game state
                {
                    TestCreeper->Update(mWorld, playerPosition, dt);
                }
                TestCreeper->Render(fppCam->mFrustum, dt);

                if(TestCreeper->playHurtSound)
                {
                    mSoundMgr->PlayCreeperHurtSound(TestCreeper->DistanceToPlayer());
                    TestCreeper->playHurtSound = false;
                }

                if(TestCreeper->playFuseSound)
                {
                    mSoundMgr->PlayFuseSound();
                    TestCreeper->playFuseSound = false;
                }

                if(TestCreeper->toDestroy == true || (TestCreeper->killed == true && TestCreeper->deathTimer < 0.0f))
                {
                    if(TestCreeper->killed == true)
                    {
                        if(TestCreeper->lastKickType != 2) // killed by player or other death
                        {
                            mWorld->DropThis(Gunpowder::getID(),1,true,Vector3(TestCreeper->position.x,TestCreeper->position.y+0.3f,TestCreeper->position.z));
                            mWorld->DropThis(Gunpowder::getID(),rand()%3,true,Vector3(TestCreeper->position.x,TestCreeper->position.y+0.3f,TestCreeper->position.z));
                        }
                        else
                        {
                            int drop_id = 1;
                            int disk_number = rand()%12;
                            if(disk_number <= 2)
                            {
                                drop_id = Disk1::getID() + disk_number;
                            }
                            else
                            {
                                drop_id = Disk4::getID() + disk_number-3;
                            }
                            mWorld->DropThis(drop_id,1,false,Vector3(TestCreeper->position.x,TestCreeper->position.y+0.3f,TestCreeper->position.z));
                        }
                    }

                    for(int k = 0; k <= 7; k++)
                    {
                        Particle* Smoke = new Particle(mWorld, "smoke",Vector3(TestCreeper->position.x-0.5+(rand()%11/10.0f),TestCreeper->position.y+0.2+(rand()%4/5.0f),TestCreeper->position.z-0.5+(rand()%11/10.0f)));
                        k % 2 == 0 ? Smoke->SetVelocity(Vector3(-3*rand()%2,0,-3*rand()%2)) : Smoke->SetVelocity(Vector3(3*rand()%2,0,3*rand()%2));
                        mParticles.push_back(Smoke);
                    }

                    mWorld->DestroyCreeper(d);
					if(TestCreeper->killed == true && TestCreeper->lastKickType == 1)
					{
						mWorld->EXPcount += (2+rand()%5)*15/Acumulado;
						mSoundMgr->PlayOrbSound();
					}
                    d --;
                }
                if(TestCreeper->exploded == true)
                {
                    for(float verAngle = 0.0f; verAngle <= PI; verAngle += PI/4.0f)
                    {
                        for(float horAngle = 0.0f; horAngle <= PI*2; horAngle += PI/2.0f)
                        {
                            if(mParticles.size() > 52)
                            {
                                continue;
                            }
                            float radius = 4.5f + (rand() % 100)/10.0f;

                            Particle* Smoke = new Particle(mWorld,"smoke",Vector3(TestCreeper->position.x,TestCreeper->position.y,TestCreeper->position.z));
                            Smoke->SetVelocity(Vector3(mWorld->LengthDirX(horAngle,verAngle)*radius,mWorld->LengthDirY(horAngle,verAngle)*radius,mWorld->LengthDirZ(horAngle,verAngle)*radius));
                            Smoke->SetScale(0.3f+(rand()%25)/100.0f);
                            Smoke->friction = 4.25f;

                            mParticles.push_back(Smoke);
                        }
                    }
                    if(mParticles.size() < 64)
                    {
                        Particle* Explosion = new Particle(mWorld,"explosion",Vector3(TestCreeper->position.x-1.2f,TestCreeper->position.y-1.2f,TestCreeper->position.z));
                        Particle* Explosion2 = new Particle(mWorld,"explosion",Vector3(TestCreeper->position.x+1.2f,TestCreeper->position.y+1.2f,TestCreeper->position.z));
                        Particle* Explosion3 = new Particle(mWorld,"explosion",Vector3(TestCreeper->position.x,TestCreeper->position.y-1.2f,TestCreeper->position.z-1.2f));
                        Particle* Explosion4 = new Particle(mWorld,"explosion",Vector3(TestCreeper->position.x,TestCreeper->position.y+1.2f,TestCreeper->position.z+1.2f));
                        Particle* Explosion5 = new Particle(mWorld,"explosion",Vector3(TestCreeper->position.x,TestCreeper->position.y,TestCreeper->position.z));
                        mParticles.push_back(Explosion);
                        mParticles.push_back(Explosion2);
                        mParticles.push_back(Explosion3);
                        mParticles.push_back(Explosion4);
                        mParticles.push_back(Explosion5);
                    }

                    mSoundMgr->PlayTNTSound();

                    mWorld->DestroyCreeper(d);
                    d --;
                }
            }
        }
    }

    /// render snowballs
    if (mSnowBalls.empty() == false)
    {
        for(unsigned int i = 0; i < mSnowBalls.size(); i++)
        {
            if(i < mSnowBalls.size() && mSnowBalls.empty() == false)
            {
                TextureManager::Instance()->SetTextureModeulate(snowBall4);
                SnowBall2* UseSnowBall = mSnowBalls[i];

                if(menuState == 0)//game state
                {
                    UseSnowBall->Update(mWorld, dt);
                }

                MatrixPush();
                MatrixTranslation(Vector3(0,0,0));
                DrawSetDepthTest(true);
                DrawSetAlphaTest(true);

                UseSnowBall->Render();

                DrawSetAlphaTest(false);
                DrawSetDepthTest(false);
                MatrixPop();

                if(UseSnowBall->CheckCollision(mWorld) == true)
                {
                    if(mParticles.size() <= 52)
                    {
                        for(int k = 0; k <= 3; k++) // 4 particles
                        {
                            Particle* SnowballPart = new Particle(mWorld,"item crack",Vector3(UseSnowBall->position.x,UseSnowBall->position.y+0.1f,UseSnowBall->position.z));
                            SnowballPart->var1 = SnowBallItem::getID()-250;
                            SnowballPart->SetScale(0.06f, 0.12f);
                            mParticles.push_back(SnowballPart);
                        }
                    }

                    delete mSnowBalls[i];
                    mSnowBalls.erase(mSnowBalls.begin()+i);
                    i --;
                }
            }
        }
    }
	
	/// render arrows
    if (mArrows.empty() == false)
    {
        for(unsigned int i = 0; i < mArrows.size(); i++)
        {
            if(i < mArrows.size() && mArrows.empty() == false)
            {
                TextureManager::Instance()->SetTextureModeulate(snowBall4);
                Arrow2* UseArrow = mArrows[i];

                if(menuState == 0)//game state
                {
                    UseArrow->Update(mWorld, dt);
                }

                MatrixPush();
                MatrixTranslation(Vector3(0,0,0));
                DrawSetDepthTest(true);
                DrawSetAlphaTest(true);

                UseArrow->Render();

                DrawSetAlphaTest(false);
                DrawSetDepthTest(false);
                MatrixPop();

                if(UseArrow->CheckCollision(mWorld) == true)
                {

					mWorld->DropThis(Arrow::getID(),1,true,Vector3(UseArrow->position.x,UseArrow->position.y+0.3f,UseArrow->position.z));

                    delete mArrows[i];
                    mArrows.erase(mArrows.begin()+i);
                    i --;
                }
            }
        }
    }

    /// render particles
    if (mParticles.empty() == false)
    {
        for(unsigned int i = 0; i < mParticles.size(); i++)
        {
            if(i < mParticles.size() && mParticles.empty() == false)
            {
                Particle* TestParticle = mParticles[i];
                if(menuState == 0)//game state
                {
                    TestParticle->Update(mWorld, dt);
                }

                TestParticle->Render(fppCam->mFrustum);

                if(TestParticle->toDestroy == true)
                {
                    delete mParticles[i];
                    mParticles.erase(mParticles.begin()+i);
                    i --;
                }
            }
        }
    }

    /// render items in item frames
    if (mWorld->mItemFrames.empty() == false)
    {
        for(std::vector<ItemFrameEntity*>::size_type i = 0; i < mWorld->mItemFrames.size(); i++)
        {
            ItemFrameEntity* TestItemFrame = mWorld->mItemFrames[i];

            if(TestItemFrame->GetItem() != -1)
            {
                TestItemFrame->Render(mWorld,fppCam->mFrustum,dt);
            }
        }
    }

    if(mWorld->mainOptions.fogRendering == 1)
    {
        sceGuDisable(GU_FOG); // disable fog
    }

    /// weather effects rendering
    if(menuState == 0 || makeScreen == true)
    {
        if(mWorld->worldWeather->GetWeatherType() == 1) // if it is rain
        {
            if(mWorld->rainyColorAlpha < 1.0f)
            {
                mWorld->rainyColorAlpha += dt/3.0f;
            }
            if(mWorld->rainyColorAlpha > 1.0f)
            {
                mWorld->rainyColorAlpha = 1.0f;
            }
			
			/*tormenta -= dt;
			if(tormenta < 0.0f)
			{
				tormenta = 20+rand()%30/1.0f;
				mSoundMgr->PlayThunderSound();
			}*/

            rainSoundFrequency -= dt;
            if(rainSoundFrequency <= 0.0f)
            {
                int raindropsCount = mWorld->worldWeather->raindropsCount;
                if(raindropsCount > 18)
                {
                    raindropsCount = 18;
                }
                if(raindropsCount > 0)
                {
                    mSoundMgr->PlayRainSound((raindropsCount/20.0f)*45);
                }
                rainSoundFrequency = 1.2 + (rand()%6)/10.0f;
            }
        }
        else // clear weather
        {
            if(mWorld->rainyColorAlpha > 0.0f)
            {
                mWorld->rainyColorAlpha -= dt/8.0f;
            }
            if(mWorld->rainyColorAlpha < 0.0f)
            {
                mWorld->rainyColorAlpha = 0.0f;
            }
        }
        mWorld->worldWeather->Update(mWorld, dt);
        mWorld->worldWeather->Render(mWorld, fppCam->mFrustum, fppCam->upDownAngle, playerPosition, dt);
    }

    /// render destroy model
    if (((startDt == 1 && dStd >= 0 && dStd <= 9) || showCube == true) && makeScreen == false && sleepTime <= 0.0f)
	{
		MatrixPush();

        DrawSetDepthTest(true);
        DrawSetAlphaTest(true);
		DrawSetBlend(true);

		sceGuDepthMask(1);

		MatrixTranslation(Vector3(cubePos.x,cubePos.y,cubePos.z));
		MatrixColor(0xFFFFFFFF);

        sceGuTexFilter(GU_NEAREST,GU_NEAREST);
        TextureManager::Instance()->SetTextureModeulate(texture);

		destroyer->Update(dStd, mWorld, mWorld->GetBlock(cubePos.x,cubePos.y,cubePos.z));
		destroyer->Render(dStd);

		sceGuDepthMask(0);

		DrawSetBlend(false);
        DrawSetAlphaTest(false);
        DrawSetDepthTest(false);

		MatrixPop();
	}

	if(mWorld->mainOptions.guiDrawing == 1 && mWorld->HP > 0)
	{
		float cubeBob = sinf(bobCycle - (3.14/2) + 3.14)/16; // some animtaion vars
		float cubeBob2 = cosf(bobCycle - (3.14/2) + 3.14)/18;

        sceGuColor(GU_COLOR(cubeLight,cubeLight,cubeLight,1)); // set color to drawing model
        if(sleepTime <= 0.0f)
        {
            if(mWorld->invId[27+barPosition] == -1) // slot is empty so we need to draw steve's hand
            {
                TextureManager::Instance()->SetTextureModeulate(invPlayerTex);
                sceGumPushMatrix();

                //set view matrix to identity
                sceGumMatrixMode(GU_VIEW);
                sceGumLoadIdentity();
				
				if(headInWater == true)
				{
					//translate
					ScePspFVector3 move = {0.53f+cubeBob+shift_x+(mWorld->mainOptions.fov-70)/200.0f+sinf(animDest)*-0.35,-0.42f+shift_y+cubeBob2+changeY,-0.5f+(mWorld->mainOptions.fov-60)/130.0f}; //446
					sceGumTranslate(&move);
				}
				else
				{
					//translate
					ScePspFVector3 move = {0.53f+cubeBob+shift_x+(mWorld->mainOptions.fov-70)/200.0f+sinf(animDest)*-0.35,-0.42f+shift_y+cubeBob2+changeY,-0.5f+(mWorld->mainOptions.fov-70)/130.0f}; //446
					sceGumTranslate(&move);
				}
				
                //rotate
                sceGumRotateX(-0.72f+sinf(animDest)*-1.5);//0.1
                sceGumRotateY(3.4839f+sinf(animDest)*-0.3); //0.9
                sceGumRotateZ(0.57f); //0.9
                //scale
                ScePspFVector3 scale = {0.6f,0.6f,0.6f};
                sceGumScale(&scale);
                //draw hand
                sceGuDisable(GU_DEPTH_TEST);
                sceGuDepthMask(1);
                mWorld->drawHand();
                sceGuEnable(GU_DEPTH_TEST);
                sceGuDepthMask(0);

                sceGumMatrixMode(GU_MODEL);
                sceGumPopMatrix();
            }

            if(mWorld->invId[27+barPosition] < 250 && mWorld->invId[27+barPosition] != -1)
            {
                TextureManager::Instance()->SetTextureModeulate(texture);
                sceGumPushMatrix();

                //set view matrix to identity
                sceGumMatrixMode(GU_VIEW);
                sceGumLoadIdentity();
				
				//translate
				ScePspFVector3 move = {0.5f + (cubeBob + shift_x) + sinf(animDest)*-0.35, -0.45f + (cubeBob2 + shift_y + changeY + idle_y), -0.725f}; //446
				sceGumTranslate(&move);

                //rotate
                sceGumRotateX(0.0+sinf(animDest)*-1.1);
                sceGumRotateY(45+sinf(animDest)*-0.3); //3
                //scale
                ScePspFVector3 scale = {0.38f,0.38f,0.38f};
                sceGumScale(&scale);
                //draw cube
                sceGuDisable(GU_DEPTH_TEST);
                sceGuDepthMask(1);
                mWorld->drawCubes(mWorld->invId[27+barPosition],cubeLight);
                sceGuEnable(GU_DEPTH_TEST);
                sceGuDepthMask(0);

                sceGumMatrixMode(GU_MODEL);
                sceGumPopMatrix();
            }
			
            if((mWorld->invId[27+barPosition] >= 250 && mWorld->invId[27+barPosition] <= 254) || mWorld->invId[27+barPosition] >= 260)	//items
            {
                mWorld->ItemHaveTerrainTexture(mWorld->invId[27+barPosition]) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);
                sceGumPushMatrix();

                //set view matrix to identity
                sceGumMatrixMode(GU_VIEW);
                sceGumLoadIdentity();
				
				if(headInWater == true)
				{
					//translate
					ScePspFVector3 move = {0.523f+cubeBob+shift_x+(mWorld->mainOptions.fov-70)/250.0f+sinf(animDest)*-0.25,-0.24f+cubeBob2+idle_y+shift_y+changeY,-0.6f+(mWorld->mainOptions.fov-60)/180.0f};//-0.17
					sceGumTranslate(&move); //-0.22
				}
				else
				{
					//translate
					ScePspFVector3 move = {0.523f+cubeBob+shift_x+(mWorld->mainOptions.fov-65)/250.0f+sinf(animDest)*-0.25,-0.24f+cubeBob2+idle_y+shift_y+changeY,-0.6f+(mWorld->mainOptions.fov-65)/180.0f};//-0.17
					sceGumTranslate(&move); //-0.22
				}
				
                //rotate
                sceGumRotateX(-0.48f);
                sceGumRotateY(-1.47+sinf(animDest)*-0.3+PI); //-1.47
                sceGumRotateZ(-3.13-sinf(animDest)*1.4); //-3.8
                //scale					
				//ScePspFVector3 scale = {0.465f,0.465f,0.465f};
				ScePspFVector3 scale = {0.5f,0.5f,0.5f};
				sceGumScale(&scale);

                //draw item
                sceGuDisable(GU_DEPTH_TEST);
                sceGuDepthMask(1);
                mWorld->drawHandItems(mWorld->invId[27+barPosition],cubeLight);
                sceGuEnable(GU_DEPTH_TEST);
                sceGuDepthMask(0);

                sceGumMatrixMode(GU_MODEL);
                sceGumPopMatrix();
            }
			
			//espadas grandes :D
			if(mWorld->invId[27+barPosition] >= 255 && mWorld->invId[27+barPosition] <= 259)
            {
                mWorld->ItemHaveTerrainTexture(mWorld->invId[27+barPosition]) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);
                sceGumPushMatrix();

                //set view matrix to identity
                sceGumMatrixMode(GU_VIEW);
                sceGumLoadIdentity();

                if(headInWater == true)
				{
					//translate
					ScePspFVector3 move = {0.523f+cubeBob+shift_x+(mWorld->mainOptions.fov-70)/250.0f+sinf(animDest)*-0.25,-0.24f+cubeBob2+idle_y+shift_y+changeY,-0.6f+(mWorld->mainOptions.fov-60)/180.0f};//-0.17
					sceGumTranslate(&move); //-0.22
				}
				else
				{
					//translate
					ScePspFVector3 move = {0.523f+cubeBob+shift_x+(mWorld->mainOptions.fov-65)/250.0f+sinf(animDest)*-0.25,-0.24f+cubeBob2+idle_y+shift_y+changeY,-0.6f+(mWorld->mainOptions.fov-65)/180.0f};//-0.17
					sceGumTranslate(&move); //-0.22
				}
                //rotate
                sceGumRotateX(-0.48f);
                sceGumRotateY(-1.47+sinf(animDest)*-0.3+PI); //-1.47
                sceGumRotateZ(-3.13-sinf(animDest)*1.4); //-3.8
                //scale					
				ScePspFVector3 scale = {0.55f,0.55f,0.55f};
				sceGumScale(&scale);

                //draw item
                sceGuDisable(GU_DEPTH_TEST);
                sceGuDepthMask(1);
                mWorld->drawHandItems(mWorld->invId[27+barPosition],cubeLight);
                sceGuEnable(GU_DEPTH_TEST);
                sceGuDepthMask(0);

                sceGumMatrixMode(GU_MODEL);
                sceGumPopMatrix();
            }
        }
		MatrixColor(0xFFFFFFFF);
	}
	MatrixColor(0xFFFFFFFF);

	/// GUI
	mRender->SetOrtho(0,0,0,0,0,0);

    if(mWorld->HP <= 0)
	{
        DrawSetDepthTest(false);
        DrawSetDepthMask(true);
        DrawSetBlend(true);
        DrawPlaceTexture(true);

		MatrixColor(GU_COLOR(1,1,1,0.5f));

		TextureManager::Instance()->SetTextureModeulate(red);
		advancedBlit(0,0,SCR_WIDTH,SCR_HEIGHT,0,0,32);

        DrawPlaceTexture(false);
        DrawSetBlend(false);
        DrawSetDepthMask(false);
        DrawSetDepthTest(true);
	}

    if(hurt == true)
	{
        DrawSetDepthTest(false);
        DrawSetDepthMask(true);
        DrawSetBlend(true);
        DrawPlaceTexture(true);

        MatrixColor(GU_COLOR(1,1,1,hurt_time));

		TextureManager::Instance()->SetTextureModeulate(red);
		advancedBlit(0,0,SCR_WIDTH,SCR_HEIGHT,0,0,32);

        DrawPlaceTexture(false);
        DrawSetBlend(false);
        DrawSetDepthMask(false);
        DrawSetDepthTest(true);
	}

	if(headInLava == 1 || footInLava == 1)
	{
        DrawSetDepthTest(false);
        DrawSetDepthMask(true);
        DrawSetBlend(true);
        DrawPlaceTexture(true);

        MatrixColor(GU_COLOR(1,1,1,0.7));

		TextureManager::Instance()->SetTextureModeulate(red);
		advancedBlit(0,0,SCR_WIDTH,SCR_HEIGHT,0,0,32);

        DrawPlaceTexture(false);
        DrawSetBlend(false);
        DrawSetDepthMask(false);
        DrawSetDepthTest(true);
	}
	// draw dark mask
    if((invEn == 1 || craft3xEn == 1 || chestEn == 1 || menuState != 0 || furnaceEn == 1) && makeScreen == false)
    {
        DrawSetDepthTest(false);
        DrawSetDepthMask(true);
        DrawSetBlend(true);
        DrawPlaceTexture(true);

        MatrixColor(GU_COLOR(1,1,1,0.7));

        TextureManager::Instance()->SetTextureModeulate(black);
        advancedBlit(0,0,SCR_WIDTH,SCR_HEIGHT,0,0,32);

        DrawPlaceTexture(false);
        DrawSetBlend(false);
        DrawSetDepthMask(false);
        DrawSetDepthTest(true);
    }

    /*if(mWorld->rainyColorAlpha != 0.0f) // dark mask again
    {
        DrawSetDepthTest(false);
        DrawSetDepthMask(true);
        DrawSetBlend(true);
        DrawPlaceTexture(true);

        MatrixColor(GU_COLOR(1,1,1,mWorld->rainyColorAlpha/9.0f));

        TextureManager::Instance()->SetTextureModeulate(black);
        advancedBlit(0,0,SCR_WIDTH,SCR_HEIGHT,0,0,32);

        DrawPlaceTexture(false);
        DrawSetBlend(false);
        DrawSetDepthMask(false);
        DrawSetDepthTest(true);
    }*/

    if(sleepTime > 0.0f) // dark mask again
    {
        DrawSetDepthTest(false);
        DrawSetDepthMask(true);
        DrawSetBlend(true);
        DrawPlaceTexture(true);

        MatrixColor(GU_COLOR(1,1,1,(4.7-sleepTime)/4.8f));

        TextureManager::Instance()->SetTextureModeulate(black);
        advancedBlit(0,0,SCR_WIDTH,SCR_HEIGHT,0,0,32);

        DrawPlaceTexture(false);
        DrawSetBlend(false);
        DrawSetDepthMask(false);
        DrawSetDepthTest(true);
    }

    /*if(mWorld->BlockSolid(playerPosition.x,playerPosition.y,playerPosition.z))  // dark mask again
    {
        DrawSetDepthTest(false);
        DrawSetDepthMask(true);
        DrawSetBlend(true);
        DrawPlaceTexture(true);

        MatrixColor(GU_COLOR(1,1,1,0.9));

        TextureManager::Instance()->SetTextureModeulate(black);
        advancedBlit(0,0,SCR_WIDTH,SCR_HEIGHT,0,0,32);

        DrawPlaceTexture(false);
        DrawSetBlend(false);
        DrawSetDepthMask(false);
        DrawSetDepthTest(true);
    }*/

    // draw water mask
	if(headInWater)
	{
        if (GRAVITY != -4.0f)
        {
            GRAVITY = -4.0f;
        }

        DrawSetDepthTest(false);
        DrawSetDepthMask(true);
        DrawSetBlend(true);
        DrawPlaceTexture(true);

        MatrixColor(GU_COLOR(1,1,1,0.8));

		waterScreen->Draw();

        DrawPlaceTexture(false);
        DrawSetBlend(false);
        DrawSetDepthMask(false);
        DrawSetDepthTest(true);
	}
	else
    {
        if (GRAVITY != -6.8f)
        {
            GRAVITY = -6.8f;
        }
    }
    // draw pumpkin mask
    if(mWorld->armorId[0] == Pumpkin3::getID())
    {
        DrawSetDepthTest(false);
        DrawSetDepthMask(true);
        DrawSetBlend(true);
        DrawPlaceTexture(true);
     //   sceGuTexFilter( GU_LINEAR, GU_LINEAR );

        MatrixColor(GU_COLOR(1,1,1,1));

		pumpkinMask->Draw();

       // sceGuTexFilter( GU_NEAREST, GU_NEAREST );
        DrawPlaceTexture(false);
        DrawSetBlend(false);
        DrawSetDepthMask(false);
        DrawSetDepthTest(true);
    }

	/// GUI
	DrawSetDepthTest(false);
	DrawSetDepthMask(true);

	DrawSetBlend(true);
	MatrixColor(GU_COLOR(1,1,1,1.0f));

	if ((invEn == false && craft3xEn == false && chestEn == false && furnaceEn == false && menuState == 0 && mWorld->mainOptions.guiDrawing == 1) || (makeScreen == true && mWorld->mainOptions.guiDrawing == 1))
    {
        hpAnim += dt * 5.9 * PI;
        if(hpAnim >= 10 * PI)
        {
            hpAnim = 0.0f;
        }
		
		XPbarSprite->Draw();
		barSprite->Draw();
		
		int EXPbarra = mWorld->EXPcount;
		XPprogressSprite[EXPbarra]->Draw();
		
        if (menuState != 1 || makeScreen == true)
        {
            /// HP and HG RENDERING
            // draw cells
            hpCellSpriteW->SetPosition(190,206);	//193
            hpCellSprite->SetPosition(190,206);
			
			if(HungerPoisoned == true)
			{
				hg2CellSprite->SetPosition(291,206);
			}
			else
			{
				hgCellSprite->SetPosition(291,206);
			}
			
            arCellSprite->SetPosition(190,195);
            if(hurt == true && sinf(hpAnim) > 0)
            {
                hpCellSpriteW->Draw();
            }
            if(hurt == false || sinf(hpAnim) <= 0)
            {
                hpCellSprite->Draw();
            }
			
			if(HungerPoisoned == true)
			{
				hg2CellSprite->Draw();
			}
			else
			{
				hgCellSprite->Draw();
			}
			
            if(mWorld->AP > 0)
            {
                arCellSprite->Draw();
            }

            for(int i = 1; i <= 10; i++)
            {
                hpSprite->SetPosition(146+i*8,206);
                hpSpriteW->SetPosition(146+i*8,206);
                hpHalfSprite->SetPosition(146+i*8,206);
                hpHalfSpriteW->SetPosition(146+i*8,206);
				
                hpVenenoSprite->SetPosition(146+i*8,206);
                hpVenenoSpriteW->SetPosition(146+i*8,206);
                hpVenenoHalfSprite->SetPosition(146+i*8,206);
                hpVenenoHalfSpriteW->SetPosition(146+i*8,206);

                hpHardSprite->SetPosition(146+i*8,206);
                hpHardHalfSprite->SetPosition(146+i*8,206);
                hpHardSpriteW->SetPosition(146+i*8,206);
                hpHardHalfSpriteW->SetPosition(146+i*8,206);

                if(hurt == true && sinf(hpAnim) > 0)
                {
                    if(last_HP > mWorld->HP)
                    {
                        if(i*2 >= floorf(mWorld->HP/2.0f)*2)
                        {
                            if(i * 2 <= ceilf(last_HP))
                            {
                                if(mWorld->gameModeWorld == 2) // hardcore mode
                                {
									if(HPPoisoned == true)
									{
										hpVenenoSpriteW->Draw();
									}
									else
									{
										hpHardSpriteW->Draw();
									}
                                }
                                else
                                {
									if(HPPoisoned == true)
									{
										hpVenenoSpriteW->Draw();
									}
									else
									{
										hpSpriteW->Draw();
									}
                                }
                            }
                            if(i * 2 - ceilf(last_HP) == 1)
                            {
                                if(mWorld->gameModeWorld == 2) // hardcore mode
                                {
									if(HPPoisoned == true)
									{
										hpVenenoHalfSpriteW->Draw();
									}
									else
									{
										hpHardHalfSpriteW->Draw();
									}
                                }
                                else
                                {
									if(HPPoisoned == true)
									{
										hpVenenoHalfSpriteW->Draw();
									}
									else
									{
										hpHalfSpriteW->Draw();
									}
                                }
                            }
                        }
                    }
                }

                if(i * 2 <= ceilf(mWorld->HP) && mWorld->HP >= 0)
                {
                    if(mWorld->gameModeWorld == 2) // hardcore mode
                    {
						if(HPPoisoned == true)
						{
							hpVenenoSprite->Draw();
						}
						else
						{
							hpHardSprite->Draw();
						}
                    }
                    else
                    {
						if(HPPoisoned == true)
						{
							hpVenenoSprite->Draw();
						}
						else
						{
							hpSprite->Draw();
						}
                    }
                }
                if(i * 2 - ceilf(mWorld->HP) == 1 && mWorld->HP >= 0)
                {
                    if(mWorld->gameModeWorld == 2) // hardcore mode
                    {
						if(HPPoisoned == true)
						{
							hpVenenoHalfSprite->Draw();
						}
						else
						{
							hpHardHalfSprite->Draw();
						}
                    }
                    else
                    {
						if(HPPoisoned == true)
						{
							hpVenenoHalfSprite->Draw();
						}
						else
						{
							hpHalfSprite->Draw();
						}
                    }
                }


				if(HungerPoisoned == true)
				{
					hg2Sprite->SetPosition(335-i*8,206);
					hg2HalfSprite->SetPosition(335-i*8,206);
				}
				else
				{
					hgSprite->SetPosition(335-i*8,206);
					hgHalfSprite->SetPosition(335-i*8,206);
				}

                if(i * 2 <= ceilf(mWorld->HG))
                {
					if(HungerPoisoned == true)
					{
						hg2Sprite->Draw();
					}
					else
					{
						hgSprite->Draw();
					}
                }
                if(i * 2 - ceilf(mWorld->HG) == 1)
                {
					if(HungerPoisoned == true)
					{
						hg2HalfSprite->Draw();
					}
					else
					{
						hgHalfSprite->Draw();
					}
                }

                arSprite->SetPosition(146+i*8,195);
                arHalfSprite->SetPosition(146+i*8,195);

                if(mWorld->AP > 0)
                {
                    if(i * 2 <= ceilf(mWorld->AP))
                    {
                        arSprite->Draw();
                    }
                    if(i * 2 - ceilf(mWorld->AP) == 1)
                    {
                        arHalfSprite->Draw();
                    }
                }

                if(headInWater)
                {
                    bubbleSprite->SetPosition(335-i*8,195);
                    if(i <= mWorld->OS)
                    {
                        bubbleSprite->Draw();
                    }
                }
            }

            if(mWorld->HP > 0)
            {
                DrawSetInverseColors();
                crossSprite->Draw();
                DrawSetDefaultColors();
            }

            selectSprite->Draw();
			
			/// BUTTONS
			if(mWorld->mainOptions.buttonsDrawing)
			{
				lSprite->SetPosition(10,260);
				lSprite->Draw();
				rSprite->SetPosition(40,260);
				rSprite->Draw();
				
				l2Sprite->SetPosition(175,258);
				l2Sprite->Draw();
				
				if(mWorld->invId[27+barPosition] != -1)
				{
					if(mWorld->invId[27+barPosition] < 250)
					{
						if(showCube == true)
						{
							r2Sprite->SetPosition(250,258);
							r2Sprite->Draw();
						}
					}
				}
				
				mRender->SetFontStyle(default_size,GU_COLOR(1,1,1,1),GU_COLOR(0.0f,0.0f,0.0f,1),0x00000000);
				mRender->DebugPrint(30,274,"+");
				mRender->DebugPrint(60,272,"inventory");
				
				if(mWorld->invId[27+barPosition] >= 255 && mWorld->invId[27+barPosition] <= 259)
				{
					mRender->DebugPrint(193,272,"mine & attack");
				}
				else
				{
					mRender->DebugPrint(193,272,"mine");
				}
				if(mWorld->invId[27+barPosition] != -1)
				{
					if(mWorld->invId[27+barPosition] < 250)
					{
						if(showCube == true)
						{
							mRender->DebugPrint(267,272,"place");
						}
					}
				}
			}
        }
    }
    else
    {
        if (invEn == true)
        {
            invSprite->Draw();
        }
        if (craft3xEn == true)
        {
            crtSprite->Draw();
        }
        if (chestEn == true)
        {
            chtSprite->Draw();

        }
        if (furnaceEn == true)
        {
            furSprite->Draw();

            int furFireStage = 0;

            if(UseFurnace->fuelTime >= 10.0f)
            {
                furFireStage = 13;
            }
            if(UseFurnace->fuelTime < 10.0f)
            {
                furFireStage = ((int)(UseFurnace->fuelTime/10.0f*14.0f));
            }

            if(UseFurnace->fuelTime != 0.0f)
            {
                if(furFireStage >= 0 && furFireStage <= 13)
                {
                    furFireSprite[furFireStage]->SetPosition(190,50-((furFireStage-13)*2));
                    furFireSprite[furFireStage]->Draw();
                }
                if(furFireStage == 14)
                {
                    furFireSprite[13]->SetPosition(190,50-((13-4)*6));
                    furFireSprite[13]->Draw();
                }
            }

            if(UseFurnace->meltingTime != 0.0f)
            {
                int furArrowStage = 0;
                furArrowStage = ((int)(UseFurnace->meltingTime/10.0f*22.0f));

                if(furArrowStage >= 0 && furArrowStage <= 21)
                {
                    furArrowSprite[furArrowStage]->SetPosition(236,46);
                    furArrowSprite[furArrowStage]->Draw();
                }
                if(furArrowStage == 22)
                {
                    furArrowSprite[21]->SetPosition(236,46);
                    furArrowSprite[21]->Draw();
                }
            }
        }
    }
	DrawSetBlend(false);

    /// 3D BLOCKS AND ITEMS ON 2D PANEL //ui
    if ((invEn == false && craft3xEn == false && chestEn == false && menuState == 0 && mWorld->mainOptions.guiDrawing == 1 && furnaceEn == false) || (makeScreen == true && mWorld->mainOptions.guiDrawing == 1))
    {
        for(int k = 0; k <= 8; k++)
        {
            if(mWorld->invId[27+k] != -1)
            {
                MatrixPush();

                if(mWorld->invId[27+k] < 250)
                {
                    MatrixTranslation(Vector3(160+k*20,230,0));
                    TextureManager::Instance()->SetTextureModeulate(texture);

                    sceGuDisable(GU_DEPTH_TEST);
                    DrawSetDepthMask(true);

                    if(k == slotForChangeScale)
                    {
                        MatrixScale(Vector3(10-changeScale*5,10+changeScale*10,10-changeScale*5));

                        changeScale -= dt*3.4f;
                        if(changeScale < 0.0f)
                        {
                            changeScale = 0.0f;
                            slotForChangeScale = -1;
                        }
                    }
                    else
                    {
                        MatrixScale(Vector3(10,10,10));
                    }

                    MatrixAngle(Vector3(150,44.6f,0));

                    DrawSetCulling(true);
                    mWorld->drawCubes(mWorld->invId[27+k],1.0f);
                    DrawSetCulling(false);

                    DrawSetDepthMask(false);
                    sceGuEnable(GU_DEPTH_TEST);
                }

                if(mWorld->invId[27+k] >= 250)
                {
                    MatrixTranslation(Vector3(160+k*20,230,0));
                    mWorld->ItemHaveTerrainTexture(mWorld->invId[27+k]) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);

                    MatrixAngle(Vector3(0,0,0));
                    if(k == slotForChangeScale)
                    {
                        MatrixScale(Vector3(16-changeScale*5,16+changeScale*10,16-changeScale*5));

                        changeScale -= dt*3.4f;
                        if(changeScale < 0.0f)
                        {
                            changeScale = 0.0f;
                            slotForChangeScale = -1;
                        }
                    }
                    else
                    {
                        MatrixScale(Vector3(16,16,16));
                    }

                    DrawSetCulling(true);
                    mWorld->drawItems(mWorld->invId[27+k]);
                    DrawSetCulling(false);
                }
                MatrixPop();
            }
        }
    }

    /// INVENTORY CRAFT MENU
    if(invEn == true)
    {
        for(int i = 0; i <= 1; i++)
        {
            for(int j = 0; j <= 1; j++)
            {
                if(craftSlotId[i*2+j] != -1)
                {
                    MatrixPush();

                    MatrixTranslation(Vector3(248+j*18,87+i*18,0.0f));
					

                    if(craftSlotId[i*2+j] < 250)
                    {
                        TextureManager::Instance()->SetTextureModeulate(texture);
						MatrixScale(Vector3(0.55,0.55,0.55));

                        mWorld->drawHudCubes(craftSlotId[i*2+j]);
                    }

                    if(craftSlotId[i*2+j] >= 250)
                    {
                        mWorld->ItemHaveTerrainTexture(craftSlotId[i*2+j]) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);
						MatrixScale(Vector3(0.5,0.5,0.5));

                        mWorld->drawHudItems(craftSlotId[i*2+j]);
                    }
                    MatrixPop();
                }
            }
        }

        for(int i = 0; i <= 1; i++)
        {
            for(int j = 0; j <= 1; j++)
            {
                if(mWorld->armorId[i*2+j] != -1)
                {
                    MatrixPush();

                    MatrixTranslation(Vector3(96+j*108,26+i*72,0.0f));

                    if(mWorld->armorId[i*2+j] < 250)
                    {
                        TextureManager::Instance()->SetTextureModeulate(texture);

                        mWorld->drawHudCubes(mWorld->armorId[i*2+j]);
                    }

                    if(mWorld->armorId[i*2+j] >= 250)
                    {
                        mWorld->ItemHaveTerrainTexture(mWorld->armorId[i*2+j]) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);

                        mWorld->drawHudItems(mWorld->armorId[i*2+j]);
                    }
                    MatrixPop();
                }
            }
        }

        //End

        //Item which we are crafting //probable
        if (craftItemId != -1)
        {
            MatrixPush();

            MatrixTranslation(Vector3(384,66,0.0f));

            if(craftItemId < 250)
            {
                TextureManager::Instance()->SetTextureModeulate(texture);
				MatrixScale(Vector3(0.55,0.55,0.55));
                mWorld->drawHudCubes(craftItemId);
            }

            if(craftItemId >= 250)
            {
                mWorld->ItemHaveTerrainTexture(craftItemId) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);
				MatrixScale(Vector3(0.5,0.5,0.5));
                mWorld->drawHudItems(craftItemId);
            }
            MatrixPop();
        }

        if(upEn == false)
        {
            if(invSteveAngle < invXPosition/8.0f*48)
            {
                invSteveAngle += (invXPosition/8.0f*48-invSteveAngle)*0.05;
            }
            if(invSteveAngle > invXPosition/8.0f*48)
            {
                invSteveAngle -= (invSteveAngle-invXPosition/8.0f*48)*0.05;
            }
        }

        MatrixPush();

        MatrixTranslation(Vector3(205,93,-14));
        //MatrixScale(Vector3(45,45,45));
		MatrixScale(Vector3(29,29,29));
        MatrixAngle(Vector3(-14,invSteveAngle,180));// 24

        TextureManager::Instance()->SetTextureModeulate(invPlayerTex);

        invPlayer->mainAngle = 0.45f;

        DrawSetDepthMask(false);
        DrawSetCulling(true);
        invPlayer->Render(mWorld, dt, invSteveAngle, invPlayerTex, mWorld->invId[27+barPosition]);
        DrawSetCulling(false);
        DrawSetDepthMask(true);

        MatrixPop();
    }


    ///CRAFTING TABLE
    if(craft3xEn == true)
    {
        for(int i = 0; i <= 2; i++)
        {
            for(int j = 0; j <= 2; j++)
            {
                if(craftSlotId3[i*3+j] != -1)
                {
                    MatrixPush();

                    MatrixTranslation(Vector3(204+j*36,26+i*36,0.0f));

                    if(craftSlotId3[i*3+j] < 250)
                    {
                        TextureManager::Instance()->SetTextureModeulate(texture);
						MatrixScale(Vector3(0.55,0.55,0.55));
                        mWorld->drawHudCubes(craftSlotId3[i*3+j]);
                    }

                    if(craftSlotId3[i*3+j] >= 250)
                    {
                        mWorld->ItemHaveTerrainTexture(craftSlotId3[i*3+j] ) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);
						MatrixScale(Vector3(0.5,0.5,0.5));
                        mWorld->drawHudItems(craftSlotId3[i*3+j]);
                    }
                    MatrixPop();
                }
            }
        }
        //End

        //Item which we are crafting 3x
        if (craftItemId3 != -1)
        {
            MatrixPush();

            MatrixTranslation(Vector3(348,62,0.0f));

            if(craftItemId3 < 250)
            {
                TextureManager::Instance()->SetTextureModeulate(texture);
				MatrixScale(Vector3(0.55,0.55,0.55));
                mWorld->drawHudCubes(craftItemId3);
            }

            if(craftItemId3 >= 250)
            {
                mWorld->ItemHaveTerrainTexture(craftItemId3) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);
				MatrixScale(Vector3(0.5,0.5,0.5));
                mWorld->drawHudItems(craftItemId3);
            }
            MatrixPop();
        }
    }

    ///CHEST///opensound//


    if(chestEn == true)
    {
		
        for(int i = 0; i <= 2; i++)
        {
            for(int j = 0; j <= 8; j++)
            {
                if(UseChest->chestSlotId[i*9+j] != -1)
                {
                    MatrixPush();

                    MatrixTranslation(Vector3(96+j*36,26+i*36,0.0f));

                    if(UseChest->chestSlotId[i*9+j] < 250)
                    {
                        TextureManager::Instance()->SetTextureModeulate(texture);
						MatrixScale(Vector3(0.55,0.55,0.55));
                        mWorld->drawHudCubes(UseChest->chestSlotId[i*9+j]);
                    }

                    if(UseChest->chestSlotId[i*9+j] >= 250)
                    {
                        mWorld->ItemHaveTerrainTexture(UseChest->chestSlotId[i*9+j]) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);
						MatrixScale(Vector3(0.5,0.5,0.5));
                        mWorld->drawHudItems(UseChest->chestSlotId[i*9+j]);
                    }
                    MatrixPop();
                }
            }
        }				
    }

    ///FURNACE
	
    if(furnaceEn == true)
    {
        if(UseFurnace->furnaceSlotId[0] != -1)
        {
            sceGumPushMatrix();
            ScePspFVector3 loc = {204,26,0.0f};
            sceGumTranslate(&loc);

            if(UseFurnace->furnaceSlotId[0] < 250)
            {
                TextureManager::Instance()->SetTextureModeulate(texture);

                mWorld->drawHudCubes(UseFurnace->furnaceSlotId[0]);
            }

            if(UseFurnace->furnaceSlotId[0] >= 250)
            {
                mWorld->ItemHaveTerrainTexture(UseFurnace->furnaceSlotId[0]) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);

                mWorld->drawHudItems(UseFurnace->furnaceSlotId[0]);
            }
            sceGumPopMatrix();
        }

        if(UseFurnace->furnaceSlotId[1] != -1)
        {
            sceGumPushMatrix();
            ScePspFVector3 loc = {204,98,0.0f};
            sceGumTranslate(&loc);

            if(UseFurnace->furnaceSlotId[1] < 250)
            {
                TextureManager::Instance()->SetTextureModeulate(texture);

                mWorld->drawHudCubes(UseFurnace->furnaceSlotId[1]);
            }

            if(UseFurnace->furnaceSlotId[1] >= 250)
            {
                mWorld->ItemHaveTerrainTexture(UseFurnace->furnaceSlotId[1]) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);

                mWorld->drawHudItems(UseFurnace->furnaceSlotId[1]);
            }
            sceGumPopMatrix();
        }

        if(UseFurnace->furnaceSlotId[2] != -1)
        {
            sceGumPushMatrix();
            ScePspFVector3 loc = {312,62,0.0f};
            sceGumTranslate(&loc);

            if(UseFurnace->furnaceSlotId[2] < 250)
            {
                TextureManager::Instance()->SetTextureModeulate(texture);

                mWorld->drawHudCubes(UseFurnace->furnaceSlotId[2]);
            }

            if(UseFurnace->furnaceSlotId[2] >= 250)
            {
                mWorld->ItemHaveTerrainTexture(UseFurnace->furnaceSlotId[2]) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);

                mWorld->drawHudItems(UseFurnace->furnaceSlotId[2]);
            }
            sceGumPopMatrix();
        }
    }

    if (invEn == true || craft3xEn == true || chestEn == true || furnaceEn == true)
    {
        for(int i = 0; i <= 3; i++)
        {
            for(int j = 0; j <= 8; j++)
            {
                if(mWorld->invId[i*9+j] != -1)
                {
                    MatrixPush();
                    MatrixTranslation(Vector3(168+j*18,149+i*18,0.0f));

                    if(mWorld->invId[i*9+j] < 250)
                    {
                        TextureManager::Instance()->SetTextureModeulate(texture);
						MatrixScale(Vector3(0.55,0.55,0.55));

                        mWorld->drawHudCubes(mWorld->invId[i*9+j]);
                    }

                    if(mWorld->invId[i*9+j] >= 250)
                    {
                        mWorld->ItemHaveTerrainTexture(mWorld->invId[i*9+j]) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);
						MatrixScale(Vector3(0.5,0.5,0.5));

                        mWorld->drawHudItems(mWorld->invId[i*9+j]);
                    }
                    MatrixPop();
                }
            }
        }

        DrawSetBlend(true);

        invCellSprite->SetPosition(selectInvSprite->GetPositionX(),selectInvSprite->GetPositionY());
        invCellSprite->Draw();

        if (mWorld->mId != -1) //MOUSE ITEM
        {
            MatrixPush();
            if (upEn == 1)
            {
                if (invEn == 1)
                {
                    MatrixTranslation(Vector3(248+invXPosition*18,35+invYPosition*18,0.0f));
                }
                if (craft3xEn == 1)
                {
                    MatrixTranslation(Vector3(204+invXPosition*36,18+invYPosition*36,0.0f));
                }
                if (chestEn == 1)
                {
                    MatrixTranslation(Vector3(96+invXPosition*36,18+invYPosition*36,0.0f));
                }
                if (furnaceEn == 1)
                {
                    MatrixTranslation(Vector3(204,18+(invYPosition * 72),0.0f));
                }
            }
            else
            {
                if(armorEn == false)
                {
                    MatrixTranslation(Vector3(96 + (invXPosition * 36),130+(invYPosition * 36),0.0f)); // 8 upper
                }
                else
                {
                    MatrixTranslation(Vector3(96 + (invXPosition * 108),18+(invYPosition * 72),0.0f)); // 8 upper
                }
            }

            if (mWorld->mId < 250)
            {
                TextureManager::Instance()->SetTextureModeulate(texture);
				MatrixScale(Vector3(0.55,0.55,0.55));
                mWorld->drawHudCubes(mWorld->mId);
            }

            if (mWorld->mId >= 250)
            {
                mWorld->ItemHaveTerrainTexture(mWorld->mId) ? TextureManager::Instance()->SetTextureModeulate(texture) : TextureManager::Instance()->SetTextureModeulate(barItems);
				MatrixScale(Vector3(0.5,0.5,0.5));
                mWorld->drawHudItems(mWorld->mId);
            }
            MatrixPop();
        }
    }

	DrawSetDepthTest(false);
	DrawSetDepthMask(true);
	DrawSetBlend(true);
	MatrixColor(GU_COLOR(1,1,1,1.0f));
    // durability strip drawing //ui
    if ((invEn == false && craft3xEn == false && chestEn == false && furnaceEn == false && menuState == 0 && mWorld->mainOptions.guiDrawing == 1) || (makeScreen == true && mWorld->mainOptions.guiDrawing == 1))
    {
        for(int k = 0; k <= 8; k++)
        {
            if(mWorld->invId[27+k] >= 250)
            {
                if(mWorld->DurabilityPointsItem(mWorld->invId[27+k]) != -1) // if it is tool
                {
                    if(mWorld->invAm[27+k] != mWorld->DurabilityPointsItem(mWorld->invId[27+k])) // checking durability
                    {
                        unsigned int toolPointStd = roundf((float)(mWorld->invAm[27+k]) / (float)(mWorld->DurabilityPointsItem(mWorld->invId[27+k])) * 13);

                        toolPointSprite[toolPointStd]->SetPosition(154+k*20,235);
                        toolPointSprite[toolPointStd]->Draw();
                    }
                }
            }
        }
    }

    if(invEn == true)
    {
        for(int i = 0; i <= 1; i++)
        {
            for(int j = 0; j <= 1; j++)
            {
                if(craftSlotId[i*2+j] >= 250 && mWorld->DurabilityPointsItem(craftSlotId[i*2+j]) != -1)
                {
                    if(craftSlotAm[i*2+j] != mWorld->DurabilityPointsItem(craftSlotId[i*2+j])) // checking durability
                    {
                        unsigned int toolPointStd = roundf((float)(craftSlotAm[i*2+j]) / (float)(mWorld->DurabilityPointsItem(craftSlotId[i*2+j])) * 13);

                        toolPointSprite[toolPointStd]->SetPosition(242+j*18,92+i*18);
                        toolPointSprite[toolPointStd]->Draw();
                    }
                }
            }
        }

        for(int i = 0; i <= 1; i++)
        {
            for(int j = 0; j <= 1; j++)
            {
                if(mWorld->armorId[i*2+j] >= 250 && mWorld->DurabilityPointsItem(mWorld->armorId[i*2+j]) != -1)
                {
                    if(mWorld->armorAm[i*2+j] != mWorld->DurabilityPointsItem(mWorld->armorId[i*2+j])) // checking durability
                    {
                        unsigned int toolPointStd = roundf((float)(mWorld->armorAm[i*2+j]) / (float)(mWorld->DurabilityPointsItem(mWorld->armorId[i*2+j])) * 13);

                        toolPointSprite[toolPointStd]->SetPosition(84+j*108,36+i*72);
                        toolPointSprite[toolPointStd]->Draw();
                    }
                }
            }
        }
    }

    if(craft3xEn == true)
    {
        for(int i = 0; i <= 2; i++)
        {
            for(int j = 0; j <= 2; j++)
            {
                if(craftSlotId3[i*3+j] >= 250 && mWorld->DurabilityPointsItem(craftSlotId3[i*3+j]) != -1)
                {
                    if(craftSlotAm3[i*3+j] != mWorld->DurabilityPointsItem(craftSlotId3[i*3+j])) // checking durability
                    {
                        unsigned int toolPointStd = roundf((float)(craftSlotAm3[i*3+j]) / (float)(mWorld->DurabilityPointsItem(craftSlotId3[i*3+j])) * 13);

                        toolPointSprite[toolPointStd]->SetPosition(192+j*36,36+i*36);
                        toolPointSprite[toolPointStd]->Draw();
                    }
                }
            }
        }
    }

    if(chestEn == true) //chest
    {
        for(int i = 0; i <= 2; i++)
        {
            for(int j = 0; j <= 8; j++)
            {
                if(UseChest->chestSlotId[i*9+j] >= 250 && mWorld->DurabilityPointsItem(UseChest->chestSlotId[i*9+j]) != -1)
                {
                    if(UseChest->chestSlotAm[i*9+j] != mWorld->DurabilityPointsItem(UseChest->chestSlotId[i*9+j])) // checking durability
                    {
                        unsigned int toolPointStd = roundf((float)(UseChest->chestSlotAm[i*9+j]) / (float)(mWorld->DurabilityPointsItem(UseChest->chestSlotId[i*9+j])) * 13);

                        toolPointSprite[toolPointStd]->SetPosition(84+j*36,36+i*36);
                        toolPointSprite[toolPointStd]->Draw();
                    }
                }
            }
        }
    }

    if(furnaceEn == true)
    {
        if(UseFurnace->furnaceSlotId[0] >= 250 && mWorld->DurabilityPointsItem(UseFurnace->furnaceSlotId[0]) != -1) // it is item and it have durability points
        {
            if(UseFurnace->furnaceSlotAm[0] != mWorld->DurabilityPointsItem(UseFurnace->furnaceSlotId[0]) != -1)
            {
                unsigned int toolPointStd = roundf((float)(UseFurnace->furnaceSlotAm[0]) / (float)(mWorld->DurabilityPointsItem(UseFurnace->furnaceSlotId[0])) * 13);

                toolPointSprite[toolPointStd]->SetPosition(192,36);
                toolPointSprite[toolPointStd]->Draw();
            }
        }

        if(UseFurnace->furnaceSlotId[1] >= 250 && mWorld->DurabilityPointsItem(UseFurnace->furnaceSlotId[1]) != -1) // it is item and it have durability points
        {
            if(UseFurnace->furnaceSlotAm[1] != mWorld->DurabilityPointsItem(UseFurnace->furnaceSlotId[1]) != -1)
            {
                unsigned int toolPointStd = roundf((float)(UseFurnace->furnaceSlotAm[1]) / (float)(mWorld->DurabilityPointsItem(UseFurnace->furnaceSlotId[1])) * 13);

                toolPointSprite[toolPointStd]->SetPosition(192,108);
                toolPointSprite[toolPointStd]->Draw();
            }
        }
    }

    if (invEn == true || craft3xEn == true || chestEn == true || furnaceEn == true)
    {
        for(int i = 0; i <= 3; i++)
        {
            for(int j = 0; j <= 8; j++)
            {
                if(mWorld->invId[i*9+j] >= 250)
                {
                    if(mWorld->DurabilityPointsItem(mWorld->invId[i*9+j]) != -1) // if it is tool
                    {
                        if(mWorld->invAm[i*9+j] != mWorld->DurabilityPointsItem(mWorld->invId[i*9+j])) // checking durability
                        {
                            unsigned int toolPointStd = roundf((float)(mWorld->invAm[i*9+j]) / (float)(mWorld->DurabilityPointsItem(mWorld->invId[i*9+j])) * 13);

                            toolPointSprite[toolPointStd]->SetPosition(162+j*18,154+i*18);
                            toolPointSprite[toolPointStd]->Draw();
                        }
                    }
                }
            }
        }

        if (mWorld->mId >= 250 && mWorld->DurabilityPointsItem(mWorld->mId) != -1)
        {
            if(mWorld->mAm != mWorld->DurabilityPointsItem(mWorld->mId))
            {
                unsigned int toolPointStd = roundf((float)(mWorld->mAm) / (float)(mWorld->DurabilityPointsItem(mWorld->mId)) * 13);
                if (upEn == 1)
                {
                    if (invEn == 1)
                    {
                        toolPointSprite[toolPointStd]->SetPosition(242+invXPosition*18,92+invYPosition*18);
                    }
                    if (craft3xEn == 1)
                    {
                        toolPointSprite[toolPointStd]->SetPosition(192+invXPosition*36,28+invYPosition*36);
                    }
                    if (chestEn == 1)
                    {
                        toolPointSprite[toolPointStd]->SetPosition(84+invXPosition*36,28+invYPosition*36);

                    }
                    if (furnaceEn == 1)
                    {
                        toolPointSprite[toolPointStd]->SetPosition(192,28+invYPosition*72);
                    }
                }
                else
                {
                    if(armorEn == false)
                    {
                        toolPointSprite[toolPointStd]->SetPosition(84+invXPosition*36,140+invYPosition*36);
                    }
                    else
                    {
                        toolPointSprite[toolPointStd]->SetPosition(84+invXPosition*108,28+invYPosition*72);
                    }
                }
                toolPointSprite[toolPointStd]->Draw();
            }
        }
    }

    DrawSetBlend(false);


	sceGuEnable(GU_DEPTH_TEST);
	sceGuDepthMask(0);

    //text
    if(mWorld->HP <= 0)//muerte
    {
        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        if(mRender->GetFontLanguage() == ENGLISH)
        {
            if(mWorld->gameModeWorld == 2)
            {
				//check for update
				sbuttonSprite->SetPosition(240,135);
				sbuttonSprite->Draw();
				
                mRender->SetFontStyle(default_big_size,GU_COLOR(0.25f,0.25f,0.25f,1),0,0x00000200);
				mRender->DebugPrint(242,52,"Game over!");
				
				mRender->SetFontStyle(0.5,GU_COLOR(0.25f,0.25f,0.25f,1),0,0x00000200);
                mRender->DebugPrint(242,122,"You cannot respawn in hardcore mode!");
				
				mRender->SetFontStyle(default_big_size,GU_COLOR(1,0,0,1),0,0x00000200);
				mRender->DebugPrint(240,50,"Game over!");
				
				int NumeroExperiencia = mWorld->EXP + mWorld->mainStatistics.minutesPlayed + mWorld->mainStatistics.daysInGame + mWorld->mainStatistics.soilPlowed + mWorld->mainStatistics.cropsGrowned * 100 / 7;
				mRender->SetFontStyle(0.4,GU_COLOR(1,1,1,1),0,0x00000200);
                mRender->DebugPrint(242,85,"Score: %i",NumeroExperiencia);	//mmvrg
				
				mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),0,0x00000200);
                mRender->DebugPrint(240,120,"You cannot respawn in hardcore mode!");
				
				diePos == 0 ? DrawText(240,143,GU_COLOR(1,1,0.25,1) ,default_size,"Delete world") : DrawText(240,143,GU_COLOR(1,1,1,1) ,default_size,"Delete world");
				

            }
            else
            {	
				//check for update
				buttonSprite->SetPosition(240,120);
				buttonSprite->Draw();

				buttonSprite->SetPosition(240,150);
				buttonSprite->Draw();

				//back
				sbuttonSprite->SetPosition(240,(diePos * 30) + 120);
				sbuttonSprite->Draw();
				
                mRender->SetFontStyle(default_big_size,GU_COLOR(0.25f,0.25f,0.25f,1),0,0x00000200);
                mRender->DebugPrint(242,52,"You died!");

                mRender->SetFontStyle(default_big_size,GU_COLOR(1,0,0,1),0,0x00000200);
                mRender->DebugPrint(240,50,"You died!");
				
				int NumeroExperiencia = mWorld->EXP + mWorld->mainStatistics.minutesPlayed + mWorld->mainStatistics.daysInGame + mWorld->mainStatistics.soilPlowed + mWorld->mainStatistics.cropsGrowned * 100 / 7;
				mRender->SetFontStyle(0.4,GU_COLOR(1,1,1,1),0,0x00000200);
                mRender->DebugPrint(242,85,"Score: %i",NumeroExperiencia);
				
				diePos == 0 ? DrawText(240,128,GU_COLOR(1,1,0.25,1) ,default_size,"Respawn") : DrawText(240,128,GU_COLOR(1,1,1,1) ,default_size,"Respawn");
				diePos == 1 ? DrawText(240,158,GU_COLOR(1,1,0.25,1) ,default_size,"Title screen") : DrawText(240,158,GU_COLOR(1,1,1,1) ,default_size,"Title screen");
				
				sceGuDisable(GU_DEPTH_TEST);
				sceGuEnable(GU_BLEND);
				sceGuColor(GU_COLOR(1,1,1,1.0f));


            }
        }
        if(mRender->GetFontLanguage() == RUSSIAN)
        {
            if(mWorld->gameModeWorld == 2)
            {
				//check for update
				sbuttonSprite->SetPosition(240,135);
				sbuttonSprite->Draw();
				
                mRender->SetFontStyle(default_big_size,GU_COLOR(0.25f,0.25f,0.25f,1),0,0x00000200);
				mRender->DebugPrint(242,52,"Juego perdido!");
				
				mRender->SetFontStyle(0.5,GU_COLOR(0.25f,0.25f,0.25f,1),0,0x00000200);
                mRender->DebugPrint(242,122,"No puedes reaparecer en modo hardcore!");
				
				mRender->SetFontStyle(default_big_size,GU_COLOR(1,0,0,1),0,0x00000200);
				mRender->DebugPrint(240,50,"Juego perdido!");
				
				int NumeroExperiencia = mWorld->EXP + mWorld->mainStatistics.minutesPlayed + mWorld->mainStatistics.daysInGame + mWorld->mainStatistics.soilPlowed + mWorld->mainStatistics.cropsGrowned * 100 / 7;
				mRender->SetFontStyle(0.4,GU_COLOR(1,1,1,1),0,0x00000200);
                mRender->DebugPrint(242,85,"Puntaje: %i",NumeroExperiencia);	//mmvrg
				
				mRender->SetFontStyle(0.5,GU_COLOR(1,1,1,1),0,0x00000200);
                mRender->DebugPrint(240,120,"No puedes reaparecer en modo hardcore!");
				
				diePos == 0 ? DrawText(240,143,GU_COLOR(1,1,0.25,1) ,default_size,"Borrar mundo") : DrawText(240,143,GU_COLOR(1,1,1,1) ,default_size,"Borrar mundo");
				

            }
            else
            {	
				//check for update
				buttonSprite->SetPosition(240,120);
				buttonSprite->Draw();

				buttonSprite->SetPosition(240,150);
				buttonSprite->Draw();

				//back
				sbuttonSprite->SetPosition(240,(diePos * 30) + 120);
				sbuttonSprite->Draw();
				
                mRender->SetFontStyle(default_big_size,GU_COLOR(0.25f,0.25f,0.25f,1),0,0x00000200);
                mRender->DebugPrint(242,52,"Moriste!");

                mRender->SetFontStyle(default_big_size,GU_COLOR(1,0,0,1),0,0x00000200);
                mRender->DebugPrint(240,50,"Moriste!");
				
				int NumeroExperiencia = mWorld->EXP + mWorld->mainStatistics.minutesPlayed + mWorld->mainStatistics.daysInGame + mWorld->mainStatistics.soilPlowed + mWorld->mainStatistics.cropsGrowned * 100 / 7;
				mRender->SetFontStyle(0.4,GU_COLOR(1,1,1,1),0,0x00000200);
                mRender->DebugPrint(242,85,"Puntaje: %i",NumeroExperiencia);
				
				diePos == 0 ? DrawText(240,128,GU_COLOR(1,1,0.25,1) ,default_size,"Reaparecer") : DrawText(240,128,GU_COLOR(1,1,1,1) ,default_size,"Reaparecer");
				diePos == 1 ? DrawText(240,158,GU_COLOR(1,1,0.25,1) ,default_size,"Menu principal") : DrawText(240,158,GU_COLOR(1,1,1,1) ,default_size,"Menu principal");
				
				sceGuDisable(GU_DEPTH_TEST);
				sceGuEnable(GU_BLEND);
				sceGuColor(GU_COLOR(1,1,1,1.0f));


            }
        }
    }

    if(sleepMessageTime > 0.0f)
    {
        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        if(sleepMessageType == 1)
        {
            if(mRender->GetFontLanguage() == ENGLISH)
            {
                mRender->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00000200);
                mRender->DebugPrint(242,158,"You can only sleep at night!");
            }
            if(mRender->GetFontLanguage() == RUSSIAN)
            {
                mRender->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00000200);
                mRender->DebugPrint(242,158,"Solo puedes dormir en la noche!");
            }
        }
        else
        {
            if(mRender->GetFontLanguage() == ENGLISH)
            {
                mRender->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00000200);
                mRender->DebugPrint(242,158,"You can’t sleep yet, there are monsters nearby");
            }
            if(mRender->GetFontLanguage() == RUSSIAN)
            {
                mRender->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00000200);
                mRender->DebugPrint(242,158,"No puedes dormir ahora! hay monstruos cerca");
            }
        }
    }

    sleepMessageTime -= dt;
    if(sleepMessageTime < 0.1f && sleepMessageTime > 0.0f)
    {
        sleepMessageTime = 0.0f;
    }

    mRender->SetFontStyle(default_size,0xFFFFFFFF,0,0x00000200);

    if((freeMemory/1024)/1024 <= 3 && mWorld->mainStatistics.minutesPlayed > 2)
    {
        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));
        mRender->SetFontStyle(default_size,GU_COLOR(1,0,0,1),0,0x00000000);
        //mRender->DebugPrint(5,15,"You have few free ram! Please, reduce the settings or restart the game!");
        mRender->SetFontStyle(default_size,0xFFFFFFFF,0,0x00000200);
    }

    mRender->SetFontStyle(default_size,0xFFFFFFFF,0xFF000000,0x00000200);

    if(invEn == true || craft3xEn == true || chestEn == true || furnaceEn == true)
    {	
        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuColor(GU_COLOR(1,1,1,1.0f));

        for(int i = 0; i <= 3; i++)
        {
            for(int j = 0; j <= 8; j++)
            {
                if(mWorld->invId[i*9+j] != -1 && mWorld->invAm[i*9+j] > 1)
                {
                    if(mWorld->invId[i*9+j] >= 250)
                    {
                        if(mWorld->DurabilityPointsItem(mWorld->invId[i*9+j]) == -1)
                        {
                            DrawAmount(173+j*18,162+i*18,mWorld->invAm[i*9+j]); //mit4d
                        }
                    }
                    else
                    {
                        DrawAmount(173+j*18,162+i*18,mWorld->invAm[i*9+j]);
                    }
                }
            }
        }

        if (mWorld->mId != -1)
        {
            bool canDraw = false;

            if(mWorld->mId >= 250)
            {
                if(mWorld->DurabilityPointsItem(mWorld->mId) == -1)
                {
                    canDraw = true;
                }
            }
            else
            {
                canDraw = true;
            }

            if(canDraw == true)
            {
                if (upEn == 0)
                {
                    if(armorEn == false)
                    {
                        DrawAmount(106+(invXPosition * 36),156+(invYPosition * 36),mWorld->mAm);
                    }
                    else
                    {
                        DrawAmount(106+(invXPosition * 108),44+(invYPosition * 72),mWorld->mAm);
                    }
                }
                else
                {
                    if(invEn == 1)
                    {
                        DrawAmount(286+(invXPosition * 18),64+(invYPosition * 18),mWorld->mAm);
                    }
                    if(craft3xEn == 1)
                    {
                        DrawAmount(214+(invXPosition * 36),44+(invYPosition * 36),mWorld->mAm);
                    }
                    if(chestEn == 1)
                    {
                        DrawAmount(106+(invXPosition * 36),44+(invYPosition * 36),mWorld->mAm);
                    }
                    if(furnaceEn == 1)
                    {
                        DrawAmount(214,44+(invYPosition * 72),mWorld->mAm);
                    }
                }
            }
        }

        sceGuEnable(GU_BLEND);
        selectInvSprite->Draw();
    }

    if((invEn == false && craft3xEn == false && chestEn == false && furnaceEn == false && menuState == 0 && mWorld->mainOptions.guiDrawing == 1) || (makeScreen == true && mWorld->mainOptions.guiDrawing == 1))
    { //ui
        for(int k = 0; k <= 8; k++)
        {
            if (mWorld->invAm[27+k] > 1)
            {
                if(mWorld->invId[27+k] >= 250) // if it is item but not a tool
                {
                    if(mWorld->DurabilityPointsItem(mWorld->invId[27+k]) == -1)
                    {
                        DrawAmount(165+k*20,243,mWorld->invAm[27+k]);
                    }
                }
                else
                {
                    DrawAmount(165+k*20,243,mWorld->invAm[27+k]);
                }
            }
        }
    }

    if(invEn == true)
    {
        for(int i = 0; i <= 1; i++)
        {
            for(int j = 0; j <= 1; j++)
            {
                if(craftSlotId[i*2+j] != -1 && craftSlotAm[i*2+j] > 1)
                {
                    if(craftSlotId[i*2+j] >= 250)
                    {
                        if(mWorld->DurabilityPointsItem(craftSlotId[i*2+j]) == -1)
                        {
                            DrawAmount(286+j*36,72+i*36,craftSlotAm[i*2+j]);
                        }
                    }
                    else
                    {
                        DrawAmount(286+j*36,72+i*36,craftSlotAm[i*2+j]);
                    }
                }
            }
        }

        if(craftItemId != -1)
        {
            if(craftItemId >= 250)
            {
                if(mWorld->DurabilityPointsItem(craftItemId) == -1)
                {
                    DrawAmount(394,92,craftItemAm);
                }
            }
            else
            {
                DrawAmount(394,92,craftItemAm);
            }
        }
    }

    if(craft3xEn == true)
    {
        for(int i = 0; i <= 2; i++)
        {
            for(int j = 0; j <= 2; j++)
            {
                if(craftSlotId3[i*3+j] != -1 && craftSlotAm3[i*3+j] > 1)
                {
                    if(craftSlotId3[i*3+j] >= 250)
                    {
                        if(mWorld->DurabilityPointsItem(craftSlotId3[i*3+j]) == -1)
                        {
                            DrawAmount(214+j*36,52+i*36,craftSlotAm3[i*3+j]);
                        }
                    }
                    else
                    {
                        DrawAmount(214+j*36,52+i*36,craftSlotAm3[i*3+j]);
                    }
                }
            }
        }

        if(craftItemId3 != -1)
        {
            if(craftItemId3 >= 250)
            {
                if(mWorld->DurabilityPointsItem(craftItemId3) == -1)
                {
                    DrawAmount(358,88,craftItemAm3);
                }
            }
            else
            {
                DrawAmount(358,88,craftItemAm3);
            }
        }
    }

    if(chestEn == true) //chest
    {
        for(int i = 0; i <= 2; i++)
        {
            for(int j = 0; j <= 8; j++)
            {
                if(UseChest->chestSlotId[i*9+j] != -1 && UseChest->chestSlotAm[i*9+j] > 1)
                {
                    if(UseChest->chestSlotId[i*9+j] >= 250)
                    {
                        if(mWorld->DurabilityPointsItem(UseChest->chestSlotId[i*9+j]) == -1)
                        {
                            DrawAmount(106+j*36,52+i*36,UseChest->chestSlotAm[i*9+j]);
                        }
                    }
                    else
                    {
                        DrawAmount(106+j*36,52+i*36,UseChest->chestSlotAm[i*9+j]);
                    }
                }
            }
        }
    }

    if(furnaceEn == true)
    {
        if(UseFurnace->furnaceSlotId[0] != -1 && UseFurnace->furnaceSlotAm[0] > 1)
        {
            if(UseFurnace->furnaceSlotId[0] >= 250)
            {
                if(mWorld->DurabilityPointsItem(UseFurnace->furnaceSlotId[0]) == -1)
                {
                    DrawAmount(214,52,UseFurnace->furnaceSlotAm[0]);
                }
            }
            else
            {
                 DrawAmount(214,52,UseFurnace->furnaceSlotAm[0]);
            }
        }
        if(UseFurnace->furnaceSlotId[1] != -1 && UseFurnace->furnaceSlotAm[1] > 1)
        {
            if(UseFurnace->furnaceSlotId[1] >= 250)
            {
                if(mWorld->DurabilityPointsItem(UseFurnace->furnaceSlotId[1]) == -1)
                {
                    DrawAmount(214,124,UseFurnace->furnaceSlotAm[1]);
                }
            }
            else
            {
                DrawAmount(214,124,UseFurnace->furnaceSlotAm[1]);
            }
        }
        if(UseFurnace->furnaceSlotId[2] != -1 && UseFurnace->furnaceSlotAm[2] > 1)
        {
            DrawAmount(322,88,UseFurnace->furnaceSlotAm[2]);
        }
    }
    sceGuDisable(GU_BLEND);
    sceGuEnable(GU_DEPTH_TEST);

    if(makeScreen)
	{
		//end frame now to update frame buffer
		mRender->EndFrame();
		//make screenshot
		mRender->TakeNextScreenshot();
		makeScreen = false;
	}

    int language = mRender->GetFontLanguage();
    mRender->SetFontStyle(0.345f,0xFFFFFFFF,0,0x00000200);


    if(invEn == false && craft3xEn == false && chestEn == false && furnaceEn == false && menuState == 0) //ui
    {
        if(tickShowSlotName > 0.0f)
        {
            if(tickShowSlotName > 1.0f)
            {
                mRender->SetFontStyle(0.5f,GU_COLOR(1,1,1,1),GU_COLOR(0.15f,0.15f,0.15f,1),0x00000200);
            }
            else
            {
                mRender->SetFontStyle(0.5f,GU_COLOR(1,1,1,tickShowSlotName),GU_COLOR(0.15f,0.15f,0.15f,tickShowSlotName),0x00000200);
            }
            if(mWorld->invId[27+barPosition] != -1)
            {
                std::string slotName = mWorld->NameBlock(mWorld->invId[27+barPosition]);
                mRender->DebugPrint(240,190,"%s",slotName.c_str());
            }
        }

        inputDiskNameTimer -= dt;
        if(inputDiskNameTimer > 0.0f)
        {
            float red, blue, green, alpha;
            if(inputDiskNameTimer > 1.8f && inputDiskNameTimer <= 2.4f)
            {
                red = 1.0f;
                green = 1.0f - (2.4f-inputDiskNameTimer)/0.6f;
                blue = 0.0f;
                alpha = 1.0f;
            }
            if(inputDiskNameTimer > 1.2f && inputDiskNameTimer <= 1.8f)
            {
                red = 1.0f - (1.8f-inputDiskNameTimer)/0.6f;
                green = 0.0f;
                blue = (1.8f-inputDiskNameTimer)/0.6f;
                alpha = 1.0f;
            }
            if(inputDiskNameTimer > 0.6f && inputDiskNameTimer <= 1.2f)
            {
                red = 0.0f;
                green = (1.2f-inputDiskNameTimer)/0.6f;
                blue = 1.0f - (1.2f-inputDiskNameTimer)/0.6f;
                alpha = 1.0f;
            }
            if(inputDiskNameTimer > 0.0f && inputDiskNameTimer <= 0.6f)
            {
                red = (0.6f-inputDiskNameTimer)/0.6f;
                green = 1.0f;
                blue = 0.0f;
                alpha = 1.0f-(0.6f-inputDiskNameTimer)/0.6f;
            }
            if(alpha < 0.0f)
            {
                alpha = 0.0f;
            }

            mRender->SetFontStyle(0.5f,GU_COLOR(red,green,blue,alpha),999,0x00000200);

            mRender->SetFont(ENGLISH);
            mRender->DebugPrint(240,50,"%s",inputDiskName.c_str());
            mRender->SetDefaultFont();
        }

        if(tutorialTimer > -1.0f)
        {
            tutorialTimer -= dt;
        }
        if(tutorialTimer > 0.0f)
        {
            if(tutorialTimer > 9.0f && tutorialTimer <= 10.0f)
            {
                mRender->SetFontStyle(0.5f,GU_COLOR(1,1,1,10.0f-tutorialTimer),GU_COLOR(1,1,1,10.0f-tutorialTimer),0x00000000);
            }
            if(tutorialTimer > 1.0f && tutorialTimer <= 9.0f)
            {
                mRender->SetFontStyle(0.5f,GU_COLOR(1,1,1,1),GU_COLOR(1,1,1,1),0x00000000);
            }
            if(tutorialTimer <= 1.0f)
            {
                mRender->SetFontStyle(0.5f,GU_COLOR(1,1,1,tutorialTimer),GU_COLOR(1,1,1,tutorialTimer),0x00000000);
            }

            if(language == ENGLISH)
            {
                mRender->DebugPrint(5,40,"Tutorial");
                mRender->DebugPrint(5,57,"Press R+L to open inventory");
                mRender->DebugPrint(5,74,"Press R+arrow up to drop item");
				mRender->DebugPrint(5,91,"Press double up to sprint");
            }
            else
            {
                mRender->DebugPrint(5,40,"Tutorial");
                mRender->DebugPrint(5,57,"Presione R+L para abrir el inventario");
                mRender->DebugPrint(5,74,"Presione R+arrow up para dropear items");
				mRender->DebugPrint(5,91,"Presione doble up para esprintar");
            }
        }
        mRender->SetFontStyle(0.345f,0xFFFFFFFF,0,0x00000000);
    }

    //menu buttons
	if(invEn == false && craft3xEn == false && chestEn == false && furnaceEn == false && menuState == 0 && mWorld->mainOptions.guiDrawing == 1)
	{
		int NumeroExperiencia = mWorld->EXP;
		
		if(NumeroExperiencia >= 1)
		{
			RenderManager::InstancePtr()->SetFontStyle(0.45,GU_COLOR(0,0.25,0,1),0,0x00004000|0x00000200);
			RenderManager::InstancePtr()->DebugPrint(242.25,216.55,"%i",NumeroExperiencia);
			
			RenderManager::InstancePtr()->SetFontStyle(0.45,GU_COLOR(0,1,0,1),0,0x00004000|0x00000200);
			RenderManager::InstancePtr()->DebugPrint(241.5,215,"%i",NumeroExperiencia);
		}
	}
			
	
    if(menuState == 1)  
    {
        if(menuOptions)
        {
            sceGuDisable(GU_DEPTH_TEST);
            sceGuEnable(GU_BLEND);
            sceGuColor(GU_COLOR(1,1,1,1.0f));

            nbuttonSprite->SetPosition(110,40);
            nbuttonSprite->Draw();

            nbuttonSprite->SetPosition(370,40);
            nbuttonSprite->Draw();

            nbuttonSprite->SetPosition(110,70);
            nbuttonSprite->Draw();

            nbuttonSprite->SetPosition(370,70);
            nbuttonSprite->Draw();

            buttonSprite->SetPosition(110,100);
            buttonSprite->Draw();

            buttonSprite->SetPosition(370,100);
            buttonSprite->Draw();

            buttonSprite->SetPosition(110,130);
            buttonSprite->Draw();

            buttonSprite->SetPosition(370,130);
            buttonSprite->Draw();

            buttonSprite->SetPosition(110,160);
            buttonSprite->Draw();

            buttonSprite->SetPosition(370,160);
            buttonSprite->Draw();

            buttonSprite->SetPosition(110,190);
            buttonSprite->Draw();

            buttonSprite->SetPosition(370,190);
            buttonSprite->Draw();

            buttonSprite->SetPosition(110,220);
            buttonSprite->Draw();

            buttonSprite->SetPosition(370,220);
            buttonSprite->Draw();

            buttonSprite->SetPosition(110,250);
            buttonSprite->Draw();

            buttonSprite->SetPosition(370,250);
            buttonSprite->Draw();

            if (optionsMenuPos < 8)
            {
                sbuttonSprite->SetPosition(110,(optionsMenuPos * 30) + 40);
            }
            if(optionsMenuPos >= 8)
            {
                sbuttonSprite->SetPosition(370,((optionsMenuPos-8) * 30) + 40);
            }
            sbuttonSprite->Draw();


            if(mWorld->mainOptions.fov == 40)
            {
                moverSprite->SetPosition(21,40);
            }
            else
            {
                moverSprite->SetPosition((mWorld->mainOptions.fov - 40)*3.0f+19,40);
            }
            moverSprite->Draw();

            if(mWorld->mainOptions.horizontalViewDistance == 1)
            {
                moverSprite->SetPosition(21,70);
            }
            else
            {
                moverSprite->SetPosition((mWorld->mainOptions.horizontalViewDistance - 1)*63.3+9,70);
            }
            moverSprite->Draw();

            if(mWorld->mainOptions.verticalViewDistance == 1)
            {
                moverSprite->SetPosition(281,40);
            }
            else
            {
                moverSprite->SetPosition((mWorld->mainOptions.verticalViewDistance - 1)*190.0f+269,40);
            }
            moverSprite->Draw();

            if(mWorld->mainOptions.difficult != 3)
            {
                moverSprite->SetPosition((mWorld->mainOptions.difficult)*(190.0f/3.0f)+265+16,70);
            }
            else
            {
                moverSprite->SetPosition((mWorld->mainOptions.difficult)*(190.0f/3.0f)+269,70);
            }
            moverSprite->Draw();

            sceGuDisable(GU_BLEND);
            sceGuEnable(GU_DEPTH_TEST);

            //draw subtitles on buttons
            if(language == ENGLISH)
            {
                if(mWorld->mainOptions.fov == 65)
                {
                    optionsMenuPos == 0 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(110,49,"FOV: Normal");
                }
                else
                {
                    optionsMenuPos == 0 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(110,49,"FOV: %f",mWorld->mainOptions.fov);
                }

                if(mWorld->mainOptions.horizontalViewDistance == 1)
                {
                    optionsMenuPos == 1 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(110,79,"H View Distance: %i",mWorld->mainOptions.horizontalViewDistance);
                }
                else
                {
                    optionsMenuPos == 1 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(110,79,"H View Distance: %i",mWorld->mainOptions.horizontalViewDistance);
                }

                if(mWorld->mainOptions.verticalViewDistance == 1)
                {
                    optionsMenuPos == 8 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,49,"V View Distance: %i",mWorld->mainOptions.verticalViewDistance);
                }
                else
                {
                    optionsMenuPos == 8 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,49,"V View Distance: %i",mWorld->mainOptions.verticalViewDistance);
                }


                switch(mWorld->mainOptions.difficult)
                {
                case 0:
                    optionsMenuPos == 9 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,79,"Difficult: Peaceful");
                break;
                case 1:
                    optionsMenuPos == 9 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,79,"Difficult: Easy");
                break;
                case 2:
                    optionsMenuPos == 9 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,79,"Difficult: Normal");
                break;
                case 3:
                    optionsMenuPos == 9 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,79,"Difficult: Hard");
                break;
                }


                optionsMenuPos == 2 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.fogRendering == true ? mRender->DebugPrint(110,109,"Fog Rendering: ON"): mRender->DebugPrint(110,109,"Fog Rendering: OFF");

                optionsMenuPos == 3 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.detailedSky == true ? mRender->DebugPrint(110,139,"Detailed Sky: ON"): mRender->DebugPrint(110,139,"Detailed Sky: OFF");

                optionsMenuPos == 4 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.newSprintOption == true ? mRender->DebugPrint(110,169,"New-Sprint: ON"): mRender->DebugPrint(110,169,"New-Sprint: OFF");

                optionsMenuPos == 5 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.smoothLighting == true ? mRender->DebugPrint(110,199,"Smooth Lighting: ON"): mRender->DebugPrint(110,199,"Smooth Lighting: OFF");

                optionsMenuPos == 6 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.worldBlockAnimation == true ? mRender->DebugPrint(110,229,"Animated Blocks: ON"): mRender->DebugPrint(110,229,"Animated Blocks: OFF");

                optionsMenuPos == 7 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.autoJump == true ? mRender->DebugPrint(110,259,"Auto-jump: ON"): mRender->DebugPrint(110,259,"Auto-jump: OFF");

                optionsMenuPos == 10 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.buttonsDrawing == true ? mRender->DebugPrint(370,109,"Help-buttons: ON"): mRender->DebugPrint(370,109,"Help-buttons: OFF");

                optionsMenuPos == 11 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.guiDrawing == true ? mRender->DebugPrint(370,139,"GUI Rendering: ON"): mRender->DebugPrint(370,139,"GUI Rendering: OFF");

                optionsMenuPos == 12 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.sounds == true ? mRender->DebugPrint(370,169,"Sounds: ON"): mRender->DebugPrint(370,169,"Sounds: OFF");

                optionsMenuPos == 13 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.music == true ? mRender->DebugPrint(370,199,"Music : ON") : mRender->DebugPrint(370,199,"Music : OFF");

                optionsMenuPos == 14 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mRender->DebugPrint(370,229,"Take Screenshot");

                optionsMenuPos == 15 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                devMode == true ? mRender->DebugPrint(370,259,"Dev Mode: ON") : mRender->DebugPrint(370,259,"Dev Mode: OFF");

                DrawText(240,24,GU_COLOR(1,1,1,1),default_size,"Options");

                DrawText(110,24,GU_COLOR(1,1,1,1),default_size,"L");
                DrawText(370,24,GU_COLOR(1,1,1,1),default_size,"R");
            }
            if(language == RUSSIAN)
            {
                if(mWorld->mainOptions.fov == 65)
                {
                    optionsMenuPos == 0 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(110,49,"FOV: Normal");
                }
                else
                {
                    optionsMenuPos == 0 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(110,49,"FOV: %f",mWorld->mainOptions.fov);
                }

                if(mWorld->mainOptions.horizontalViewDistance == 1)
                {
                    optionsMenuPos == 1 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(110,79,"H View Distance: %i",mWorld->mainOptions.horizontalViewDistance);
                }
                else
                {
                    optionsMenuPos == 1 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(110,79,"H View Distance: %i",mWorld->mainOptions.horizontalViewDistance);
                }

                if(mWorld->mainOptions.verticalViewDistance == 1)
                {
                    optionsMenuPos == 8 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,49,"V View Distance: %i",mWorld->mainOptions.verticalViewDistance);
                }
                else
                {
                    optionsMenuPos == 8 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,49,"V View Distance: %i",mWorld->mainOptions.verticalViewDistance);
                }


                switch(mWorld->mainOptions.difficult)
                {
                case 0:
                    optionsMenuPos == 9 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,79,"Dificultad: Pacifica");
                break;
                case 1:
                    optionsMenuPos == 9 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,79,"Dificultad: Facil");
                break;
                case 2:
                    optionsMenuPos == 9 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,79,"Dificultad: Normal");
                break;
                case 3:
                    optionsMenuPos == 9 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                    mRender->DebugPrint(370,79,"Dificultad: Dificil");
                break;
                }


                optionsMenuPos == 2 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.fogRendering == true ? mRender->DebugPrint(110,109,"Niebla: ON"): mRender->DebugPrint(110,109,"Niebla: OFF");

                optionsMenuPos == 3 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.detailedSky == true ? mRender->DebugPrint(110,139,"Cielo detallado: ON"): mRender->DebugPrint(110,139,"Cielo detallado: OFF");

                optionsMenuPos == 4 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.newSprintOption == true ? mRender->DebugPrint(110,169,"Nuevo-Sprint: ON"): mRender->DebugPrint(110,169,"Nuevo-Sprint: OFF");

                optionsMenuPos == 5 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.smoothLighting == true ? mRender->DebugPrint(110,199,"Luz suave: ON"): mRender->DebugPrint(110,199,"Luz suave: OFF");

                optionsMenuPos == 6 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.worldBlockAnimation == true ? mRender->DebugPrint(110,229,"Animaciones: ON"): mRender->DebugPrint(110,229,"Animaciones: OFF");

                optionsMenuPos == 7 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.autoJump == true ? mRender->DebugPrint(110,259,"Auto salto: ON"): mRender->DebugPrint(110,259,"Auto salto: OFF");

                optionsMenuPos == 10 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.buttonsDrawing == true ? mRender->DebugPrint(370,109,"Help-buttons: ON"): mRender->DebugPrint(370,109,"Help-buttons: OFF");

                optionsMenuPos == 11 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.guiDrawing == true ? mRender->DebugPrint(370,139,"Renderizar GUI: ON"): mRender->DebugPrint(370,139,"Renderizar GUI: OFF");

                optionsMenuPos == 12 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.sounds == true ? mRender->DebugPrint(370,169,"Sonidos: ON"): mRender->DebugPrint(370,169,"Sonidos: OFF");

                optionsMenuPos == 13 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mWorld->mainOptions.music == true ? mRender->DebugPrint(370,199,"Musica : ON") : mRender->DebugPrint(370,199,"Musica : OFF");

                optionsMenuPos == 14 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                mRender->DebugPrint(370,229,"Capturar pantalla");

                optionsMenuPos == 15 ? RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,0,1),0,0x00004000|0x00000200) : RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
                devMode == true ? mRender->DebugPrint(370,259,"Modo desarrollor: ON") : mRender->DebugPrint(370,259,"Modo desarrollor: OFF");

                DrawText(240,24,GU_COLOR(1,1,1,1),default_size,"Opciones");

                DrawText(110,24,GU_COLOR(1,1,1,1),default_size,"L");
                DrawText(370,24,GU_COLOR(1,1,1,1),default_size,"R");
            }
        }
        else
        {
            sceGuDisable(GU_DEPTH_TEST);
            sceGuEnable(GU_BLEND);
            sceGuColor(GU_COLOR(1,1,1,1.0f));

            buttonSprite->SetPosition(240,50);
            buttonSprite->Draw();

            //resume
            buttonSprite->SetPosition(240,80);
            buttonSprite->Draw();

            //options
            buttonSprite->SetPosition(240,110);
            buttonSprite->Draw();

            //save
            buttonSprite->SetPosition(240,160);
            buttonSprite->Draw();

            //quit
            buttonSprite->SetPosition(240,190);
            buttonSprite->Draw();

            //quit and save
            buttonSprite->SetPosition(240,220);
            buttonSprite->Draw();

            //selected button
            if(selectPos <= 2)
            {
                sbuttonSprite->SetPosition(240,(selectPos * 30) + 50);
            }
            if(selectPos > 2)
            {
                sbuttonSprite->SetPosition(240,(selectPos * 30) + 70);
            }
            sbuttonSprite->Draw();

            sceGuDisable(GU_BLEND);
            sceGuEnable(GU_DEPTH_TEST);

            mRender->SetFontStyle(default_size ,GU_COLOR(1,1,1,1),0,0x00000000|0x00004000);
            if(language == ENGLISH)
            {
                //mRender->DebugPrint(2,15,"World seed is: %i",mWorld->worldSeed);

                selectPos == 0 ? DrawText(240,59,GU_COLOR(1,1,0,1),default_size,"Back to game") : DrawText(240,59,GU_COLOR(1,1,1,1),default_size,"Back to game");
                selectPos == 1 ? DrawText(240,89,GU_COLOR(1,1,0,1),default_size,"Options...") : DrawText(240,89,GU_COLOR(1,1,1,1),default_size,"Options...");
                selectPos == 2 ? DrawText(240,119,GU_COLOR(1,1,0,1),default_size,"Statistics") : DrawText(240,119,GU_COLOR(1,1,1,1),default_size,"Statistics");
                selectPos == 3 ? DrawText(240,169,GU_COLOR(1,1,0,1),default_size,"Save") : DrawText(240,169,GU_COLOR(1,1,1,1),default_size,"Save");
                selectPos == 4 ? DrawText(240,199,GU_COLOR(1,1,0,1),default_size,"Save and exit") : DrawText(240,199,GU_COLOR(1,1,1,1),default_size,"Save and exit");
                selectPos == 5 ? DrawText(240,229,GU_COLOR(1,1,0,1),default_size,"Exit") : DrawText(240,229,GU_COLOR(1,1,1,1),default_size,"Exit");

                DrawText(240,29,GU_COLOR(1,1,1,1),default_size,"Game menu");
            }
            if(language == RUSSIAN)
            {
                //mRender->DebugPrint(2,15,"Sid mira: %i",mWorld->worldSeed);

                selectPos == 0 ? DrawText(240,59,GU_COLOR(1,1,0,1),default_size,"Regresar") : DrawText(240,59,GU_COLOR(1,1,1,1),default_size,"Regresar");
                selectPos == 1 ? DrawText(240,89,GU_COLOR(1,1,0,1),default_size,"Opciones") : DrawText(240,89,GU_COLOR(1,1,1,1),default_size,"Opciones");
                selectPos == 2 ? DrawText(240,119,GU_COLOR(1,1,0,1),default_size,"Estadisticas") : DrawText(240,119,GU_COLOR(1,1,1,1),default_size,"Estadisticas");
                selectPos == 3 ? DrawText(240,169,GU_COLOR(1,1,0,1),default_size,"Guardar") : DrawText(240,169,GU_COLOR(1,1,1,1),default_size,"Guardar");
                selectPos == 4 ? DrawText(240,199,GU_COLOR(1,1,0,1),default_size,"Guardar y salir") : DrawText(240,199,GU_COLOR(1,1,1,1),default_size,"Guardar y salir");
                selectPos == 5 ? DrawText(240,229,GU_COLOR(1,1,0,1),default_size,"Salir") : DrawText(240,229,GU_COLOR(1,1,1,1),default_size,"Salir");

                DrawText(240,29,GU_COLOR(1,1,1,1),default_size,"Menu");
            }
        }
    }
    if (menuState == 3)
    {
        if(language == ENGLISH)
        {
            if(statisticsPage == 0)
            {
				mRender->DebugPrint(30,15,"World seed is: %i",mWorld->worldSeed);
                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,54,"Blocks Placed: %i",mWorld->mainStatistics.blockPlaced);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,74,"Blocks Destroyed: %i",mWorld->mainStatistics.blockDestroyed);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,94,"Age Of The World: %i d",mWorld->mainStatistics.daysInGame);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,114,"Minutes Played: %i m",mWorld->mainStatistics.minutesPlayed);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,134,"Items Crafted: %i",mWorld->mainStatistics.itemsCrafted);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,154,"Items Smelted: %i",mWorld->mainStatistics.itemsSmelted);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,174,"Soil Plowed: %i",mWorld->mainStatistics.soilPlowed);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,194,"Crops Grown: %i",mWorld->mainStatistics.cropsGrowned);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,214,"Trees Grown: %i",mWorld->mainStatistics.treesGrowned);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,234,"Damage Dealt: %i",mWorld->mainStatistics.damageRecieved);
            }
            else
            {
				mRender->DebugPrint(30,15,"World seed is: %i",mWorld->worldSeed);
                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,54,"Badly falls: %i",mWorld->mainStatistics.badlyFalls);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,74,"Food eaten: %i",mWorld->mainStatistics.foodEaten);
				
				//RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                //RenderManager::InstancePtr()->DebugPrint(200,94,"Experience accumulated: %i",mWorld->mainStatistics.experienceAccumulated);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,94,"Jumps: %i",mWorld->mainStatistics.jumps);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,114,"Dies: %i",mWorld->mainStatistics.dies);
            }
            DrawText(240,24,GU_COLOR(1,1,1,1),default_size,"Statistics");

            RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
            RenderManager::InstancePtr()->DebugPrint(240,254,"Page: %i / 2",statisticsPage+1);
        }
        if(language == RUSSIAN)
        {
            if(statisticsPage == 0)
            {
				mRender->DebugPrint(30,15,"World seed is: %i",mWorld->worldSeed);
                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,54,"Bloques colocados: %i",mWorld->mainStatistics.blockPlaced);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,74,"Bloques destruidos: %i",mWorld->mainStatistics.blockDestroyed);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,94,"Dias transcurridos: %i d",mWorld->mainStatistics.daysInGame);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,114,"Minutos jugados: %i m",mWorld->mainStatistics.minutesPlayed);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,134,"Items crafteados: %i",mWorld->mainStatistics.itemsCrafted);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,154,"Items fundidos: %i",mWorld->mainStatistics.itemsSmelted);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,174,"Suelo arado: %i",mWorld->mainStatistics.soilPlowed);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,194,"Cultivos cultivados: %i",mWorld->mainStatistics.cropsGrowned);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,214,"Arboles cultivados: %i",mWorld->mainStatistics.treesGrowned);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,234,"Dano infligido: %i",mWorld->mainStatistics.damageRecieved);
            }
            else
            {
				mRender->DebugPrint(30,15,"World seed is: %i",mWorld->worldSeed);
                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,54,"Caidas: %i",mWorld->mainStatistics.badlyFalls);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,74,"Comida ingerida: %i",mWorld->mainStatistics.foodEaten);
				
				//RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                //RenderManager::InstancePtr()->DebugPrint(200,94,"Experience accumulated: %i",mWorld->mainStatistics.experienceAccumulated);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,94,"Saltos: %i",mWorld->mainStatistics.jumps);

                RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.8,0.8,0.8,1),0,0x00004000|0x00000000);
                RenderManager::InstancePtr()->DebugPrint(200,114,"Muertes: %i",mWorld->mainStatistics.dies);
            }
            DrawText(240,24,GU_COLOR(1,1,1,1),default_size,"Estadisticas");

            RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(1,1,1,1),0,0x00004000|0x00000200);
            RenderManager::InstancePtr()->DebugPrint(240,254,"Pagina: %i / 2",statisticsPage+1);
        }
    }

    //debug info
    if(devMode)
    {
        mRender->SetFontStyle(default_size,0xFFFFFFFF,0,0x00000000);

       /* mRender->DebugPrint(240,34,"world iron ore %i",mWorld->mainWorldVariables.worldIronOre);
        mRender->DebugPrint(240,44,"world coal ore %i",mWorld->mainWorldVariables.worldCoalOre);
        mRender->DebugPrint(240,54,"world golden ore %i",mWorld->mainWorldVariables.worldGoldenOre);
        mRender->DebugPrint(240,64,"world diamond ore %i",mWorld->mainWorldVariables.worldDiamondOre);
        mRender->DebugPrint(240,74,"world redstone ore %i",mWorld->mainWorldVariables.worldRedstoneOre);
        mRender->DebugPrint(240,84,"world lapis ore %i",mWorld->mainWorldVariables.worldLapisOre);
        mRender->DebugPrint(240,94,"world clay %i",mWorld->mainWorldVariables.worldClay); */

        if(dt > 0.0f)
        {
            tick_fps += dt;
            if(tick_fps > dt/500.0f) // each 2 ms
            {
                pre_fps += (1.0f/dt);
                tick_fps - dt/500.0f;
                ticks ++;
            }

            if(ticks == 50)
            {
                average_fps = pre_fps/ticks;
                tick_fps = 0;
                pre_fps = 0;
                ticks = 0;
            }
        }
        mRender->DebugPrint(20,24,"fps: %d",average_fps+1);
        //mRender->DebugPrint(20,34,"cpu: %d%%",mRender->GetCpuUsage());
        //mRender->DebugPrint(20,44,"gpu: %d%%",mRender->GetGpuUsage());
		mRender->DebugPrint(20,34,"pos x: %d%%",invXPosition);
        mRender->DebugPrint(20,44,"pos y: %d%%",invYPosition);
        /*mRender->DebugPrint(20,54,"free memory: %d kb (%d mb)",freeMemory/1024,(freeMemory/1024)/1024);
        mRender->DebugPrint(20,64,"polygons: %d",(mWorld->GetDrawntTrianglesCount() / 3));
        mRender->DebugPrint(20,74,"vertices: %d",mWorld->GetDrawntTrianglesCount());
        mRender->DebugPrint(20,84,"world time: %f",mWorld->worldDayTime);
        mRender->DebugPrint(20,94,"camera angle: %f",fppCam->horAngle);

        mRender->DebugPrint(20,104,"player.x: %f",playerPosition.x);
        mRender->DebugPrint(20,114,"player.y: %f",playerPosition.y);
        mRender->DebugPrint(20,124,"player.z: %f",playerPosition.z);

        mRender->DebugPrint(20,134,"walkingonground: %d",walkingOnGround);
        mRender->DebugPrint(20,144,"veloc.y: %f",playerVelocity.y);

        mRender->DebugPrint(20,164,"skyTime: %f",mWorld->skyTime);

       // mRender->DebugPrint(20,154,"Chunks in process : %d",mWorld->toUpdate.size());

        if(mWorld->mZombies.size() > 0)
        {
            mRender->DebugPrint(20,184,"size of vector zombies : %d",mWorld->mZombies.size());
        }
        if(mWorld->mCows.size() > 0)
        {
            mRender->DebugPrint(20,194,"size of vector cows : %d",mWorld->mCows.size());
        }

        if(mWorld->mPigs.size() > 0)
        {
            mRender->DebugPrint(20,204,"size of vector pigs : %d",mWorld->mPigs.size());
        }

        if(mWorld->mDrops.size() > 0)
        {
            mRender->DebugPrint(20,214," size of vector Drops : %i", mWorld->mDrops.size());
        }
        if(mWorld->updatingChunks.size() > 0)
        {
            mRender->DebugPrint(20,224," size of vector updatingChunks : %i", mWorld->updatingChunks.size());
        }*/

        mRender->SetFontStyle(default_size,0xFFFFFFFF,0,0x00000200);
    }

    //end frame
    mRender->EndFrame();
}

//additional functions
void StatePlay::advancedBlit(int sx, int sy, int sw, int sh, int dx, int dy, int slice)
{
    int start, end;

    // blit maximizing the use of the texture-cache

    for (start = sx, end = sx+sw; start < end; start += slice, dx += slice)
    {
        TexturesPSPVertex16* vertices = (TexturesPSPVertex16*)sceGuGetMemory(2 * sizeof(TexturesPSPVertex16));
        int width = (start + slice) < end ? slice : end-start;

        vertices[0].u = start;
        vertices[0].v = sy;
        vertices[0].x = dx;
        vertices[0].y = dy;
        vertices[0].z = 0;

        vertices[1].u = start + width;
        vertices[1].v = sy + sh;
        vertices[1].x = dx + width;
        vertices[1].y = dy + sh;
        vertices[1].z = 0;

        sceGuDrawArray(GU_SPRITES,GU_TEXTURE_16BIT|GU_VERTEX_16BIT|GU_TRANSFORM_2D,2,0,vertices);
    }
}

bool StatePlay::TryToMove(Vector3 moveVector,float dt)
{
    // Build a "test vector" that is a little longer than the move vector.
    float moveLength = moveVector.magnitude();
    Vector3 testVector = moveVector;
    testVector.normalize();
    testVector = testVector * (moveLength + 0.2f);

    // Apply this test vector.
    Vector3 movePosition = playerPosition + testVector;
    Vector3 midBodyPoint = movePosition + Vector3(0, -mWorld->playerGrowth+1.1f, 0);
    Vector3 midLowBodyPoint = movePosition + Vector3(0, -mWorld->playerGrowth+0.6f, 0);
    Vector3 lowerBodyPoint = movePosition + Vector3(0, -mWorld->playerGrowth+0.1f, 0);

    float slowDown = 1.0f;
    if(footInLava || footInWater)
    {
        slowDown = 0.89f;
    }
    if(headInLava || headInWater )
    {
        slowDown = 0.66f;
    }
	if(playerInCobweb)
	{
		slowDown = 0.1f;
	}
	if(crouch)
	{
		slowDown = 0.4f;
	}

    if (!mWorld->SolidAtPointForPlayer(movePosition) && !mWorld->SolidAtPointForPlayer(lowerBodyPoint) && !mWorld->SolidAtPointForPlayer(midBodyPoint))
    {
        playerPosition = playerPosition + moveVector*slowDown;

        return true;
    }

    //teraz mo¿na robiæ ciekawe rzeczy

    testVector = moveVector;
    testVector.normalize();
    testVector = testVector * (moveLength + 0.42f);
    movePosition = playerPosition + testVector;
    midBodyPoint = movePosition + Vector3(0, -mWorld->playerGrowth+1.1f, 0);
    midLowBodyPoint = movePosition + Vector3(0, -mWorld->playerGrowth+0.6f, 0);
    lowerBodyPoint = movePosition + Vector3(0, -mWorld->playerGrowth+0.1f, 0);

    Vector3 footPosition = playerPosition + Vector3(0.0f, -1.65f, 0.0f);

    unsigned char lowerBlock = mWorld->BlockAtPoint(lowerBodyPoint);
    unsigned char midLowBlock = mWorld->BlockAtPoint(midLowBodyPoint);
    unsigned char midBlock = mWorld->BlockAtPoint(midBodyPoint);
    unsigned char upperBlock = mWorld->BlockAtPoint(movePosition);


    if ((mWorld->blockTypes[lowerBlock].blockModel == 1 || (mWorld->blockTypes[lowerBlock].blockModel >= 6 && mWorld->blockTypes[lowerBlock].blockModel <= 9)) &&
        mWorld->blockTypes[midLowBlock].solid == false &&
        mWorld->blockTypes[midBlock].solid == false &&
        mWorld->blockTypes[upperBlock].solid == false)
    {
        playerVelocity.y = CLIMBVELOCITY*1.29f;
        if (mWorld->SolidAtPointForPlayer(footPosition))
            playerPosition.y += 0.13f;
        return true;
    }

    unsigned char footBlock = mWorld->GetBlock(footPosition.x,footPosition.y,footPosition.z);
    if (mWorld->blockTypes[footBlock].blockModel == 1 &&
        mWorld->blockTypes[lowerBlock].solid == true &&
        mWorld->blockTypes[midLowBlock].solid == false &&
        mWorld->blockTypes[midBlock].solid == false &&
        mWorld->blockTypes[upperBlock].solid == false)
    {
        playerVelocity.y = CLIMBVELOCITY*1.29f;
        if(mWorld->SolidAtPointForPlayer(footPosition))
            playerPosition.y += 0.13f;
        return true;
    }

    int cameraAngle = (int)fppCam->horAngle;
    if(moveForward)
    {
        if(mWorld->GetBlock(playerPosition.x,playerPosition.y-1.55f,playerPosition.z) == Ladder1::getID())
        {
            if(cameraAngle > 125 && cameraAngle < 235)
            {
                if (upperBlock != 0 || lowerBlock != 0 || midBlock != 0)
                {
                    playerVelocity.y = CLIMBVELOCITY;
                    if (mWorld->SolidAtPointForPlayer(footPosition))
                        playerPosition.y += 0.1f;
                    return true;
                }
            }
        }

        if(mWorld->GetBlock(playerPosition.x,playerPosition.y-1.55f,playerPosition.z) == Ladder2::getID())	//ladder
        {
            if(cameraAngle < 55 || cameraAngle > 305)
            {
                if (upperBlock != 0 || lowerBlock != 0 || midBlock != 0)
                {
                    playerVelocity.y = CLIMBVELOCITY;
                    if (mWorld->SolidAtPointForPlayer(footPosition))
                        playerPosition.y += 0.1f;
                    return true;
                }
            }
        }

        if(mWorld->GetBlock(playerPosition.x,playerPosition.y-1.55f,playerPosition.z) == Ladder3::getID())
        {
            if(cameraAngle > 215 && cameraAngle < 325)
            {
                if (upperBlock != 0 || lowerBlock != 0 || midBlock != 0)
                {
                    playerVelocity.y = CLIMBVELOCITY;
                    if (mWorld->SolidAtPointForPlayer(footPosition))
                        playerPosition.y += 0.1f;
                    return true;
                }
            }
        }

        if(mWorld->GetBlock(playerPosition.x,playerPosition.y-1.55f,playerPosition.z) == Ladder4::getID())
        {
            if(cameraAngle > 35 && cameraAngle < 145)
            {
                if (upperBlock != 0 || lowerBlock != 0 || midBlock != 0)
                {
                    playerVelocity.y = CLIMBVELOCITY;
                    if (mWorld->SolidAtPointForPlayer(footPosition))
                        playerPosition.y += 0.1f;
                    return true;
                }
            }
        }
    }

    return false;
}

bool StatePlay::keyPressed(int currentKey)
{
    //analog reset
    if(analogUp)
    {
        if(mSystemMgr->GetAnalogY() < InputHelper::Instance()->analogYup)
            analogUp = false;
    }
    if(analogDown)
    {
        if(mSystemMgr->GetAnalogY() > InputHelper::Instance()->analogYdown)
            analogDown = false;
    }
    if(analogLeft)
    {
        if(mSystemMgr->GetAnalogX() > InputHelper::Instance()->analogXleft)
            analogLeft = false;
    }
    if(analogRight)
    {
        if(mSystemMgr->GetAnalogX() < InputHelper::Instance()->analogXright)
            analogRight = false;
    }

    //keys
    if(currentKey == 0)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_UP);
    if(currentKey == 1)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_DOWN);
    if(currentKey == 2)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_LEFT);
    if(currentKey == 3)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_RIGHT);

    if(currentKey == 4)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_TRIANGLE);
    if(currentKey == 5)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_CROSS);
    if(currentKey == 6)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_SQUARE);
    if(currentKey == 7)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_CIRCLE);

    if(currentKey == 8)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_LTRIGGER);
    if(currentKey == 9)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_RTRIGGER);
    if(currentKey == 10)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_SELECT);
    if(currentKey == 15)//cross
        return mSystemMgr->KeyPressed(PSP_CTRL_START);

    //analog stick....
    if(currentKey == 12)//cross
    {
        if(!analogUp)
        {
            if(mSystemMgr->GetAnalogY() > InputHelper::Instance()->analogYup)
            {
                analogUp = true;
                return true;//analog up
            }
        }
    }
    if(currentKey == 11)//cross
    {
        if(!analogDown)
        {
            if(mSystemMgr->GetAnalogY() < InputHelper::Instance()->analogYdown)
            {
                analogDown = true;
                return true;//analog down
            }
        }
    }
    if(currentKey == 13)//cross
    {
        if(!analogLeft)
        {
            if(mSystemMgr->GetAnalogX() < InputHelper::Instance()->analogXleft)
            {
                analogLeft = true;
                return true;//analog left
            }
        }
    }
    if(currentKey == 14)//cross
    {
        if(!analogRight)
        {
            if(mSystemMgr->GetAnalogX() > InputHelper::Instance()->analogXright)
            {
                analogRight = true;
                return true;//analog right
            }
        }
    }

    return false;
}

bool StatePlay::keyHold(int currentKey)
{
    if(currentKey == 0)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_UP);
    if(currentKey == 1)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_DOWN);
    if(currentKey == 2)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_LEFT);
    if(currentKey == 3)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_RIGHT);

    if(currentKey == 4)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_TRIANGLE);
    if(currentKey == 5)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_CROSS);
    if(currentKey == 6)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_SQUARE);
    if(currentKey == 7)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_CIRCLE);

    if(currentKey == 8)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_LTRIGGER);
    if(currentKey == 9)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_RTRIGGER);
    if(currentKey == 10)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_SELECT);
    if(currentKey == 15)//cross
        return mSystemMgr->KeyHold(PSP_CTRL_START);

    //analog stick....
    if(currentKey == 12)//cross
        return (mSystemMgr->GetAnalogY() > InputHelper::Instance()->analogYup);//analog up
    if(currentKey == 11)//cross
        return (mSystemMgr->GetAnalogY() < InputHelper::Instance()->analogYdown);//analog down
    if(currentKey == 13)//cross
        return (mSystemMgr->GetAnalogX() < InputHelper::Instance()->analogXleft);//analog left
    if(currentKey == 14)//cross
        return (mSystemMgr->GetAnalogX() > InputHelper::Instance()->analogXright);//analog right

    return false;
}


void StatePlay::HungerTime()
{
    if(mWorld->HG >= 16)
    {
        if (rand() % 4 == 0)
        {
            mWorld->HG -= 1;
        }
    }
    else
    {
        mWorld->HG -= 1;
    }
    if(mWorld->HG < 0)
    {
        mWorld->HG = 0;
    }
}

void StatePlay::FireBurning()
{
	tickFR += dt;
	if(tickFR >= 0.0f)
	{
		if(mWorld->HP > 0)
		{
			if(mWorld->FR <= 0)
			{
				burning = false;
			}
			if(mWorld->FR > 0)
			{
				mWorld->FR -= 1;
			}
		}
		tickFR -= 1.0f;
	}
}

void StatePlay::HealthTime()
{
    if(mWorld->HG <= 0 && mWorld->HP > 0)
    {
        if(mWorld->mainOptions.difficult == 0)
        {
            if(mWorld->HP > 1)
            {
                if(mWorld->HP == 2)
                {
                    HurtPlayer(1);
                }
                else
                {
                    HurtPlayer(2);
                }
            }
        }
        else
        {
            HurtPlayer(2);
        }
    }

    if(mWorld->HG >= 18)
    {
        if(mWorld->HP > 0)
        {
            mWorld->HP += 1;
        }
    }

    if(mWorld->HP > 20)
    {
        mWorld->HP = 20;
    }
}

void StatePlay::OxygenTime()
{
    if(mWorld->HP > 0)
    {
        if(mWorld->OS <= 0)
        {
            HurtPlayer(2);
        }
        if(mWorld->OS > 0)
        {
            mWorld->OS -= 1;
        }
    }
}

void StatePlay::HurtPlayer(float damage)
{
    mSoundMgr->PlayHitSound();
    last_HP = mWorld->HP;
    mWorld->HP -= damage;
    mWorld->mainStatistics.damageRecieved += damage;

    if(mWorld->HP > 0)
    {
        hurt = true;
        hurt_time = 1.0f;
        hpAnim = 0.0f;
    }
}

void StatePlay::HurtPlayerConsiderArmor(float damage)
{
    mSoundMgr->PlayHitSound();
    last_HP = mWorld->HP;
    mWorld->HP -= damage*(1.0f-(mWorld->AP/25.0f));
    mWorld->mainStatistics.damageRecieved += damage*(1.0f-(mWorld->AP/25.0f));

    if(mWorld->HP > 0)
    {
        hurt = true;
        hurt_time = 1.0f;
        hpAnim = 0.0f;
    }
}

void StatePlay::PutInInventory(int id, int num, bool st)
{
    bool ok;
    ok = false;
    for(int j = 27; j <= 35; j++)
    {
        if (mWorld->invSt[j]==1)
        {
            if (mWorld->invId[j]== id)
            {
                if(mWorld->invAm[j] + num <= 64)
                {
                    mWorld->invAm[j] += num;
                    chestId = -1;
                    ok = true;
                    break;
                }
                else
                {
                    continue;
                }
            }
        }

        if (mWorld->invId[j]== -1)
        {
            mWorld->invSt[j] = st;
            mWorld->invId[j] = id;
            mWorld->invAm[j] = num;
            ok = true;
            break;
        }
    }

    if(ok == false)
    {
        for(int j = 0; j <= 26; j++)
        {
            if (mWorld->invSt[j]==1)
            {
                if (mWorld->invId[j]== id)
                {
                    if(mWorld->invAm[j] + num <= 64)
                    {
                        mWorld->invAm[j] += num;
                        chestId = -1;
                        break;
                        return;
                    }
                    else
                    {
                        continue;
                    }
                }
            }

            if (mWorld->invId[j]== -1)
            {
                mWorld->invSt[j] = st;
                mWorld->invId[j] = id;
                mWorld->invAm[j] = num;
                break;
                return;
            }
        }
    }
}

void StatePlay::DrawText(int x,int y, unsigned int color, float size, const char *message, ...)
{
    RenderManager::InstancePtr()->SetFontStyle(size,color,0,0x00000200|0x00000000);
    RenderManager::InstancePtr()->DebugPrint(x,y,message);
}

void StatePlay::DrawText2(int x,int y, unsigned int color, float size, const char *message, ...)
{
    RenderManager::InstancePtr()->SetFontStyle(size,color,0,0x00004000|0x00000000);
    RenderManager::InstancePtr()->DebugPrint(x,y,message);
}

void StatePlay::DrawAmount(int x,int y, int amount) //ui
{
	int dozens, units;

	dozens = floorf(amount / 10.0f);
	units = amount % 10;

	RenderManager::InstancePtr()->SetFont(0);
	if(dozens != 0)
	{
		RenderManager::InstancePtr()->SetFontStyle(0.343,GU_COLOR(0.25f,0.25f,0.25f,1.0f),0,0x00000400|0x00004000);
		RenderManager::InstancePtr()->DebugPrint(x+1,y+1,"%i",dozens);
		RenderManager::InstancePtr()->SetFontStyle(0.343,GU_COLOR(1.0f,1.0f,1.0f,1.0f),0,0x00000400|0x00004000);
		RenderManager::InstancePtr()->DebugPrint(x,y,"%i",dozens);
	}

	RenderManager::InstancePtr()->SetFontStyle(0.343,GU_COLOR(0.25f,0.25f,0.25f,1.0f),0,0x00000400|0x00004000);
	RenderManager::InstancePtr()->DebugPrint(x+7,y+1,"%i",units);
	RenderManager::InstancePtr()->SetFontStyle(0.343,GU_COLOR(1.0f,1.0f,1.0f,1.0f),0,0x00000400|0x00004000);
	RenderManager::InstancePtr()->DebugPrint(x+6,y,"%i",units);
	RenderManager::InstancePtr()->SetDefaultFont();
		
	///	/* --- HOLA BUSCA CODIGOS :) SALUDOS DESDE ECUADOR --- */	///
}

