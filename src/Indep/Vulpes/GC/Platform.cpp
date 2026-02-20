// Gamecube-specific stuff

#include <ogc/system.h>
#include <Vulpes/vulpes.h>
#include <tWare/Time.h>
#include <tWare/File.h>
#include <tWare/Hash.h>
#include <tWare/Math.h>

#include "DebugMenu.h"
#include "DebugAssistant.h"
#include "DebugMenuRender.h"
#include "ScreenPrintf.h"

#include "World.h"
#include "Vehicle.h"

//---------------------------------------------------------------------------------

static uint8_t currentBuffer = 0;
static void *xfb[2] = { NULL, NULL}; // double buffered
GXRModeObj *rmode = NULL;

f32 yscale;
u32 xfbHeight;
GXColor background = {0x00, 0x00, 0x00, 0xff};

bool bSplitScreen = false;
bool bWideScreen = true;
int twkVblankCount = 1;
bool twkDeflicker = false;

const tMatrix4 gIdentityMatrix;

extern double gFrameTime;

//---------------------------------------------------------------------------------

void vDisplayFrame()
{
	static tVector3 camTarget[2] { tVector3(0.0f, 0.0f, 0.0f), tVector3(0.0f, 0.0f, 0.0f) };
	static tVector3 camPos[2] { tVector3(0.0f, 0.0f, 0.0f), tVector3(30.0f, 30.0f, 30.0f) };
	static tVector3 camUp[2] { tVector3(0.0f, 1.0f, 0.0f), tVector3(0.0f, 1.0f, 0.0f) };
	static float tilt[2] { 0.0f, 0.0f };
	static float distance[2] { 10.0f, 10.0f };
	
	tMatrix4 guiMtx;
		
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,twkDeflicker,rmode->vfilter);
	
	// TODO - replace this with camera movers
	World* world = World::GetInstance();
	if (world) 
	{
		for (size_t veh = 0; veh < world->mVehicles.size(); veh++)
		{
			Vehicle* vehicle = world->mVehicles[veh];
			
			if (!vehicle || !vehicle->mBody)
				continue;
			
			if (veh >= 2)
				break;
			
			btTransform trans;
			btRigidBody* body = vehicle->mBody;
	
			if (body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
			{
				trans = body->getWorldTransform();
			}
			
			tMatrix4 transform;
			float transformFlt[16];
			trans.getOpenGLMatrix(transformFlt);
			
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
			
			tVector3 localVehPos = tVector3(0, 0, 0);
			tVector3 globalVehPos;
			tMulVector(&globalVehPos, &transform, &localVehPos);
			
			tilt[veh] = std::lerp(tilt[veh], body->getAngularVelocity().getY(), gFrameTime * 0.004f);
			
			float speed = std::fmax(0.0f, body->getLinearVelocity().length() - 0.01f);
			
			float kBaseDistance = 7.0f;
			float targetDistance = kBaseDistance;
			float speedFOVThing = std::min(1.0f, std::powf(speed * 0.015f, 1.5f) * 0.65f);
			vViews[VVIEW_FIRST_PLAYER + veh].FovDegrees = 60.0f * (1 + speedFOVThing * 0.45f); 
			targetDistance -= (speedFOVThing * 4.0f); // bring in closer when getting faster
			targetDistance += std::min(3.0f, std::powf(speed * 0.02f, 2.0f) * 1.5f); // move it away when initially gaining speed
			
			distance[veh] = std::lerp(distance[veh], targetDistance, gFrameTime * 0.002f);
			
			tVector3 camPosLocal = tVector3(tilt[veh], 3.0f, -distance[veh]);
			tVector3 camTargetLocal = tVector3(0.0f, 2.0f, 15.0f + (kBaseDistance - distance[veh]));
			tVector3 camUpOffset = tVector3(-tilt[veh] * 0.05f, 0.0f, 0.0f);
			
			tMulVector(&camPos[veh], &transform, &camPosLocal);
			tMulVector(&camTarget[veh], &transform, &camTargetLocal);
			tMulVector(&camUp[veh], &transform, &camUpOffset);
			
			// fix them up
			camTarget[veh].y = transformFlt[13] + 2.0f;
			camPos[veh].y = transformFlt[13] + 3.0f;
			camUp[veh] -= globalVehPos; // make it still local to the vehicle but rotated
			camUp[veh].y = 1.0f;
		}
	}
	
	// setup our camera at the origin
	// looking down the -z axis with y up
	guVector cam = {camPos[0].x, camPos[0].y, camPos[0].z},
			up = {camUp[0].x, camUp[0].y, camUp[0].z},
		look = {camTarget[0].x, camTarget[0].y, camTarget[0].z};
	guLookAt(*(Mtx44*)&vViews[VVIEW_PLAYER1].ViewMatrix, &cam, &up, &look);
	
	cam = {camPos[1].x, camPos[1].y, camPos[1].z},
			up = {camUp[1].x, camUp[1].y, camUp[1].z},
		look = {camTarget[1].x, camTarget[1].y, camTarget[1].z};
	guLookAt(*(Mtx44*)&vViews[VVIEW_PLAYER2].ViewMatrix, &cam, &up, &look);
	
	// handle view stuff
	gCurViewMode = bSplitScreen ? VIEW_MODE_TWOH : VIEW_MODE_ONE;
	MaybeChangeViewMode();
	for (int viewNum = 0; viewNum < NUM_VVIEWS; viewNum++)
	{
		vViews[viewNum].CalculateViewMatricies();
	}
	
	for (int viewNum = VVIEW_FIRST_PLAYER; viewNum <= VVIEW_LAST_PLAYER; viewNum++)
	{
		if (!vViews[viewNum].Active)
			continue;
		
		vSetCurrentRenderTarget(vViews[viewNum].RenderTarget);
		GX_LoadProjectionMtx(*(Mtx44*)&vViews[viewNum].ProjectionMatrix, GX_PERSPECTIVE);
		
		// render the sky
		StuffSky(&vViews[viewNum]);
		
		// render test ground - TODO: replace this with actual track
		vPoly poly;
	
		poly.Vertices[0].x = -50.0f;
		poly.Vertices[0].y = 0;
		poly.Vertices[0].z = -10000.0f;
		poly.UVs[0][0] = 0.0f;
		poly.UVs[0][1] = 0.0f;
		poly.Vertices[1].x = -50.0f;
		poly.Vertices[1].y = 0;
		poly.Vertices[1].z = 100.0f;
		poly.UVs[1][0] = 0.0f;
		poly.UVs[1][1] = 400.0f;
		poly.Vertices[2].x = 50.0f;
		poly.Vertices[2].y = 0;
		poly.Vertices[2].z = 100.0f;
		poly.UVs[2][0] = 4.0f;
		poly.UVs[2][1] = 400.0f;
		poly.Vertices[3].x = 50.0f;
		poly.Vertices[3].y = 0;
		poly.Vertices[3].z = -10000.0f;
		poly.UVs[3][0] = 4.0f;
		poly.UVs[3][1] = 0.0f;
		
		poly.Colours[0][0] = 0xFF;
		poly.Colours[0][1] = 0xFF;
		poly.Colours[0][2] = 0xFF;
		poly.Colours[0][3] = 0xFF;
		
		*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];
		
		GX_LoadPosMtxImm(*(Mtx44*)&vViews[viewNum].ViewMatrix, GX_PNMTX0);
		vEffectStaticState::pCurrentEffect = vEffects[VEFFECT_STANDARD];
	
		vEffectStaticState::pCurrentEffect->SetTexture(vTextureCache::GetTexture(CTStringHash("DefaultTexture")));
		vEffectStaticState::pCurrentEffect->Start();
		vPolyRender(&poly);
		vEffectStaticState::pCurrentEffect->End();
		
		// render vehicles
		DrawVehicles(&vViews[viewNum]);
	}
	
	// gui
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	
	GX_LoadProjectionMtx(*(Mtx44*)&vViews[VVIEW_FE].ProjectionMatrix, GX_ORTHOGRAPHIC);
	GX_LoadPosMtxImm(*(Mtx44*)&gIdentityMatrix, GX_PNMTX0);
	
	GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	
	DebugMenu::render();
	DebugMenu::renderBackground();
	
	DrawScreenPrintfs();
	
	// push frame
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(xfb[currentBuffer],GX_FALSE);
	
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
		gp_fifo = tWareMalloc(GX_FIFO_MINSIZE, "Vulpes GP FIFO", __LINE__, ALLOC_PARAMS(MAIN_POOL, 32));
		memset(gp_fifo,0,GX_FIFO_MINSIZE);
		
		xfb[0] = MEM_K0_TO_K1(tWareMalloc(VIDEO_GetFrameBufferSize(rmode), "Vulpes Display Buffer 1", __LINE__, ALLOC_PARAMS(MAIN_POOL, 32)));
		xfb[1] = MEM_K0_TO_K1(tWareMalloc(VIDEO_GetFrameBufferSize(rmode), "Vulpes Display Buffer 2", __LINE__, ALLOC_PARAMS(MAIN_POOL, 32)));
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
	
	// clear texture cache
	GX_InvalidateTexAll();
	
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
	
	//if (!initialized)
	//{
	//	GXRModeObj rmodeObjTmp;
	//	memcpy(&rmodeObjTmp, rmode, sizeof(GXRModeObj));
	//	GX_AdjustForOverscan(&rmodeObjTmp, rmode, 0, 16);
	//}
	
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
