#ifndef VULPES_VMODEL_H
#define VULPES_VMODEL_H

#include <tWare/Hash.h>
#include <tWare/Align.h>
#include <tiny_gltf/tiny_gltf.h>
#include <cstdint>
#include <Vulpes/TextureCache.h>
#include <Vulpes/Vectors.h>

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
	
	vMesh() {};
	vMesh(tinygltf::Model *model, tinygltf::Primitive &primitive, const char* basePath);
	
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
		size_t alignedVtxSize = (((uintptr_t)mVertexBufferSize - 1u + 32) & -32);
		mVerticesUnaligned = malloc(alignedVtxSize); // aligned alloc is broken xd
		mVertices = (vVertex*)(((uintptr_t)mVerticesUnaligned - 1u + 32) & -32);
	}
	void* mVerticesUnaligned = NULL;
};

struct ALIGN(32) vSolid
{
	std::vector<vMesh> mMeshes;
	
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
	void BuildFromGLTFModel(tinygltf::Model* model, const char* basePath = "");
	vModel(tinygltf::Model *model, const char* basePath = "");
	vModel(const char *path);
	void Render(Mtx view, Mtx transform);
	void CreateMeshesFromNode(tinygltf::Model* model, size_t nodeIndex, const char* basePath = "");
};

#endif
