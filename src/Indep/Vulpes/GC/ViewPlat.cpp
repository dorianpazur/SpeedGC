
#include <Vulpes/View.h>
#include <ogc/system.h>
#include <gccore.h>

vRenderTarget vRenderTargets[NUM_RENDER_TARGETS];
extern GXRModeObj *rmode;
extern bool bSplitScreen;
extern bool bWideScreen;

void vView::CalculateViewMatricies()
{
	if (ID == VVIEW_PLAYER1 || ID == VVIEW_PLAYER2)
	{
		f32 w = rmode->viWidth;
		f32 h = rmode->viHeight;
		
		f32 aspectCorrect = 1.1f; // not quite the right size
		f32 aspect = (4.0f/3.0f);
		
		if (bSplitScreen)
			aspect *= 2.0f;
		if (bWideScreen)
			aspect *= 1.33333334f;
		
		guPerspective(*(Mtx44*)&ProjectionMatrix, FovDegrees, aspect, NearZ, FarZ);
		ProjectionMatrix[1][1] /= aspectCorrect;
	}
	else if (ID == VVIEW_ENVMAP)
	{
		guPerspective(*(Mtx44*)&ProjectionMatrix, 90, 1.0f, NearZ, FarZ);
	}
	else
	{
		guOrtho(*(Mtx44*)&ProjectionMatrix, -1.1f, 1.1f, bWideScreen ? -1.33333334f : -1.0f, bWideScreen ? 1.33333334f : 1.0f, -1.0f, 1.0f);
	}
}

void MaybeChangeViewMode()
{
	switch (gCurViewMode)
	{
	case VIEW_MODE_ONE:
		vViews[VVIEW_PLAYER1].Active = true;
		vViews[VVIEW_PLAYER2].Active = false;
		
		vRenderTargets[TARGET_DISPLAYBUFFER_P1].Left = 0;
		vRenderTargets[TARGET_DISPLAYBUFFER_P1].Top = 0;
		vRenderTargets[TARGET_DISPLAYBUFFER_P1].Width = rmode->fbWidth;
		vRenderTargets[TARGET_DISPLAYBUFFER_P1].Height = rmode->efbHeight;
		
		break;
	case VIEW_MODE_TWOH:
		vViews[VVIEW_PLAYER1].Active = true;
		vViews[VVIEW_PLAYER2].Active = true;
		
		vRenderTargets[TARGET_DISPLAYBUFFER_P1].Left = 0;
		vRenderTargets[TARGET_DISPLAYBUFFER_P1].Top = 0;
		vRenderTargets[TARGET_DISPLAYBUFFER_P1].Width = rmode->fbWidth;
		vRenderTargets[TARGET_DISPLAYBUFFER_P1].Height = (rmode->efbHeight / 2);
		
		vRenderTargets[TARGET_DISPLAYBUFFER_P2].Left = 0;
		vRenderTargets[TARGET_DISPLAYBUFFER_P2].Top = (rmode->efbHeight / 2);
		vRenderTargets[TARGET_DISPLAYBUFFER_P2].Width = rmode->fbWidth;
		vRenderTargets[TARGET_DISPLAYBUFFER_P2].Height = (rmode->efbHeight / 2);
		
		break;
	}
	
	vViews[TARGET_ENVMAP].Active = true;
	vRenderTargets[TARGET_ENVMAP].Left = 0;
	vRenderTargets[TARGET_ENVMAP].Top = 0;
	vRenderTargets[TARGET_ENVMAP].Width = 64;
	vRenderTargets[TARGET_ENVMAP].Height = 64;
}

void vSetCurrentRenderTarget(vRenderTarget* renderTarget)
{
	GX_SetViewport(renderTarget->Left,renderTarget->Top,renderTarget->Width,renderTarget->Height,0,1);
	GX_SetScissor(renderTarget->Left,renderTarget->Top,renderTarget->Width,renderTarget->Height);
}
