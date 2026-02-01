#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <iso9660.h>
#include <iostream>
#include <limits>
#include <fat.h>
#include <dirent.h>

#include <tWare/Time.h>
#include <tWare/File.h>
#include <tWare/Hash.h>
#include "ScreenPrintf.h"

#include <Vulpes/vulpes.h> // graphics

#include "World.h"

// debug stuff
#include "DebugMenu.h"
#include "DebugAssistant.h"
#include "DebugMenuRender.h"

static uint8_t currentBuffer = 0;
static void *xfb[2] = { NULL, NULL}; // double buffered
static GXRModeObj *rmode = NULL;

f32 yscale;
u32 xfbHeight;
GXColor background = {0x7F, 0x40, 0xFF, 0xff};
Mtx44 viewMtx[2];
Mtx44 projMtx[2]; // view and perspective matrices

void InitializeEverything(int argc, char** argv);
void InitializePlatform(int argc, char** argv);
 
void draw_init();

tFile *gTestGLBFile = NULL;
vModel *gTestModel = NULL;

bool bSplitScreen = true;
bool bWideScreen = true;

float CPUTime = 0.0f;
float GPUTime = 0.0f;
float gAvgFps = 0.0f;

int twkVblankCount = 1;

//---------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	#ifdef GEKKO
	SYS_STDIO_Report(true); // enable logging to dolphin logs
	#endif
	
	InitializeEverything(argc, argv);
	
	unsigned int prevFrameTime = tGetTicker();
	VIDEO_WaitVSync();
	
	float avgfpsaccum = 0.0f;
	int avgfpsaccumcount = 0;
	float fps = 0.0f;
	
	prevFrameTime = tGetTicker();
	
	Mtx guiMtx;
	Mtx identityMtx;
	
	
	while(1) {
		guOrtho(guiMtx, -1.1f, 1.1f, bWideScreen ? -1.33333334f : -1.0f, bWideScreen ? 1.33333334f : 1.0f, -1.0f, 1.0f);
		guMtxIdentity(identityMtx);
		
		// setup our camera at the origin
		// looking down the -z axis with y up
		guVector cam = {0.0F, -2.0F, 18.0F},
				up = {0.0F, 1.0F, 0.0F},
			look = {0.0F, -1.0F, 1.0F};
		guLookAt(viewMtx[0], &cam, &up, &look);
		
		cam = {4.0F, -2.0F, 10.0F},
				up = {0.0F, 1.0F, 0.0F},
			look = {-8.0F, -1.0F, 0.5F};
		guLookAt(viewMtx[1], &cam, &up, &look);
	
		// setup our projection matrix
		// this creates a perspective matrix with a view angle of 60,
		// an aspect ratio of 4/3 (i'm not sure if that's the right
		// way to do it but i just went by what made a square on my screen)
		// and z near and far distances
		f32 w = rmode->viWidth;
		f32 h = rmode->viHeight;
		f32 aspect = (f32)w/h;
		f32 aspectCorrect = aspect / (4.0f/3.0f); // not quite the right size
		
		if (bSplitScreen)
			aspect *= 2.0f;
		if (bWideScreen)
			aspect *= 1.33333334f;
		
		guPerspective(projMtx[0], 60 * aspectCorrect, aspect / aspectCorrect, 0.1F, 1000.0F);
		GX_LoadProjectionMtx(projMtx[0], GX_PERSPECTIVE);
		
		w = rmode->viWidth;
		h = rmode->viHeight;
		aspect = (f32)w/h;
		aspectCorrect = aspect / (4.0f/3.0f); // not quite the right size
		
		if (bSplitScreen)
			aspect *= 2.0f;
		
		guPerspective(projMtx[1], 60 * aspectCorrect, aspect / aspectCorrect, 0.1F, 1000.0F);
	
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
		
		unsigned int CPUTimeStart = tGetTicker();
		
		PAD_ScanPads();
		
		if (gDebugMenuIOHandler)
			gDebugMenuIOHandler->PollInput();
		
		int buttonsDown = PAD_ButtonsDown(0);
		int buttonsPressed = PAD_ButtonsHeld(0);
		
		World::GetInstance()->Simulate(frameTime * 0.001f);
		
		CPUTime = tGetTickerDifference(CPUTimeStart, tGetTicker());
		
		// draw
		
		unsigned int GPUTimeTemp = tGetTicker();
		
		GX_InvVtxCache();
		GX_InvalidateTexAll();
		
		for (int viewNum = 0; viewNum < (bSplitScreen ? 2 : 1); viewNum++)
		{
			GX_LoadProjectionMtx(projMtx[0], GX_PERSPECTIVE);
			if (bSplitScreen)
			{
				GX_SetViewport(0,(rmode->efbHeight / 2) * viewNum,rmode->fbWidth,rmode->efbHeight/2,0,1); // TODO - add actual view class
				GX_SetScissor(0,(rmode->efbHeight / 2) * viewNum,rmode->fbWidth,rmode->efbHeight/2);
			}
			else
			{
				GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1); // TODO - add actual view class
				GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
			}
			
			float transformFlt[16];
			Mtx44 transform;
			
			for (int j = World::GetInstance()->dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
			{
				btCollisionObject* obj = World::GetInstance()->dynamicsWorld->getCollisionObjectArray()[j];
				btRigidBody* body = btRigidBody::upcast(obj);
				btTransform trans;
				if (body && body->getMotionState())
				{
					body->getMotionState()->getWorldTransform(trans);
				}
				else
				{
					trans = obj->getWorldTransform();
				}
				
				// TODO: TEMPORARY - REMOVE THIS
				if (!World::GetInstance()->ShouldPauseWorld() && body)
				{
					if (buttonsPressed & PAD_BUTTON_A)
					{
						body->activate();
						body->applyCentralImpulse( btVector3( 0.f, 100.0f * frameTime * 0.001f, 0.0f ) );
					}
					
					if (buttonsDown & PAD_BUTTON_B)
					{
						body->activate();
						body->applyCentralImpulse( btVector3( 0.f, 0.0f, -100.0f * frameTime * 0.001f ) );
					}
					
					if (buttonsDown & PAD_BUTTON_X)
					{
						body->activate();
						body->applyCentralImpulse( btVector3( 0.f, 100.0f * frameTime * 0.001f, -10000.0f * frameTime * 0.001f ) );
					}
				}
				
				trans.getOpenGLMatrix(transformFlt);
				
				transform[0][0]=transformFlt[0];
				transform[1][0]=transformFlt[1];
				transform[2][0]=transformFlt[2];
				
				transform[0][1]=transformFlt[4];
				transform[1][1]=transformFlt[5];
				transform[2][1]=transformFlt[6];
				
				transform[0][2]=transformFlt[8];
				transform[1][2]=transformFlt[9];
				transform[2][2]=transformFlt[10];
				
				transform[0][3]=transformFlt[12];
				transform[1][3]=transformFlt[13];
				transform[2][3]=transformFlt[14];
				
				gTestModel->Render(viewMtx[viewNum], transform);
			}
		}
		
		GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
		GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
		
		GX_LoadProjectionMtx(guiMtx, GX_ORTHOGRAPHIC);
		GX_LoadPosMtxImm(identityMtx, GX_PNMTX0);
		
		GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
		
		DebugMenu::render();
		DebugMenu::renderBackground();
		
		DrawScreenPrintfs();
		
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(xfb[currentBuffer],GX_TRUE);
		
		GX_DrawDone();
		
		VIDEO_SetNextFramebuffer(xfb[currentBuffer]);
		
		VIDEO_Flush();
		
		for (uint8_t i = 0; i < twkVblankCount; i++)
			VIDEO_WaitVSync();
		
		currentBuffer ^= 1;
		
		DisplayDebugScreenPrints();
		
		GPUTime = tGetTickerDifference(GPUTimeTemp, tGetTicker());
	}
	
	delete gTestModel;

	return 0;
}

//---------------------------------------------------------------------------------

void InitializeEverything(int argc, char** argv)
{
	#ifdef GEKKO
	printf("Free memory before init: %u kb\n", ((uint32_t)SYS_GetArenaHi() - (uint32_t)SYS_GetArenaLo()) / 1024);
	#endif
	
	InitializePlatform(argc, argv);
	draw_init();
	World::Initialize();
	DebugMenuInit();
	
	#ifdef GEKKO
	printf("Free memory after init: %u kb\n", ((uint32_t)SYS_GetArenaHi() - (uint32_t)SYS_GetArenaLo()) / 1024);
	#endif
}

//---------------------------------------------------------------------------------

void InitializePlatform(int argc, char** argv) {
	VIDEO_Init();
	PAD_Init();
	
	rmode = VIDEO_GetPreferredMode(NULL);

	void *gp_fifo = NULL;
	gp_fifo = memalign(32,GX_FIFO_MINSIZE);
	memset(gp_fifo,0,GX_FIFO_MINSIZE);
	
	xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	//console_init(xfb[currentBuffer],20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	FILE* checkFile = NULL;
	// check if SD gecko is present and game is at the especified location
	if (fatInitDefault() && (checkFile = fopen("./Speed/Speed.dol", "rb")))
	{
		printf("\n\nFound SD Gecko\n");
		tChangeBaseDir("./Speed/");
	}
	else // otherwise try loading from DVD
	{
		printf("\n\nMounting DVD\n");
		
		DVD_Init();
		ISO9660_Mount("dvd", &__io_gcdvd);
		
		tChangeBaseDir("dvd://");
	}
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb[currentBuffer]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
	
	// initialize GX
	GX_Init(gp_fifo, GX_FIFO_MINSIZE);
	
	// clear
	GX_SetCopyClear(background, 0x00ffffff);
	
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));
	
	if (rmode->aa) {
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	} else {
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	}
	
	GX_SetCullMode(GX_CULL_BACK);
	GX_CopyDisp(xfb[currentBuffer],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
	
	// init some sort of matrix to prevent noise on boot
	Mtx44 matrix;
	guOrtho(matrix, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	GX_LoadProjectionMtx(matrix, GX_ORTHOGRAPHIC);
}

//---------------------------------------------------------------------------------

void draw_init()
{
	vTextureCache::LoadTextureFromPath("Global/DefaultTexture.tpl");
	vTextureCache::LoadTextureFromPath("Global/Fonts/Arial.tpl");
	
	gTestModel = new vModel("sonic/sonic.glb");
}
