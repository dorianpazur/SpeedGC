// Gamecube-specific stuff

#include <ogc/system.h>
#include <Vulpes/vulpes.h>
#include <tWare/Time.h>
#include <tWare/File.h>
#include <tWare/Hash.h>

#include "DebugMenu.h"
#include "DebugAssistant.h"
#include "DebugMenuRender.h"
#include "ScreenPrintf.h"

#include "World.h"
#include "Vehicle.h"

//---------------------------------------------------------------------------------

static uint8_t currentBuffer = 0;
static void *xfb[2] = { NULL, NULL}; // double buffered
static GXRModeObj *rmode = NULL;

f32 yscale;
u32 xfbHeight;
GXColor background = {0x7F, 0x40, 0xFF, 0xff};
Mtx44 viewMtx[2];
Mtx44 projMtx[2]; // view and perspective matrices

bool bSplitScreen = false;
bool bWideScreen = true;
int twkVblankCount = 1;
bool twkDeflicker = false;

// test models
extern tFile *gTestGLBFile;
extern vModel *gTestModel;

extern vModel *gCarModel;

//---------------------------------------------------------------------------------

void vDisplayFrame()
{
	static vVector3 camTarget[2];
	
	Mtx guiMtx;
	Mtx identityMtx;
		
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,twkDeflicker,rmode->vfilter);
	guOrtho(guiMtx, -1.1f, 1.1f, bWideScreen ? -1.33333334f : -1.0f, bWideScreen ? 1.33333334f : 1.0f, -1.0f, 1.0f);
	guMtxIdentity(identityMtx);
	
	// setup our camera at the origin
	// looking down the -z axis with y up
	guVector cam = {0.0F, 5.0F, 18.0F},
			up = {0.0F, 1.0F, 0.0F},
		look = {camTarget[0].x, camTarget[0].y, camTarget[0].z};
	guLookAt(viewMtx[0], &cam, &up, &look);
	
	cam = {0.0F, 5.0F, 18.0F},
			up = {0.0F, 1.0F, 0.0F},
		look = {camTarget[1].x, camTarget[1].y, camTarget[1].z};
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
		
		vPoly poly;
	
		poly.Vertices[0].x = -100.0f;
		poly.Vertices[0].y = 0;
		poly.Vertices[0].z = -100.0f;
		poly.UVs[0][0] = 0.0f;
		poly.UVs[0][1] = 0.0f;
		poly.Vertices[1].x = -100.0f;
		poly.Vertices[1].y = 0;
		poly.Vertices[1].z = 100.0f;
		poly.UVs[1][0] = 0.0f;
		poly.UVs[1][1] = 1.0f;
		poly.Vertices[2].x = 100.0f;
		poly.Vertices[2].y = 0;
		poly.Vertices[2].z = 100.0f;
		poly.UVs[2][0] = 1.0f;
		poly.UVs[2][1] = 1.0f;
		poly.Vertices[3].x = 100.0f;
		poly.Vertices[3].y = 0;
		poly.Vertices[3].z = -100.0f;
		poly.UVs[3][0] = 1.0f;
		poly.UVs[3][1] = 0.0f;
		
		poly.Colours[0][0] = 0xFF;
		poly.Colours[0][1] = 0xFF;
		poly.Colours[0][2] = 0xFF;
		poly.Colours[0][3] = 0xFF;
		
		*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];
		
		GX_LoadPosMtxImm(viewMtx[viewNum], GX_PNMTX0);
		vPolyRender(&poly, vTextureCache::GetTexture(CTStringHash("DefaultTexture")));
		
		if (gCarModel)
		{
			for (size_t veh = 0; veh < World::GetInstance()->mVehicles.size(); veh++)
			{
				btTransform trans;
				btRigidBody* body = World::GetInstance()->mVehicles[veh]->mBody;
	
				if (body->getMotionState())
				{
					body->getMotionState()->getWorldTransform(trans);
				}
				else
				{
					trans = body->getWorldTransform();
				}
	
				trans.getOpenGLMatrix(transformFlt);
					
				camTarget[veh].x = transformFlt[12];
				camTarget[veh].y = transformFlt[13];
				camTarget[veh].z = transformFlt[14];
	
				// Bullet (OpenGL) using GX matrix
				transform[0][0] = transformFlt[0];
				transform[1][0] = transformFlt[1];
				transform[2][0] = transformFlt[2];
	
				transform[0][1] = transformFlt[4];
				transform[1][1] = transformFlt[5];
				transform[2][1] = transformFlt[6];
	
				transform[0][2] = transformFlt[8];
				transform[1][2] = transformFlt[9];
				transform[2][2] = transformFlt[10];
	
				transform[0][3] = transformFlt[12];
				transform[1][3] = transformFlt[13];
				transform[2][3] = transformFlt[14];
	
				gCarModel->Render(viewMtx[viewNum], transform);
			}
		}
	}
	
	// gui
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	
	GX_LoadProjectionMtx(guiMtx, GX_ORTHOGRAPHIC);
	GX_LoadPosMtxImm(identityMtx, GX_PNMTX0);
	
	GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	
	DebugMenu::render();
	DebugMenu::renderBackground();
	
	DrawScreenPrintfs();
	
	// push frame
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(xfb[currentBuffer],GX_TRUE);
	
	GX_DrawDone();
	
	VIDEO_SetNextFramebuffer(xfb[currentBuffer]);
	
	VIDEO_Flush();
	
	for (uint8_t i = 0; i < twkVblankCount; i++)
		VIDEO_WaitVSync();
	
	currentBuffer ^= 1;
}

//---------------------------------------------------------------------------------

void InitializePlatform(int argc, char** argv) {
	static bool initialized = false;
	if (!initialized)
	{
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
	}
	
	// clear
	GX_SetCopyClear(background, 0x00ffffff);
	
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
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
	
	initialized = true;
	bSplitScreen = false;
}

//---------------------------------------------------------------------------------

extern bool bWantsReset;

void UpdatePlatform()
{	
	PAD_ScanPads();
	
	int buttonsPressed = PAD_ButtonsHeld(0);
	
	if (SYS_ResetButtonDown() || (buttonsPressed & PAD_TRIGGER_R && buttonsPressed & PAD_TRIGGER_Z && buttonsPressed & PAD_BUTTON_B && buttonsPressed & PAD_BUTTON_START))
	{
		bWantsReset = true;
	}
}
