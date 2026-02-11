#ifndef WORLD_WORLD_H
#define WORLD_WORLD_H

#include <cstddef>
#include <cstdio>
#include <vector>
#include <cstdint>
#include <EABase/eabase.h>
#ifdef EA_PLATFORM_GAMECUBE
#include <gccore.h>
#endif

#include "btBulletDynamicsCommon.h"
#include "Vehicle.h"
#include <tWare/Align.h>
#include <tWare/Memory.h>

class ALIGN(32) World
{
	btDefaultCollisionConfiguration* collisionConfiguration = NULL;
	btCollisionDispatcher* dispatcher = NULL;
	btBroadphaseInterface* overlappingPairCache = NULL;
	btSequentialImpulseConstraintSolver* solver = NULL;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	
	static World* gWorld;
	static bool ContactProcessedCallback(btManifoldPoint& cp, void* body0, void* body1);
public:
	btDiscreteDynamicsWorld* dynamicsWorld = NULL;
	
	DEF_TWARE_NEW_OVERRIDE(World)
	
	static void Initialize();
	static void Uninit();
	static World* GetInstance();
	
	void Simulate(float timestep);
	bool ShouldPauseWorld();
	
	std::vector<Vehicle*, tStdAllocator<Vehicle*>> mVehicles;
	
	World();
	~World();
};

#endif
