#ifndef VULPES_VMODEL_H
#define VULPES_VMODEL_H

#include <tiny_gltf/tiny_gltf.h>
#include <cstdint>

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
	vVector3 normal;
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

#endif
