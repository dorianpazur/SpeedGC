#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string.h>
#include <malloc.h>
#include <iostream>
#include <limits>

#include <tWare/Time.h>
#include <tWare/File.h>
#include <tWare/Hash.h>
#include "ScreenPrintf.h"

#include <Vulpes/vulpes.h> // graphics

// debug stuff
#include "DebugMenu.h"
#include "DebugAssistant.h"
#include "DebugMenuRender.h"

#include "World.h" // physics stuff

#include <EABase/eabase.h> // platform checks

void InitializeEverything(int argc, char** argv);
void InitializePlatform(int argc, char** argv);
 
void draw_init();

tFile *gTestGLBFile = NULL;
vModel *gTestModel = NULL;

float CPUTime = 0.0f;
float GPUTime = 0.0f;
float gAvgFps = 0.0f;

//---------------------------------------------------------------------------------

void Main_DisplayFrame()
{
	unsigned int GPUTimeTemp = tGetTicker();
	
	vDisplayFrame();
	
	GPUTime = tGetTickerDifference(GPUTimeTemp, tGetTicker());
	
	DisplayDebugScreenPrints();
}

//---------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	#ifdef EA_PLATFORM_GAMECUBE
	SYS_STDIO_Report(true); // enable logging to dolphin logs
	#endif
	
	InitializeEverything(argc, argv);
	
	unsigned int prevFrameTime = tGetTicker();
	
	float avgfpsaccum = 0.0f;
	int avgfpsaccumcount = 0;
	float fps = 0.0f;
	
	while(1) {
		unsigned int CPUTimeStart = tGetTicker();
		unsigned int now = tGetTicker();
		
		float frameTime = tGetTickerDifference(prevFrameTime, now);
		prevFrameTime = now;
		
		if (frameTime > 1000.0f/12.0f) // limit frame time
			frameTime = 1000.0f/12.0f;
		
		fps = 1.0f / (frameTime * 0.001f);
		
		if (avgfpsaccumcount++ < 10)
			avgfpsaccum += fps;
		else
		{
			gAvgFps = avgfpsaccum / 10.0f;
			avgfpsaccum = 0;
			avgfpsaccumcount = 0;
		}
		
		PollInputs();
		
		if (gDebugMenuIOHandler)
			gDebugMenuIOHandler->PollInput();
		
		World::GetInstance()->Simulate(timestep * 0.001f);
			
		CPUTime = tGetTickerDifference(CPUTimeStart, tGetTicker());
		
		Main_DisplayFrame();
	}
	
	delete gTestModel;

	return 0;
}

//---------------------------------------------------------------------------------

void InitializeEverything(int argc, char** argv)
{
	#ifdef EA_PLATFORM_GAMECUBE
	printf("Free memory before init: %u kb\n", ((uint32_t)SYS_GetArenaHi() - (uint32_t)SYS_GetArenaLo()) / 1024);
	#endif
	
	InitializePlatform(argc, argv);
	draw_init();
	World::Initialize();
	DebugMenuInit();
	
	#ifdef EA_PLATFORM_GAMECUBE
	printf("Free memory after init: %u kb\n", ((uint32_t)SYS_GetArenaHi() - (uint32_t)SYS_GetArenaLo()) / 1024);
	#endif
}

//---------------------------------------------------------------------------------

void draw_init()
{
	vTextureCache::LoadTextureFromPath("Global/DefaultTexture.tpl");
	vTextureCache::LoadTextureFromPath("Global/Fonts/Arial.tpl");
	
	gTestModel = new vModel("sonic/sonic.glb");
}
