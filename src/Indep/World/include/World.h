#ifndef WORLD_WORLD_H
#define WORLD_WORLD_H

#include <cstddef>
#include <cstdio>
#include <cstdint>

#include "btBulletDynamicsCommon.h"

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

#endif
