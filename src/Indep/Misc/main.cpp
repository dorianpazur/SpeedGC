#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>

#include <tWare/Time.h>

#include "includetest.h"
#include "btBulletDynamicsCommon.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void *Initialise();

void* alignedMallocLog(size_t size, int alignment)
{	
    size += alignment - (size % alignment);
	
	printf("Allocating %u aligned bytes\n", size);
	for (int i = 0; i < 30; i++)
	{
		VIDEO_WaitVSync();
	}
	void* memory = malloc(size);
	if (!memory)
	{
		printf("Allocating %u aligned bytes failed!\n", size);
		for (int i = 0; i < 60; i++)
		{
			VIDEO_WaitVSync();
		}
	}
	else
	{
		printf("Succeeded in allocating %u aligned bytes!\n", size);
	}
	return memory;
}	

void alignedFreeLog(void* block)
{
	printf("Freeing aligned memory.\n");
	free(block);
}

void* mallocLog(size_t size)
{
	printf("Allocating %u bytes\n", size);
	for (int i = 0; i < 30; i++)
	{
		VIDEO_WaitVSync();
	}
	void* memory = malloc(size);
	if (!memory)
	{
		printf("Allocating %u bytes failed!\n", size);
		for (int i = 0; i < 240; i++)
		{
			VIDEO_WaitVSync();
		}
	}
	else
	{
		printf("Succeeded in allocating %u bytes!\n", size);
	}
	return memory;
}	

void freeLog(void* block)
{
	printf("Freeing memory.\n");
	free(block);
}

int main(int argc, char **argv) {

	xfb = Initialise();

	printf("\nHello World!\n");
	printf("%s\n", szTestString);
	
	//btAlignedAllocSetCustom(mallocLog, freeLog);
	//btAlignedAllocSetCustomAligned(alignedMallocLog, alignedFreeLog);
	
	///-----includes_end-----

	int i;
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	
	btDefaultCollisionConstructionInfo collisionConstructionInfo;
	collisionConstructionInfo.m_defaultMaxCollisionAlgorithmPoolSize = 1023;
	collisionConstructionInfo.m_defaultMaxPersistentManifoldPoolSize = 1023;
	//collisionConstructionInfo.m_defaultStackAllocatorSize = 0;
	
	
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration(collisionConstructionInfo);
	
	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	
	//void* alloctest = mallocLog(1024 * 1024 * 12);
	//freeLog(alloctest);
	
	//void* allocaligntest = alignedMallocLog(1024 * 1024 * 12, 16);
	//alignedFreeLog(allocaligntest);
	
	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	
	printf("btBroadphaseInterface size: %u\n", sizeof(btBroadphaseInterface));
	
	
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	
	btContactSolverInfo& info = dynamicsWorld->getSolverInfo();
	info.m_numIterations = 4;
	
	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	///-----initialization_end-----

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	
	///create a few basic rigid bodies
	
	//the ground is a cube of side 100 at position y = -56.
	//the sphere will hit it at y = -6, with center at -5
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
	
		collisionShapes.push_back(groundShape);
	
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -56, 0));
	
		btScalar mass(0.);
	
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);
	
		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);
	
		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
	
		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}
	
	{
		//create a dynamic rigidbody
	
		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(colShape);
	
		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();
	
		btScalar mass(1.f);
	
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);
	
		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);
	
		startTransform.setOrigin(btVector3(2, 10, 0));
	
		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
	
		dynamicsWorld->addRigidBody(body);
	}
	
	/// Do some simulation
	
	unsigned int prevFrameTime = tGetTicker();
	VIDEO_WaitVSync();
	
	float avgfps = 0.0f;
	float fps = 0.0f;
	
	///-----stepsimulation_start-----
	for (i = 0; i < 150; i++)
	{
		unsigned int now = tGetTicker();
		float frameTime = tGetTickerDifference(prevFrameTime, now);
		prevFrameTime = now;
		
		fps = 1.0f / (frameTime * 0.1f);
		if (i > 75)
			avgfps += fps;
		
		dynamicsWorld->stepSimulation(1.f / 60.f, 2);
		
		//print positions of all objects
		for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform trans;
			if (body && body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
			{
				trans = obj->getWorldTransform();
			}
			//printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
		}
	}
	
	avgfps /= 75.0f;
	
	printf("average fps during latter half of physics sim = %.2f\n", avgfps);
	printf("last fps value = %.2f\n", fps);
	
	///-----stepsimulation_end-----
	
	//cleanup in the reverse order of creation/initialization
	
	///-----cleanup_start-----
	
	//remove the rigidbodies from the dynamics world and delete them
	for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
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
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	// delete dynamics world
	delete dynamicsWorld;
	
	//delete solver
	delete solver;
	
	//delete broadphase
	delete overlappingPairCache;
	
	//delete dispatcher
	delete dispatcher;
	
	delete collisionConfiguration;
	
	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();

	prevFrameTime = tGetTicker();

	while(1) {

		VIDEO_WaitVSync();
		PAD_ScanPads();
		
		unsigned int now = tGetTicker();
		float frameTime = tGetTickerDifference(prevFrameTime, now);
		prevFrameTime = now;
		
		float fps = 1.0f / (frameTime * 0.001f);
		
		int buttonsDown = PAD_ButtonsDown(0);
		
		if( buttonsDown & PAD_BUTTON_A ) {
			printf("Button A pressed.\n");
		}
		
		if( buttonsDown & PAD_BUTTON_B ) {
			printf("fps = %.2f\n", fps);
		}

		if (buttonsDown & PAD_BUTTON_START) {
			exit(0);
		}
	}

	return 0;
}

void * Initialise() {

	void *framebuffer;
	
	VIDEO_Init();
	PAD_Init();
	
	rmode = VIDEO_GetPreferredMode(NULL);

	framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(framebuffer,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(framebuffer);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	return framebuffer;

}
