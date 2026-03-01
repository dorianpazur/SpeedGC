
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
	if (!view)
		return;

	World* world = World::GetInstance();
	if (!world)
		return;
	for (int i = 0; i < world->mBatteryCount; i++)
	{
		Battery* bat = world->mBatteries[i];
		if (!bat || bat->mCollected || !bat->mBody)
			continue;
		bat->Render(view);
	}
}

void RenderWorld(vView* view)
{
	World* world = World::GetInstance();
	if (!world)
		return;
	
	// render test ground - TODO: replace this with actual track
	
	GX_LoadPosMtxImm(*(Mtx44*)&view->ViewMatrix, GX_PNMTX0);
	vEffectStaticState::pCurrentEffect = vEffects[VEFFECT_WORLDROAD];
	
	vEffectStaticState::pCurrentEffect->SetTexture(vTextureCache::GetTexture(CTStringHash("tarmac_diffuse")));
	
	if (view->ID == VVIEW_ENVMAP)
	{
		vEffectStaticState::pCurrentEffect->HalfBrightness = true;
	}
	
	vEffectStaticState::pCurrentEffect->Start();
	
	float maxRenderDistance = view->ID == VVIEW_ENVMAP ? 300.0f : 1000.0f;
	
	vPoly poly;
	
	// road
	for (int slice = -10; slice < 110; slice++)
	{
		if (std::abs((-(slice * 100.0f)) - view->Position.z) > maxRenderDistance)
			continue;
	
		poly.Vertices[0].x = -25.0f;
		poly.Vertices[0].y = 0;
		poly.Vertices[0].z = -(slice * 100.0f) - 50.0f;
		poly.UVs[0][0] = 0.0f;
		poly.UVs[0][1] = 0.0f;
		poly.Vertices[1].x = -25.0f;
		poly.Vertices[1].y = 0;
		poly.Vertices[1].z = -(slice * 100.0f) + 50.0f;
		poly.UVs[1][0] = 0.0f;
		poly.UVs[1][1] = 8.0f;
		poly.Vertices[2].x = 25.0f;
		poly.Vertices[2].y = 0;
		poly.Vertices[2].z = -(slice * 100.0f) + 50.0f;
		poly.UVs[2][0] = 16.06f;
		poly.UVs[2][1] = 8.0f;
		poly.Vertices[3].x = 25.0f;
		poly.Vertices[3].y = 0;
		poly.Vertices[3].z = -(slice * 100.0f) - 50.0f;
		poly.UVs[3][0] = 16.06f;
		poly.UVs[3][1] = 0.0f;
		
		poly.Colours[0][0] = 0xF6;
		poly.Colours[0][1] = 0xF6;
		poly.Colours[0][2] = 0xF9;
		poly.Colours[0][3] = 0xFF;
		
		*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];
		vPolyRender(&poly);
	}
	
	vEffectStaticState::pCurrentEffect->End();
	
	vEffectStaticState::pCurrentEffect->SetTexture(vTextureCache::GetTexture(CTStringHash("concrete")));
	
	if (view->ID == VVIEW_ENVMAP)
	{
		vEffectStaticState::pCurrentEffect->HalfBrightness = true;
	}
	
	vEffectStaticState::pCurrentEffect->Start();
	
	// edges
	for (int slice = -10; slice < 110; slice++)
	{
		if (std::abs((-(slice * 100.0f)) - view->Position.z) > maxRenderDistance)
			continue;
	
		poly.Vertices[0].x = -35.0f;
		poly.Vertices[0].y = 7;
		poly.Vertices[0].z = -(slice * 100.0f) - 50.0f;
		poly.UVs[0][0] = 0.0f;
		poly.UVs[0][1] = 0.0f;
		poly.Vertices[1].x = -35.0f;
		poly.Vertices[1].y = 7;
		poly.Vertices[1].z = -(slice * 100.0f) + 50.0f;
		poly.UVs[1][0] = 4.0f;
		poly.UVs[1][1] = 0.0f;
		poly.Vertices[2].x = -25.0f;
		poly.Vertices[2].y = 0;
		poly.Vertices[2].z = -(slice * 100.0f) + 50.0f;
		poly.UVs[2][0] = 4.0f;
		poly.UVs[2][1] = 1.0f;
		poly.Vertices[3].x = -25.0f;
		poly.Vertices[3].y = 0;
		poly.Vertices[3].z = -(slice * 100.0f) - 50.0f;
		poly.UVs[3][0] = 0.0f;
		poly.UVs[3][1] = 1.0f;
		
		poly.Colours[0][0] = 0xDF;
		poly.Colours[0][1] = 0xDA;
		poly.Colours[0][2] = 0xD6;
		poly.Colours[0][3] = 0xFF;
		
		*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];
		vPolyRender(&poly);
	
		poly.Vertices[0].x = 25.0f;
		poly.Vertices[0].y = 0;
		poly.Vertices[0].z = -(slice * 100.0f) - 50.0f;
		poly.UVs[0][0] = 0.0f;
		poly.UVs[0][1] = 1.0f;
		poly.Vertices[1].x = 25.0f;
		poly.Vertices[1].y = 0;
		poly.Vertices[1].z = -(slice * 100.0f) + 50.0f;
		poly.UVs[1][0] = 4.0f;
		poly.UVs[1][1] = 1.0f;
		poly.Vertices[2].x = 35.0f;
		poly.Vertices[2].y = 7;
		poly.Vertices[2].z = -(slice * 100.0f) + 50.0f;
		poly.UVs[2][0] = 4.0f;
		poly.UVs[2][1] = 0.0f;
		poly.Vertices[3].x = 35.0f;
		poly.Vertices[3].y = 7;
		poly.Vertices[3].z = -(slice * 100.0f) - 50.0f;
		poly.UVs[3][0] = 0.0f;
		poly.UVs[3][1] = 0.0f;
		
		poly.Colours[0][0] = 0x80;
		poly.Colours[0][1] = 0x80;
		poly.Colours[0][2] = 0x90;
		poly.Colours[0][3] = 0xFF;
		
		*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];
		vPolyRender(&poly);
	}
	
	vEffectStaticState::pCurrentEffect->End();
	
	// draw prop cubes and batteries before motion blur so they are blurred with the world
	DrawPropCubes(view);
	DrawBatteries(view);
	
	vEffectStaticState::pCurrentEffect = vEffects[VEFFECT_WORLDROAD];
	
	vEffectStaticState::pCurrentEffect->SetTexture(vTextureCache::GetTexture(CTStringHash("barrier")));
	
	if (view->ID == VVIEW_ENVMAP)
	{
		vEffectStaticState::pCurrentEffect->HalfBrightness = true;
	}
	
	vEffectStaticState::pCurrentEffect->Start();
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_CLEAR); // additive
	
	float UVOffset = std::floorf(world->mTimeElapsed * 2.0f) / 2.0f;
	float HorizUVScale = 16.0f;
	
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE); // disable zwrite
	GX_SetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 1.0f, {0,0,0} );
	
	// barriers
	for (int slice = 0; slice < 100; slice++)
	{
		if (std::abs((-(slice * 100.0f)) - view->Position.z) > maxRenderDistance)
			continue;
	
		poly.Vertices[0].x = -25.0f;
		poly.Vertices[0].y = 4;
		poly.Vertices[0].z = -(slice * 100.0f) - 50.0f;
		poly.UVs[0][0] = UVOffset;
		poly.UVs[0][1] = 0.0f;
		poly.Vertices[1].x = -25.0f;
		poly.Vertices[1].y = 4;
		poly.Vertices[1].z = -(slice * 100.0f) + 50.0f;
		poly.UVs[1][0] = HorizUVScale + UVOffset;
		poly.UVs[1][1] = 0.0f;
		poly.Vertices[2].x = -25.0f;
		poly.Vertices[2].y = 0;
		poly.Vertices[2].z = -(slice * 100.0f) + 50.0f;
		poly.UVs[2][0] = HorizUVScale + UVOffset;
		poly.UVs[2][1] = 1.0f;
		poly.Vertices[3].x = -25.0f;
		poly.Vertices[3].y = 0;
		poly.Vertices[3].z = -(slice * 100.0f) - 50.0f;
		poly.UVs[3][0] = UVOffset;
		poly.UVs[3][1] = 1.0f;
		
		poly.Colours[0][0] = 0xFF;
		poly.Colours[0][1] = 0xFF;
		poly.Colours[0][2] = 0xFF;
		poly.Colours[0][3] = 0xFF;
		
		*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
		*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];
		vPolyRender(&poly);
	
		poly.Vertices[0].x = 25.0f;
		poly.Vertices[0].y = 0;
		poly.Vertices[0].z = -(slice * 100.0f) - 50.0f;
		poly.UVs[0][0] = UVOffset;
		poly.UVs[0][1] = 1.0f;
		poly.Vertices[1].x = 25.0f;
		poly.Vertices[1].y = 0;
		poly.Vertices[1].z = -(slice * 100.0f) + 50.0f;
		poly.UVs[1][0] = UVOffset + HorizUVScale;
		poly.UVs[1][1] = 1.0f;
		poly.Vertices[2].x = 25.0f;
		poly.Vertices[2].y = 4;
		poly.Vertices[2].z = -(slice * 100.0f) + 50.0f;
		poly.UVs[2][0] = UVOffset + HorizUVScale;
		poly.UVs[2][1] = 0.0f;
		poly.Vertices[3].x = 25.0f;
		poly.Vertices[3].y = 4;
		poly.Vertices[3].z = -(slice * 100.0f) - 50.0f;
		poly.UVs[3][0] = UVOffset;
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
	
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE); // enable zwrite
	
	vEffectStaticState::pCurrentEffect->End();
}

