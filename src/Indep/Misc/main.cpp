#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string.h>
#include <malloc.h>
#include <iostream>
#include <limits>

#include <tWare/Memory.h>
#include <tWare/Thread.h>
#include <tWare/Time.h>
#include <tWare/File.h>
#include <tWare/Hash.h>
#include <tWare/Debug.h>
#include "Audio.h"
#include "ScreenPrintf.h"

#include <Vulpes/vulpes.h> // graphics
#include "InputManager.h"

// debug stuff
#include "DebugMenu.h"
#include "DebugAssistant.h"
#include "DebugMenuRender.h"

#include "World.h" // physics stuff
#include "Vehicle.h"

#include "Revision.h"

#include <EABase/eabase.h> // platform checks

void InitializeEverything(int argc, char** argv);
void InitializePlatform(int argc, char** argv);
void InitializeMemory();
void LoadAssets();
void MainLoop();

tFile *gTestGLBFile = NULL;
vModel *gTestModel = NULL;

vModel* gCarModel = NULL;
vModel* gSkydomeModel = NULL;
vModel* gCubeModel = NULL;
vModel* gBatteryModel = NULL;

float CPUTime = 0.0f;
float GPUTime = 0.0f;
float gAvgFps = 0.0f;
double gFrameTime = 0.0f;

extern bool bSplitScreen;

bool bWantsExit = false;
bool bWantsReset = false;

//---------------------------------------------------------------------------------

void Main_DisplayFrame()
{
	unsigned int GPUTimeTemp = tGetTicker();
	
	vDisplayFrame();
	
	GPUTime = (float)tGetTickerDifference(GPUTimeTemp);
	
	DisplayDebugScreenPrints();
}

//---------------------------------------------------------------------------------

void* btAllocOverride(size_t size)
{
	return tWareMalloc(size, "Bullet physics", 0, ALLOC_PARAMS(PHYSICS_POOL, 16));
}

void* btAlignedAllocOverride(size_t size, int alignment)
{
	return tWareMalloc(size, "Bullet physics", 0, ALLOC_PARAMS(PHYSICS_POOL, alignment));
}

//---------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	InputManager::Initialize(); 
	do
	{
		if (bWantsReset)
		{
			puts("Resetting...");
			tMemoryPrintAllocationsByAddress(MAIN_POOL);
		}
		bWantsReset = false;
		#ifdef EA_PLATFORM_GAMECUBE
		SYS_STDIO_Report(true); // enable logging to dolphin logs
		#endif
		
		InitializeEverything(argc, argv);
		
		while (!bWantsExit && !bWantsReset)
		{
			MainLoop();
		}
		
		if (gTestModel)
		{
			delete gTestModel;
			gTestModel = NULL;
		}
		
		if (gCarModel)
		{
			delete gCarModel;
			gCarModel = NULL;
		}
		
		if (gCubeModel)
		{
			delete gSkydomeModel;
			gSkydomeModel = NULL;
		}
	
		if (gCubeModel)
		{
			delete gCubeModel;
			gCubeModel = NULL;
		}
		if (gBatteryModel && gBatteryModel != gCubeModel)
			delete gBatteryModel;
		gBatteryModel = NULL;

		Audio::Uninit();
		World::Uninit();
		NGSpriteManager.Reset();
		vTextureCache::Uninit();
		
		printf("Memory at exit:");
		tMemoryPrintAllocationsByAddress(MAIN_POOL);
		
	} while (bWantsReset);
	
	return 0;
}

//---------------------------------------------------------------------------------

void MainLoop()
{
	static unsigned int prevFrameTime = tGetTicker();
	
	static double avgfpsaccum = 0.0f;
	static int avgfpsaccumcount = 0;
	static double fps = 0.0f;
	
	unsigned int CPUTimeStart = tGetTicker();
	tThreadYield();
	unsigned int now = tGetTicker();
	
	gFrameTime = tGetTickerDifference(prevFrameTime, now);
	prevFrameTime = now;
	
	fps = 1.0 / (gFrameTime * 0.001);
	
	if (gFrameTime > 1000.0/12.0) // limit frame time
		gFrameTime = 1000.0/12.0;
	
	if (avgfpsaccumcount++ < 10)
		avgfpsaccum += fps;
	else
	{
		gAvgFps = avgfpsaccum / 10.0f;
		avgfpsaccum = 0;
		avgfpsaccumcount = 0;
	}
	
	InputManager::Update();
	
	if (InputManager::ShouldReset())
		bWantsReset = true;
	
	if (gDebugMenuIOHandler)
		gDebugMenuIOHandler->PollInput();
	
	World::GetInstance()->Simulate();
	
	CPUTime = (float)tGetTickerDifference(CPUTimeStart);
	
	Main_DisplayFrame();
	Audio::Update();
}

//---------------------------------------------------------------------------------

void InitializeEverything(int argc, char** argv)
{
	printf("Speed revision: %s\n", Revision);
	#ifdef EA_PLATFORM_GAMECUBE
	printf("Free arena memory before init: %u kb\n", ((uint32_t)SYS_GetArenaHi() - (uint32_t)SYS_GetArenaLo()) / 1024);
	#endif
	
	InitializeMemory();
	tInitThreads();
	InitializePlatform(argc, argv);
	vEffectInit();
	vpInitViews();
	tInitTicker();
	vTextureCache::Init();
	DebugMenuInit();
	LoadAssets();
	World::Initialize();
	NGSpriteManager.Init();
	Audio::Init();
	
	#ifdef EA_PLATFORM_GAMECUBE
	printf("Free arena memory after init: %u kb\n", ((uint32_t)SYS_GetArenaHi() - (uint32_t)SYS_GetArenaLo()) / 1024);
	#endif
	
	//tMemoryPrintAllocationsByAddress(MAIN_POOL);
	//tMemoryPrintAllocationsByAddress(PHYSICS_POOL);
	//tMemoryPrintAllocationsByAddress(TINYGLTF_POOL);
}

//---------------------------------------------------------------------------------

void InitializeMemory()
{
	static bool initializedMemory = false;
	
	if (!initializedMemory)
	{
		tInitializeMemory();
		
		const size_t kPhysicsMemoryPoolSize = 0x400000; // 4mb
		tInitMemoryPool(PHYSICS_POOL, tWareMalloc(kPhysicsMemoryPoolSize, "Physics Pool", __LINE__, ALLOC_PARAMS(MAIN_POOL, 0)), kPhysicsMemoryPoolSize, "Physics Pool");
		btAlignedAllocSetCustom(btAllocOverride, tFree);
		btAlignedAllocSetCustomAligned(btAlignedAllocOverride, tFree);
		
		const size_t kTinyGLTFMemoryPoolSize = 0x80000; // increase this if memory runs out in the pool when loading a model
		tInitMemoryPool(TINYGLTF_POOL, tWareMalloc(kTinyGLTFMemoryPoolSize, "TinyGLTF Pool", __LINE__, ALLOC_PARAMS(MAIN_POOL, 0)), kTinyGLTFMemoryPoolSize, "TinyGLTF Pool");
		
		const size_t kMeshMemoryPoolSize = 0x200000; // increase this if memory runs out in the pool when loading a model
		tInitMemoryPool(MODEL_POOL, tWareMalloc(kMeshMemoryPoolSize, "Mesh Pool", __LINE__, ALLOC_PARAMS(MAIN_POOL, 0)), kMeshMemoryPoolSize, "Mesh Pool");
		
		const size_t kTextureMemoryPoolSize = 0x400000; // increase this if memory runs out in the pool when loading a model
		tInitMemoryPool(TEXTURE_POOL, tWareMalloc(kTextureMemoryPoolSize, "Texture Pool", __LINE__, ALLOC_PARAMS(MAIN_POOL, 0)), kTextureMemoryPoolSize, "Texture Pool");
		
		initializedMemory = true;
	}
}

//---------------------------------------------------------------------------------

void LoadAssets()
{
	vTextureCache::LoadTextureFromPath("Global/DefaultTexture.tpl");
	vTextureCache::LoadTextureFromPath("Global/particles.tpl");
	vTextureCache::LoadTextureFromPath("Global/Fonts/Arial.tpl");
	vTextureCache::LoadTextureFromPath("Global/EnvmapFresnel.tpl");
	vTextureCache::LoadTextureFromPath("World/tarmac_diffuse.tpl");
	vTextureCache::LoadTextureFromPath("World/tarmac_spec.tpl");
	vTextureCache::LoadTextureFromPath("World/concrete.tpl");
	vTextureCache::LoadTextureFromPath("Global/CarShadow.tpl");
	vTextureCache::LoadTextureFromPath("Global/vignette.tpl");
	vTextureCache::LoadTextureFromPath("Global/FE/RVM.tpl");
	vTextureCache::LoadTextureFromPath("Global/FE/RVMMask.tpl");
	
	//tMemoryPrintAllocationsByAddress(MAIN_POOL);
	
	gCubeModel = vModel::CreateCube({ 0x30, 0x30, 0x38, 0xFF }); // dark grey-blue cube
	
	gSkydomeModel = new vModel("Global/skydome.glb");
	
	gCarModel = new vModel("Vehicles/126p/126p.glb");

	gBatteryModel = new vModel("sphere.glb");
	if (gBatteryModel && gBatteryModel->IsEmpty())
	{
		delete gBatteryModel;
		gBatteryModel = vModel::CreateCube({ 0xFF, 0xFF, 0x00, 0xFF });
	}
}
