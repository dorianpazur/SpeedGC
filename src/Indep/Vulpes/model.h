#ifndef VULPES_VMODEL_H
#define VULPES_VMODEL_H

#include <tWare/Hash.h>
#include <tWare/Align.h>
#include <tWare/Memory.h>
#include <tWare/Math.h>
#include <tiny_gltf/tiny_gltf.h>
#include <cstdint>
#include <Vulpes/TextureCache.h>
#include <Vulpes/Vectors.h>
#include <Vulpes/Effect.h>
#include <Vulpes/View.h>
#include <malloc.h>

struct ALIGN(32) vVertex {
	vVector3 position;
	vColor color;
	vVector2 texcoord;
	vVector3 normal;
};

struct ALIGN(32) vMesh
{
	struct TextureHashes
	{
		tHash DiffuseMap = 0;
		
		~TextureHashes()
		{
			vTextureCache::ReleaseTexture(DiffuseMap);
		}
	};
	
	uint32_t mVertexCount = 0;
	uint32_t mIndexCount = 0;
	size_t mVertexBufferSize = 0;
	vVertex* mVertices = NULL;
	uint16_t* mIndices = NULL;
	
	TextureHashes mTextures;
	VEFFECT_ID mEffectID = VEFFECT_STANDARD;
	
	DEF_TWARE_NEW_OVERRIDE(vMesh, MODEL_POOL)
	
	vMesh() {};
	vMesh(tinygltf::Model *model, tinygltf::Primitive &primitive, const char* basePath);
	
	~vMesh()
	{
		if (mVertices)
		{
			tFree(mVertices);
		}
		if (mIndices)
		{
			tFree(mIndices);
		}
	}
private:
	void CreateBuffer(size_t vertexCount)
	{
		mVertexBufferSize = sizeof(vVertex)*vertexCount;
		mVertices = (vVertex*)tWareMalloc(mVertexBufferSize, "VertexBuffer", __LINE__, ALLOC_PARAMS(MODEL_POOL, 32));
	}
};

struct ALIGN(32) vSolid
{
	std::vector<vMesh> mMeshes;
		
	DEF_TWARE_NEW_OVERRIDE(vSolid, MODEL_POOL)
	vSolid() {}  //to allow creating an empty vSolid when building a cube
	vSolid(tinygltf::Model *model, tinygltf::Node &node, const char* basePath = "")
	{	
		if (node.mesh >= 0)
		{
			tinygltf::Mesh &mesh = model->meshes[node.mesh];
			mMeshes.reserve(mesh.primitives.size());
			for (size_t primIdx = 0; primIdx < mesh.primitives.size(); primIdx++)
			{	
				mMeshes.emplace_back(model, mesh.primitives[primIdx], basePath);
			}
		}
	}
};

struct ALIGN(32) vModel
{
	std::vector<vSolid> mSolids;
	
	DEF_TWARE_NEW_OVERRIDE(vModel, MODEL_POOL)
	
	void BuildFromGLTFModel(tinygltf::Model* model, const char* basePath = "");
	vModel() {}
	vModel(tinygltf::Model *model, const char* basePath = "");
	vModel(const char *path);
	void Render(vView* view, tMatrix4 *transform);
	void CreateMeshesFromNode(tinygltf::Model* model, size_t nodeIndex, const char* basePath = "");

	// Build a unit cube (half-extent = 1) with a flat vertex color
	static vModel* CreateCube(vColor color);
};

#endif
