#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <gcmodplay.h>
#include <iso9660.h>
#include <tiny_gltf/tiny_gltf.h>
#include <iostream>
#include <limits>
#include <fat.h>
#include <dirent.h>

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

void Initialise(int argc, char** argv);
 
void draw_init();
//void draw_cube(Mtx view, Mtx pos);
//
tFile *gTestGLBFile = NULL;
tinygltf::Model gTestGLBModel;
tinygltf::TinyGLTF gTestGLBLoader;

struct vGlTFVector3 {
	float x;
	float y;
	float z;
};

struct vVector3 {
	float x;
	float y;
	float z;
private:
	float pad;
};

struct vVector4 {
	float x;
	float y;
	float z;
	float w;
};

struct vColor {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
};

struct vColorShort {
	u16 r;
	u16 g;
	u16 b;
	u16 a;
};

struct vVertex {
	vVector3 position;
	vColor color;
};

struct vMesh
{
	uint32_t mVertexCount = 0;
	uint32_t mIndexCount = 0;
	size_t mVertexBufferSize = 0;
	vVertex* mVertices = NULL;
	uint16_t* mIndices = NULL;
	
	vMesh() {};
	vMesh(tinygltf::Model *model, size_t nodeIndex);
	
	~vMesh()
	{
		if (mVertices)
		{
			free(mVerticesUnaligned);
		}
		if (mIndices)
		{
			free(mIndices);
		}
	}
private:
	void CreateBuffer(size_t vertexCount)
	{
		mVertexBufferSize = sizeof(vVertex)*vertexCount;
		size_t alignedVtxSize = mVertexBufferSize + 32 - (mVertexBufferSize % 32);
		mVerticesUnaligned = malloc(alignedVtxSize); // aligned alloc is broken xd
		mVertices = (vVertex*)(((int)mVerticesUnaligned - 1u + 32) & -32);
	}
	void* mVerticesUnaligned = NULL;
};

struct vModel
{
	std::vector<vMesh> mMeshes;
	vModel(tinygltf::Model *model);
	void Render(Mtx view, Mtx transform);
	void CreateMeshesFromNode(tinygltf::Model* model, size_t nodeIndex);
};

int main(int argc, char **argv) {

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
		
	vModel *testModel = new vModel(&gTestGLBModel);
	
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
			
			testModel->Render(v, transform);
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
	f32 aspect = (f32)w/h;
	f32 aspectCorrect = aspect / (4.0f/3.0f); // not quite the right size
	guPerspective(p, 60 / aspectCorrect, aspect * aspectCorrect, 0.1F, 1000.0F);
	GX_LoadProjectionMtx(p, GX_PERSPECTIVE);
}

//---------------------------------------------------------------------------------
void draw_init() {
//---------------------------------------------------------------------------------
	gTestGLBFile = tOpenFile("teapot.glb");
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
}
 
f32 bswap_float(f32 f)
{
	uint32_t bytes = *(uint32_t*)&f;
	bytes = __builtin_bswap32(bytes);
	return *(f32*)&bytes;
}

vMesh::vMesh(tinygltf::Model *model, size_t nodeIndex)
{
	const auto& node = model->nodes[nodeIndex];

	if (node.mesh >= 0)
	{
		tinygltf::Mesh &mesh = model->meshes[node.mesh];
		for (size_t primIdx = 0; primIdx < mesh.primitives.size(); primIdx++)
		{
			auto &primitive = mesh.primitives[primIdx];
			tinygltf::Accessor& posAccessor = model->accessors[primitive.attributes["POSITION"]];
			tinygltf::BufferView& posBufferView = model->bufferViews[posAccessor.bufferView];
			tinygltf::Buffer& posBuffer = model->buffers[posBufferView.buffer];
			tinygltf::Accessor& colorAccessor = model->accessors[primitive.attributes["COLOR_0"]];
			tinygltf::BufferView& colorBufferView = model->bufferViews[colorAccessor.bufferView];
			tinygltf::Buffer& colorBuffer = model->buffers[colorBufferView.buffer];
			tinygltf::Accessor& indexAccessor = model->accessors[primitive.indices];
			tinygltf::BufferView& indexBufferView = model->bufferViews[indexAccessor.bufferView];
			tinygltf::Buffer& indexBuffer = model->buffers[indexBufferView.buffer];
			
			vGlTFVector3* bufVtx = (vGlTFVector3*)&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset];
			
			mVertexCount = posAccessor.count;
			
			CreateBuffer(mVertexCount);
			
			// values are little endian and the space is wrong, fix that
			for (size_t i = 0; i < mVertexCount; i++)
			{
				mVertices[i].position.x = bswap_float(bufVtx[i].z);
				mVertices[i].position.y = bswap_float(bufVtx[i].y);
				mVertices[i].position.z = bswap_float(bufVtx[i].x);
			}
			
			vColorShort* bufColor = (vColorShort*)&colorBuffer.data[colorBufferView.byteOffset + colorAccessor.byteOffset];
			
			// get colors
			for (size_t i = 0; i < mVertexCount; i++)
			{
				mVertices[i].color.r = (int)(((float)__builtin_bswap16(bufColor[i].r) / 0xFFFF) * 0xFF);
				mVertices[i].color.g = (int)(((float)__builtin_bswap16(bufColor[i].g) / 0xFFFF) * 0xFF);
				mVertices[i].color.b = (int)(((float)__builtin_bswap16(bufColor[i].b) / 0xFFFF) * 0xFF);
				mVertices[i].color.a = (int)(((float)__builtin_bswap16(bufColor[i].a) / 0xFFFF) * 0xFF);
			}
			
			const uint16_t* indices = reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
			
			mIndexCount = indexAccessor.count;
			
			mIndices = (uint16_t*)malloc(mIndexCount * sizeof(uint16_t));
			
			for (size_t i = 0; i < mIndexCount; i++)
			{
				// little endian so we have to fix it
				mIndices[i] = __builtin_bswap16(indices[i]);
			}
		}
	}
}

vModel::vModel(tinygltf::Model* model)
{
	auto& scene = model->scenes[model->defaultScene];
	
	for (auto nodeIndex : scene.nodes) {
		CreateMeshesFromNode(model, nodeIndex);
	}
}

void vModel::CreateMeshesFromNode(tinygltf::Model* model, size_t nodeIndex)
{
	mMeshes.emplace_back(model, nodeIndex);
	
	for (auto childNodeIndex : model->nodes[nodeIndex].children)
		CreateMeshesFromNode(model, childNodeIndex);
}

//---------------------------------------------------------------------------------
void vModel::Render(Mtx view, Mtx transform) {
//---------------------------------------------------------------------------------
	Mtx44 mv; // modelview matrix.	
	
	// bad naming: this means multiply a by b and put the result into c (ab)
	guMtxConcat(view,transform,mv);
	
	// load the modelview matrix into matrix memory
	GX_LoadPosMtxImm(mv, GX_PNMTX0);
	
	for (size_t mesh = 0; mesh < mMeshes.size(); mesh++)
	{
		// tells gx where our position and color data is
		// args: type of data, pointer, array stride
		GX_SetArray(GX_VA_POS, &mMeshes[mesh].mVertices[0].position, sizeof(vVertex));
		GX_SetArray(GX_VA_CLR0, &mMeshes[mesh].mVertices[0].color, sizeof(vVertex));
		DCFlushRange(mMeshes[mesh].mVertices, mMeshes[mesh].mVertexBufferSize);
		
		// setup the vertex descriptor
		// tells the flipper to expect 16bit indexes for position
		// and color data. could also be set to direct.
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
		GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX16);
		
		// setup the vertex attribute table
		// describes the data
		// args: vat location 0-7, type of data, data format, size, scale
		// so for ex. in the first call we are sending position data with
		// 3 values X,Y,Z of size S16. scale sets the number of fractional
		// bits for non float data.
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		
		// no idea...sets to no textures
		// i don't know anything about textures or lighting yet :|
		GX_SetNumChans(1);
		GX_SetNumTexGens(0);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		
		// have to step through index buffer manually
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, mMeshes[mesh].mIndexCount);
		
		for (size_t i = 0; i < mMeshes[mesh].mIndexCount; i++)
		{
			uint16_t index = mMeshes[mesh].mIndices[i];
			GX_Position1x16(index);
			GX_Color1x16(index);
		}
		
		GX_End();
	}
}
