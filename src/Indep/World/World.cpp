
#include "World.h"
#include "DebugAssistant.h"
#include "ISimable.h"
#include <BulletCollision/NarrowPhaseCollision/btPersistentManifold.h>
#include <Vulpes/Platform.h>
#include "InputManager.h"
#include "InputCommand.h"

World* World::gWorld = NULL;

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
	mVehicles.emplace_back(new Vehicle(dynamicsWorld, btVector3(0, 10, 0)));
	
	for (int i = 0; i < 100; i++)
	{
		{
			btCollisionShape* groundShape = new btBoxShape(btVector3(50, 1, 50)); //the ground
			
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
			groundTransform.setOrigin(btVector3(51, -6, -100 * i));
		
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
			groundTransform.setOrigin(btVector3(-51, -6, -100 * i));
		
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
		
		timeElapsedFix += timestep;
		
		while (timeElapsedFix >= kStepTime)
		{
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
			
			dynamicsWorld->stepSimulation(kStepTime, 2);
			
			timeElapsedFix -= kStepTime;
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
	
	if (simable1)
		simable1->OnCollide(simable2);
	
	if (simable2)
		simable2->OnCollide(simable1);
	
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
	
	//remove the rigidbodies from the dynamics world and delete them
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
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
