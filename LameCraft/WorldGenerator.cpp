#include "WorldGenerator.h"
#include "CraftWorld2.h"
#include "LoadingScreen.h"

#include <Aurora/Utils/Logger.h>

#define PI 3.1415926535897f

#define DEFAULT 0
#define SUPERFLAT 1

using namespace noisepp;

void WorldGenerator::initRandompMap(int worldSize, int worldHeight, int chunkSize, CraftWorld *world, bool makeTrees,bool makeDungeons,bool makeBonus,bool makeCaves, unsigned int seedII, int worldType)
{
    LoadingScreen* loading = new LoadingScreen();

    loading->readiness = 1;
    loading->stateName = 1;

    //inicjuj mape
    int WORLD_SIZE = worldSize;
    int CHUNK_SIZE = chunkSize;
    int WORLD_HEIGHT = 98;

    if (seedII == 0)
    {
        seedII = rand() % 1000000;
    }
    unsigned int seed = seedII;
    srand(seed);

    world->worldSeed = seed;
    SetSeed(seed);

    int grass_;
    bool genGrass = true;

    if(worldType == DEFAULT)
    {
		
	  /*int octave_count = 6;
        int waterLevel = 62;
        int middleHeight = 68;

        Real persistence = 0.05;
        Real frequency = 1.4;
        Real scale = 2.12;
        Real lacunarity = 2.0;*/
		
        int octave_count = 1;
        int waterLevel = 62;
        int middleHeight = 68;

        Real persistence = 0.05;
        Real frequency = 1.4;
        Real scale = 2.12;
        Real lacunarity = 2.0;

        {
            std::string path = "Terrain/options.txt";
            std::string str;
            std::string file_contents;
            std::string flag = "";

            std::ifstream file(path.c_str());

            if (file.is_open())
            {
                while (std::getline(file, str))
                {
                    if(flag.compare("<water_level>") == 0)
                    {
                        file_contents = "";
                        file_contents += str;
                        file_contents.push_back('\n');

                        waterLevel = std::atoi(file_contents.c_str());
                        flag = "";

                        continue;
                    }
                    if(flag.compare("<grass>") == 0)
                    {
                        file_contents = "";
                        file_contents += str;
                        file_contents.push_back('\n');

                        grass_ = std::atoi(file_contents.c_str());
                        flag = "";

                        continue;
                    }

                    flag.clear();
                    flag += str;
                    flag.erase(flag.end()-1);
                }
            }

            file.close();
        }

        if(grass_ == 0)
        {
            genGrass = false;
        }

        PerlinModule* perlin = new PerlinModule();
        perlin->setQuality(noisepp::NOISE_QUALITY_HIGH);
		
		
		
        float *dataBeach = new float[WORLD_SIZE * WORLD_SIZE];
        perlin->setSeed(seed+10);
        perlin->setOctaveCount(4);
        perlin->setPersistence(0.17f);
        perlin->setFrequency(0.8f);
        perlin->setScale(4.5f);
        perlin->setLacunarity(3.5f);

        noisepp::utils::PlaneBuilder2D builderTrees;
        builderTrees.setModule(perlin);
        builderTrees.setSize(WORLD_SIZE, WORLD_SIZE);
        builderTrees.setBounds(0, 0, 4, 5.5f);
        builderTrees.setDestination(dataBeach);
        builderTrees.build ();



        loading->stateName = 1; // terrain generation
        for (int z = 0; z < WORLD_SIZE; z++)
        {
            for (int x = 0; x < WORLD_SIZE; x++)
            {
                //int distanceToCenter = world->FastDistance2d(abs(x-WORLD_SIZE/2),abs(z-WORLD_SIZE/2));
                loading->readiness = ((float)(x+z*WORLD_SIZE)/(66503.0f))*100;

				//int Height  = data[x + z*WORLD_SIZE]* 128/12 + middleHeight;
                float HillFrequency = (GetValue2D(x,z,1,0.05f,1)+1.0f)/2.0f;

				int Height = (InterpolateElevationNoise(x,z) + InterpolateRoughnessNoise(x,z)*GetValue2D3(x,z,6,0.1f,1))*64*HillFrequency+62;

                if(Height <= 48)
                {
                    Height = 48-(48-Height)/2;
                }
                if(Height >= 98)
                {
                   Height = 98+(Height-98)/2;
                }

                float perlinHeight = dataBeach[x + z*WORLD_SIZE]* 128.0f/12.0f + 69;

                for (int y = Height; y >= 0; y--)
                {
                    if(y == Height)
                    {
                        if(world->GetBlock(x,y+1,z) == 0)
                        {
                            if(Height <= waterLevel+2)
                            {
                                world->GetBlock(x, y, z) = SandBlock::getID();
                                if(perlinHeight > 75.0f)
                                {
                                    world->GetBlock(x, y, z) = GrassBlock::getID();
                                    if(y < waterLevel)
                                    {
                                        world->GetBlock(x, y, z) = DirtBlock::getID();
                                    }
                                }
                            }
                            else
                            {
                                world->GetBlock(x, y, z) = GrassBlock::getID();
                            }
                        }
                    }
                    else if(y < Height-4)
                    {
                        if(world->GetBlock(x,y,z) == RockBlock::getID())
                        {
                            break;
                        }
                        world->GetBlock(x, y, z) = RockBlock::getID();
                    }
                    else
                    {
                        if(Height <= waterLevel+2)
                        {
                            world->GetBlock(x, y, z) = SandBlock::getID();
                            if(perlinHeight > 75.0f)
                            {
                                world->GetBlock(x, y, z) = DirtBlock::getID();
                            }
                        }
                        else
                        {
                            world->GetBlock(x, y, z) = DirtBlock::getID();
                        }
                    }
                }

                if(Height < waterLevel)
                {
                    for (int y = waterLevel; y > Height; y--)
                    {
                        world->GetBlock(x, y, z) = WaterBlock::getID();
                    }
                }
            }
        }

       // delete []data;
        delete []dataBeach;

        loading->readiness = 60;
        loading->stateName = 2; // cave generation

        if(makeCaves)
        {
            float *dataVer = new float[150];
            float *dataHor = new float[150];
            noisepp::utils::PlaneBuilder2D wormBuilder;

            int probX = -1;
            int probY = -1;
            int probZ = -1;

            int numWorms = 7 + rand() % 5;
            for(int i = 0; i < numWorms; i++)
            {
                int steps = 50 + rand() % 100;
                int radius = 2.5f;

                perlin->setSeed(seed+1030+i);
                perlin->setOctaveCount(6);
                perlin->setPersistence(0.6f);
                perlin->setFrequency(1.2f);
                perlin->setScale(3);
                perlin->setLacunarity(1.8f);

                wormBuilder.setModule(perlin);
                wormBuilder.setSize(1, 150);
                wormBuilder.setBounds(-3, -3, 3, 3);

                wormBuilder.setDestination(dataVer);
                wormBuilder.build();


                perlin->setSeed(seed+2030+i);
                perlin->setOctaveCount(6);
                perlin->setPersistence(0.6f);
                perlin->setFrequency(1.2f);
                perlin->setScale(3);
                perlin->setLacunarity(1.8f);

                wormBuilder.setModule(perlin);
                wormBuilder.setSize(1, 150);
                wormBuilder.setBounds(-3, -3, 3, 3);

                wormBuilder.setDestination(dataHor);
                wormBuilder.build();

                int x,y,z;

                if(probX == -1)
                {
                    x = 10 + rand() % (WORLD_SIZE-10);
                    z = 10 + rand() % (WORLD_SIZE-10);
                    y = 6 + rand() % (world->groundHeight(x,z)-32);
                }
                else
                {
                    x = probX;
                    y = probY;
                    z = probZ;

                    probX = -1;
                    probY = -1;
                    probZ = -1;
                }

                if(rand() % 3 == 0) // ground cave
                {
                    y = world->groundHeight(x,z)+2;
                }

                bool reverseX = false;
                bool reverseY = false;
                bool reverseZ = false;

                for(int j = 0; j < steps; j++)
                {


                    radius = 1.5f+(rand()%14)/10.0f;

                    if(j == (int)(steps/2) && rand() % 3 == 0)
                    {
                        probX = x;
                        probY = y;
                        probZ = z;
                    }

                    if(rand()%32 == 0)
                    {
                        continue;
                    }

                    bool haveWater = false;
                    for(int xx = x-radius; xx <= x+radius; xx ++)
                    {
                        for(int zz = z-radius; zz <= z+radius; zz ++)
                        {
                            for(int yy = y-radius; yy <= y+radius; yy ++)
                            {
                                if(world->GetBlock(xx,yy,zz) == IceBlock::getID() || world->GetBlock(xx,yy,zz) == WaterBlock::getID())
                                {
                                    haveWater = true;
                                }
                            }
                        }
                    }

                    if(haveWater == false)
                    {
                        world->BuildBlockSphere(radius,0,x,y,z);

                        if(rand()%14 == 0)
                        {
                            radius = 1.5f+(rand()%10)/10.0f;
                            world->BuildBlockSphere(radius,0,x+radius*1.5f,y,z,RockBlock::getID());
                            world->BuildBlockSphere(radius,0,x+radius*1.5f,y,z+radius*1.5f,RockBlock::getID());
                            world->BuildBlockSphere(radius,0,x+radius*1.5f,y+radius*1.5f,z+radius*1.5f,RockBlock::getID());
                            world->BuildBlockSphere(radius,0,x,y-radius*1.5f,z+radius*1.5f,RockBlock::getID());
                            world->BuildBlockSphere(radius,0,x,y+radius*1.5f,z+radius*1.5f,RockBlock::getID());
                        }
                    }

                    reverseX == false ? x += world->LengthDirX((dataHor[j]),(dataVer[j]))*radius*1.5f : x -= world->LengthDirX((dataHor[j]),(dataVer[j]))*radius*1.5f;
                    reverseY == false ? y += world->LengthDirY((dataHor[j]),(dataVer[j]))*radius*1.5f : y -= world->LengthDirY((dataHor[j]),(dataVer[j]))*radius*1.5f;
                    reverseZ == false ? z += world->LengthDirZ((dataHor[j]),(dataVer[j]))*radius*1.5f : z -= world->LengthDirZ((dataHor[j]),(dataVer[j]))*radius*1.5f;

                    if(y <= 6)
                    {
                        y = 7;
                        reverseY = !reverseY;
                    }

                    if(x > WORLD_SIZE-5 || x <= 4)
                    {
                        if(x > 30)
                        {
                            x = WORLD_SIZE-6;
                        }
                        else
                        {
                            x = 5;
                        }
                        reverseX = !reverseX;
                    }

                    if(z > WORLD_SIZE-5 || z <= 4)
                    {
                        if(z > 30)
                        {
                            z = WORLD_SIZE-6;
                        }
                        else
                        {
                            z = 5;
                        }
                        reverseZ = !reverseZ;
                    }
                }
            }
            delete [] dataVer;
            delete [] dataHor;
        }

        initDirt(WORLD_SIZE, world);
        initGravel(WORLD_SIZE, world);
        initErosion(WORLD_HEIGHT, world);
		initPools(WORLD_SIZE, world);
        initBiome(WORLD_SIZE,world,perlin,seed);

        initPumpkins(WORLD_SIZE,world);
        initCanes(WORLD_SIZE,world,waterLevel);

        loading->readiness = 80;
        loading->stateName = 4; // trees planting
		
		if(makeDungeons)
        {
			initEstructuras(WORLD_SIZE, world);
		}
		
        initFlowers(WORLD_SIZE, world);

        if(makeTrees)
        {
            initRandomTrees(WORLD_SIZE, world);
        }

        loading->readiness = 85;
        loading->stateName = 5; // ore placing

        initBedrock(WORLD_SIZE, world);
        initClay(WORLD_SIZE, world);

        loading->readiness = 90;
        loading->stateName = 6;

        if(genGrass)
        {
            initGrass(WORLD_SIZE, world);
        }

        world->initWorldBlocksLight();

        loading->readiness = 100;
        loading->stateName = 7;
		
        initOre(WORLD_SIZE,world,CoalOre::getID(),0.83,2,80,3);
        initOre(WORLD_SIZE,world,IronOre::getID(),0.57,2,61,3);
        initOre(WORLD_SIZE,world,GoldBlock::getID(),0.25,2,30,2);
        initOre(WORLD_SIZE,world,DiamondOre::getID(),0.094,1,15,2);
        initOre(WORLD_SIZE,world,LapisOre::getID(),0.13,1,34,3);
        initOre(WORLD_SIZE,world,RedstoneOre::getID(),0.6,1,14,3);
		
		if(makeDungeons)
		{
			initDungeons(WORLD_SIZE, world);
		}
		
		initLavaLakes(WORLD_SIZE, world);
        initWaterLakes(WORLD_SIZE, world);

        /*for(int i = 0; i <= 1; i++)
        {
            int spawnX,spawnY,spawnZ;

            spawnX = 20 + rand() % (WORLD_SIZE-20);
            spawnZ = 20 + rand() % (WORLD_SIZE-20);
            spawnY = world->groundHeightWater(spawnX,spawnZ);

            world->SpawnCow(spawnX+0.5f,spawnY+2.0f,spawnZ+0.5f);
            world->SpawnCow(spawnX+2.0f+0.5f,world->groundHeightWater(spawnX+2.0f,spawnZ+2.0f)+2.0f,spawnZ+2.0f+0.5f);
        }

        for(int i = 0; i <= 1; i++)
        {
            int spawnX,spawnY,spawnZ;

            spawnX = 20 + rand() % (WORLD_SIZE-20);
            spawnZ = 20 + rand() % (WORLD_SIZE-20);
            spawnY = world->groundHeightWater(spawnX,spawnZ);

            world->SpawnChicken(spawnX+0.5f,spawnY+2.0f,spawnZ+0.5f);
            world->SpawnChicken(spawnX+2.0f+0.5f,world->groundHeightWater(spawnX+2.0f,spawnZ+2.0f)+2.0f,spawnZ+2.0f+0.5f);
        }

        for(int i = 0; i < 1; i++)
        {
            int spawnX,spawnY,spawnZ;

            spawnX = 30 + rand() % (WORLD_SIZE-30);
            spawnZ = 30 + rand() % (WORLD_SIZE-30);
            spawnY = world->groundHeightWater(spawnX,spawnZ);

            world->SpawnSheep(spawnX+0.5f,spawnY+2.0f,spawnZ+0.5f);
            world->SpawnSheep(spawnX+2.5f,world->groundHeightWater(spawnX+2.0f,spawnZ+2.0f)+2.0f,spawnZ+2.5f);
        }

        for(int i = 0; i < 1; i++)
        {
            int spawnX,spawnY,spawnZ;

            spawnX = 30 + rand() % (WORLD_SIZE-30);
            spawnZ = 30 + rand() % (WORLD_SIZE-30);
            spawnY = world->groundHeightWater(spawnX,spawnZ);

            world->SpawnPig(spawnX+0.5f,spawnY+2.0f,spawnZ+0.5f);
            world->SpawnPig(spawnX+2.5f,world->groundHeightWater(spawnX+2.0f,spawnZ+2.0f)+2.0f,spawnZ+2.5f);
        }*/

        int tries = 0;
        Vector3 newPlayerSpawnPosition = Vector3(1,0,1);
        while(tries < 3072)
        {
            int border = 48;
            if(tries > 512)
            {
                border -= 16;
            }
            if(tries > 1024)
            {
                border -= 16;
            }
            if(tries > 1536)
            {
                border -= 8;
            }

            newPlayerSpawnPosition = Vector3(border+rand()%(WORLD_SIZE-border-1),0,border+rand()%(WORLD_SIZE-border-1));
            int groundBlock = world->GetBlock(newPlayerSpawnPosition.x,world->groundHeightWater(newPlayerSpawnPosition.x,newPlayerSpawnPosition.z),newPlayerSpawnPosition.z);
            if(groundBlock != WaterBlock::getID())
            {
                if(groundBlock == Snow2::getID())
                {
                    newPlayerSpawnPosition = Vector3(newPlayerSpawnPosition.x+0.5f,world->groundHeightWater(newPlayerSpawnPosition.x,newPlayerSpawnPosition.z)+world->playerGrowth-0.05f,newPlayerSpawnPosition.z+0.5f);
                }
                else
                {
                    newPlayerSpawnPosition = Vector3(newPlayerSpawnPosition.x+0.5f,world->groundHeightWater(newPlayerSpawnPosition.x,newPlayerSpawnPosition.z)+world->playerGrowth-0.05f+1,newPlayerSpawnPosition.z+0.5f);
                }
                world->SetPlayerSpawnPosition(newPlayerSpawnPosition);
                break;
            }
            if(tries == 3071)
            {
                world->SetPlayerSpawnPosition(Vector3(WORLD_SIZE/2+0.5f,world->groundHeightWater(WORLD_SIZE/2,WORLD_SIZE/2)+world->playerGrowth-0.05f-0.5f,WORLD_SIZE/2+0.5f));
            }
            tries ++;
        }
		if(makeBonus)
		{
			initCofreBonus(WORLD_SIZE, world);
		}

        delete perlin;
    }

    int spawnX = 128;
    int spawnZ = 128;
    int spawnY = world->groundHeightWater(spawnX,spawnZ);
    world->SpawnSheep(spawnX,spawnY+12.0f/16.0f,spawnZ);

    if(worldType == SUPERFLAT)
    {
        loading->readiness = 1;
        loading->stateName = 1;
        int superflatWorldHeight = 48;
        for(int x = 0; x < WORLD_SIZE; x++)
        {
            for(int z = 0; z < WORLD_SIZE; z++)
            {
                loading->readiness = ((float)(z+x*WORLD_SIZE)/(66503.0f))*100;
                for(int y = 0; y <= superflatWorldHeight; y++)
                {
                    if(y == 0)
                    {
                        world->GetBlock(x,y,z) = IronBlock::getID(); // bedrock
                    }
                    else
                    {
                        world->GetBlock(x,y,z) = RockBlock::getID(); // stone
                        if(rand() % 384 == 0)
                        {
                            //world->GetBlock(x,y,z) = 41 + rand() % 5; // stuff for survival mod // it is stupid
                        }

                        if(y >= superflatWorldHeight-2)
                        {
                            world->GetBlock(x,y,z) = DirtBlock::getID();
                            if(rand() % 384 == 0)
                            {
                                //world->GetBlock(x,y,z) = WoodBlock::getID(); // stuff for survival mod // it is stupid
                            }
                        }
                        if(y == superflatWorldHeight)
                        {
                            world->GetBlock(x,y,z) = GrassBlock::getID();
                        }
                    }
                }
            }
        }
        loading->stateName = 7;
        loading->stateName = 90;
        world->initWorldBlocksLight();
		if(makeDungeons)
		{
			//initEstructuras(WORLD_SIZE, world);
		}
		if(makeBonus)
		{
			initCofreBonus(WORLD_SIZE, world);
		}

        world->SetPlayerSpawnPosition(Vector3(WORLD_SIZE/2,world->groundHeight(WORLD_SIZE/2,WORLD_SIZE/2)+world->playerGrowth-0.05f+1,WORLD_SIZE/2));
    }

    loading->KillLoadingScreen();
    delete loading;
}

float WorldGenerator::InterpolateBiomeNoise(int wx, int wz)
{
	float q1, q2, q3, q4, xinterp, yinterp, r1, r2, r3;
	int minx, maxx, minz, maxz;

	minx = floor(wx/8.0f);
	maxx = ceil(wx/8.0f);

	minz = floor(wz/8.0f);
	maxz = ceil(wz/8.0f);

	q1 = (GetValue2D3(minx,minz,1,0.1f,1.0f)+1.0f)/2.0f;
	q2 = (GetValue2D3(maxx,minz,1,0.1f,1.0f)+1.0f)/2.0f;
	q3 = (GetValue2D3(minx,maxz,1,0.1f,1.0f)+1.0f)/2.0f;
	q4 = (GetValue2D3(maxx,maxz,1,0.1f,1.0f)+1.0f)/2.0f;

	xinterp = (wx/8.0f - minx);
	yinterp = (wz/8.0f - minz);

	r1 = func1.Linear(q1,q2,xinterp);
	r2 = func1.Linear(q3,q4,xinterp);

	r3 = func1.Linear(r1,r2,yinterp);

    if(r3 > 1)
    {
        r3 = 1.0f;
    }

    if(r3 < 0)
    {
        r3 = 0.0f;
    }

	return r3;
}

float WorldGenerator::InterpolateMoistureNoise(int wx, int wz)
{
	float q1, q2, q3, q4, xinterp, yinterp, r1, r2, r3;
	int minx, maxx, minz, maxz;

	minx = floor(wx/6.0f);
	maxx = ceil(wx/6.0f);

	minz = floor(wz/6.0f);
	maxz = ceil(wz/6.0f);

    q1 = (GetValue2D2(minx,minz,1,0.1f,1.0f)*2)+0.3f;
	q2 = (GetValue2D2(maxx,minz,1,0.1f,1.0f)*2)+0.3f;
	q3 = (GetValue2D2(minx,maxz,1,0.1f,1.0f)*2)+0.3f;
	q4 = (GetValue2D2(maxx,maxz,1,0.1f,1.0f)*2)+0.3f;


	xinterp = (wx/6.0f - minx);
	yinterp = (wz/6.0f - minz);

	r1 = func1.Linear(q1,q2,xinterp);
	r2 = func1.Linear(q3,q4,xinterp);

	r3 = func1.Linear(r1,r2,yinterp);

    if(r3 > 1)
    {
        r3 = 1.0f;
    }

    if(r3 < 0)
    {
        r3 = 0.0f;
    }

	return r3;
}

float WorldGenerator::InterpolateElevationNoise(int wx, int wz)
{
	float q1, q2, q3, q4, xinterp, yinterp, r1, r2, r3;
	int minx, maxx, minz, maxz;

	minx = floor(wx/4.0f);
	maxx = ceil(wx/4.0f);

	minz = floor(wz/4.0f);
	maxz = ceil(wz/4.0f);

	q1 = GetValue2D(minx,minz,1,0.1f,1.0f);
	q2 = GetValue2D(maxx,minz,1,0.1f,1.0f);
	q3 = GetValue2D(minx,maxz,1,0.1f,1.0f);
	q4 = GetValue2D(maxx,maxz,1,0.1f,1.0f);

	xinterp = (wx/4.0f - minx);
	yinterp = (wz/4.0f - minz);

	r1 = func1.Linear(q1,q2,xinterp);
	r2 = func1.Linear(q3,q4,xinterp);

	r3 = func1.Linear(r1,r2,yinterp);

	return r3;
}

float WorldGenerator::InterpolateRoughnessNoise(int wx, int wz)
{
	float q1, q2, q3, q4, xinterp, yinterp, r1, r2, r3;
	int minx, maxx, minz, maxz;

	minx = floor(wx/2.0f);
	maxx = ceil(wx/2.0f);

	minz = floor(wz/2.0f);
	maxz = ceil(wz/2.0f);

	q1 = GetValue2D(minx,minz,3,0.1f,1.0f);
	q2 = GetValue2D(maxx,minz,3,0.1f,1.0f);
	q3 = GetValue2D(minx,maxz,3,0.1f,1.0f);
	q4 = GetValue2D(maxx,maxz,3,0.1f,1.0f);

	xinterp = (wx/2.0f - minx);
	yinterp = (wz/2.0f - minz);

	r1 = func1.Linear(q1,q2,xinterp);
	r2 = func1.Linear(q3,q4,xinterp);

	r3 = func1.Linear(r1,r2,yinterp);

	return r3;
}

void WorldGenerator::SetSeed(int seed)
{
	func1.setSeed(seed);
	func2.setSeed(seed+500);
	func3.setSeed(seed+1000);
}

float WorldGenerator::GetValue3D(int wx, int wy, int wz, int octaves, float startFrequency, float startAmplitude)
{
	float DIVISOR = 1.0f / 16.0f;

	float x = wx * DIVISOR;
	float y = wy * DIVISOR;
	float z = wz * DIVISOR;

	float frequency = startFrequency;
	float amplitude = startAmplitude;

	float sum = 0.0f;

	for (int octave = 0; octave < octaves; octave++)
	{
		float noise = func1.sdnoise3(x * frequency, y * frequency, z * frequency,0,0,0);

		sum += amplitude * noise;
		frequency *= 2;
		amplitude *= 0.5;
	}

	return sum;
}

float WorldGenerator::GetValue2D(int wx, int wz, int octaves, float startFrequency, float startAmplitude)
{
	float DIVISOR = 1.0f / 16.0f;

	float x = wx * DIVISOR;
	float z = wz * DIVISOR;

	float frequency = startFrequency;
	float amplitude = startAmplitude;

	float sum = 0.0f;

	for (int octave = 0; octave < octaves; octave++)
	{
		float noise = func1.sdnoise2(x * frequency, z * frequency,0,0);

		sum += amplitude * noise;
		frequency *= 2;
		amplitude *= 0.5;
	}

	return sum;
}

float WorldGenerator::GetValue2D2(int wx, int wz, int octaves, float startFrequency, float startAmplitude)
{
	float DIVISOR = 1.0f / 16.0f;

	float x = wx * DIVISOR;
	float z = wz * DIVISOR;

	float frequency = startFrequency;
	float amplitude = startAmplitude;

	float sum = 0.0f;

	for (int octave = 0; octave < octaves; octave++)
	{
		float noise = func2.sdnoise2(x * frequency, z * frequency,0,0);

		sum += amplitude * noise;
		frequency *= 2;
		amplitude *= 0.5;
	}

	return sum;
}

float WorldGenerator::GetValue2D3(int wx, int wz, int octaves, float startFrequency, float startAmplitude)
{
	float DIVISOR = 1.0f / 16.0f;

	float x = wx * DIVISOR;
	float z = wz * DIVISOR;

	float frequency = startFrequency;
	float amplitude = startAmplitude;

	float sum = 0.0f;

	for (int octave = 0; octave < octaves; octave++)
	{
		float noise = func3.sdnoise2(x * frequency, z * frequency,0,0);

		sum += amplitude * noise;
		frequency *= 2;
		amplitude *= 0.5;
	}

	return sum;
}


void WorldGenerator::initErosion(int WORLD_SIZE, CraftWorld *world)
{
    int num = 13 + rand() % 8;

    for(int i = 0; i < num; i++)
    {
        int x = 6 + rand() % WORLD_SIZE-9;
        int z = 6 + rand() % WORLD_SIZE-9;
        int y = world->groundHeight(x,z);

        if(y <= 60)
        {
            i--;
            continue;
        }

        world->BuildBlockSphere(2+rand()%3,0,x,y+rand()%2,z);

        int xx, yy, zz;
        xx = x;
        yy = y+1;
        zz = z;

        for(int j = 0; j <= 10; j++)
        {
            world->GetBlock(xx,yy,zz) = 0;
            world->GetBlock(xx,yy-2,zz) = 0;
            world->GetBlock(xx,yy-1,zz) = 0;
            world->GetBlock(xx,yy+1,zz) = 0;

            world->GetBlock(xx+1,yy,zz) = 0;
            world->GetBlock(xx+1,yy-2,zz) = 0;
            world->GetBlock(xx+1,yy-1,zz) = 0;
            world->GetBlock(xx+1,yy+1,zz) = 0;

            world->GetBlock(xx-1,yy,zz) = 0;
            world->GetBlock(xx-1,yy-2,zz) = 0;
            world->GetBlock(xx-1,yy-1,zz) = 0;
            world->GetBlock(xx-1,yy+1,zz) = 0;

            world->GetBlock(xx,yy,zz+1) = 0;
            world->GetBlock(xx,yy-2,zz+1) = 0;
            world->GetBlock(xx,yy-1,zz+1) = 0;
            world->GetBlock(xx,yy+1,zz+1) = 0;

            world->GetBlock(xx,yy,zz-1) = 0;
            world->GetBlock(xx,yy-2,zz-1) = 0;
            world->GetBlock(xx,yy-1,zz-1) = 0;
            world->GetBlock(xx,yy+1,zz-1) = 0;



            switch(rand()%2)
            {
                case 0: xx += -1+rand()%3; break;
                case 1: zz += -1+rand()%3; break;
            }
        }

        for(int x2 = x-4; x2 <= x+4; x2++)
        {
            for(int z2 = z-4; z2 <= z+4; z2++)
            {
                for(int y2 = y-5; y2 <= y+5; y2++)
                {
                    if(world->GetBlock(x2,y2,z2) == 0)
                    {
                        if(y2 <= 60)
                        {
                            world->GetBlock(x2,y2,z2) = WaterBlock::getID();
                        }
                    }
                    if(world->GetBlock(x2,y2,z2) == DirtBlock::getID())
                    {
                        if(world->GetBlock(x2,y2+1,z2) == 0)
                        {
                            world->GetBlock(x2,y2,z2) = GrassBlock::getID();
                            if(y2 <= 62)
                            {
                                world->GetBlock(x2,y2,z2) = SandBlock::getID();
                            }
                        }
                        continue;
                    }
                    //if(erosionType > 0)
                   // {
                        if(world->GetBlock(x2,y2,z2) == RockBlock::getID())
                        {
                            if(world->GetBlock(x2,y2+1,z2) == 0)
                            {
                                if(y > 62)
                                {
                                    world->GetBlock(x2,y2+1,z2) = GrassBlock::getID();
                                }
                            }
                            continue;
                        }
                   // }

                }
            }
        }
    }
}

void WorldGenerator::initPools(int WORLD_SIZE, CraftWorld *world)
{
    int num = 3+rand()%5;

    for(int i = 0; i < num; i++)
    {
        int x = 9 + rand() % WORLD_SIZE-12;
        int z = 9 + rand() % WORLD_SIZE-12;
		int y = world->groundHeight(x,z);

		float radius = 4;
		float radius2 = 5;
		
		if(y <= 62)
        {
            i--;
            continue;
        }
		
		for(int x2 = x - radius2; x2 < x + radius2; x2++)
		{
			for(int y2 = y-6; y2 < y + radius2-6; y2++)	
			{
				for(int z2 = z - radius2; z2 < z + radius2; z2++)
				{
					if(world->GetBlock(x2,y2,z2) == 0 || world->GetBlock(x2,y2,z2) == 93 || world->GetBlock(x2,y2,z2) == TallGrass::getID())
					{
						world->GetBlock(x2,y2,z2) = DirtBlock::getID();
					}
				}
			}
		}
		if(rand() % 10 < 2)
		{
			world->BuildBlockSphere(radius,Lava::getID(),x,y,z);
			
			for(int x2 = x - radius2; x2 < x + radius2; x2++)
			{
				for(int y2 = y-1; y2 < y + radius2+1; y2++)
				{
					for(int z2 = z - radius2; z2 < z + radius2; z2++)
					{
						if(world->GetBlock(x2,y2,z2) == Lava::getID())
						{
							world->GetBlock(x2,y2,z2) = 0;
						}
					}
				}
			}
			
			world->UpdateLightAreaIn(Vector3(x,y,z));
		}
		else
		{
			world->BuildBlockSphere(radius,WaterBlock::getID(),x,y,z);
		
			for(int x2 = x - radius2; x2 < x + radius2; x2++)
			{
				for(int y2 = y-1; y2 < y + radius2+1; y2++)
				{
					for(int z2 = z - radius2; z2 < z + radius2; z2++)
					{
						if(world->GetBlock(x2,y2,z2) == WaterBlock::getID())
						{
							world->GetBlock(x2,y2,z2) = 0;
						}
					}
				}
			}
		}
    }
}

void WorldGenerator::initLavaLakes(int WORLD_SIZE, CraftWorld *world)
{
    char lavaLakesNum = 7 + rand() % 4;

    for(int i = 0; i <= lavaLakesNum; i++)
    {
        int x = 8 + rand() % WORLD_SIZE-9;
        int z = 8 + rand() % WORLD_SIZE-9;
        int y = 9 + rand() % 16;

        float radius = 3 + (rand() % 21)/10.0f;
        bool flag1 = true;

        for(int x2 = x - radius; x2 < x + radius; x2++)
        {
            for(int y2 = y - radius; y2 < y + radius; y2++)
            {
                for(int z2 = z - radius; z2 < z + radius; z2++)
                {
                    if(world->GetBlock(x2,y2,z2) == 0)
                    {
                        flag1 = false;
                    }
                }
            }
        }

        if(flag1)
        {
            world->BuildBlockSphere(radius,Lava::getID(),x,y,z);

            for(int x2 = x - radius; x2 < x + radius; x2++)
            {
                for(int y2 = y-1; y2 < y + radius+1; y2++)
                {
                    for(int z2 = z - radius; z2 < z + radius; z2++)
                    {
                        if(world->GetBlock(x2,y2,z2) == Lava::getID())
                        {
                            world->GetBlock(x2,y2,z2) = 0;
                        }
                    }
                }
            }

            for(int x2 = x - radius; x2 < x + radius; x2++)
            {
                for(int y2 = y - radius - 3 ; y2 < y - radius; y2++)
                {
                    for(int z2 = z - radius; z2 < z + radius; z2++)
                    {
                        if(world->GetBlock(x2,y2,z2) == RockBlock::getID())
                        {
                            if(y2 <= 13 && rand() % 100 == 0)
                            {
                                world->GetBlock(x2,y2,z2) = DiamondOre::getID();
                            }
                        }
                    }
                }
            }

            world->UpdateLightAreaIn(Vector3(x,y,z));
        }
        else
        {
            i--;
            continue;
        }
    }
}

void WorldGenerator::initWaterLakes(int WORLD_SIZE, CraftWorld *world)
{
    char waterLakesNum = 10 + rand() % 7;

    for(int i = 0; i <= waterLakesNum; i++)
    {
        int x = 5 + rand() % WORLD_SIZE-9;
        int z = 5 + rand() % WORLD_SIZE-9;
        int y = 14 + rand() % 32;

        float radius = 2.5 + (rand() % 18)/10.0f;
        bool flag1 = true;

        for(int x2 = x - radius; x2 < x + radius; x2++)
        {
            for(int y2 = y - radius; y2 < y + radius; y2++)
            {
                for(int z2 = z - radius; z2 < z + radius; z2++)
                {
                    if(world->GetBlock(x2,y2,z2) == 0)
                    {
                        flag1 = false;
                    }
                }
            }
        }

        if(flag1)
        {
            world->BuildBlockSphere(radius,WaterBlock::getID(),x,y,z);

            for(int x2 = x - radius; x2 < x + radius; x2++)
            {
                for(int y2 = y; y2 < y + radius+1; y2++)
                {
                    for(int z2 = z - radius; z2 < z + radius; z2++)
                    {
                        if(world->GetBlock(x2,y2,z2) == WaterBlock::getID())
                        {
                            world->GetBlock(x2,y2,z2) = 0;
                        }
                    }
                }
            }

            for(int x2 = x - radius; x2 < x + radius; x2++)
            {
                for(int y2 = y - radius - 3 ; y2 < y - radius; y2++)
                {
                    for(int z2 = z - radius; z2 < z + radius; z2++)
                    {
                        if(world->GetBlock(x2,y2,z2) == RockBlock::getID())
                        {
                            if(y2 <= 13 && rand() % 33 == 0)
                            {
                                world->GetBlock(x2,y2,z2) = CoalOre::getID();
                            }
                        }
                    }
                }
            }
        }
        else
        {
            i--;
            continue;
        }
    }
}

void WorldGenerator::initBiome(int WORLD_SIZE, CraftWorld *world, PerlinModule* perlin, int seed)
{
    /// ANOMALIES SECTION
    bool snowBiomeIncluded = true;
    bool snowBiomeSuperLarge = false;
    bool desertBiomeIncluded = true;
    bool desertBiomeSuperLarge = false;

    if(rand() % 15 == 0)
    {
        snowBiomeIncluded = false;
    }
    if(rand() % 15 == 0)
    {
        desertBiomeIncluded = false;
    }
    if(rand() % 15 == 0)
    {
        snowBiomeSuperLarge = true;
    }
    if(rand() % 100 == 0)
    {
        desertBiomeSuperLarge = true;
    }

    /// SNOW BIOME
    float angle = (rand() % (629))/100.0f;
    int biomeCenterX = WORLD_SIZE/2+sinf(angle)*(126);
    int biomeCenterZ = WORLD_SIZE/2+cosf(angle)*(126);
    int biomeRadius = 75+rand()%11+snowBiomeSuperLarge*(25+rand()%140);

    if(snowBiomeIncluded)
    {
        world->snowBiomePosition = Vector3(biomeCenterX,0,biomeCenterZ);
        world->snowBiomeRadius = biomeRadius;

        for (int z = biomeCenterZ-biomeRadius; z <= biomeCenterZ+biomeRadius; z++)
        {
            for (int x = biomeCenterX-biomeRadius; x <= biomeCenterX+biomeRadius; x++)
            {
                if(world->FastDistance2d(abs(x-biomeCenterX),abs(z-biomeCenterZ)) < biomeRadius+5)
                {
                    if(x == biomeCenterX-biomeRadius || x == biomeCenterX+biomeRadius || z == biomeCenterZ-biomeRadius || z == biomeCenterZ+biomeRadius)
                    {
                        if(rand() % 4 == 1)
                        {
                            continue;
                        }
                    }
                    if(world->GetBlock(x, world->groundHeight(x, z), z) == 0) continue;
                    if(world->GetBlock(x, world->groundHeightExcept(x, z, 9), z) == 1)
                    {
                        world->GetBlock(x, world->groundHeightExcept(x, z, 9), z) = 29; // snow
                    }
                    else if(world->GetBlock(x, world->groundHeightWater(x, z), z) == 4)
                    {
                        world->GetBlock(x, world->groundHeightWater(x, z), z) = 32; // ice
                        continue;
                    }

                    if(world->GetBlock(x, world->groundHeight(x, z), z) != 93 && world->GetBlock(x, world->groundHeight(x, z), z) != 32 && world->GetBlock(x, world->groundHeight(x, z), z) != 112)
                    {
                        if(world->GetBlock(x, world->groundHeight(x, z)+1, z) == 0)
                        {
                            world->GetBlock(x, world->groundHeight(x, z)+1, z) = 93;
                        }
                    }
                }
            }
        }
    }


    /// DESERT BIOME
    if(desertBiomeIncluded && (!snowBiomeSuperLarge || !snowBiomeIncluded))
    {
		bool badlands = false;
		if(rand()%100 > 90)
		{
			badlands = true;
		}
        angle += PI + (-20+rand()%41)/100.0f;

        int rad = rand() % 15;

        biomeCenterX = WORLD_SIZE/2+sinf(angle)*(126+rad);
        biomeCenterZ = WORLD_SIZE/2+cosf(angle)*(126+rad);
        biomeRadius = 75+rand()%11+snowBiomeSuperLarge*(25+rand()%140);

        world->desertBiomePosition = Vector3(biomeCenterX,0,biomeCenterZ);
        world->desertBiomeRadius = biomeRadius;

        for (int z = biomeCenterZ-biomeRadius; z <= biomeCenterZ+biomeRadius; z++)
        {
            for (int x = biomeCenterX-biomeRadius; x <= biomeCenterX+biomeRadius; x++)
            {
                if(world->FastDistance2d(abs(x-biomeCenterX),abs(z-biomeCenterZ)) < biomeRadius+5)
                {
                    if(x == biomeCenterX-biomeRadius || x == biomeCenterX+biomeRadius || z == biomeCenterZ-biomeRadius || z == biomeCenterZ+biomeRadius)
                    {
                        if(rand() % 4 == 1)
                        {
                            continue;
                        }
                    }
                    if(world->GetBlock(x, world->groundHeight(x, z), z) == 0) continue;
					
					///BADLANDS
					if(badlands == true)
					{
						for(int y1 = 70; y1 < 75; y1++)
						{
							if(world->GetBlock(x, y1, z) == 1 || world->GetBlock(x, y1, z) == 2)
							{
								if(y1 >= 70 && y1 < 73)
								{
									world->GetBlock(x, y1, z) = BrownTerracotta::getID();
								}
								if(y1 >= 73 && y1 < 75)
								{
									world->GetBlock(x, y1, z) = Terracotta::getID();
								}
							}
						}
						
						for(int y1 = 75; y1 < 81; y1++)
						{
							if(world->GetBlock(x, y1, z) == 1 || world->GetBlock(x, y1, z) == 2)
							{
								if(y1 == 75)
								{
									world->GetBlock(x, y1, z) = OrangeTerracotta::getID();
								}
								if(y1 == 76)
								{
									world->GetBlock(x, y1, z) = WhiteTerracotta::getID();
								}
								if(y1 == 77)
								{
									world->GetBlock(x, y1, z) = LightGrayTerracotta::getID();
								}
								if(y1 >= 78 && y1 < 81)
								{
									world->GetBlock(x, y1, z) = RedTerracotta::getID();
								}
							}
						}
						
						for(int y1 = 81; y1 < 97; y1++)
						{
							if(world->GetBlock(x, y1, z) == 1 || world->GetBlock(x, y1, z) == 2)
							{
								if(y1 == 81 || y1 == 87 || y1 == 91 || y1 == 94)
								{
									world->GetBlock(x, y1, z) = OrangeTerracotta::getID();
								}
								if(y1 == 82 || y1 == 86 || y1 == 88 || y1 == 89 || y1 == 92 || y1 == 93)
								{
									world->GetBlock(x, y1, z) = Terracotta::getID();
								}
								if(y1 == 90)
								{
									world->GetBlock(x, y1, z) = RedTerracotta::getID();
								}
								if(y1 == 95)
								{
									world->GetBlock(x, y1, z) = WhiteTerracotta::getID();
								}
								if(y1 == 96)
								{
									world->GetBlock(x, y1, z) = LightGrayTerracotta::getID();
								}
								if(y1 >= 83 && y1 < 86)
								{
									world->GetBlock(x, y1, z) = YellowTerracotta::getID();
								}
							}
						}
						
						if(world->GetBlock(x, world->groundHeight(x, z), z) == 1 || world->GetBlock(x, world->groundHeight(x, z), z) == 2)
						{
							for(int Y = world->groundHeight(x, z); Y >= world->groundHeight(x, z) - 4; Y--)
							{
								if(world->GetBlock(x, Y, z) == 3)
								{
									world->GetBlock(x, Y, z) = 26;
								}
								if(world->GetBlock(x, Y, z) == 1 || world->GetBlock(x, Y, z) == 2)
								{
									world->GetBlock(x, Y, z) = 227;
									if(world->GetBlock(x, Y+1, z) == 0)
									{
										if(rand() % 375 == 1) //if(rand() % 275 == 1)
										{
											int cactusHeight = 2+rand()%2;
											for(int YY = Y+1; YY <= Y+cactusHeight; YY ++)
											{
												if(world->GetBlock(x+1, YY, z) != 0 ||
												   world->GetBlock(x-1, YY, z) != 0 ||
												   world->GetBlock(x, YY, z+1) != 0 ||
												   world->GetBlock(x, YY, z-1) != 0)
												{
													break;
												}
												world->GetBlock(x, YY, z) = CactusBlock::getID();
											}
										}
										
										if(rand() % 375 == 1)
										{
											for(int YY = Y+1; YY <= Y+1; ++YY)
											{
												if(world->GetBlock(x, YY+1, z) == 0)
												{
													world->GetBlock(x, YY, z) = DeadBush::getID();
												}
											}
										}
									}
								}
							}
						}
					}
					else
					{
						/// DESIERTO
						if(world->GetBlock(x, world->groundHeight(x, z), z) == 1 || world->GetBlock(x, world->groundHeight(x, z), z) == 2)
						{
							for(int Y = world->groundHeight(x, z); Y >= world->groundHeight(x, z) - 4; Y--)
							{
								if(world->GetBlock(x, Y, z) == 3)
								{
									world->GetBlock(x, Y, z) = 26;
								}
								if(world->GetBlock(x, Y, z) == 1 || world->GetBlock(x, Y, z) == 2)
								{
									world->GetBlock(x, Y, z) = 7;
									if(world->GetBlock(x, Y+1, z) == 0)
									{
										if(rand() % 375 == 1) //if(rand() % 275 == 1)
										{
											int cactusHeight = 2+rand()%2;
											for(int YY = Y+1; YY <= Y+cactusHeight; YY ++)
											{
												if(world->GetBlock(x+1, YY, z) != 0 ||
												   world->GetBlock(x-1, YY, z) != 0 ||
												   world->GetBlock(x, YY, z+1) != 0 ||
												   world->GetBlock(x, YY, z-1) != 0)
												{
													break;
												}
												world->GetBlock(x, YY, z) = CactusBlock::getID();
											}
										}
										
										if(rand() % 375 == 1)
										{
											for(int YY = Y+1; YY <= Y+1; ++YY)
											{
												if(world->GetBlock(x, YY+1, z) == 0)
												{
													world->GetBlock(x, YY, z) = DeadBush::getID();
												}
											}
										}
									}
								}
							}
						}
					}
                }
            }
        }
    }
}

void WorldGenerator::initCanes(int WORLD_SIZE, CraftWorld *world, int waterLevel)
{
    for (int z = 1; z < WORLD_SIZE-1; z++)
    {
        for (int x = 1; x < WORLD_SIZE-1; x++)
        {
            if (world->GetBlock(x,waterLevel,z) == SandBlock::getID())
            {
                if (world->GetBlock(x-1,waterLevel,z) == WaterBlock::getID() || world->GetBlock(x+1,waterLevel,z) == WaterBlock::getID() || world->GetBlock(x,waterLevel,z-1) == WaterBlock::getID() || world->GetBlock(x,waterLevel,z+1) == WaterBlock::getID())
                {
                    if(rand() % 48 == 0)
                    {
                        int height = 1+rand()%3;
                        for(int y = waterLevel+1; y <= waterLevel+1+height; y++)
                        {
                            world->GetBlock(x,y,z) = CaneBlock::getID();
                        }
                    }
                }
            }
        }
    }
}

void WorldGenerator::initBedrock(int WORLD_SIZE, CraftWorld *world)
{
    for (int z = 0; z < WORLD_SIZE; ++z)
    {
        for (int x = 0; x < WORLD_SIZE; ++x)
        {
            world->GetBlock(x, 0, z) = IronBlock::getID();//last one in set
        }
    }

    for (int z = 0; z < WORLD_SIZE; ++z)
    {
        for (int x = 0; x < WORLD_SIZE; ++x)
        {
            world->GetBlock(x, 1, z) = IronBlock::getID();//last one in set
        }
    }
	
    for (int z = 0; z < WORLD_SIZE; ++z)
    {
        for (int x = 0; x < WORLD_SIZE; ++x)
        {
            short r = rand() % 2;
            if (r == 1)
            {
                world->GetBlock(x, 2, z) = IronBlock::getID();//last one in set
            }
        }
    }

    for (int z = 0; z < WORLD_SIZE; ++z)
    {
        for (int x = 0; x < WORLD_SIZE; ++x)
        {
            short r = rand() % 3;
            if (r == 1)
            {
                world->GetBlock(x, 3, z) = IronBlock::getID();//last one in set
            }
        }
    }
}

void WorldGenerator::GeneratePileOfLeaves(CraftWorld *world, int x, int y, int z)
{
    for(int x1 = x-2; x1 <= x+2; x1++)
    {
        for(int z1 = z-1; z1 <= z+1; z1++)
        {
            for(int y1 = y-1; y1 <= y+1; y1++)
            {
                if(world->GetBlock(x1,y1,z1) == 0)
                {
                    world->GetBlock(x1,y1,z1) = LeavesBlock::getID();
                }
            }
        }
    }
    for(int x1 = x-1; x1 <= x+1; x1++)
    {
        for(int z1 = z-2; z1 <= z+2; z1++)
        {
            for(int y1 = y-1; y1 <= y+1; y1++)
            {
                if(world->GetBlock(x1,y1,z1) == 0)
                {
                    world->GetBlock(x1,y1,z1) = LeavesBlock::getID();
                }
            }
        }
    }
    for(int x1 = x-1; x1 <= x+1; x1++)
    {
        if(world->GetBlock(x1,y+2,z) == 0)
        {
            world->GetBlock(x1,y+2,z) = LeavesBlock::getID();
        }
        if(world->GetBlock(x1,y-2,z) == 0)
        {
            world->GetBlock(x1,y-2,z) = LeavesBlock::getID();
        }
    }
    for(int z1 = z-1; z1 <= z+1; z1++)
    {
        if(world->GetBlock(x,y+2,z1) == 0)
        {
            world->GetBlock(x,y+2,z1) = LeavesBlock::getID();
        }
        if(world->GetBlock(x,y-2,z1) == 0)
        {
            world->GetBlock(x,y-2,z1) = LeavesBlock::getID();
        }
    }
}

void WorldGenerator::GenerateClassicTree(int WORLD_SIZE, CraftWorld *world, int x, int y, int z, int trunkBlock, int leavesBlock)
{
    int trunkHeight = 3 + rand() % 3;
    if(trunkBlock == WhiteWoodBlock::getID()) // if it is oak
    {
        trunkHeight = 4 + rand() % 2;
    }
    bool flag = false;

    if(world->GetBlock(x, y, z) == GrassBlock::getID())
    {
        flag = true;
    }

    for(int x1 = x-1; x1 <= x+1; x1++)
    {
        for(int z1 = z-1; z1 <= z+1; z1++)
        {
            for (int y1 = y+1; y1 < y + 1 + trunkHeight && y1 < world->WORLD_HEIGHT; ++y1)
            {
                if(world->GetBlock(x1, y1, z1) != 0 && world->GetBlock(x1, y1, z1) != 93)
                {
                    flag = false;
                    break;
                }
            }
        }
    }

    if(flag == false) return;

    if(y + 2 + trunkHeight >= world->WORLD_HEIGHT-1)
    {
        return;
    }

    // gen tree trunk
    for (int y1 = y+1; y1 < y + 2 + trunkHeight && y1 < world->WORLD_HEIGHT; ++y1)
    {
        block_t& Block = world->GetBlock(x, y1, z);
        Block = trunkBlock;
    }

    bool mooshroomTree = 1*(rand() % 12 == 0);
    if(trunkBlock == WoodBlock::getID() && mooshroomTree)
    {
        for(int xx = x-2; xx <= x+2; xx++)
        {
            for(int zz = z-2; zz <= z+2; zz++)
            {
                if(world->GetBlock(xx,world->groundHeight(xx,zz),zz) == GrassBlock::getID())
                {
                    if(rand() % 5 == 0)
                    {
                        world->GetBlock(xx,world->groundHeight(xx,zz)+1,zz) = Mooshroom1::getID() + rand()%2;
                    }
                }
            }
        }
    }
    // remove grass under trunk
    world->GetBlock(x, y, z) = DirtBlock::getID();

    bool baloonTree = false;
    if(trunkBlock == WoodBlock::getID())
    {
        if(trunkHeight >= 4)
        {
            if(rand() % 12 == 0)
            {
                baloonTree = true;
            }
        }
    }


    //create my leaves
    if(!baloonTree)
    {
        for(int yy = 0; yy < 2; yy++)
        {
            for(int xx = 0; xx < 5; xx++)
            {
                for(int zz = 0; zz < 3; zz++)
                {
                    int x1 = xx + x - 2;// :D - what a shitty code
                    int y1 = yy + y + trunkHeight - 1;
                    int z1 = zz + z - 1;

                    if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                    if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                    block_t& Block = world->GetBlock(x1, y1, z1);
                    if (Block == 0) Block = leavesBlock;
                }
            }
        }

        for(int yy = 0; yy < 2; yy++)
        {
            for(int xx = 0; xx < 3; xx++)
            {
                    int x1 = xx + x - 1;// :D - what a shitty code
                    int y1 = yy + y + trunkHeight +1;
                    int z1 = z;

                    if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                    if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                    block_t& Block = world->GetBlock(x1, y1, z1);
                    if (Block == 0) Block = leavesBlock;
            }
        }

        for(int xx = 0; xx < 3; xx++)
        {
            for(int zz = 0; zz < 3; zz++)
            {
                int x1 = xx + x - 1;// :D - what a shitty code
                int y1 = y + trunkHeight + 1;
                int z1 = zz + z - 1;
                int rand1 = rand() % 10;

                if (rand1 > 4)
                {
                    if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                    if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                    block_t& Block = world->GetBlock(x1, y1, z1);
                    if (Block == 0) Block = leavesBlock;
                }
            }
        }

        for(int yy = 0; yy < 2; yy++)
        {
            for(int zz = 0; zz < 3; zz++)
            {

                    int x1 = x;// :D - what a shitty code
                    int y1 = yy + y + trunkHeight + 1;
                    int z1 = zz + z - 1;

                    if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                    if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                    block_t& Block = world->GetBlock(x1, y1, z1);
                    if (Block == 0) Block = leavesBlock;
            }
        }

        for(int yy = 0; yy < 2; yy++)
        {
            for(int xx = 0; xx < 3; xx++)
            {
                for(int zz = 0; zz < 5; zz++)
                {
                    int x1 = xx + x - 1;// :D - what a shitty code
                    int y1 = yy + y + trunkHeight - 1;
                    int z1 = zz + z - 2;

                    if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                    if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                    block_t& Block = world->GetBlock(x1, y1, z1);
                    if (Block == 0) Block = leavesBlock;
                }
            }
        }

        for(int yy = 0; yy < 2; yy++)
        {
            int x1 = x - 2;// :D - what a shitty code
            int y1 = yy + y + trunkHeight - 1;
            int z1 = z - 2;
            int rand1 = rand() % 3;

            if (rand1 != 1)
            {
                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = leavesBlock;
            }
        }

        for(int yy = 0; yy < 2; yy++)
        {
            int x1 = x + 2;// :D - what a shitty code
            int y1 = yy + y + trunkHeight - 1;
            int z1 = z - 2;
            int rand1 = rand() % 3;

            if (rand1 != 1)
            {
                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = leavesBlock;
            }
        }

        for(int yy = 0; yy < 2; yy++)
        {
            int x1 = x - 2;// :D - what a shitty code
            int y1 = yy + y + trunkHeight - 1;
            int z1 = z + 2;
            int rand1 = rand() % 2;

            if (rand1 != 1)
            {
                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = leavesBlock;
            }
        }

        for(int yy = 0; yy < 2; yy++)
        {
            int x1 = x + 2;// :D - what a shitty code
            int y1 = yy + y + trunkHeight - 1;
            int z1 = z + 2;
            int rand1 = rand() % 2;

            if (rand1 != 1)
            {
                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = leavesBlock;
            }
        }
    }
    else
    {
        GeneratePileOfLeaves(world,x,y+trunkHeight,z);
    }
}

void WorldGenerator::GenerateSpruceTree(int WORLD_SIZE, CraftWorld *world, int x, int y, int z)
{
    int trunkBlock = DarkWoodBlock::getID();

    int trunkHeight = 6+rand()%2;
    int type = 0;

    //check if there is an water on there or another tree
    bool flag = true;

    for (int y1 = y+1; y1 <= y + trunkHeight ; y1++)
    {
        if(world->GetBlock(x, y1, z) != 0)
        {
            flag = false;
            break;
        }
    }
    if (flag == false) return;;

    if(rand() % 7 == 1)
    {
        type = 1;
        trunkHeight = 6 + rand() % 3;
    }
    else
    {
        type = 0;
    }

    //Create the tree trunk
    for (int y1 = y; y1 <= y + trunkHeight ; y1++)
    {
        block_t& Block = world->GetBlock(x, y1, z);
        Block = trunkBlock;
    }
    // remove grass under trunk
    world->GetBlock(x, y-1, z) = DirtBlock::getID();

    if(type == 0)
    {
        for(int xx = -1; xx <= 1; xx++)
        {
            for(int zz = -2; zz <= 2; zz++)
            {
                int x1 = xx + x;// :D - what a shitty code
                int y1 = y + 1 + (trunkHeight-6);
                int z1 = zz + z;

                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = 37;
            }
        }

        for(int xx = -2; xx <= 2; xx++)
        {
            for(int zz = -1; zz <= 1; zz++)
            {
                int x1 = xx + x;// :D - what a shitty code
                int y1 = y + 1 + (trunkHeight-6);
                int z1 = zz + z;

                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = 37;
            }
        }

        for(int xx = -1; xx <= 1; xx++)
        {
            for(int zz = -2; zz <= 2; zz++)
            {
                int x1 = xx + x;// :D - what a shitty code
                int y1 = y + 3 + (trunkHeight-6);
                int z1 = zz + z;

                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = 37;
            }
        }

        for(int xx = -2; xx <= 2; xx++)
        {
            for(int zz = -1; zz <= 1; zz++)
            {
                int x1 = xx + x;// :D - what a shitty code
                int y1 = y + 3 + (trunkHeight-6);
                int z1 = zz + z;

                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = 37;
            }
        }

        for(int xx = -1; xx <= 1; xx++)
        {
                int x1 = xx + x;// :D - what a shitty code
                int y1 = y + 2 + (trunkHeight-6);
                int z1 = z;

                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = 37;
        }


        for(int zz = -1; zz <= 1; zz++)
        {
            int x1 = x;// :D - what a shitty code
            int y1 = y + 2 + (trunkHeight-6);
            int z1 = zz + z;

            if (x1 < 0 || y1 < 0 || z1 < 0) continue;
            if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

            block_t& Block = world->GetBlock(x1, y1, z1);
            if (Block == 0) Block = 37;
        }

        for(int xx = -1; xx <= 1; xx++)
        {
                int x1 = xx + x;// :D - what a shitty code
                int y1 = y + 4 + (trunkHeight-6);
                int z1 = z;

                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = 37;
        }


        for(int zz = -1; zz <= 1; zz++)
        {
            int x1 = x;// :D - what a shitty code
            int y1 = y + 4 + (trunkHeight-6);
            int z1 = zz + z;

            if (x1 < 0 || y1 < 0 || z1 < 0) continue;
            if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

            block_t& Block = world->GetBlock(x1, y1, z1);
            if (Block == 0) Block = 37;
        }

        for(int xx = -1; xx <= 1; xx++)
        {
                int x1 = xx + x;// :D - what a shitty code
                int y1 = y + 6 + (trunkHeight-6);
                int z1 = z;

                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = 37;
        }

        for(int zz = -1; zz <= 1; zz++)
        {
            int x1 = x;// :D - what a shitty code
            int y1 = y + 6 + (trunkHeight-6);
            int z1 = zz + z;

            if (x1 < 0 || y1 < 0 || z1 < 0) continue;
            if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

            block_t& Block = world->GetBlock(x1, y1, z1);
            if (Block == 0) Block = 37;
        }

        block_t& Block = world->GetBlock(x, y+7+(trunkHeight-6), z);
        if (Block == 0) Block = 37;
    }
    else // type == 1
    {
        block_t& Block = world->GetBlock(x, y+trunkHeight+1, z);
        if (Block == 0) Block = 37;

        for(int xx = -1; xx <= 1; xx++)
        {
                int x1 = xx + x;// :D - what a shitty code
                int y1 = y + trunkHeight;
                int z1 = z;

                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = 37;
        }

        for(int zz = -1; zz <= 1; zz++)
        {
            int x1 = x;// :D - what a shitty code
            int y1 = y + trunkHeight;
            int z1 = zz + z;

            if (x1 < 0 || y1 < 0 || z1 < 0) continue;
            if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

            block_t& Block = world->GetBlock(x1, y1, z1);
            if (Block == 0) Block = 37;
        }

        for(int xx = -1; xx <= 1; xx++)
        {
                int x1 = xx + x;// :D - what a shitty code
                int y1 = y + trunkHeight-1;
                int z1 = z;

                if (x1 < 0 || y1 < 0 || z1 < 0) continue;
                if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

                block_t& Block = world->GetBlock(x1, y1, z1);
                if (Block == 0) Block = 37;
        }

        for(int zz = -1; zz <= 1; zz++)
        {
            int x1 = x;// :D - what a shitty code
            int y1 = y + trunkHeight-1;
            int z1 = zz + z;

            if (x1 < 0 || y1 < 0 || z1 < 0) continue;
            if (x1 >= WORLD_SIZE || y1 >= world->WORLD_HEIGHT || z1 >= WORLD_SIZE) continue;

            block_t& Block = world->GetBlock(x1, y1, z1);
            if (Block == 0) Block = 37;
        }
    }
}


void WorldGenerator::initRandomTrees(int WORLD_SIZE, CraftWorld *world)
{
    int treesNum = 500+rand()%51;


    for (int i = 0; i < treesNum; i ++)
    {
        int x = 3+rand()%(WORLD_SIZE-4);
        int z = 3+rand()%(WORLD_SIZE-4);
        int y = world->groundHeight(x,z);

        if(y >= 85)
        {
            i--;
            continue;
        }

        bool snowTree = false;
        if(world->GetBlock(x,y,z) == Snow2::getID() && (world->GetBlock(x,y-1,z) == SnowSoil::getID() || world->GetBlock(x,y-1,z) == GrassBlock::getID() || world->GetBlock(x,y-1,z) == DirtBlock::getID()))
        {
            snowTree = true;
        }
        else
        {
            snowTree = false;
        }

        if(!snowTree)
        {
            if(rand() % 100 > 75)
            {
                GenerateClassicTree(WORLD_SIZE,world,x,y,z,WhiteWoodBlock::getID(),BirchLeaves::getID());
            }
            else
            {
                GenerateClassicTree(WORLD_SIZE,world,x,y,z,WoodBlock::getID(),LeavesBlock::getID());
            }
        }
        else
        {
            if(rand() % 3 == 1)
            {
                GenerateSpruceTree(WORLD_SIZE,world,x,y,z);
            }
        }
    }
}

void WorldGenerator::initPumpkins(int WORLD_SIZE, CraftWorld *world)
{
    int NumPumpkinsSpawns = 12 + rand() % 4;
    int Frequency = 4; // 1 - 100

    for (int i = 0; i < NumPumpkinsSpawns; ++i)
    {
        //Choose the tpumpkin position
        int x = rand() % WORLD_SIZE;
        int z = rand() % WORLD_SIZE;
        //check if there is an water on there or another tree
        int rad = 0;

        for(int x2 = x-3; x2 < x+3; ++x2)
        {
            for(int z2 = z-3; z2 < z+3; ++z2)
            {
                if (x2 > 0 && x2 < WORLD_SIZE && z2 > 0 && z2 < WORLD_SIZE)
                {

                    rad = rand() % 100;

                    if (rad > Frequency)
                        continue;

                    if(world->GetBlock(x2, world->groundHeight(x2, z2), z2 ) == GrassBlock::getID() || world->GetBlock(x2, world->groundHeight(x2, z2), z2 ) == DirtBlock::getID() || world->GetBlock(x2, world->groundHeight(x2, z2), z2 ) == SnowSoil::getID())
                    {
                        if(world->GetBlock(x2, world->groundHeight(x2, z2)+1, z2 ) == 0)
                        {
                            int rad2 = rand() % 4;

                            world->GetBlock(x2, world->groundHeight(x2, z2), z2) = DirtBlock::getID();
                            world->GetBlock(x2, world->groundHeight(x2, z2)+1, z2) = 73+rad2;
                        }
                    }
                }
            }
        }
    }
}

void WorldGenerator::initFlowers(int WORLD_SIZE, CraftWorld *world) //generate flowers
{
    int num = 24 + rand() % 12;

    for (int i = 0; i < num; ++i)
    {
        int centerX = 10 + rand() % (WORLD_SIZE-10);
        int centerZ = 10 + rand() % (WORLD_SIZE-10);

        int size = 2 + rand() % 3;
        int typeProcent = 40 + rand() % 20;

        for(int x = centerX-size; x <= centerX+size; x++)
        {
            for(int z = centerZ-size; z <= centerZ+size; z++)
            {
                int y = world->groundHeight(x, z);
                if(world->GetBlock(x,y,z) == 1)
                {
                    if(x == centerX-size || x == centerX+size || z == centerZ-size || z == centerZ+size) // check for corner flower
                    {
                        if(rand() % 8 == 1) // 1/8
                        {
                            if(rand() % 100 > typeProcent)
                            {
                                world->GetBlock(x,y+1,z) = Flower1::getID();
                            }
                            else
                            {
                                world->GetBlock(x,y+1,z) = Flower2::getID();
                            }
                        }
                    }
                    else
                    {
                        if(rand() % 5 == 1) // 1/5
                        {
                            if(rand() % 100 > typeProcent)
                            {
                                world->GetBlock(x,y+1,z) = Flower1::getID();
                            }
                            else
                            {
                                world->GetBlock(x,y+1,z) = Flower2::getID();
                            }
                        }
                    }
                }
            }
        }
    }
}

void WorldGenerator::initGrass(int WORLD_SIZE, CraftWorld *world)
{

    int NumTypes = 1200;//cambiado //600

    for (int i = 0; i < NumTypes; ++i)
    {
        //Choose the tpumpkin position
        int x2 = rand() % WORLD_SIZE;
        int z2 = rand() % WORLD_SIZE;


        if(world->GetBlock(x2, world->groundHeight(x2, z2)+1, z2 ) == 0 && world->GetBlock(x2, world->groundHeight(x2, z2), z2 ) == 1)
        {
            world->GetBlock(x2, world->groundHeight(x2, z2)+1, z2 ) = 116;
        }
    }
}

int WorldGenerator::GenerateOreVine(int WORLD_SIZE, CraftWorld *world, int x, int y, int z, int oreID, int type)
{
    // type = {0 - sm, 1 - med, 2 - big}
    int orePlaced = 0;
    int m = rand() % 3;
    switch(type)
    {
        // small
        case 0:

        if(m == 0)
        {
            for(int z2 = z; z2 <= z+1; z2++)
            {
                if(world->GetBlock(x,y,z2) == RockBlock::getID())
                {
                    world->GetBlock(x,y,z2) = oreID;
                    orePlaced ++;
                }
            }
        }
        else if(m == 1)
        {
            for(int x2 = x; x2 <= x+1; x2++)
            {
                if(world->GetBlock(x2,y,z) == RockBlock::getID())
                {
                    world->GetBlock(x2,y,z) = oreID;
                    orePlaced ++;
                }
            }
        }
        else
        {
            for(int y2 = y; y2 <= y+1; y2++)
            {
                if(world->GetBlock(x,y2,z) == RockBlock::getID())
                {
                    world->GetBlock(x,y2,z) = oreID;
                    orePlaced ++;
                }
            }
        }

        break;
        // mid-size
        case 1:


        if(m == 0)
        {
            for(int y2 = y; y2 <= y+1; y2++)
            {
                for(int z2 = z; z2 <= z+1; z2++)
                {
                    if(world->GetBlock(x,y2,z2) == RockBlock::getID())
                    {
                        world->GetBlock(x,y2,z2) = oreID;
                        orePlaced ++;
                    }
                }
            }
        }
        else if(m == 1)
        {
            for(int x2 = x; x2 <= x+1; x2++)
            {
                for(int z2 = z; z2 <= z+1; z2++)
                {
                    if(world->GetBlock(x2,y,z2) == RockBlock::getID())
                    {
                        world->GetBlock(x2,y,z2) = oreID;
                        orePlaced ++;
                    }
                }
            }
        }
        else
        {
            for(int x2 = x; x2 <= x+1; x2++)
            {
                for(int y2 = y; y2 <= y+1; y2++)
                {
                    if(world->GetBlock(x2,y2,z) == RockBlock::getID())
                    {
                        world->GetBlock(x2,y2,z) = oreID;
                        orePlaced ++;
                    }
                }
            }
        }

        break;
        // large
        case 2:

        for(int x2 = x; x2 <= x+1; x2++)
        {
            for(int y2 = y; y2 <= y+1; y2++)
            {
                for(int z2 = z; z2 <= z+1; z2++)
                {
                    if(world->GetBlock(x2,y2,z2) == RockBlock::getID())
                    {
                        world->GetBlock(x2,y2,z2) = oreID;
                        orePlaced ++;
                    }
                }
            }
        }

        break;
    }

    return orePlaced;
}

void WorldGenerator::initOre(int WORLD_SIZE, CraftWorld *world, int oreID, float oreConcentration, int oreLowestBoundary, int oreHighestBoundary, int variations) // brief and sweet function
{
    // oreConcentration - var between 0 and 100

    oreConcentration = oreConcentration / 100.0f; // percents
    int oreAmount = oreConcentration*(WORLD_SIZE*WORLD_SIZE*(oreHighestBoundary-oreLowestBoundary));

    switch(oreID)
    {
        case 41: world->mainWorldVariables.worldIronOre = oreAmount;
        case 42: world->mainWorldVariables.worldCoalOre = oreAmount;
        case 43: world->mainWorldVariables.worldDiamondOre = oreAmount;
        case 44: world->mainWorldVariables.worldRedstoneOre = oreAmount;
        case 45: world->mainWorldVariables.worldRedstoneOre = oreAmount;
        case 5: world->mainWorldVariables.worldGoldenOre = oreAmount;
    }

    while(oreAmount > 0)
    {
        int x = 1+rand()%(WORLD_SIZE-2);
        int y = oreLowestBoundary+rand()%(oreHighestBoundary-oreLowestBoundary+1);
        int z = 1+rand()%(WORLD_SIZE-2);

        oreAmount -= GenerateOreVine(WORLD_SIZE,world,x,y,z,oreID,rand()%variations);
    }
}

void WorldGenerator::initClay(int WORLD_SIZE, CraftWorld *world)
{
    int NumOres = 40 + rand() % 20;

    for (int i = 0; i < NumOres; ++i)
    {
        //Choose the tpumpkin positionh
        int x = rand() % WORLD_SIZE;
        int z = rand() % WORLD_SIZE;
        int y = world->groundHeight(x,z);

        if(y >= 63)
        {
            i--;
            continue;
        }
        float radius = 1.5+(rand()%12)/10.0f;

        world->BuildBlockSphere(radius,ClayBlock::getID(),x,y+1,z,SandBlock::getID());
    }
}



void WorldGenerator::initDirt(int WORLD_SIZE, CraftWorld *world)
{
    int NumBlocks = 1500;
    float radius = 2.0f + (rand() % 35)/10.0f;

    short PlaceBlock = DirtBlock::getID();

    for (int i = 0; i < NumBlocks; ++i)
    {
        int x = rand() % WORLD_SIZE;
        int z = rand() % WORLD_SIZE;
        int y = 4 + rand() % (world->groundHeight(x,z)-1);

        world->BuildBlockSphere(radius, PlaceBlock, x, y, z, RockBlock::getID());

    }
}

void WorldGenerator::initGravel(int WORLD_SIZE, CraftWorld *world)
{
    int NumBlocks = 400;
    float radius = 2.0f + (rand() % 30)/10.0f;

    short PlaceBlock = Gravel::getID();

    for (int i = 0; i < NumBlocks; ++i)
    {
        int x = rand() % WORLD_SIZE;
        int z = rand() % WORLD_SIZE;
        int y = 4 + rand() % (world->groundHeight(x,z)-1);

        world->BuildBlockSphere(radius, PlaceBlock, x, y, z, RockBlock::getID());
    }
}

void WorldGenerator::initCofreBonus(int WORLD_SIZE, CraftWorld *world)
{
	int x1,z1,y1;
	int tempy;
	
	x1 = world->playerSpawnPointPosition.x + 2+rand()%4;
	z1 = world->playerSpawnPointPosition.z + 2+rand()%4;
	y1 = world->groundHeight(x1,z1)+1;
	
	tempy = y1;
	
	if(world->GetBlock(x1, y1, z1 ) == 93){
		y1 -= 1;
	}
	
	Chest *NewChest;						///MODIFICAR
	
	world->GetBlock(x1+1,y1,z1) = 48;
	y1 = tempy;
	world->GetBlock(x1-1,y1,z1) = 48;
	y1 = tempy;
	world->GetBlock(x1,y1,z1+1) = 48;
	y1 = tempy;	
	world->GetBlock(x1,y1,z1-1) = 48;
	y1 = tempy;
	
	world->GetBlock(x1,y1,z1) = 94;
	NewChest = new Chest(x1,y1,z1);
	
	for(int i = 0; i <= 8; i++)
	{
		int slot = rand() % 27;
		int item = -1;
		int am = 0;
		bool st = true;

		int rand1 = rand() % 100;

		if(rand1 < 65)
		{
			switch(rand() % 6)
			{
			case 0:
				item = 284;//apple
				am = 1 + rand() % 2;
			break;
			case 1:
				item = 250;//wooden pick
				am = 60;
				st = false;
			break;
			case 2:
				item = 265;//wooden axe
				am = 60;
				st = false;
			break;
			case 3:
				item = 34;//oak planks
				am = 1+rand()%12;
			break;
			case 4:
				item = 276;//stick
				am = 1+rand()%12;
			break;
			case 5:
				item = 288;//bread
				am = 1+rand()%2;
			}
		}
		else if(rand1 >= 65)
		{
			switch(rand() % 5)
			{
			case 0:
				item = 31;//birch planks
				am = 1+rand()%3;
			break;
			case 1:
				item = 30;//other planks
				am = 1+rand()%3;
			break;
			case 2:
				item = 8;//other planks
				am = 1+rand()%3;
			break;
			case 3:
				item = 266;//stone axe
				am = 132;
				st = false;
			break;
			case 4:
				item = 251;//stone pickaxe
				am = 132;
				st = false;
			break;
			}
		}

		NewChest->chestSlotId[slot] = item;
		NewChest->chestSlotAm[slot] = am;
		NewChest->chestSlotSt[slot] = st;
	}
	world->mChests.push_back(NewChest);
}

void WorldGenerator::initEstructuras(int WORLD_SIZE, CraftWorld *world)
{
	int x1,z1,y1;
	int tempx,tempy,tempz;
	
	bool zona1 = false;
	bool zona2 = false;
	bool zona3 = false;
	bool test = true;
	
	x1 = rand() % (WORLD_SIZE-50);
	z1 = rand() % (WORLD_SIZE-50);
	y1 = world->groundHeight(x1,z1)+1;
	
	tempx = x1;
	tempy = y1;
	tempz = z1;
	
	if(1+rand()%10 < 3)
	{
		if(1+rand()%10 < 6){
			zona1 = true;
		}
		if(1+rand()%10 < 6){
			zona2 = true;
		}
		if(1+rand()%10 < 6){
			zona3 = true;
		}
		if(zona1 == false && zona2 == false && zona3 == false){
			switch(rand()%3){
				case 0:
					zona1 = true;
				break;
				case 1:
					zona2 = true;
				break;
				case 2:
					zona3 = true;
				break;
			}
		}
	}
	
	if(zona3 == true)
	{
		///-----*CALLE*------///
		x1 += 50;
		for(int x2 = 0; x2 < 50; x2++)
		{
			for(int z2 = 5; z2 < 8; z2++)
			{
				if(world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == GrassBlock::getID() || world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == DirtBlock::getID() || world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == 32 || world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == 7)
				{
					if(world->groundHeight(x1+x2, z1+z2) < 60)
					{
						world->GetBlock(x1+x2,60,z1+z2) = 34;
					}
					else
					{
						world->GetBlock(x1+x2,world->groundHeight(x1+x2, z1+z2),z1+z2) = 113;
					}
				}else if(world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == 93){
					world->GetBlock(x1+x2,world->groundHeight(x1+x2, z1+z2)-1,z1+z2) = 113;
					world->GetBlock(x1+x2,world->groundHeight(x1+x2, z1+z2),z1+z2) = 0;
				}
			}
		}
		x1 = tempx;
		z1 = tempz;
	}
	
	if(zona1 == true)
	{
		///-----*CALLE*------///
		for(int x2 = 0; x2 < 50; x2++)
		{
			for(int z2 = 5; z2 < 8; z2++)
			{
				if(world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == GrassBlock::getID() || world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == DirtBlock::getID() || world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == 32 || world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == 7)
				{
					if(world->groundHeight(x1+x2, z1+z2) < 60)
					{
						world->GetBlock(x1+x2,60,z1+z2) = 34;
					}
					else
					{
						world->GetBlock(x1+x2,world->groundHeight(x1+x2, z1+z2),z1+z2) = 113;
					}
				}else if(world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == 93){
					world->GetBlock(x1+x2,world->groundHeight(x1+x2, z1+z2)-1,z1+z2) = 113;
					world->GetBlock(x1+x2,world->groundHeight(x1+x2, z1+z2),z1+z2) = 0;
				}
			}
			
			x1 = tempx;
			z1 = tempz;
		}
		
		if(test)//casasimple
		{
			for(int j = 1; j <= 3; j++)
			{				
				y1 = tempy;
				if(j == 1){
					y1 = world->groundHeight(x1,z1+4)+1;
				}else if(j == 2){
					
					x1 += 15;
					z1 += 8;
					y1 = world->groundHeight(x1,z1)+1;
				}else{
					if(zona3 == false){
						continue;
					}
					x1 += 85;
					y1 = world->groundHeight(x1,z1)+1;
				}
				if(world->GetBlock(x1, y1, z1 ) == 93)
				{
					y1 -= 1;
				}
				if(y1 <= 60)
				{
					y1 = 61;
				}
				
				///-----*CASA SIMPLE*-----///
				//madera
				for(int y2 = 0; y2 < 5; y2++)
				{
					for(int x2 = 0; x2 < 5; x2++)
					{
						for(int z2 = 0; z2 < 5; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
						}
					}
				}
				//parchear suelo
				for(int y2 = -10; y2 < 0; y2++)
				{
					for(int x2 = 0; x2 < 5; x2++)
					{
						for(int z2 = 0; z2 < 5; z2++)
						{
							if(world->GetBlock(x1+x2,y1+y2,z1+z2) == 0 || world->GetBlock(x1+x2,y1+y2,z1+z2) == WaterBlock::getID() || world->GetBlock(x1+x2,y1+y2,z1+z2) == 93)
							{
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
							}
						}
					}
				}
				//suelo rocoso
				for(int y2 = 0; y2 < 1; y2++)
				{
					for(int x2 = 0; x2 < 5; x2++)
					{
						for(int z2 = 0; z2 < 5; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
						}
					}
				}	
				//vaciar interior
				for(int y2 = 1; y2 < 6; y2++)
				{
					for(int x2 = 1; x2 < 4; x2++)
					{
						for(int z2 = 1; z2 < 4; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 0;
						}
					}
				}	
				//techo de madera
				for(int y2 = 4; y2 < 5; y2++)
				{
					for(int x2 = 0; x2 < 5; x2++)
					{
						for(int z2 = 0; z2 < 5; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 8;
						}
					}
				}	
				//columnas de roca
				for(int x2 = 0; x2 < 5; x2+=4)
				{
					for(int z2 = 0; z2 < 5; z2+=4)
					{
						for(int y2 = 1; y2 < 4; y2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
						}
					}
				}
				
				//techo de cercas
				if(1+rand()%10 <= 5){
					for(int x2 = 0; x2 < 5; x2++)
					{
						for(int z2 = 0; z2 < 5; z2++)
						{
							for(int y2 = 5; y2 < 6; y2++)
							{
								if(z2 >= 1 && z2 <= 3 && x2 >= 1 && x2 <= 3){
									continue;
								}
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 59;
							}
						}
					}
				}
				
				//puerta
				if(j == 1 || j == 3){
					world->GetBlock(x1+2,y1+2,z1+4) = 57;
					world->GetBlock(x1+2,y1+1,z1+4) = 50;
					if(world->GetBlock(x1+2,y1-1,z1+5) != 0)//escalon
						world->GetBlock(x1+2,y1,z1+5) = 61;
				}
				if(j == 2){
					world->GetBlock(x1+2,y1+2,z1) = 57;
					world->GetBlock(x1+2,y1+1,z1) = 49;
					if(world->GetBlock(x1+2,y1-1,z1-1) != 0)//escalon
						world->GetBlock(x1+2,y1,z1-1) = 60;
				}										
				//ventanas
				world->GetBlock(x1,y1+2,z1+2) = 114;
				world->GetBlock(x1+4,y1+2,z1+2) = 114;
				
				x1 = tempx;
				z1 = tempz;
			}
		}
		
		if(test)//biblioteca
		{
			y1 = tempy;
			z1 = tempz;
			for(int j = 1; j < 3; j++)
			{
				if(j == 1){
					x1 += 11;
					y1 = world->groundHeight(x1,z1+4)+1;
				}else{
					if(zona3 == false){
						continue;
					}
					x1 += 65;
					y1 = world->groundHeight(x1,z1+4)+1;
				}
				
				if(world->GetBlock(x1, y1, z1 ) == 93)
				{
					y1 -= 1;
				}
				if(y1 <= 60)
				{
					y1 = 61;
				}
				
				///-----*BIBLIOTECA*-----///
				//base de roca
				for(int y2 = 0; y2 < 6; y2++)
				{
					for(int x2 = 0; x2 < 9; x2++)
					{
						for(int z2 = -1; z2 < 5; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
						}
					}
				}
				//parchear suelo
				for(int y2 = -10; y2 < 0; y2++)
				{
					for(int x2 = 0; x2 < 9; x2++)
					{
						for(int z2 = -1; z2 < 5; z2++)
						{
							if(world->GetBlock(x1+x2,y1+y2,z1+z2) == 0 || world->GetBlock(x1+x2,y1+y2,z1+z2) == WaterBlock::getID() || world->GetBlock(x1+x2,y1+y2,z1+z2) == 93)
							{
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
							}
						}
					}
				}
				//madera 1
				for(int y2 = 2; y2 < 5; y2++)
				{
					for(int x2 = 1; x2 < 8; x2++)
					{
						for(int z2 = -1; z2 < 5; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
						}
					}
				}
				//madera 2
				for(int y2 = 2; y2 < 5; y2++)
				{
					for(int x2 = 0; x2 < 9; x2++)
					{
						for(int z2 = 0; z2 < 4; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
						}
					}
				}	
				//interior
				for(int y2 = 1; y2 < 5; y2++)
				{
					for(int x2 = 1; x2 < 8; x2++)
					{
						for(int z2 = 0; z2 < 4; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 0;
						}
					}
				}	
				//interior decoracion	
				//libros
				for(int y2 = 3; y2 < 4; y2++)
				{
					for(int x2 = 1; x2 < 8; x2++)
					{
						for(int z2 = 0; z2 < 1; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 35;
						}
					}
				}
				//parte de arriba de madera
				for(int y2 = 4; y2 < 5; y2++)
				{
					for(int x2 = 1; x2 < 8; x2++)
					{
						for(int z2 = 0; z2 < 4; z2+=3)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
						}
					}
				}
				//escaleras
				for(int y2 = 1; y2 < 2; y2++)
				{
					for(int x2 = 3; x2 < 7; x2++)
					{
						for(int z2 = 0; z2 < 1; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 126;
						}
					}
				}
				world->GetBlock(x1+7,y1+1,z1) = 34;
				world->GetBlock(x1+7,y1+1,z1+1) = 127;
				world->GetBlock(x1+7,y1+1,z1+3) = 105;	
				//ventanas
				for(int y2 = 2; y2 < 4; y2++)
				{
					for(int x2 = 4; x2 < 7; x2++)
					{
						for(int z2 = 4; z2 < 5; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 114;
						}
					}
				}	
				for(int y2 = 2; y2 < 4; y2++)
				{
					for(int x2 = 0; x2 < 9; x2+=8)
					{
						for(int z2 = 1; z2 < 3; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 114;
						}
					}
				}
				world->GetBlock(x1+2,y1+2,z1-1) = 114;
				world->GetBlock(x1+3,y1+2,z1-1) = 114;
				world->GetBlock(x1+5,y1+2,z1-1) = 114;
				world->GetBlock(x1+6,y1+2,z1-1) = 114;	
				//techo final
				//bloques de roca
				for(int y2 = 6; y2 <= 7; y2++)
				{
					for(int x2 = 0; x2 < 9; x2++)
					{
						for(int z2 = 0; z2 < 4; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
						}
					}
				}
				//escaleras
				for(int y2 = 8; y2 < 9; y2++)
				{
					for(int x2 = 0; x2 < 9; x2++)
					{
						for(int z2 = 1; z2 <= 2; z2++)
						{
							if(z2 == 2){
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 126;
							}else{
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 125;
							}
						}
					}
				}
				for(int y2 = 7; y2 < 8; y2++)
				{
					for(int x2 = 0; x2 < 9; x2++)
					{
						for(int z2 = 0; z2 <= 3; z2+=3)
						{
							if(z2 == 3){
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 126;
							}else{
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 125;
							}
						}
					}
				}
				for(int y2 = 6; y2 < 7; y2++)
				{
					for(int x2 = 0; x2 < 9; x2++)
					{
						for(int z2 = -1; z2 <= 4; z2+=5)
						{
							if(z2 == 4){
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 126;
							}else{
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 125;
							}
						}
					}
				}
				for(int y2 = 5; y2 < 6; y2++)
				{
					for(int x2 = 0; x2 < 9; x2++)
					{
						for(int z2 = -2; z2 <= 5; z2+=7)
						{
							if(z2 == 5){
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 126;
							}else{
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 125;
							}
						}
					}
				}
				//puerta y otras cosas
				world->GetBlock(x1+1,y1+2,z1+4) = 57;
				world->GetBlock(x1+1,y1+1,z1+4) = 50;
				if(world->GetBlock(x1+1,y1-1,z1+5) != 0)
				{
					world->GetBlock(x1+1,y1,z1+5) = 61;
				}
				
				x1 = tempx;
				z1 = tempz;
			}
		}
		
		if(test)//carniceria
		{
			x1 += 25;
			y1 = tempy;
			y1 = world->groundHeight(x1,z1+4)+1;
			if(world->GetBlock(x1, y1, z1 ) == 93)
			{
				y1 -= 1;
			}
			if(y1 <= 60)
			{
				y1 = 61;
			}
			
			///-----*CARNICERIA*-----///
			//roca
			for(int y2 = 0; y2 < 4; y2++)
			{
				for(int x2 = 0; x2 < 9; x2++)
				{
					for(int z2 = -1; z2 < 5; z2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
					}
				}
			}
			//parchear suelo
			for(int y2 = -10; y2 < 0; y2++)
			{
				for(int x2 = 0; x2 < 9; x2++)
				{
					for(int z2 = -1; z2 < 5; z2++)
					{
						if(world->GetBlock(x1+x2,y1+y2,z1+z2) == 0 || world->GetBlock(x1+x2,y1+y2,z1+z2) == WaterBlock::getID() || world->GetBlock(x1+x2,y1+y2,z1+z2) == 93)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
						}
					}
				}
			}
			//maderacasa
			for(int y2 = 2; y2 < 4; y2++)
			{
				for(int x2 = 1; x2 < 8; x2++)
				{
					for(int z2 = -1; z2 < 5; z2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
					}
				}
			}
			//techo
			for(int y2 = 4; y2 <= 5; y2++)
			{
				for(int x2 = 0; x2 < 9; x2++)
				{
					for(int z2 = 0; z2 < 4; z2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
					}
				}
			}
			//interior
			for(int y2 = 1; y2 < 5; y2++)
			{
				for(int x2 = 1; x2 < 8; x2++)
				{
					for(int z2 = 0; z2 < 4; z2++)
					{
						if((y2 == 4 && z2 == 0) || (y2 == 4 && z2 == 3)){
							continue;
						}
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 0;
					}
				}
			}
			
			//interior decoracion
			world->GetBlock(x1+1,y1+1,z1) = 34;
			world->GetBlock(x1+2,y1+1,z1) = 126;
			world->GetBlock(x1+1,y1+1,z1+1) = 128;
			
			world->GetBlock(x1+6,y1+1,z1+2) = 91;
			world->GetBlock(x1+6,y1+1,z1+3) = 91;
			for(int y2 = 0; y2 < 1; y2++)
			{
				for(int x2 = 5; x2 < 8; x2++)
				{
					for(int z2 = 1; z2 < 4; z2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 91;
					}
				}
			}
			
			//cercas
			for(int y2 = 1; y2 < 2; y2++)
			{
				for(int x2 = 0; x2 < 9; x2++)
				{
					for(int z2 = -6; z2 < -1; z2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 59;
					}
				}
			}
			for(int y2 = 1; y2 < 2; y2++)
			{
				for(int x2 = 1; x2 < 8; x2++)
				{
					for(int z2 = -5; z2 < -1; z2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 0;
					}
				}
			}
			for(int y2 = 0; y2 < 1; y2++)
			{
				for(int x2 = 0; x2 < 9; x2++)
				{
					for(int z2 = -6; z2 < -1; z2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 2;
					}
				}
			}
			//escaleras
			for(int y2 = 6; y2 < 7; y2++)
			{
				for(int x2 = 0; x2 < 9; x2++)
				{
					for(int z2 = 1; z2 <= 2; z2++)
					{
						if(z2 == 2){
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 126;
						}else{	
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 125;
						}
					}
				}
			}
			for(int y2 = 5; y2 < 6; y2++)
			{
				for(int x2 = 0; x2 < 9; x2++)
				{
					for(int z2 = 0; z2 <= 3; z2+=3)
					{
						if(z2 == 3){
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 126;
						}else{	
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 125;
						}
					}
				}
			}
			for(int y2 = 4; y2 < 5; y2++)
			{
				for(int x2 = 0; x2 < 9; x2++)
				{
					for(int z2 = -1; z2 <= 4; z2+=5)
					{
						if(z2 == 4){
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 126;
						}else{	
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 125;
						}
					}
				}
			}
			for(int y2 = 3; y2 < 4; y2++)
			{
				for(int x2 = 0; x2 < 9; x2++)
				{
					for(int z2 = -2; z2 <= 5; z2+=7)
					{
						if(z2 == 5){
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 126;
						}else{	
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 125;
						}
					}
				}
			}
			
			//puerta y otras cosas
			world->GetBlock(x1+2,y1+1,z1+4) = 50;
			world->GetBlock(x1+2,y1+2,z1+4) = 57;
			
			world->GetBlock(x1+6,y1+2,z1-1) = 57;
			world->GetBlock(x1+6,y1+1,z1-1) = 49;
			if(world->GetBlock(x1+2,y1-1,z1+5) != 0)
			{
				world->GetBlock(x1+2,y1,z1+5) = 61;
			}
			//ventanas
			world->GetBlock(x1+5,y1+2,z1+4) = 114;
			world->GetBlock(x1+0,y1+2,z1) = 8;
			world->GetBlock(x1+0,y1+2,z1+3) = 8;
			world->GetBlock(x1+0,y1+2,z1+1) = 114;
			world->GetBlock(x1+0,y1+2,z1+2) = 114;
			
			world->GetBlock(x1+8,y1+2,z1) = 8;
			world->GetBlock(x1+8,y1+2,z1+3) = 8;
			world->GetBlock(x1+8,y1+2,z1+1) = 114;
			world->GetBlock(x1+8,y1+2,z1+2) = 114;
			
			world->GetBlock(x1+2,y1+2,z1-1) = 114;
			world->GetBlock(x1+3,y1+2,z1-1) = 114;
			
			x1 = tempx;
			z1 = tempz;
		}
		
		if(test)//sembrios
		{
			///-----*SEMBRIOS*-----///
			for(int j = 1; j <= 4; j++)
			{
				if(j == 1){
					x1 += 39;
					y1 = tempy;
					y1 = world->groundHeight(x1,z1)+1;
				}
				if(j == 2){
					x1 += 30;
					z1 += 12;
					y1 = tempy;
					y1 = world->groundHeight(x1,z1)+1;
				}
				if(j == 3){
					x1 += 24;
					z1 += 12;
					y1 = tempy;
					y1 = world->groundHeight(x1,z1)+1;
				}
				if(j == 4){
					if(zona2 == false){
						continue;
					}
					x1 += 43;
					z1 += 27;
					y1 = tempy;
					y1 = world->groundHeight(x1,z1)+1;
				}					
				if(world->GetBlock(x1, y1, z1 ) == 93)
				{
					y1 -= 1;
				}
				if(y1 <= 60)
				{
					y1 = 61;
				}
				
				//vaciar espacio
				
				for(int y2 = 1; y2 < 11; y2++)
				{
					for(int x2 = 0; x2 < 7; x2++)
					{
						for(int z2 = -4; z2 < 5; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 0;
						}
					}
				}
				
				//madera
				for(int y2 = 0; y2 < 1; y2++)
				{
					for(int x2 = 0; x2 < 7; x2++)
					{
						for(int z2 = -4; z2 < 5; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 8;
						}
					}
				}
				//parchear suelo
				for(int y2 = -10; y2 < 0; y2++)
				{
					for(int x2 = 0; x2 < 7; x2++)
					{
						for(int z2 = -4; z2 < 5; z2++)
						{
							if(world->GetBlock(x1+x2,y1+y2,z1+z2) == 0 || world->GetBlock(x1+x2,y1+y2,z1+z2) == WaterBlock::getID() || world->GetBlock(x1+x2,y1+y2,z1+z2) == 93)
							{
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 2;
							}
						}
					}
				}
				//tierra para cultivo/agua
				for(int y2 = 0; y2 < 1; y2++)
				{
					for(int x2 = 1; x2 < 6; x2++)
					{
						for(int z2 = -3; z2 < 4; z2++)
						{
							if(x2 == 3)
							{
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 4;
								continue;
							}
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 80;
						}
					}
				}	
				//cultivos
				for(int y2 = 1; y2 < 2; y2++)
				{
					for(int x2 = 1; x2 < 3; x2++)
					{
						for(int z2 = -3; z2 < 4; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 139;
						}
					}
				}	
				for(int y2 = 1; y2 < 2; y2++)
				{
					for(int x2 = 4; x2 < 6; x2++)
					{
						for(int z2 = -3; z2 < 4; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 189;
						}
					}
				}
				
				x1 = tempx;
				z1 = tempz;
			}
		}
	}
	
	if(zona2 == true)
	{
		///-----*CALLE*------///
		for(int x2 = 50; x2 < 53; x2++)
		{
			for(int z2 = -21; z2 < 35; z2++)
			{
				if(world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == GrassBlock::getID() || world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == DirtBlock::getID() || world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == 32 || world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == 7)
				{
					if(world->groundHeight(x1+x2, z1+z2) < 60)
					{
						world->GetBlock(x1+x2,60,z1+z2) = 34;
					}
					else
					{
						world->GetBlock(x1+x2,world->groundHeight(x1+x2, z1+z2),z1+z2) = 113;
					}
				}else if(world->GetBlock(x1+x2, world->groundHeight(x1+x2, z1+z2), z1+z2 ) == 93){
					world->GetBlock(x1+x2,world->groundHeight(x1+x2, z1+z2)-1,z1+z2) = 113;
					world->GetBlock(x1+x2,world->groundHeight(x1+x2, z1+z2),z1+z2) = 0;
				}
			}
			
			x1 = tempx;
		}		
		
		if(test)//casasimple
		{
			///-----*CASA SIMPLE*-----///
			for(int j = 1; j < 3; j++)
			{
				if(j == 1){
					y1 = tempy;
					y1 = world->groundHeight(x1+53,z1-5)+1;					
				}else{
					z1 += 26;
					y1 = tempy;
					y1 = world->groundHeight(x1+53,z1-5)+1;	
				}
				if(world->GetBlock(x1, y1, z1 ) == 93)
				{
					y1 -= 1;
				}
				if(y1 <= 60)
				{
					y1 = 61;
				}
				
				//BaseDeRoca
				for(int y2 = 0; y2 < 4; y2++)
				{
					for(int x2 = 53; x2 < 58; x2++)
					{
						for(int z2 = -5; z2 < 0; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
						}
					}
				}
				//parchear suelo
				for(int y2 = -10; y2 < 0; y2++)
				{
					for(int x2 = 53; x2 < 58; x2++)
					{
						for(int z2 = -5; z2 < 0; z2++)
						{
							if(world->GetBlock(x1+x2,y1+y2,z1+z2) == 0 || world->GetBlock(x1+x2,y1+y2,z1+z2) == WaterBlock::getID() || world->GetBlock(x1+x2,y1+y2,z1+z2) == 93)
							{
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
							}
						}
					}
				}
				//BaseDeMadera
				for(int y2 = 1; y2 < 4; y2++)
				{
					for(int x2 = 54; x2 < 57; x2++)
					{
						for(int z2 = -5; z2 < 0; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
						}
					}
				}
				for(int y2 = 1; y2 < 4; y2++)
				{
					for(int x2 = 53; x2 < 58; x2++)
					{
						for(int z2 = -4; z2 < -1; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
						}
					}
				}
				//VaciarInterior
				for(int y2 = 1; y2 < 4; y2++)
				{
					for(int x2 = 54; x2 < 57; x2++)
					{
						for(int z2 = -4; z2 < -1; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 0;
						}
					}
				}
				//TechoDeMadera
				for(int y2 = 4; y2 < 5; y2++)
				{
					for(int x2 = 53; x2 < 58; x2++)
					{
						for(int z2 = -5; z2 < 0; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 8;
						}
					}
				}
				//techo de cercas
				if(1+rand()%10 <= 5){
					for(int x2 = 53; x2 < 58; x2++)
					{
						for(int z2 = -5; z2 < 0; z2++)
						{
							for(int y2 = 5; y2 < 6; y2++)
							{
								if(z2 >= -4 && z2 <= -2 && x2 >= 54 && x2 <= 56){
									continue;
								}
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 59;
							}
						}
					}
				}
				//Puerta&Escaleras
				world->GetBlock(x1+53,y1+2,z1-3) = 57;
				world->GetBlock(x1+53,y1+1,z1-3) = 51;
				
				if(world->GetBlock(x1+52,y1-1,z1-3) != 0)
				{
					world->GetBlock(x1+52,y1,z1-3) = 62;
				}
				
				x1 = tempx;
				z1 = tempz;
			}
		}
		
		if(test)//cabaña
		{
			///-----*CABAÑA*-----///
			for(int j = 1; j < 4; j++)
			{
				if(j == 1){
					y1 = tempy;
					y1 = world->groundHeight(x1+49,z1-15)+1;
				}else if(j == 2){
					if(zona3 == false){
						continue;
					}
					x1 += 10;
					z1 += 23;
					y1 = tempy;
					y1 = world->groundHeight(x1+49,z1-15)+1;
				}else{
					if(zona3 == false){
						continue;
					}
					x1 += 30;
					z1 += 23;
					y1 = tempy;
					y1 = world->groundHeight(x1+49,z1-15)+1;
				}
				if(world->GetBlock(x1, y1, z1 ) == 93)
				{
					y1 -= 1;
				}
				if(y1 <= 60)
				{
					y1 = 61;
				}
				
				//BaseDeTronco
				for(int y2 = 1; y2 < 5; y2++)
				{
					for(int x2 = 45; x2 < 50; x2++)
					{
						for(int z2 = -15; z2 < -11; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 8;
						}
					}
				}
				//parchear suelo
				for(int y2 = -10; y2 < 0; y2++)
				{
					for(int x2 = 45; x2 < 50; x2++)
					{
						for(int z2 = -15; z2 < -11; z2++)
						{
							if(world->GetBlock(x1+x2,y1+y2,z1+z2) == 0 || world->GetBlock(x1+x2,y1+y2,z1+z2) == WaterBlock::getID() || world->GetBlock(x1+x2,y1+y2,z1+z2) == 93)
							{
								world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
							}
						}
					}
				}
				//BaseDeTronco2
				for(int y2 = 5; y2 < 6; y2++)
				{
					for(int x2 = 46; x2 < 49; x2++)
					{
						for(int z2 = -14; z2 < -12; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 8;
						}
					}
				}
				//BaseDeMadera
				for(int y2 = 1; y2 < 4; y2++)
				{
					for(int x2 = 46; x2 < 49; x2++)
					{
						for(int z2 = -15; z2 < -11; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
						}
					}
				}
				//BaseDeMadera2
				for(int y2 = 1; y2 < 4; y2++)
				{
					for(int x2 = 45; x2 < 50; x2++)
					{
						for(int z2 = -14; z2 < -12; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
						}
					}
				}
				//BaseDePiedra2
				for(int y2 = 0; y2 < 1; y2++)
				{
					for(int x2 = 45; x2 < 50; x2++)
					{
						for(int z2 = -15; z2 < -11; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
						}
					}
				}
				//Interior
				for(int y2 = 1; y2 < 5; y2++)
				{
					for(int x2 = 46; x2 < 49; x2++)
					{
						for(int z2 = -14; z2 < -12; z2++)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 0;
						}
					}
				}
				
				//Detalles
				world->GetBlock(x1+45,y1+4,z1-15) = 0;
				world->GetBlock(x1+45,y1+4,z1-12) = 0;
				world->GetBlock(x1+49,y1+4,z1-15) = 0;
				world->GetBlock(x1+49,y1+4,z1-12) = 0;
				//Ventanas
				world->GetBlock(x1+47,y1+2,z1-15) = 114;
				world->GetBlock(x1+47,y1+2,z1-12) = 114;
				
				//Puerta&Escaleras
				if(rand()%10 < 5)
				{
					if(j == 1){
						world->GetBlock(x1+49,y1+2,z1-13) = 57;
						world->GetBlock(x1+49,y1+1,z1-13) = 52;
					}
					if(j == 2 || j== 3){
						world->GetBlock(x1+47,y1+2,z1-15) = 57;
						world->GetBlock(x1+47,y1+1,z1-15) = 49;
					}
				}
				else
				{
					if(j == 1){
						world->GetBlock(x1+49,y1+2,z1-13) = 0;
						world->GetBlock(x1+49,y1+1,z1-13) = 0;
					}else{
						world->GetBlock(x1+47,y1+2,z1-15) = 0;
						world->GetBlock(x1+47,y1+1,z1-15) = 0;
					}
				}
				if(world->GetBlock(x1+50,y1-1,z1-13) != 0)
				{
					if(j == 1){
						world->GetBlock(x1+50,y1,z1-13) = 63;
					}
					if(j == 2 || j == 3){
						world->GetBlock(x1+47,y1,z1-16) = 60;
					}
				}
				
				x1 = tempx;
				z1 = tempz;
			}
		}
		
		if(test)//herreria
		{
			z1 += 9;
			x1 += 43;
			y1 = tempy;
			y1 = world->groundHeight(x1+6,z1+8)+1;
			if(world->GetBlock(x1, y1, z1 ) == 93)
			{
				y1 -= 1;
			}
			if(y1 <= 60)
			{
				y1 = 61;
			}
			///-----*HERRERIA*-----///		
			for(int y2 = 0; y2 <= 4; y2++)	//roca
			{
				for(int x2 = 0; x2 <= 6; x2++)
				{
					for(int z2 = 0; z2 <= 9; z2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
					}
				}
			}
			for(int y2 = -10; y2 < 0; y2++)	//parchear suelo
			{
				for(int x2 = 0; x2 <= 6; x2++)
				{
					for(int z2 = 0; z2 <= 9; z2++)
					{
						if(world->GetBlock(x1+x2,y1+y2,z1+z2) == 0 || world->GetBlock(x1+x2,y1+y2,z1+z2) == WaterBlock::getID() || world->GetBlock(x1+x2,y1+y2,z1+z2) == 93)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 36;
						}
					}
				}
			}
			for(int y2 = 1; y2 <= 3; y2++)	//madera
			{
				for(int x2 = 0; x2 <= 6; x2++)
				{
					for(int z2 = 0; z2 <= 5; z2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
					}
				}
			}
			for(int y2 = 1; y2 <= 3; y2++)	//vacio por fuera
			{
				for(int x2 = 3; x2 <= 6; x2++)
				{
					for(int z2 = 4; z2 <= 9; z2++)
					{
						if(x2 == 3 && z2 >= 4 && z2 <= 6 ){
							continue;
						}
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 0;
					}
				}
			}
			for(int y2 = 5; y2 < 6; y2++)	//lozas del techo
			{
				for(int x2 = 0; x2 <= 6; x2++)
				{
					for(int z2 = 0; z2 < 10; z2++)
					{
						if(x2 >= 1 && x2 <= 5 && z2 >= 1 && z2 <= 8 ){
							continue;
						}
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 85;
					}
				}
			}
			for(int y2 = 1; y2 <= 3; y2++)	//vaciar interior
			{
				for(int x2 = 1; x2 <= 5; x2++)
				{
					for(int z2 = 1; z2 <= 5; z2++)
					{
						if((x2 >= 3 && x2 <= 5 && z2 >= 3 && z2 <= 5) || (x2 == 1 && z2 == 1 && y2 == 1)){
							continue;
						}
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 0;
					}
				}
			}
			for(int y2 = 1; y2 <= 3; y2++)	//cercas de afuera
			{
				for(int x2 = 6; x2 < 7; x2++)
				{
					for(int z2 = 9; z2 >= 5; z2-=4)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 59;
					}
				}
			}
			for(int y2 = 1; y2 <= 4; y2++)	//troncos de madera
			{
				for(int x2 = 0; x2 <= 6; x2+=6)
				{
					for(int z2 = 0; z2 <= 3; z2+=3)
					{
						if(y2 >= 1 && x2 == 0 && z2 == 3){
							continue;
						}
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 8;
					}
				}
			}
			for(int y2 = 0; y2 < 1; y2++)	//escaleras
			{
				for(int x2 = 7; x2 < 8; x2++)
				{
					for(int z2 = 6; z2 < 9; z2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 63;
					}
				}
			}
			
			//decoracion
			world->GetBlock(x1+2,y1+1,z1+1) = 126;
			world->GetBlock(x1+1,y1+1,z1+2) = 128;
			world->GetBlock(x1+2,y1+1,z1+2) = 59;
			world->GetBlock(x1+2,y1+2,z1+2) = 204;
			world->GetBlock(x1+3,y1+1,z1+3) = 0;
			world->GetBlock(x1+3,y1+2,z1+3) = 0;
			world->GetBlock(x1+3,y1+3,z1+3) = 0;
			//exterior decoracion
			world->GetBlock(x1+1,y1+2,z1+9) = 115;
			world->GetBlock(x1+2,y1+2,z1+9) = 115;
			world->GetBlock(x1+1,y1+1,z1+8) = 112;
			world->GetBlock(x1+1,y1+1,z1+7) = 112;
			world->GetBlock(x1+3,y1+2,z1+6) = 106;
			world->GetBlock(x1+3,y1+3,z1+6) = 106;
			world->GetBlock(x1+5,y1+1,z1+8) = 91;
			world->GetBlock(x1+1,y1+2,z1+8) = 0;
			world->GetBlock(x1+1,y1+2,z1+7) = 0;
			world->GetBlock(x1+2,y1+2,z1+8) = 0;
			world->GetBlock(x1+2,y1+2,z1+7) = 0;
			
			world->GetBlock(x1+5,y1+2,z1+3) = 0;
			world->GetBlock(x1+5,y1+1,z1+3) = 0;
			
			///COFRE
			
			Chest *NewChest;
			world->GetBlock(x1+1,y1+1,z1+5) = 96;
			NewChest= new Chest(x1+1,y1+1,z1+5);
			for(int i = 0; i <= 5; i++)
			{
				int slot = rand() % 27;
				int item = -1;
				int am = 0;
				bool st = true;
				
				switch(rand() % 12)
				{
				case 0:
					item = 284;//apple
					am = 1+rand()%3;
				break;
				case 1:
					item = 278;//iron
					am = 1+rand()%5;
				break;
				case 2:
					item = 288;//bread
					am = 1+rand()%3;
				break;
				case 3:
					item = 46;//obisdian
					am = 3+rand()%7;
				break;
				case 4:
					item = 280;//gold
					am = 1+rand()%3;
				break;
				case 5:
					item = 279;//diamond
					am = 1+rand()%3;
				break;
				case 6:
					item = 252;//iron pickaxe
					am = 251;
					st = false;
				case 7:
					item = 257;//iron sword
					am = 251;
					st = false;
				break;
				case 8:
					item = 336;//iron helmet
					am = 165;
					st = false;
				break;
				case 9:
					item = 337;//iron chestplate
					am = 240;
					st = false;
				break;
				case 10:
					item = 338;//iron leggings
					am = 225;
					st = false;
				break;
				case 11:
					item = 339;//iron boots
					am = 195;
					st = false;
				break;
				}
				NewChest->chestSlotId[slot] = item;
				NewChest->chestSlotAm[slot] = am;
				NewChest->chestSlotSt[slot] = st;
			}
			world->mChests.push_back(NewChest);
			
			x1 = tempx;
			z1 = tempz;
		}
	}

}

void WorldGenerator::initDungeons(int WORLD_SIZE, CraftWorld *world)
{
    int xSize, ySize, zSize;
    int x, y, z;

    ySize = 5;

    for(int t = 0; t <= 4; t++)
    {
        xSize = 5+rand()%3;
        zSize = 5+rand()%3;

        x = 1 + rand() % (WORLD_SIZE-8);
        z = 1 + rand() % (WORLD_SIZE-8);

        y = 42-(t*12) + rand() % 12;
        if(t == 4)
        {
            y = 42-(3*12) + rand() % 12;
        }

        if(t == 0)
        {
            world->spawner_x = x;
            world->spawner_z = z;
        }

        bool no = false;

        for(int xx = x; xx <= x+xSize; xx++)
        {
            for(int yy = y; yy <= y+ySize; yy++)
            {
                for(int zz = z; zz <= z+zSize; zz++)
                {
                    if(world->GetBlock(xx,yy,zz) == 0)
                    {
                        no = true;
                    }
                    if(rand() % 100 > 80)
                    {
                        world->GetBlock(xx,yy,zz) = CobbleStone::getID();
                    }
                    else
                    {
                        world->GetBlock(xx,yy,zz) = MossyCobblestone::getID();
                    }
                }
            }
        }

        if(no)
        {
            t--;
            continue;
        }

        for(int xx = x+1; xx <= x+xSize-1; xx++)
        {
            for(int yy = y+1; yy <= y+ySize-1; yy++)
            {
                for(int zz = z+1; zz <= z+zSize-1; zz++)
                {
                    world->GetBlock(xx,yy,zz) = 0;
                }
            }
        }

        MonsterSpawnerEntity* NewObject = new MonsterSpawnerEntity(x+xSize/2,y+1,z+zSize/2,"zombie");
        world->GetBlock(x+xSize/2,y+1,z+zSize/2) = Spawner::getID();

        world->mMonsterSpawners.push_back(NewObject);

        for(int j = 0; j <= 1; j++)
        {
            Chest *NewChest;
            if(j == 0)
            {
                world->GetBlock(x+1,y+1,z+1) = 97;
                NewChest = new Chest(x+1,y+1,z+1);
            }
            else
            {
                world->GetBlock(x+xSize-1,y+1,z+zSize-1) = 94;
                NewChest = new Chest(x+xSize-1,y+1,z+zSize-1);
            }

            for(int i = 0; i <= 7; i++)
            {
                int slot = rand() % 27;
                int item = -1;
                int am = 0;
                bool st = true;

                int rand1 = rand() % 100;

                if(rand1 < 80)
                {
                    switch(rand() % 7)
                    {
                    case 0:
                        item = 284;
                        am = 1 + rand() % 2;
                    break;
                    case 1:
                        item = 286;
                        am = 2 + rand() % 3;
                    break;
                    case 2:
                        item = 288;
                        am = 2;
                    break;
                    case 3:
                        item = 322;
                        am = 5+rand()%5;
                    break;
                    case 4:
                        item = 311;
                        am = 1+rand()%4;
                    break;
                    case 5:
                        item = 314;
                        am = 6+rand()%8;
                    break;
                    case 6:
                        item = InkSac::getID();
                        am = 12+rand()%12;
                    break;
                    }
                }
                else if(rand1 >= 80 && rand1 < 95)
                {
                    switch(rand() % 4)
                    {
                    case 0:
                        item = 315;
                        am = 8;
                    break;
                    case 1:
                        item = 291;
                        am = 1;
                        st = false;
                    break;
                    case 2:
                        item = 278;
                        am = 2+rand()%4;
                    break;
                    case 3:
                        item = 280;
                        am = 2+rand()%4;
                    break;
                    }
                }
                else
                {
                    switch(rand() % 2)
                    {
                    case 0:
                        item = 285;
                        am = 1;
                    break;
                    case 1:
                        item = 279;
                        am = 1+rand()%2;
                    break;
                    }
                }

                NewChest->chestSlotId[slot] = item;
                NewChest->chestSlotAm[slot] = am;
                NewChest->chestSlotSt[slot] = st;
            }
            NewChest->chestSlotId[26] = 312;
            NewChest->chestSlotAm[26] = 3;
            NewChest->chestSlotSt[26] = true;
            world->mChests.push_back(NewChest);
        }
    }
	
	///MINESHAFT GENERATOR///
	int mineshaft = 1+rand()%2;
	for(int t = 1; t <= mineshaft; t++)
	{
		int tempx,tempz;
		int x1,y1,z1;
		
		bool con1 = false;
		bool con2 = false;
		bool con3 = false;
		bool con4 = false;
		
		x1 = rand()%200+20;
		z1 = rand()%200+20;
		y1 = 16;
		
		tempx = x1;
		tempz = z1;
		
		//AMOOR Eres el mejor, te quiero cariño :3
		if(rand()%10 < 5)
		{
			con1 = true;
		}
		if(rand()%10 < 5)
		{
			con2 = true;
		}
		if(rand()%10 < 5)
		{
			con3 = true;
		}
		if(rand()%10 < 5)
		{
			con4 = true;
		}
		
		///VERTICAL
		for(int j = 1; j < 7; j++)
		{
			if(j == 2)
			{
				x1 -= 24;
				z1 += 24;
			}
			if(j == 3)
			{
				x1 -= 24;
			}
			if(j == 4)
			{
				x1 += 24;
			}
			if(j == 5)
			{
				x1 -= 48;
			}
			if(j == 6)
			{
				z1 -= 24;
			}
			
			//limpiar cubo
			for(int y2 = 1; y2 < 4; y2++)
			{
				for(int x2 = 1; x2 < 4; x2++)
				{
					for(int z2 = -2; z2 <= 24; z2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 0;
					}
				}
			}
			//telaraña
			for(int y2 = 1; y2 < 4; y2++)
			{
				for(int x2 = 1; x2 < 4; x2++)
				{
					for(int z2 = -2; z2 <= 24; z2++)
					{
						if(rand() % 100 > 95)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 39;
						}
					}
				}
			}
			//generar suelo pseurandom
			for(int z2 = -2; z2 <= 24; z2++)
			{
				for(int x2 = 1; x2 < 4; x2++)
				{
					world->GetBlock(x1+x2,y1,z1+z2) = 34;
				}
			}	
			///MADERA
			for(int z2 = 1; z2 <= 21; z2+=5)
			{
				for(int x2 = 1; x2 <= 3; x2++)
				{
					for(int y2 = 3; y2 < 4; y2++)
					{
						if((z2 == 16 && x2 == 2) || (z2 == 6 && x2 == 2))
						{
							continue;
						}
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
					}					
				}
			}
			///FENCE
			for(int z2 = 1; z2 <= 21; z2+=5)
			{
				for(int x2 = 1; x2 <= 3; x2+=2)
				{
					for(int y2 = 1; y2 <= 2; y2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 59;
					}					
				}
			}
			
			x1 = tempx;
			z1 = tempz;
		}
		
		///HORIZONTAL
		for(int j = 1; j < 6; j++)
		{
			if(j == 2)
			{
				x1 -= 24;
			}
			if(j == 3)
			{
				z1 -= 24;
				x1 -= 48;
			}
			if(j == 4)
			{
				z1 -= 24;
				x1 -= 24;
			}
			if(j == 5)
			{
				z1 -= 24;
				x1 += 24;
			}
			
			//limpiar cubo
			for(int z2 = 22; z2 <= 24; z2++)
			{
				for(int x2 = 1; x2 <= 24; x2++)
				{
					for(int y2 = 1; y2 < 4; y2++)
					{
					
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 0;
					}
				}
			}
			//telaraña
			for(int z2 = 22; z2 <= 24; z2++)
			{
				for(int x2 = 1; x2 <= 24; x2++)
				{
					for(int y2 = 1; y2 < 4; y2++)
					{
						if(rand() % 100 > 95)
						{
							world->GetBlock(x1+x2,y1+y2,z1+z2) = 39;
						}
					}
				}
			}
			//generar suelo pseurandom
			for(int z2 = 22; z2 <= 24; z2++)
			{
				for(int x2 = 1; x2 <= 24; x2++)
				{
					world->GetBlock(x1+x2,y1,z1+z2) = 34;
				}
			}
			///MADERA
			for(int z2 = 22; z2 <= 24; z2++)
			{
				for(int x2 = 4; x2 <= 24; x2+=5)
				{
					for(int y2 = 3; y2 < 4; y2++)
					{
						if((z2 == 23 && x2 == 9) || (z2 == 23 && x2 == 19))
						{
							continue;
						}
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 34;
					}					
				}
			}
			///FENCE
			for(int z2 = 22; z2 <= 24; z2+=2)
			{
				for(int x2 = 4; x2 <= 24; x2+=5)
				{
					for(int y2 = 1; y2 <= 2; y2++)
					{
						world->GetBlock(x1+x2,y1+y2,z1+z2) = 59;
					}					
				}
			}
			
			x1 = tempx;
			z1 = tempz;
		}

		/*if(rand()%10<=5)
		{
			MonsterSpawnerEntity* NewObject = new MonsterSpawnerEntity(x1+2,y1+1,z1+8,"cavespider");
			world->GetBlock(x1+2,y1+1,z1+8) = Spawner::getID();

			world->mMonsterSpawners.push_back(NewObject);
		}*/
		
		///COFRES
		if(rand()%10<=5)
		{
			int chest = 1+rand()%2;
			for(int j = 1; j <= chest; j++)
			{
					Chest *NewChest;
					if(j == 1)
					{
						world->GetBlock(x1+3,y1+1,z1+13) = 94;
						NewChest = new Chest(x1+3,y1+1,z1+13);
					}
					else
					{
						world->GetBlock(x1+1,y1+1,z1+3) = 95;
						NewChest = new Chest(x1+1,y1+1,z1+3);
					}

					for(int i = 0; i <= 8; i++)
					{
						int slot = rand() % 27;
						int item = -1;
						int am = 0;
						bool st = true;

						int rand1 = rand() % 100;

						if(rand1 < 80)
						{
							switch(rand() % 6)
							{
							case 0:
								item = 307;//ladder
								am = 4+rand()%8;
							break;
							case 1:
								item = 308;//torch
								am = 2+rand()%16;
							break;
							case 2:
								item = 288;//bread
								am = 1+rand()%3;
							break;
							case 3:
								item = 277;//coal
								am = 3+rand()%8;
							break;
							case 4:
								item = 312;//melon seeds
								am = 2+rand()%4;
							break;
							case 5:
								item = 377;//pumpkin seeds
								am = 2+rand()%4;
							break;
							}
						}
						else if(rand1 >= 80 && rand1 < 95)
						{
							switch(rand() % 4)
							{
							case 0:
								item = 278;//iron ingot
								am = 1+rand()%5;
							break;
							case 1:
								item = 285;//golden apple
								am = 1;
							break;
							case 2:
								item = 298;//book
								am = 2+rand()%4;
							break;
							case 3:
								item = 280;//golden
								am = 1+rand()%3;
							break;
							}
						}
						else
						{
							switch(rand() % 2)
							{
							case 0:
								item = 252;//iron pickaxe
								am = 251;
								st = false;
							break;
							case 1:
								item = 279;//diamond
								am = 1+rand()%2;
							break;
							}
						}

						NewChest->chestSlotId[slot] = item;
						NewChest->chestSlotAm[slot] = am;
						NewChest->chestSlotSt[slot] = st;
					}
					world->mChests.push_back(NewChest);
			}
		}
		
		/*for(int y2 = 0; y2 < 52; y2++)
		{
			world->GetBlock(x1+2,60+y2,z1+2) = 5;
		}*/
	}
}