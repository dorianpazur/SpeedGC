
#include <Vulpes/Render.h>
#include <Vulpes/Poly.h>
#include "World.h"
#include "Vehicle.h"
#include "PropCube.h"
#include "Battery.h"

extern vModel *gSkydomeModel;

void StuffSky(vView* view)
{
	tMatrix4 transform;
	tMatrix4 skyTransform;
	
	tInvertMatrix(&transform, &view->ViewMatrix);
	skyTransform[0][3] = transform[0][3];
	skyTransform[1][3] = transform[1][3];
	skyTransform[2][3] = transform[2][3];
	
	gSkydomeModel->Render(view, &skyTransform);
}

void DrawVehicles(vView* view)
{
	World* world = World::GetInstance();
	if (world) 
	{
		for (size_t veh = 0; veh < world->mVehicles.size(); veh++)
		{
			Vehicle* vehicle = world->mVehicles[veh];
			
			if (!vehicle || !vehicle->mBody)
				continue;
			
			vehicle->Render(view);
		}
	}
}

void DrawPropCubes(vView* view)
{
	World* world = World::GetInstance();
	
	if (!world)
		return;

	for (int i = 0; i < world->mPropCubeCount; i++)
	{
		PropCube* cube = world->mPropCubes[i];
		if (!cube || !cube->mBody)
			continue;
		cube->Render(view);
	}
}

void DrawBatteries(vView* view)
{
	World* world = World::GetInstance();
	if (!world)
		return;
	for (int i = 0; i < world->mBatteryCount; i++)
	{
		Battery* bat = world->mBatteries[i];
		if (!bat || bat->mCollected)
			continue;
		bat->Render(view);
	}
}

void RenderWorld(vView* view)
{
	// render test ground - TODO: replace this with actual track
	
	GX_LoadPosMtxImm(*(Mtx44*)&view->ViewMatrix, GX_PNMTX0);
	vEffectStaticState::pCurrentEffect = vEffects[VEFFECT_WORLDROAD];
	
	vEffectStaticState::pCurrentEffect->SetTexture(vTextureCache::GetTexture(CTStringHash("tarmac_diffuse")));
	vEffectStaticState::pCurrentEffect->Start();
	
	if (view->ID == VVIEW_ENVMAP)
	{
		vEffectStaticState::pCurrentEffect->HalfBrightness = true;
	}
	
	for (int slice = 0; slice < 100; slice++)
	{
		vPoly poly;
	
		poly.Vertices[0].x = -25.0f;
		poly.Vertices[0].y = 0;
		poly.Vertices[0].z = -(slice * 100.0f) - 100.0f;
		poly.UVs[0][0] = 0.0f;
		poly.UVs[0][1] = 0.0f;
		poly.Vertices[1].x = -25.0f;
		poly.Vertices[1].y = 0;
		poly.Vertices[1].z = -(slice * 100.0f) + 100.0f;
		poly.UVs[1][0] = 0.0f;
		poly.UVs[1][1] = 8.0f;
		poly.Vertices[2].x = 25.0f;
		poly.Vertices[2].y = 0;
		poly.Vertices[2].z = -(slice * 100.0f) + 100.0f;
		poly.UVs[2][0] = 8.0f;
		poly.UVs[2][1] = 8.0f;
		poly.Vertices[3].x = 25.0f;
		poly.Vertices[3].y = 0;
		poly.Vertices[3].z = -(slice * 100.0f) - 100.0f;
		poly.UVs[3][0] = 8.0f;
		poly.UVs[3][1] = 0.0f;
		
		poly.Colours[0][0] = 0xFF;
		poly.Colours[0][1] = 0xFF;
		poly.Colours[0][2] = 0xFF;
		poly.Colours[0][3] = 0xFF;
		
		*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];
		vPolyRender(&poly);
	}
	vEffectStaticState::pCurrentEffect->End();
}

