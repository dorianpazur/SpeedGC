
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
	
	// start line
	const float kStartLineZ = -5.0f;
	if (std::abs(kStartLineZ - view->Position.z) <= maxRenderDistance)
	{
		vEffectStaticState::pCurrentEffect = vEffects[VEFFECT_WORLDROAD];
		vEffectStaticState::pCurrentEffect->SetTexture(nullptr);
		if (view->ID == VVIEW_ENVMAP)
			vEffectStaticState::pCurrentEffect->HalfBrightness = true;
		vEffectStaticState::pCurrentEffect->Start();

		// checkerboard start line: small black/white squares across X and short depth along Z.
		const float kHalfDepth = 2.0f;   // total depth 5.4m
		const float kLineY = 0.02f;
		const int   kTilesX = 10;        // tiles across width
		const int   kTilesZ = 4;         // tiles along depth
		const float kTileWidth = 50.0f / (float)kTilesX;
		const float kTileDepth = (2.0f * kHalfDepth) / (float)kTilesZ;

		for (int ix = 0; ix < kTilesX; ++ix)
		{
			float x0 = -25.0f + (float)ix * kTileWidth;
			float x1 = x0 + kTileWidth;

			for (int iz = 0; iz < kTilesZ; ++iz)
			{
				float z0 = kStartLineZ - kHalfDepth + (float)iz * kTileDepth;
				float z1 = z0 + kTileDepth;

				bool isWhite = ((ix + iz) & 1) == 0;
				unsigned char r = isWhite ? 0xFF : 0x00;
				unsigned char g = isWhite ? 0xFF : 0x00;
				unsigned char b = isWhite ? 0xFF : 0x00;

				poly.Vertices[0].x = x0; poly.Vertices[0].y = kLineY; poly.Vertices[0].z = z0;
				poly.Vertices[1].x = x0; poly.Vertices[1].y = kLineY; poly.Vertices[1].z = z1;
				poly.Vertices[2].x = x1; poly.Vertices[2].y = kLineY; poly.Vertices[2].z = z1;
				poly.Vertices[3].x = x1; poly.Vertices[3].y = kLineY; poly.Vertices[3].z = z0;

				poly.Colours[0][0] = r; poly.Colours[0][1] = g; poly.Colours[0][2] = b; poly.Colours[0][3] = 0xFF;
				*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
				*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
				*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];

				vPolyRender(&poly);
			}
		}
		vEffectStaticState::pCurrentEffect->End();
	}


	// finish line
	if (std::abs(World::kFinishLineZ - view->Position.z) <= maxRenderDistance)
	{
		vEffectStaticState::pCurrentEffect = vEffects[VEFFECT_WORLDROAD];
		vEffectStaticState::pCurrentEffect->SetTexture(nullptr);
		if (view->ID == VVIEW_ENVMAP)
			vEffectStaticState::pCurrentEffect->HalfBrightness = true;
		vEffectStaticState::pCurrentEffect->Start();

		const float kFinishZ = World::kFinishLineZ;
		const float kHalfWidth = 8.0f;   // 16m along track so its visible
		const float kFinishY = 0.02f;   // slightly above road
		const int kNumStripes = 10;
		const float kStripeWidth = 50.0f / (float)kNumStripes;
		for (int i = 0; i < kNumStripes; i++)
		{
			float x0 = -25.0f + (float)i * kStripeWidth;
			float x1 = x0 + kStripeWidth;
			bool isWhite = (i & 1) == 0;
			unsigned char r = isWhite ? 0xFF : 0x00;
			unsigned char g = isWhite ? 0xFF : 0x00;
			unsigned char b = isWhite ? 0xFF : 0x00;

			poly.Vertices[0].x = x0;
			poly.Vertices[0].y = kFinishY;
			poly.Vertices[0].z = kFinishZ - kHalfWidth;
			poly.UVs[0][0] = 0.0f;
			poly.UVs[0][1] = 0.0f;
			poly.Colours[0][0] = r;
			poly.Colours[0][1] = g;
			poly.Colours[0][2] = b;
			poly.Colours[0][3] = 0xFF;

			poly.Vertices[1].x = x0;
			poly.Vertices[1].y = kFinishY;
			poly.Vertices[1].z = kFinishZ + kHalfWidth;
			*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];

			poly.Vertices[2].x = x1;
			poly.Vertices[2].y = kFinishY;
			poly.Vertices[2].z = kFinishZ + kHalfWidth;
			*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];

			poly.Vertices[3].x = x1;
			poly.Vertices[3].y = kFinishY;
			poly.Vertices[3].z = kFinishZ - kHalfWidth;
			*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];

			vPolyRender(&poly);
		}
		vEffectStaticState::pCurrentEffect->End();
	}

	vEffectStaticState::pCurrentEffect = vEffects[VEFFECT_WORLDROAD];
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
	
	vEffectStaticState::pCurrentEffect = vEffects[VEFFECT_WORLD];
	GX_LoadPosMtxImm(*(Mtx44*)&view->ViewMatrix, GX_PNMTX0);
	
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

