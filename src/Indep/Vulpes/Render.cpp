
#include <Vulpes/Render.h>
#include "World.h"
#include "Vehicle.h"

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
