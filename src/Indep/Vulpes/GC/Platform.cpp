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
#include "xSpark/xSpark.hpp"
#include "xSpark/Render/xSprites.hpp"
#include "xSpark/EffectDefs.h"

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
const tMatrix4 gVfxMatrix;

extern double gFrameTime;

vTextureCache::CachedTexture* gMotionBlurTexture;

//---------------------------------------------------------------------------------

void vDisplayFrame()
{	
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_FALSE,rmode->vfilter);
	
	// handle view stuff
	gCurViewMode = bSplitScreen ? VIEW_MODE_TWOH : VIEW_MODE_ONE;
	MaybeChangeViewMode();
	for (int viewNum = 0; viewNum < NUM_VVIEWS; viewNum++)
	{
		vViews[viewNum].CalculateViewMatricies();
	}
	
	GXFogAdjTbl fogAdjTable;
	
	for (int viewNum = VVIEW_FIRST_PLAYER; viewNum <= VVIEW_LAST_PLAYER; viewNum++)
	{
		if (!vViews[viewNum].Active)
			continue;
		
		vSetCurrentRenderTarget(vViews[viewNum].RenderTarget);
		GX_LoadProjectionMtx(*(Mtx44*)&vViews[viewNum].ProjectionMatrix, GX_PERSPECTIVE);
		
		// disable fog for sky
		GX_SetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 1.0f, {0,0,0} );
		
		// render the sky
		StuffSky(&vViews[viewNum]);
		
		// enable fog for world
		GX_SetFog(GX_FOG_EXP2,
			0.0f,
			3000.0f,
			vViews[viewNum].NearZ,
			vViews[viewNum].FarZ,
		{0x9C, 0xBA, 0xDC} );
		
		// fix up fog so it's more realistic
		GX_InitFogAdjTable(&fogAdjTable, vViews[viewNum].RenderTarget->Width, *(Mtx44*)&vViews[viewNum].ProjectionMatrix);
		GX_SetFogRangeAdj(GX_ENABLE, vViews[viewNum].RenderTarget->Left + (vViews[viewNum].RenderTarget->Width / 2), &fogAdjTable);
		
		// render test ground - TODO: replace this with actual track
		{
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
			poly.UVs[1][1] = 1200.0f;
			poly.Vertices[2].x = 50.0f;
			poly.Vertices[2].y = 0;
			poly.Vertices[2].z = 100.0f;
			poly.UVs[2][0] = 8.0f;
			poly.UVs[2][1] = 1200.0f;
			poly.Vertices[3].x = 50.0f;
			poly.Vertices[3].y = 0;
			poly.Vertices[3].z = -10000.0f;
			poly.UVs[3][0] = 8.0f;
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
		}
		
		// draw prop cubes before motion blur so they are blurred with the world
		DrawPropCubes(&vViews[viewNum]);

		// disable fog for rest of rendering
		GX_SetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 1.0f, {0,0,0} );
		
		// motion blur
		GX_SetTexCopySrc(	vViews[viewNum].RenderTarget->Left,
							vViews[viewNum].RenderTarget->Top,
							vViews[viewNum].RenderTarget->Width,
							vViews[viewNum].RenderTarget->Height);	//This sets the location on the efb you want to copy from
		GX_SetTexCopyDst(vViews[viewNum].RenderTarget->Width, vViews[viewNum].RenderTarget->Height, GX_TF_RGBA8, 0);	//This is what kind of texture you want to copy from 
		
		GX_LoadProjectionMtx(*(Mtx44*)&gVfxMatrix, GX_ORTHOGRAPHIC);
		GX_LoadPosMtxImm(*(Mtx44*)&gIdentityMatrix, GX_PNMTX0);	
		
		// double passed, gets effectively 16 blur samples out of just 8
		for (int pass = 0; pass < 2; pass++)
		{
			float bluroffsets[4];
			
			tVector3 velocityVector = vViews[viewNum].Velocity;
			float velocityLength = sqrtf((velocityVector.x * velocityVector.x) + (velocityVector.y * velocityVector.y) + (velocityVector.z * velocityVector.z));
			
			if (velocityLength < 10.0f)
				break;
			
			velocityVector *= 1.0f / velocityLength;
			
			velocityLength -= 10.0f;
			velocityLength = std::fmin(1.0f, velocityLength * 0.003f) / 40.0f;
			
			velocityVector *= velocityLength;
			velocityVector.x *= 0.65f;
			
			bluroffsets[0] = -(velocityVector.x + velocityVector.z);
			bluroffsets[1] = -(velocityVector.y + velocityVector.z);
			bluroffsets[2] = -(velocityVector.x - velocityVector.z);
			bluroffsets[3] = -(velocityVector.y - velocityVector.z);
				
			GX_CopyTex(GX_GetTexObjData(&gMotionBlurTexture->GXTextureObj), GX_FALSE); // copy screen to texture
			
			float width = (float)vViews[viewNum].RenderTarget->Width / (float)gMotionBlurTexture->width;
			float height = (float)vViews[viewNum].RenderTarget->Height / (float)gMotionBlurTexture->height;
			
			for (int i = 4; i > 0; i--)
			{
				vPoly poly;
			
				poly.Vertices[0].x = -1.0f;
				poly.Vertices[0].y = -1.0f;
				poly.Vertices[0].z = 1;
				poly.Vertices[1].x = -1.0f;
				poly.Vertices[1].y = 1.0f;
				poly.Vertices[1].z = 1;
				poly.Vertices[2].x = 1.0f;
				poly.Vertices[2].y = 1.0f;
				poly.Vertices[2].z = 1;
				poly.Vertices[3].x = 1.0f;
				poly.Vertices[3].y = -1.0f;
				poly.Vertices[3].z = 1;
				
				poly.UVs[0][0] = -(i * bluroffsets[0]);
				poly.UVs[0][1] = -(i * bluroffsets[1]);
				
				poly.UVs[1][0] = -(i * bluroffsets[0]);
				poly.UVs[1][1] = -(i * bluroffsets[3]) + height;
				
				poly.UVs[2][0] = -(i * bluroffsets[2]) + width;
				poly.UVs[2][1] = -(i * bluroffsets[3]) + height;
				
				poly.UVs[3][0] = -(i * bluroffsets[2]) + width;
				poly.UVs[3][1] = -(i * bluroffsets[1]);
				
				poly.Colours[0][0] = 0xFF;
				poly.Colours[0][1] = 0xFF;
				poly.Colours[0][2] = 0xFF;
				poly.Colours[0][3] = 0xFF / 3;
				
				*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
				*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
				*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];
				
				vEffectStaticState::pCurrentEffect = vEffects[VEFFECT_FE];
				
				vEffectStaticState::pCurrentEffect->SetTexture(gMotionBlurTexture);
				vEffectStaticState::pCurrentEffect->Start();
				vPolyRender(&poly);
				vEffectStaticState::pCurrentEffect->End();
			}
		}
		
		// continue 3D rendering
		GX_LoadProjectionMtx(*(Mtx44*)&vViews[viewNum].ProjectionMatrix, GX_PERSPECTIVE);
		
		// render vehicles
		DrawVehicles(&vViews[viewNum]);
		
		// particles
		GX_LoadPosMtxImm(*(Mtx44*)&vViews[viewNum].ViewMatrix, GX_PNMTX0);
		NGSpriteManager.RenderAll(&vViews[viewNum]);
		
		// postfx
		GX_LoadProjectionMtx(*(Mtx44*)&gVfxMatrix, GX_ORTHOGRAPHIC);
		GX_LoadPosMtxImm(*(Mtx44*)&gIdentityMatrix, GX_PNMTX0);
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
	if (twkDeflicker)
		GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter); // enable deflicker filter if it's on
	
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(xfb[currentBuffer],GX_FALSE);
	
	GX_DrawDone();
	
	VIDEO_SetNextFramebuffer(xfb[currentBuffer]);
	
	VIDEO_Flush();
	
	if (twkVblankCount > 0)
	{
		static unsigned int lastVbla = tGetTicker();
		double maxTime = (1.0/60.0) * twkVblankCount;
		double frameTime = tGetTickerDifference(lastVbla);
		if (frameTime * 0.001 <= maxTime)
		{
			for (uint8_t i = 0; i < twkVblankCount; i++)
				VIDEO_WaitVSync();
		}
		lastVbla = tGetTicker();
	}
	
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
		
		gMotionBlurTexture = new vTextureCache::CachedTexture("Motion Blur", rmode->fbWidth, rmode->efbHeight, GX_TF_RGBA8);
		
		NGSpriteManager.Init();
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
	guOrtho(*(Mtx44*)&gVfxMatrix, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	GX_LoadProjectionMtx(*(Mtx44*)&gVfxMatrix, GX_ORTHOGRAPHIC);
	
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
