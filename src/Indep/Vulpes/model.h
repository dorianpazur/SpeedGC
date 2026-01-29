#ifndef VULPES_VMODEL_H
#define VULPES_VMODEL_H

#include <tWare/Hash.h>
#include <tiny_gltf/tiny_gltf.h>
#include <cstdint>

struct vGlTFVector3 {
	float x;
	float y;
	float z;
};

struct vVector2 {
	float x;
	float y;
};

struct vVector3 {
	float x;
	float y;
	float z;
};

struct vVector4 {
	float x;
	float y;
	float z;
	float w;
};

struct vColor {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct vColorShort {
	uint16_t r;
	uint16_t g;
	uint16_t b;
	uint16_t a;
};

struct vVertex {
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
	};
	
	uint32_t mVertexCount = 0;
	uint32_t mIndexCount = 0;
	size_t mVertexBufferSize = 0;
	vVertex* mVertices = NULL;
	uint16_t* mIndices = NULL;
	
	TextureHashes mTextures;
	
	vMesh() {};
	vMesh(tinygltf::Model *model, tinygltf::Primitive &primitive);
	
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
		printf("VB size: %u\n", mVertexBufferSize);
		size_t alignedVtxSize = (((uintptr_t)mVertexBufferSize - 1u + 32) & -32);
		printf("Aligned VB size: %u\n", mVertexBufferSize);
		mVerticesUnaligned = malloc(alignedVtxSize); // aligned alloc is broken xd
		printf("mVerticesUnaligned: %u\n", mVerticesUnaligned);
		mVertices = (vVertex*)(((uintptr_t)mVerticesUnaligned - 1u + 32) & -32);
		printf("mVertices: %u\n", mVertices);
	}
	void* mVerticesUnaligned = NULL;
};

struct ALIGN(32) vSolid
{
	std::vector<vMesh> mMeshes;
	
	vSolid(tinygltf::Model *model, tinygltf::Node &node)
	{	
		if (node.mesh >= 0)
		{
			tinygltf::Mesh &mesh = model->meshes[node.mesh];
			mMeshes.reserve(mesh.primitives.size());
			for (size_t primIdx = 0; primIdx < mesh.primitives.size(); primIdx++)
			{	
				mMeshes.emplace_back(model, mesh.primitives[primIdx]);
			}
		}
	}
};

struct ALIGN(32) vModel
{
	std::vector<vSolid> mSolids;
	vModel(tinygltf::Model *model);
	void Render(Mtx view, Mtx transform);
	void CreateMeshesFromNode(tinygltf::Model* model, size_t nodeIndex);
};

#endif
