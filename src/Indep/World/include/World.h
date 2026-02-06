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

class World
{
	btDefaultCollisionConfiguration* collisionConfiguration = NULL;
	btCollisionDispatcher* dispatcher = NULL;
	btBroadphaseInterface* overlappingPairCache = NULL;
	btSequentialImpulseConstraintSolver* solver = NULL;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	
	static World* gWorld;
public:
	btDiscreteDynamicsWorld* dynamicsWorld = NULL;
	
	static void Initialize();
	static World* GetInstance();
	
	void Simulate(float timestep);
	bool ShouldPauseWorld();
	
	World();
	~World();
};

extern std::vector<Vehicle*> gVehicles;

#endif
