
#include <Vulpes/View.h>

#if defined(EA_PLATFORM_GAMECUBE)
#include "GC/ViewPlat.cpp"
#endif

vView vViews[NUM_VVIEWS];
VIEW_MODE gCurViewMode = VIEW_MODE_ONE;

void vpInitViews()
{
	for (int i = 0; i < NUM_VVIEWS; i++) // initialize IDs
	{
		vViews[i].ID = (VVIEW_ID)i;
	}
	
	// FE
	{
		vView* view = &vViews[VVIEW_FE];
		view->Active = true;
		view->RenderTarget = &vRenderTargets[TARGET_DISPLAYBUFFER_FULL];
	}
	
	// Player 1
	{
		vView* view = &vViews[VVIEW_PLAYER1];
		view->Active = true;
		view->RenderTarget = &vRenderTargets[TARGET_DISPLAYBUFFER_P1];
	}
	
	// Player 2
	{
		vView* view = &vViews[VVIEW_PLAYER2];
		view->Active = false;
		view->RenderTarget = &vRenderTargets[TARGET_DISPLAYBUFFER_P2];
	}
	
	// Envmap
	{
		vView* view = &vViews[VVIEW_ENVMAP];
		view->Active = true;
		view->RenderTarget = &vRenderTargets[TARGET_ENVMAP];
	}
}
