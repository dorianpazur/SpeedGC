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
#include "PropCube.h"
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
	
	DEF_TWARE_NEW_OVERRIDE(World, MAIN_POOL)
	
	static void Initialize();
	static void Uninit();
	static World* GetInstance();
	static void SpawnPlayer2();
	
	float mTimeElapsed = 0.0f;
	
	void Simulate(float timestep);
	bool ShouldPauseWorld();
	
	static constexpr int kMaxPropCubes = 300;

	std::vector<Vehicle*, tStdAllocator<Vehicle*>> mVehicles;
	PropCube* mPropCubes[kMaxPropCubes] = {};
	int mPropCubeCount = 0;

	// Race order along track (more negative Z = ahead). Indices into mVehicles (0 = player 1, 1 = player 2).
	int mFirstPlaceVehicleIndex = 0;
	int mSecondPlaceVehicleIndex = 1;
	
	static constexpr float kFinishLineZ = -4450.0f;
	bool mRaceFinished = false;
	int mWinnerVehicleIndex = -1;

	World();
	~World();
};

#endif
