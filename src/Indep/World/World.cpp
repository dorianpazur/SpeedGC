
#include "World.h"
#include "DebugAssistant.h"
#include "ISimable.h"
#include <BulletCollision/NarrowPhaseCollision/btPersistentManifold.h>
#include <Vulpes/Platform.h>
#include <Vulpes/Particles.h>
#include <tWare/Time.h>
#include "InputManager.h"
#include "InputCommand.h"
#include "PropCube.h"

World* World::gWorld = NULL;
extern double gFrameTime;

//---------------------------------------------------------------------------------

void World::Initialize()
{
	if (!gWorld)
		gWorld = new World();
}

//---------------------------------------------------------------------------------

void World::Uninit()
{
	if (gWorld)
	{
		delete gWorld;
		gWorld = NULL;
	}
}

//---------------------------------------------------------------------------------

World* World::GetInstance()
{
	return gWorld;
}

//---------------------------------------------------------------------------------

World::World()
{
	mVehicles.reserve(8);
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	
	btDefaultCollisionConstructionInfo collisionConstructionInfo;
	collisionConstructionInfo.m_defaultMaxCollisionAlgorithmPoolSize = 1023;
	collisionConstructionInfo.m_defaultMaxPersistentManifoldPoolSize = 1023;
	//collisionConstructionInfo.m_defaultStackAllocatorSize = 0;
	
	collisionConfiguration = new btDefaultCollisionConfiguration(collisionConstructionInfo);
	
	tMemoryPrintAllocationsByAddress(MAIN_POOL);
	
	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();	
	
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;
	
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	
	btContactSolverInfo& info = dynamicsWorld->getSolverInfo();
	info.m_numIterations = 4;
	
	dynamicsWorld->setGravity(btVector3(0, -9.80665f, 0));

	gContactProcessedCallback = &World::ContactProcessedCallback;

	///-----initialization_end-----

	// CREATE CUBE VEHICLE

	// Vehicle constructor creates the body internally
	mVehicles.emplace_back(new Vehicle(dynamicsWorld, btVector3(0, 1, 0)));
	
	// 100 rows every 100 units covers full 10km. 3cubes per row
	{
		const float kHalfXZ = 4.0f;
		const float kHalfY = 2.0f;
		const float kCubeY = kHalfY;
		const float kSpacing = 100.0f; // one row every 100 units // 100 rows = 10,000
		const float kStartZ = -100.0f;
		const float kLaneMin = -15.0f; //left edge
		const float kLaneMax = 15.0f; //right edge
		const float kGapHalf = 8.0f;  // half-width of each gap

		const int kNumRows = kMaxPropCubes / 3; // 100 rows ,,  3 cubes per row = 300

		for (int row = 0; row < kNumRows && mPropCubeCount + 3 <= kMaxPropCubes; row++)
		{
			float z = kStartZ - row * kSpacing;

			// pseudo-random gap centres using different hash seeds 
			uint32_t h0 = (uint32_t)(row * 2654435761u);
			uint32_t h1 = (uint32_t)(row * 2246822519u);

			// gap 0  left third of lane
			float t0 = (float)(h0 & 0xFFFF) / 65535.0f;
			float gap0 = kLaneMin + (kLaneMax - kLaneMin) * t0 * 0.4f; // keeps gap0 in the left 40% of the lane
			if (gap0 < kLaneMin + kGapHalf)          gap0 = kLaneMin + kGapHalf;
			if (gap0 > kLaneMin + (kLaneMax - kLaneMin) * 0.45f) gap0 = kLaneMin + (kLaneMax - kLaneMin) * 0.45f;

			// gap 1  right third of lane
			float t1 = (float)(h1 & 0xFFFF) / 65535.0f;
			float gap1 = kLaneMin + (kLaneMax - kLaneMin) * (0.55f + t1 * 0.4f); //starts after 55% of the lane
			if (gap1 < kLaneMin + (kLaneMax - kLaneMin) * 0.55f) gap1 = kLaneMin + (kLaneMax - kLaneMin) * 0.55f; 
			if (gap1 > kLaneMax - kGapHalf)          gap1 = kLaneMax - kGapHalf;

			// left cube: lane left edge -- gap0 left edge
			float x0 = kLaneMin + (gap0 - kGapHalf - kLaneMin) * 0.5f;
			// middle cube: gap0 right edge -- gap1 left edge
			float x1 = (gap0 + kGapHalf) + ((gap1 - kGapHalf) - (gap0 + kGapHalf)) * 0.5f;
			// right cube: gap1 right edge -- lane right edge
			float x2 = (gap1 + kGapHalf) + (kLaneMax - (gap1 + kGapHalf)) * 0.5f;

			mPropCubes[mPropCubeCount++] = new PropCube(dynamicsWorld, btVector3(x0, kCubeY, z), btVector3(kHalfXZ, kHalfY, kHalfXZ));
			mPropCubes[mPropCubeCount++] = new PropCube(dynamicsWorld, btVector3(x1, kCubeY, z), btVector3(kHalfXZ, kHalfY, kHalfXZ));
			mPropCubes[mPropCubeCount++] = new PropCube(dynamicsWorld, btVector3(x2, kCubeY, z), btVector3(kHalfXZ, kHalfY, kHalfXZ));
		}
	}

	for (int i = 0; i < 100; i++)
	{
		{
			btCollisionShape* groundShape = new btBoxShape(btVector3(25, 1, 50)); //the ground
			
			collisionShapes.push_back(groundShape);
			
			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(0, -1, -100 * i)); //the ground position
		
			btDefaultMotionState* groundMotion =
				new btDefaultMotionState(groundTransform);
		
			//the fixed ground 
			btRigidBody::btRigidBodyConstructionInfo groundInfo( 0.0f, groundMotion, groundShape);
		
			btRigidBody* groundBody = new btRigidBody(groundInfo);
			//add body to the world
			dynamicsWorld->addRigidBody(groundBody);
		}
		
		{
			btCollisionShape* wallShape = new btBoxShape(btVector3(btScalar(1.), btScalar(100.), btScalar(50.)));
		
			collisionShapes.push_back(wallShape);
		
			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(26, -6, -100 * i));
		
			btScalar mass(0.);
		
			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);
		
			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				wallShape->calculateLocalInertia(mass, localInertia);
		
			//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, wallShape, localInertia);
			btRigidBody* body = new btRigidBody(rbInfo);
		
			//add the body to the dynamics world
			dynamicsWorld->addRigidBody(body);
		}
		
		{
			btCollisionShape* wallShape = new btBoxShape(btVector3(btScalar(1.), btScalar(100.), btScalar(50.)));
		
			collisionShapes.push_back(wallShape);
		
			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(-26, -6, -100 * i));
		
			btScalar mass(0.);
		
			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);
		
			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				wallShape->calculateLocalInertia(mass, localInertia);
		
			//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, wallShape, localInertia);
			btRigidBody* body = new btRigidBody(rbInfo);
		
			//add the body to the dynamics world
			dynamicsWorld->addRigidBody(body);
		}
	}
}

//---------------------------------------------------------------------------------

void World::Simulate(float timestep)
{
	if (!ShouldPauseWorld())
	{	
		mTimeElapsed += timestep;
		static float timeElapsedFix = 0.0f;
		const float kStepTime = 1.0f / 60.0f;
		bool ticked = false;
		
		timeElapsedFix += timestep;
		
		while (timeElapsedFix >= kStepTime)
		{
			ticked = true;
			// auto default inputs per vehicle this frame
			float engineForce[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			float brakeForce[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			float steering[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	
			const auto& cmds = InputManager::GetCommands();
	
			for (const InputCommand& cmd : cmds)
			{
				int idx = cmd.playerIndex;
				if (idx < 0 || idx > 3)
				{
					continue;
				}
	
				switch (cmd.type)
				{
				case InputCommandType::Accelerate:
					engineForce[idx] = cmd.value;
					break;
				case InputCommandType::Brake:
					brakeForce[idx] = cmd.value;
					break;
				case InputCommandType::Steer:
					steering[idx] = cmd.value;
					break;
				case InputCommandType::StartPressed:
					if (idx == 1 && mVehicles.size() < 2)
					{
						printf("StartPressed received for player %d, current vehicles: %zu\n", idx, mVehicles.size());
						printf("Adding second vehicle!\n");
						bSplitScreen = true;
						mVehicles.emplace_back(new Vehicle(dynamicsWorld, btVector3(20, 10, 0)));
					}
					break;
				case InputCommandType::ResetRequested:
					break; 				// handled in main through InputManager::ShouldReset()
				case InputCommandType::ControllerDisconnected:
					// game pause and show InputManager::IsControllerConnected
					break;
				default:
					break;
				}
			}
			
			for (size_t i = 0; i < mVehicles.size(); i++)
			{
				int idx = (int)i;
				if (idx < 0 || idx > 3)
					continue;
				//else if in range
				mVehicles[i]->Update(engineForce[idx], brakeForce[idx], steering[idx], kStepTime); 
			}
			
			dynamicsWorld->stepSimulation(kStepTime, 8);
	
			tVector3 testVel{-7.0f, 0, -7.0f};
			tMatrix4 testTransform;
			

			// Update race order by progress (more negative Z = further down track = 1st)
			if (mVehicles.size() >= 2)
			{
				float z0 = 0.0f;
				float z1 = 0.0f;
				if (mVehicles[0]->mBody && mVehicles[0]->mBody->getMotionState())
					z0 = mVehicles[0]->mBody->getWorldTransform().getOrigin().getZ();
				if (mVehicles[1]->mBody && mVehicles[1]->mBody->getMotionState())
					z1 = mVehicles[1]->mBody->getWorldTransform().getOrigin().getZ();

				if (z0 <= z1)
				{
					mFirstPlaceVehicleIndex = 0;
					mSecondPlaceVehicleIndex = 1;
				}
				else
				{
					mFirstPlaceVehicleIndex = 1;
					mSecondPlaceVehicleIndex = 0;
				}
			}

			testTransform[0][3] = 25.0f;
			testTransform[1][3] = 4.0f;
			testTransform[2][3] = -30.0f;
			
			UpdateXenonEmitters(kStepTime);
			
			timeElapsedFix -= kStepTime;
		}
		
		static tVector3 camTarget[2] { tVector3(0.0f, 0.0f, 0.0f), tVector3(0.0f, 0.0f, 0.0f) };
		static tVector3 camUp[2] { tVector3(0.0f, 1.0f, 0.0f), tVector3(0.0f, 1.0f, 0.0f) };
		static tVector3 prevCamPos[2] { tVector3(0.0f, 0.0f, 0.0f), tVector3(30.0f, 30.0f, 30.0f) };
		static float tilt[2] { 0.0f, 0.0f };
		static float distance[2] { 10.0f, 10.0f };
		
		// TODO - replace this with camera movers
		for (size_t veh = 0; veh < mVehicles.size(); veh++)
		{
			Vehicle* vehicle = mVehicles[veh];
			
			if (!vehicle || !vehicle->mBody)
				continue;
			
			if (veh >= 2)
				break;
			
			btTransform trans;
			btRigidBody* body = vehicle->mBody;
		
			if (body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
			{
				trans = body->getWorldTransform();
			}
			
			tMatrix4 transform;
			float transformFlt[16];
			trans.getOpenGLMatrix(transformFlt);
			
			// Bullet (OpenGL) using GX matrix
			transform[0][0] = transformFlt[0];
			transform[1][0] = transformFlt[1];
			transform[2][0] = transformFlt[2];
			
			transform[0][1] = transformFlt[4];
			transform[1][1] = transformFlt[5];
			transform[2][1] = transformFlt[6];
			
			transform[0][2] = transformFlt[8];
			transform[1][2] = transformFlt[9];
			transform[2][2] = transformFlt[10];
			
			transform[0][3] = transformFlt[12];
			transform[1][3] = transformFlt[13];
			transform[2][3] = transformFlt[14];
			
			tVector3 localVehPos = tVector3(0, 0, 0);
			tVector3 globalVehPos;
			tMulVector(&globalVehPos, &transform, &localVehPos);
			
			tilt[veh] = std::lerp(tilt[veh], body->getAngularVelocity().getY() / 2.0f, gFrameTime * 0.006f);
			
			float speed = std::fmax(0.0f, body->getLinearVelocity().length() - 0.01f);
			
			const float kBaseDistance = gCurViewMode == VIEW_MODE_ONE ? 4.0f : 4.5f;
			const float kBaseHeight = 1.15f;
			
			float targetDistance = kBaseDistance;
			float speedFOVThing = std::min(1.0f, std::powf(speed * 0.015f, 0.95f) * 0.35f);
			vViews[VVIEW_FIRST_PLAYER + veh].FovDegrees = 60.0f * (1 + speedFOVThing * 0.45f); 
			
			if (gCurViewMode != VIEW_MODE_ONE)
				vViews[VVIEW_FIRST_PLAYER + veh].FovDegrees *= 0.75f;
			
			targetDistance -= (speedFOVThing * 2.0f); // bring in closer when getting faster
			targetDistance += std::min(3.0f, std::powf(speed * 0.04f, 2.0f) * 0.35f); // move it away when initially gaining speed
			
			distance[veh] = std::lerp(distance[veh], targetDistance, gFrameTime * 0.002f);
			
			tVector3 camPosLocal = tVector3(tilt[veh], kBaseHeight, -distance[veh]);
			tVector3 camTargetLocal = tVector3(0.0f, 1.0f, 2.0f + (kBaseDistance - distance[veh]));
			tVector3 camUpOffset = tVector3(-tilt[veh] * 0.07f, 0.0f, 0.0f);
			
			if (ticked)
				prevCamPos[veh] = vViews[VVIEW_FIRST_PLAYER + veh].Position; // store previous pos
			
			tMulVector(&vViews[VVIEW_FIRST_PLAYER + veh].Position, &transform, &camPosLocal);
			tMulVector(&camTarget[veh], &transform, &camTargetLocal);
			tMulVector(&camUp[veh], &transform, &camUpOffset);
			
			// fix them up
			camTarget[veh].y = transformFlt[13] + 1.0f;
			vViews[VVIEW_FIRST_PLAYER + veh].Position.y = transformFlt[13] + kBaseHeight;
			camUp[veh] -= globalVehPos; // make it still local to the vehicle but rotated
			camUp[veh].y = 1.0f;
			
			tVector3 cam = vViews[VVIEW_FIRST_PLAYER + veh].Position,
				up = {camUp[veh].x, camUp[veh].y, camUp[veh].z},
				look = {camTarget[veh].x, camTarget[veh].y, camTarget[veh].z};
			tCreateLookAtMatrix(&vViews[VVIEW_FIRST_PLAYER + veh].ViewMatrix, cam, look, up);
			
			tMulVector(&vViews[VVIEW_FIRST_PLAYER + veh].Velocity, &vViews[VVIEW_FIRST_PLAYER + veh].ViewMatrix, &prevCamPos[veh]); // get inverse of velocity
			vViews[VVIEW_FIRST_PLAYER + veh].Velocity *= 1.0 / std::fmaxf(kStepTime, gFrameTime * 0.001f); // make it the correct orientation and scaledish with dT
		}
	}
}

//---------------------------------------------------------------------------------

bool World::ShouldPauseWorld()
{
	return DebugMenuShouldPauseWorld();
}

//---------------------------------------------------------------------------------

bool World::ContactProcessedCallback(btManifoldPoint& cp, void* body0, void* body1)
{
	ISimable* simable1 = (ISimable*)(((btCollisionObject*)body0)->getUserPointer());
	ISimable* simable2 = (ISimable*)(((btCollisionObject*)body1)->getUserPointer());
	
	const btVector3& pointOnBody0bt = cp.getPositionWorldOnB();
	const btVector3& pointOnBody1bt = cp.getPositionWorldOnA();
	
	tVector3 pointOnBody0{pointOnBody0bt.getX(), pointOnBody0bt.getY(), pointOnBody0bt.getZ()};
	tVector3 pointOnBody1{pointOnBody1bt.getX(), pointOnBody1bt.getY(), pointOnBody1bt.getZ()};
	
	if (simable1)
		simable1->OnCollide(simable2, pointOnBody0);
	
	if (simable2)
		simable2->OnCollide(simable1, pointOnBody1);
	
	return true;
}

//---------------------------------------------------------------------------------

World::~World()
{
	//cleanup in the reverse order of creation/initialization
	
	///-----cleanup_start-----
	
	// remove vehicles
	for (size_t i = 0; i < mVehicles.size(); i++)
	{
		delete mVehicles[i];
	}
	
	mVehicles.clear();
	
	// remove prop cubes
	for (int i = 0; i < mPropCubeCount; i++)
	{
		delete mPropCubes[i];
		mPropCubes[i] = NULL;
	}
	mPropCubeCount = 0;

	// remove the remaining rigidbodies
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		// skip bodies that are owned by an ISimable they've already removed above
		if (obj->getUserPointer() != NULL)
			continue;
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}
	
	//delete collision shapes
	for (int i = 0; i < collisionShapes.size(); i++)
	{
		btCollisionShape* shape = collisionShapes[i];
		collisionShapes[i] = NULL;
		delete shape;
	}
	
	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
}
