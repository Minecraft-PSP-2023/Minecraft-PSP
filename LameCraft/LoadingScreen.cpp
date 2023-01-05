#include "LoadingScreen.h"
#include "TextureHelper.h"
#include <pspthreadman.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdisplay.h>

#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Graphics/Sprite.h>

// font vars
#define default_size 0.5
#define default_big_size 0.687

#define ENGLISH 1
#define RUSSIAN 2

using namespace Aurora::Graphics;

Sprite* backSprite;
Sprite* loadSprite;
Sprite* subLoadSprite;

int LoadingScreen::readiness = 0;
int LoadingScreen::stateName = 0;

LoadingScreen::LoadingScreen()
{
	// the loadingscreen is loaded as a thread
	thid_ = sceKernelCreateThread("LoadingThread", RunLoadingScreen, 0x18, 0x10000, THREAD_ATTR_VFPU|THREAD_ATTR_USER, NULL);
	// start the thread
	sceKernelStartThread(thid_, 0, 0);
}


void LoadingScreen::KillLoadingScreen()
{
	// shut down the loading screen again.
	sceKernelTerminateDeleteThread(thid_);
	// free the mem space of the images
	delete backSprite;
	delete loadSprite;
	delete subLoadSprite;
}


int LoadingScreen::RunLoadingScreen(SceSize args, void *argp)
{
	// load up the images
	backSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Dirt),0,0,32,32);
	backSprite->Scale(2,2);

    loadSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Load),0,0,16,8);
	loadSprite->Scale(1,0.5f);

    subLoadSprite = new Sprite(TextureHelper::Instance()->GetTexture(TextureHelper::Load),0,8,16,8);
	subLoadSprite->Scale(1,0.5f);


    int tip = rand() % 6;
	float loadingProcess = 0.0f;

	// start the render loop
	while(1)
	{
		RenderManager::InstancePtr()->StartFrame(0.466,0.72,1);

		sceGuDisable(GU_DEPTH_TEST);
		sceGuEnable(GU_BLEND);
		sceGuColor(GU_COLOR(1,1,1,1.0f));

        for(int x = 0; x < 8; x++)
        {
            for(int y = 0; y < 5; y++)
            {
                backSprite->SetPosition(x*64,y*64);
                backSprite->Draw();

                sceGuDisable(GU_BLEND);
                sceGuEnable(GU_DEPTH_TEST);
            }
        }

        if(readiness == 0)
        {
            if(loadingProcess < 160)
            {
                loadingProcess += 1.1f;
            }
        }
        else
        {
            if(loadingProcess <= readiness/100.0f*160)
            {
                loadingProcess += 0.65f;
            }
        }

        subLoadSprite->NewScale(160);
        subLoadSprite->SetPosition(160,150);

        loadSprite->NewScale(loadingProcess);
        loadSprite->SetPosition(160,150);

        if(loadingProcess < 159)
        {
            subLoadSprite->Draw();
            loadSprite->Draw();
        }

		sceGuDisable(GU_BLEND);
		sceGuEnable(GU_DEPTH_TEST);

		//draw subtitles on buttons
        RenderManager::InstancePtr()->SetFontStyle(default_size,0xFFFFFFFF,0,0x00000200|0x00000000);

        if(RenderManager::InstancePtr()->GetFontLanguage() == ENGLISH)
        {
            if(stateName == 0)
            {
                RenderManager::InstancePtr()->DebugPrint(240,103,"Loading world");
            }
            else
            {
                RenderManager::InstancePtr()->DebugPrint(240,103,"Generating world");
                switch(stateName)
                {
                    case 1:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Building terrain %i %%",readiness);
                    break;
                    case 2:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Digging caves");
                    break;
                    case 3:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Planting pumpkins");
                    break;
                    case 4:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Growing trees");
                    break;
                    case 5:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Placing ore");
                    break;
                    case 6:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Creating shadow map");
                    break;
                    case 7:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Saving chunks");
                    break;
                }
            }

            RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.45f,0.45f,0.45f,1.0f),0,0x00000200|0x00000000);
            switch(tip)
            {
                case 0:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : press R+L to open inventory menu");
                break;
                case 1:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : place items, according to recipe and");
                RenderManager::InstancePtr()->DebugPrint(240,255,"press R to craft recipe item");
                break;
                case 2:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : press up button + R trigger to drop item");
                break;
                case 3:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : aim at crafting table and");
                RenderManager::InstancePtr()->DebugPrint(240,255,"press R to open crafting table menu");
                break;
                case 4:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : take food item in your hand and");
                RenderManager::InstancePtr()->DebugPrint(240,255,"press R trigger to eat it");
                break;
                case 5:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : build a small shelter before the night");
                RenderManager::InstancePtr()->DebugPrint(240,255,"otherwise zombies will slain you");
                break;
            }
        }
        if(RenderManager::InstancePtr()->GetFontLanguage() == RUSSIAN)
        {
            if(stateName == 0)
            {
                RenderManager::InstancePtr()->DebugPrint(240,103,"Cargando mundo");
            }
            else
            {
                RenderManager::InstancePtr()->DebugPrint(240,103,"Generando mundo");
                switch(stateName)
                {
                    case 1:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Terreno en construccion %i %%",readiness);
                    break;
                    case 2:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Cavando cuevas");
                    break;
                    case 3:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Plantando calabazas");
                    break;
                    case 4:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Arboles en crecimiento");
                    break;
                    case 5:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Colocación de mineral");
                    break;
                    case 6:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Generando sombras");
                    break;
                    case 7:
                    RenderManager::InstancePtr()->DebugPrint(240,136,"Guardando chunks");
                    break;
                }
            }

            RenderManager::InstancePtr()->SetFontStyle(default_size,GU_COLOR(0.45f,0.45f,0.45f,1.0f),0,0x00000200|0x00000000);
            switch(tip)
            {
                case 0:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : presione R+L para abrir el inventario");
                break;
                case 1:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : Coloque los elementos, segun la receta y");
                RenderManager::InstancePtr()->DebugPrint(240,255,"presione R para elaborar el articulo de la receta");
                break;
                case 2:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : presione el boton up + R trigger para tirar objetos");
                break;
                case 3:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : apunta a la mesa de crafteo y");
                RenderManager::InstancePtr()->DebugPrint(240,255,"presione R para abrir el menu de elaboracion");
                break;
                case 4:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : toma el alimento en tu mano y");
                RenderManager::InstancePtr()->DebugPrint(240,255,"presione el gatillo R para comerlo");
                break;
                case 5:
                RenderManager::InstancePtr()->DebugPrint(240,240,"Tip : construye un pequeño refugio antes de la noche");
                RenderManager::InstancePtr()->DebugPrint(240,255,"de lo contrario, los zombis te mataran");
                break;
            }
        }

        RenderManager::InstancePtr()->SetFontStyle(default_size,0xFFFFFFFF,0,0x00000200|0x00000000);
		RenderManager::InstancePtr()->EndFrame();
	}
	return 0;
}

