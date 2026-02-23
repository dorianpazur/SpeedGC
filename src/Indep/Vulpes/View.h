#ifndef VULPES_VIEW_H
#define VULPES_VIEW_H

#include <EABase/eabase.h>
#include <Vulpes/TextureCache.h>

#include <tWare/Align.h>
#include <tWare/Memory.h>
#include <tWare/Math.h>

#if defined(EA_PLATFORM_GAMECUBE)
#include "GC/ViewPlat.h"
#endif

enum VVIEW_ID
{
	VVIEW_FE,
	VVIEW_PLAYER1,
	VVIEW_PLAYER2,
	
	NUM_VVIEWS,
	
	VVIEW_FIRST_PLAYER = VVIEW_PLAYER1,
	VVIEW_LAST_PLAYER = VVIEW_PLAYER2,
};

enum VIEW_MODE
{
	VIEW_MODE_ONE,
	VIEW_MODE_TWOH,
};

struct vView
{
	VVIEW_ID ID;
	
	bool Active = false;
	float NearZ = 0.75f;
	float FarZ = 15000.0f;
	float FovDegrees = 60.0f;
	
	tMatrix4 ViewMatrix;
	tMatrix4 ProjectionMatrix;
	
	tVector3 Position;
	tVector3 Velocity;
	
	vRenderTarget* RenderTarget = NULL;
	
	void CalculateViewMatricies();
};

extern vView vViews[NUM_VVIEWS];
extern void vpInitViews();
extern void vSetCurrentRenderTarget(vRenderTarget* renderTarget); 
extern void MaybeChangeViewMode();
extern VIEW_MODE gCurViewMode;

#endif
