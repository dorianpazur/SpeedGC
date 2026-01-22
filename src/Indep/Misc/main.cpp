#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <gcmodplay.h>
#include <iso9660.h>

#include <tWare/Time.h>
#include <tWare/File.h>

#include "includetest.h"
#include "btBulletDynamicsCommon.h"


static uint8_t currentBuffer = 0;
static void *xfb[2] = { NULL, NULL}; // double buffered
static GXRModeObj *rmode = NULL;

f32 yscale;
u32 xfbHeight;
GXColor background = {0x7F, 0x40, 0xFF, 0xff};
Mtx44 v,p; // view and perspective matrices

void Initialise();
 
void draw_init();
void draw_cube(Mtx v, guVector pos);

int main(int argc, char **argv) {

	Initialise();

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
	
		startTransform.setOrigin(btVector3(0.25f, 10, 0));
	
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
	
		startTransform.setOrigin(btVector3(-0.25f, 12, 0));
	
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
	
		startTransform.setOrigin(btVector3(0, 8, 0));
	
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
		
		dynamicsWorld->stepSimulation(frameTime * 0.001f, 2);
		
		// draw
		
		GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
		GX_InvVtxCache();
		GX_InvalidateTexAll();
		
		// physics 
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
			
			draw_cube(v, (guVector){float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())});
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
	
	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();

	return 0;
}

void Initialise() {
	VIDEO_Init();
	PAD_Init();
	
	DVD_Init();
	ISO9660_Mount("dvd", &__io_gcdvd);
	
	rmode = VIDEO_GetPreferredMode(NULL);

	void *gp_fifo = NULL;
	gp_fifo = memalign(32,GX_FIFO_MINSIZE);
	memset(gp_fifo,0,GX_FIFO_MINSIZE);
	
	xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb[currentBuffer],20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
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
	
	// cull none because other values produce weird results
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(xfb[currentBuffer],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
 
	// setup our camera at the origin
	// looking down the -z axis with y up
	guVector cam = {0.0F, 4.0F, 15.0F},
			up = {0.0F, 1.0F, 0.0F},
		  look = {0.0F, 0.0F, 1.0F};
	guLookAt(v, &cam, &up, &look);
 
	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 60,
	// an aspect ratio of 4/3 (i'm not sure if that's the right
	// way to do it but i just went by what made a square on my screen)
	// and z near and far distances
    f32 w = rmode->viWidth;
    f32 h = rmode->viHeight;
	guPerspective(p, 60, (f32)w/h, 0.1F, 1000.0F);
	GX_LoadProjectionMtx(p, GX_PERSPECTIVE);
}

// cube sample

//---------------------------------------------------------------------------------
// cube vertex data
//---------------------------------------------------------------------------------
s16 cube[] ATTRIBUTE_ALIGN(32) = {
	// x y z
	-1,  1, -1, 	// 0
	 1,  1, -1, 	// 1
	 1,  1,  1, 	// 2
	-1,  1,  1, 	// 3
	 1, -1, -1, 	// 4
	 1, -1,  1, 	// 5
	-1, -1,  1,  // 6
	-1, -1, -1,  // 7
};
 

//---------------------------------------------------------------------------------
// color data
//---------------------------------------------------------------------------------
u8 colors[] ATTRIBUTE_ALIGN(32) = {
	// r, g, b, a
	100,  10, 100, 255, // 0 purple
	240,   0,   0, 255,	// 1 red
	255, 180,   0, 255,	// 2 orange
	255, 255,   0, 255, // 3 yellow
	 10, 120,  40, 255, // 4 green
	  0,  20, 100, 255  // 5 blue
};

//---------------------------------------------------------------------------------
void draw_init() {
//---------------------------------------------------------------------------------
	// setup the vertex descriptor
	// tells the flipper to expect 8bit indexes for position
	// and color data. could also be set to direct.
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
 
	// setup the vertex attribute table
	// describes the data
	// args: vat location 0-7, type of data, data format, size, scale
	// so for ex. in the first call we are sending position data with
	// 3 values X,Y,Z of size S16. scale sets the number of fractional
	// bits for non float data.
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
 
	// tells gx where our position and color data is
	// args: type of data, pointer, array stride
	GX_SetArray(GX_VA_POS, cube, 3*sizeof(s16));
	GX_SetArray(GX_VA_CLR0, colors, 4*sizeof(u8));
	DCFlushRange(cube,sizeof(cube));
	DCFlushRange(colors,sizeof(colors));
 
	// no idea...sets to no textures
	// i don't know anything about textures or lighting yet :|
	GX_SetNumChans(1);
	GX_SetNumTexGens(0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
}
 

//---------------------------------------------------------------------------------
// draws a quad from 4 vertex idx and one color idx
//---------------------------------------------------------------------------------
void draw_quad(u8 v0, u8 v1, u8 v2, u8 v3, u8 c) {
//---------------------------------------------------------------------------------
	// one 8bit position idx
	GX_Position1x8(v0);
	// one 8bit color idx
	GX_Color1x8(c);
	GX_Position1x8(v1);
	GX_Color1x8(c);
	GX_Position1x8(v2);
	GX_Color1x8(c);
	GX_Position1x8(v3);
	GX_Color1x8(c);
}

//---------------------------------------------------------------------------------
void draw_cube(Mtx v, guVector pos) {
//---------------------------------------------------------------------------------
	Mtx44 m; // model matrix.
	Mtx44 mv; // modelview matrix.
	guVector axis = {-1,1,0};
	static float rotateby = 0;
 
	//rotateby++;
 
	// move the cube out in front of us and rotate it
	guMtxIdentity(m);
	//guMtxRotAxisDeg(m, &axis, rotateby);
	guMtxTransApply(m, m, pos.x, pos.y, pos.z);
	guMtxConcat(v,m,mv);
	// load the modelview matrix into matrix memory
	GX_LoadPosMtxImm(mv, GX_PNMTX0);
 
	// drawing begins!
	// tells the flipper what type of primitive we will be drawing
	// which descriptor in the VAT to use and the number of vertices
	// to expect. 24 since we will draw 6 quads with 4 verts each.
	GX_Begin(GX_QUADS, GX_VTXFMT0, 24);
 
		draw_quad(0, 3, 2, 1, 0);
		draw_quad(0, 7, 6, 3, 1);
		draw_quad(0, 1, 4, 7, 2);
		draw_quad(1, 2, 5, 4, 3);
		draw_quad(2, 3, 6, 5, 4);
		draw_quad(4, 7, 6, 5, 5);
 
	GX_End();
}
