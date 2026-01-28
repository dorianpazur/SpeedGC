#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <gcmodplay.h>
#include <iso9660.h>
#include <iostream>
#include <limits>
#include <fat.h>
#include <dirent.h>

#include <tWare/Time.h>
#include <tWare/File.h>

#include "includetest.h"
#include "btBulletDynamicsCommon.h"

#include <Vulpes/vulpes.h> // graphics

static uint8_t currentBuffer = 0;
static void *xfb[2] = { NULL, NULL}; // double buffered
static GXRModeObj *rmode = NULL;

f32 yscale;
u32 xfbHeight;
GXColor background = {0x7F, 0x40, 0xFF, 0xff};
Mtx44 v,p; // view and perspective matrices

void Initialise(int argc, char** argv);
 
void draw_init();
//void draw_cube(Mtx view, Mtx pos);
//
tFile *gTestGLBFile = NULL;
vModel *gTestModel = NULL;

TPLFile DefaultTPL;

int main(int argc, char **argv)
{

	Initialise(argc, argv);

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
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
	
		collisionShapes.push_back(groundShape);
	
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -6, -100));
	
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
	
		startTransform.setOrigin(btVector3(0.25f, 10, -0.05f));
	
		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
	
		dynamicsWorld->addRigidBody(body);
	}
	
	{
		//create another dynamic rigidbody
	
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
	
		startTransform.setOrigin(btVector3(-0.25f, 12, 0.3f));
	
		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
	
		dynamicsWorld->addRigidBody(body);
	}
	
	{
		//create another dynamic rigidbody
	
		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btBoxShape(btVector3(1.f, 1.f, 1.f));
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
	
		startTransform.setOrigin(btVector3(0, 8, 0.02));
	
		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		
		rbInfo.m_restitution = 0.8f;
		
		btRigidBody* body = new btRigidBody(rbInfo);
	
		dynamicsWorld->addRigidBody(body);
	}
	
	
	{
		//create another dynamic rigidbody
	
		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btBoxShape(btVector3(1.f, 1.f, 1.f));
		collisionShapes.push_back(colShape);
	
		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();
	
		btScalar mass(3.f);
	
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);
	
		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);
	
		startTransform.setOrigin(btVector3(-5, -2, 5));
	
		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		
		rbInfo.m_restitution = 0.8f;
		
		btRigidBody* body = new btRigidBody(rbInfo);
	
		dynamicsWorld->addRigidBody(body);
	}
	
	/// Do some simulation
	
	unsigned int prevFrameTime = tGetTicker();
	VIDEO_WaitVSync();
	
	float avgfps = 0.0f;
	float fps = 0.0f;
	
	printf("Attempting to open test.txt\n");
	tFile* testFile = tOpenFile("test.txt");
	
	draw_init();
	
	prevFrameTime = tGetTicker();

	while(1) {
		unsigned int now = tGetTicker();
		float frameTime = tGetTickerDifference(prevFrameTime, now);
		prevFrameTime = now;
		
		PAD_ScanPads();
		
		int buttonsDown = PAD_ButtonsDown(0);
		int buttonsPressed = PAD_ButtonsHeld(0);
		
		dynamicsWorld->stepSimulation(frameTime * 0.001f, 2);
		
		// draw
		
		GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
		GX_InvVtxCache();
		GX_InvalidateTexAll();
		
		// physics 
		// print positions of all objects
		
		float transformFlt[16];
		Mtx44 transform;
		
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
			
			if (body)
			{
				if (buttonsPressed & PAD_BUTTON_A)
				{
					body->activate();
					body->applyCentralImpulse( btVector3( 0.f, 100.0f * frameTime * 0.001f, 0.0f ) );
				}
				
				if (buttonsDown & PAD_BUTTON_B)
				{
					body->activate();
					body->applyCentralImpulse( btVector3( 0.f, 0.0f, -100.0f * frameTime * 0.001f ) );
				}
				
				if (buttonsDown & PAD_BUTTON_X)
				{
					body->activate();
					body->applyCentralImpulse( btVector3( 0.f, 100.0f * frameTime * 0.001f, -10000.0f * frameTime * 0.001f ) );
				}
			}
			
			trans.getOpenGLMatrix(transformFlt);
			
			transform[0][0]=transformFlt[0];
			transform[1][0]=transformFlt[1];
			transform[2][0]=transformFlt[2];
			
			transform[0][1]=transformFlt[4];
			transform[1][1]=transformFlt[5];
			transform[2][1]=transformFlt[6];
			
			transform[0][2]=transformFlt[8];
			transform[1][2]=transformFlt[9];
			transform[2][2]=transformFlt[10];
			
			transform[0][3]=transformFlt[12];
			transform[1][3]=transformFlt[13];
			transform[2][3]=transformFlt[14];
			
			gTestModel->Render(v, transform);
		}
		
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(xfb[currentBuffer],GX_TRUE);
		
		GX_DrawDone();
		
		VIDEO_SetNextFramebuffer(xfb[currentBuffer]);
		
		//if (testFile)
		//	printf("contents of test.txt:\n%s\n", testFile->data);
		//else
		//	printf("failed to open test.txt!\n");
		//
		//float fps = 1.0f / (frameTime * 0.001f);
		//
		//int buttonsDown = PAD_ButtonsDown(0);
		//
		////if( buttonsDown & PAD_BUTTON_A ) {
		////	printf("Button A pressed.\n");
		////}
		////
		////if( buttonsDown & PAD_BUTTON_B ) {
		//	printf("fps = %.2f\n", fps);
		////}
		//
		//if (buttonsDown & PAD_BUTTON_START) {
		//	break;
		//}
		
		VIDEO_Flush();
		
		VIDEO_WaitVSync();
		currentBuffer ^= 1;
	}
	
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
	
	delete gTestModel;
	
	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();
	
	tCloseFile(testFile);

	return 0;
}

void dirlist(char* path) {

	DIR* pdir = opendir(path);
	struct dirent* pent;

	if (pdir != NULL) {

		do {
			errno = 0;
			pent = readdir(pdir);

			if(pent != NULL && strcmp(".", pent->d_name) != 0 && strcmp("..", pent->d_name) != 0) {
				char dnbuf[PATH_MAX];
				sprintf(dnbuf, "%s/%s", path, pent->d_name);

				struct stat statbuf;
				stat(dnbuf, &statbuf);

				if(S_ISDIR(statbuf.st_mode)) {
					printf("%s <DIR>\n", dnbuf);
					dirlist(dnbuf);
				} else {
					printf("%s (%lld)\n", dnbuf, statbuf.st_size);
				}
			}
		} while (pent != NULL || errno == EOVERFLOW);

		closedir(pdir);
	} else {
		printf("opendir() failure.\n");
	}
}

void Initialise(int argc, char** argv) {
	SYS_STDIO_Report(true); // enable logging to dolphin logs
	VIDEO_Init();
	PAD_Init();
	
	rmode = VIDEO_GetPreferredMode(NULL);

	rmode->aa = true;

	void *gp_fifo = NULL;
	gp_fifo = memalign(32,GX_FIFO_MINSIZE);
	memset(gp_fifo,0,GX_FIFO_MINSIZE);
	
	xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	//console_init(xfb[currentBuffer],20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	FILE* checkFile = NULL;
	// check if SD gecko is present and game is at the especified location
	if (fatInitDefault() && (checkFile = fopen("./Speed/Speed.dol", "rb")))
	{
		printf("\n\nFound SD Gecko\n");
		tChangeBaseDir("./Speed/");
	}
	else // otherwise try loading from DVD
	{
		printf("\n\nMounting DVD\n");
		
		DVD_Init();
		ISO9660_Mount("dvd", &__io_gcdvd);
		
		tChangeBaseDir("dvd://");
	}
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb[currentBuffer]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
	
	// initialize GX
	GX_Init(gp_fifo, GX_FIFO_MINSIZE);
	
	// clear
	GX_SetCopyClear(background, 0x00ffffff);
	
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));
	
	if (rmode->aa) {
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	} else {
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	}
	
	GX_SetCullMode(GX_CULL_BACK);
	GX_CopyDisp(xfb[currentBuffer],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
 
	// setup our camera at the origin
	// looking down the -z axis with y up
	guVector cam = {0.0F, -2.0F, 18.0F},
			up = {0.0F, 1.0F, 0.0F},
		  look = {0.0F, -1.0F, 1.0F};
	guLookAt(v, &cam, &up, &look);
 
	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 60,
	// an aspect ratio of 4/3 (i'm not sure if that's the right
	// way to do it but i just went by what made a square on my screen)
	// and z near and far distances
    f32 w = rmode->viWidth;
    f32 h = rmode->viHeight;
	f32 aspect = (f32)w/h;
	f32 aspectCorrect = aspect / (4.0f/3.0f); // not quite the right size
	guPerspective(p, 60 / aspectCorrect, aspect * aspectCorrect, 0.1F, 1000.0F);
	GX_LoadProjectionMtx(p, GX_PERSPECTIVE);
}

//---------------------------------------------------------------------------------
void draw_init() {
//---------------------------------------------------------------------------------

	tFile* DefaultTexture = tOpenFile("Global/DefaultTexture.tpl");
	
	size_t alignedFileSize = DefaultTexture->filesize + 32 - (DefaultTexture->filesize % 32);
	void* data = (void*)(((int)malloc(alignedFileSize) - 1u + 32) & -32);
	
	memcpy(data, DefaultTexture->data, DefaultTexture->filesize);
	
	TPL_OpenTPLFromMemory(&DefaultTPL, data, DefaultTexture->filesize);
	tCloseFile(DefaultTexture);
	
	tinygltf::Model gTestGLBModel;
	tinygltf::TinyGLTF gTestGLBLoader;
	
	gTestGLBFile = tOpenFile("teapot_textured.glb");
	if (!gTestGLBFile)
		printf("oops file can't be found\n");
	std::string err;
	std::string warn;
	bool loaded = gTestGLBLoader.LoadBinaryFromMemory(&gTestGLBModel, &err, &warn, (const unsigned char*)gTestGLBFile->data, gTestGLBFile->filesize, "dvd://");
	
	if (!warn.empty())
	{
		printf("GLTF WARNING: %s\n", warn.c_str());
	}
	if (!err.empty())
	{
		printf("GLTF ERROR: %s\n", err.c_str());
	}
	
	if (!loaded)
		std::cout << "Failed to load glTF: " << gTestGLBFile->filename << std::endl;
	else
		std::cout << "Loaded glTF: " << gTestGLBFile->filename << std::endl;
	
	tCloseFile(gTestGLBFile);
	
	gTestModel = new vModel(&gTestGLBModel);
}
