#include <gccore.h>
#include <Vulpes/vulpes.h>
#include <tWare/Endianness.h>

//---------------------------------------------------------------------------------

vMesh::vMesh(tinygltf::Model *model, tinygltf::Primitive &primitive, const char* basePath)
{
	tinygltf::Accessor& posAccessor = model->accessors[primitive.attributes["POSITION"]];
	tinygltf::BufferView& posBufferView = model->bufferViews[posAccessor.bufferView];
	tinygltf::Buffer& posBuffer = model->buffers[posBufferView.buffer];
	
	tinygltf::Accessor& nrmAccessor = model->accessors[primitive.attributes["NORMAL"]];
	tinygltf::BufferView& nrmBufferView = model->bufferViews[nrmAccessor.bufferView];
	tinygltf::Buffer& nrmBuffer = model->buffers[nrmBufferView.buffer];
	
	tinygltf::Accessor& tex0Accessor = model->accessors[primitive.attributes["TEXCOORD_0"]];
	tinygltf::BufferView& tex0BufferView = model->bufferViews[tex0Accessor.bufferView];
	tinygltf::Buffer& tex0Buffer = model->buffers[tex0BufferView.buffer];
	
	tinygltf::Accessor& indexAccessor = model->accessors[primitive.indices];
	tinygltf::BufferView& indexBufferView = model->bufferViews[indexAccessor.bufferView];
	tinygltf::Buffer& indexBuffer = model->buffers[indexBufferView.buffer];
	
	vGlTFVector3* bufVtx = (vGlTFVector3*)&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset];
	
	//printf("Vertices\n");
	
	mVertexCount = posAccessor.count;
	
	CreateBuffer(mVertexCount);
	
	// values are little endian and the space is wrong, fix that
	for (size_t i = 0; i < mVertexCount; i++)
	{
		mVertices[i].position.x = bufVtx[i].x;
		mVertices[i].position.y = bufVtx[i].y;
		mVertices[i].position.z = bufVtx[i].z;
		
		tEndianSwap(mVertices[i].position.x);
		tEndianSwap(mVertices[i].position.y);
		tEndianSwap(mVertices[i].position.z);
		
		mVertices[i].position.x *= -1.0f;
		mVertices[i].position.z *= -1.0f;
	}
	
	if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
	{
		tinygltf::Accessor& colorAccessor = model->accessors[primitive.attributes["COLOR_0"]];
		tinygltf::BufferView& colorBufferView = model->bufferViews[colorAccessor.bufferView];
		tinygltf::Buffer& colorBuffer = model->buffers[colorBufferView.buffer];
		vColorShort* bufColor = (vColorShort*)&colorBuffer.data[colorBufferView.byteOffset + colorAccessor.byteOffset];
		
		//printf("Found vertex colors\n");
		// get colors
		for (size_t i = 0; i < mVertexCount; i++)
		{
			mVertices[i].color.r = (uint8_t)(((float)__builtin_bswap16(bufColor[i].r) / 0xFFFF) * 0xFF);
			mVertices[i].color.g = (uint8_t)(((float)__builtin_bswap16(bufColor[i].g) / 0xFFFF) * 0xFF);
			mVertices[i].color.b = (uint8_t)(((float)__builtin_bswap16(bufColor[i].b) / 0xFFFF) * 0xFF);
			mVertices[i].color.a = (uint8_t)(((float)__builtin_bswap16(bufColor[i].a) / 0xFFFF) * 0xFF);
		}
	}
	else
	{
		//printf("Didn't find vertex colors\n");
		// fill with white if they're missing
		for (size_t i = 0; i < mVertexCount; i++)
		{
			mVertices[i].color.r = 0xFF;
			mVertices[i].color.g = 0xFF;
			mVertices[i].color.b = 0xFF;
			mVertices[i].color.a = 0xFF;
		}
	}
	
	//printf("Texcoords\n");
	
	vVector2* bufTexcoord = (vVector2*)&tex0Buffer.data[tex0BufferView.byteOffset + tex0Accessor.byteOffset];
	
	// get texcoords
	for (size_t i = 0; i < mVertexCount; i++)
	{
		mVertices[i].texcoord.x = bufTexcoord[i].x;
		mVertices[i].texcoord.y = bufTexcoord[i].y;
		
		tEndianSwap(mVertices[i].texcoord.x);
		tEndianSwap(mVertices[i].texcoord.y);
	}
	
	//printf("Normals\n");
	
	vGlTFVector3* bufNrm = (vGlTFVector3*)&nrmBuffer.data[nrmBufferView.byteOffset + nrmAccessor.byteOffset];
	
	// values are little endian and the space is wrong, fix that
	for (size_t i = 0; i < mVertexCount; i++)
	{
		mVertices[i].normal.x = bufNrm[i].x;
		mVertices[i].normal.y = bufNrm[i].y;
		mVertices[i].normal.z = bufNrm[i].z;
		
		tEndianSwap(mVertices[i].normal.x);
		tEndianSwap(mVertices[i].normal.y);
		tEndianSwap(mVertices[i].normal.z);
		
		mVertices[i].normal.x *= -1.0f;
		mVertices[i].normal.z *= -1.0f;
	}
	
	//printf("Indices\n");
	
	const uint16_t* indices = reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
	
	mIndexCount = indexAccessor.count;
	
	mIndices = (uint16_t*)tWareMalloc(mIndexCount * sizeof(uint16_t), "ModelIndices", __LINE__, ALLOC_PARAMS(MODEL_POOL, 0));
	
	for (size_t i = 0; i < mIndexCount; i += 3)
	{
		// little endian so we have to fix it
		mIndices[i+0] = __builtin_bswap16(indices[i+0]);
		mIndices[i+1] = __builtin_bswap16(indices[i+2]);
		mIndices[i+2] = __builtin_bswap16(indices[i+1]);
	}
	
	// get texture, if any
	if (model->images.size() > 0 && model->textures.size() > 0 && model->materials.size() > 0)
	{
		int textureIndex = model->materials[primitive.material].pbrMetallicRoughness.baseColorTexture.index;
		//printf("Texture index: %d\n", textureIndex);
		
		if (textureIndex != -1)
		{
			int sourceIndex = model->textures[textureIndex].source;
			//printf("Source index: %d\n", sourceIndex);
			
			if (sourceIndex != -1)
			{
				char texturePath[TFILE_MAX_PATH] = { '\0' };
				//printf("Texture name: %s\n", model->images[sourceIndex].name.c_str());
				mTextures.DiffuseMap = tStringHash(model->images[sourceIndex].name.c_str());
				
				sprintf(texturePath, "%s/Textures/%s.tpl", basePath, model->images[sourceIndex].name.c_str());
				
				//printf("Texture path: %s\n", texturePath);
				vTextureCache::LoadTextureFromPath(texturePath);
			}
		}
		
		if (model->materials[primitive.material].extras.IsObject()) {
			if (model->materials[primitive.material].extras.Has("effect"))
			{
				const tinygltf::Value::Object &o =
					model->materials[primitive.material].extras.Get<tinygltf::Value::Object>();
				const tinygltf::Value &effect = o.find("effect")->second;

				if (effect.IsString())
				{
					const std::string &str = effect.Get<std::string>();
					
					mEffectID = vEffect::GetEffectIDFromString(str.c_str());
					
					printf("Found desired effect: %s (%d)\n", str.c_str(), mEffectID);
				}
			}
			
			if (model->materials[primitive.material].extras.Has("TextureAlphaUsageType")) {
				const tinygltf::Value::Object &o =
					model->materials[primitive.material].extras.Get<tinygltf::Value::Object>();
				const tinygltf::Value &type = o.find("TextureAlphaUsageType")->second;
				
				if (type.IsString()) 
				{
					const std::string &str = type.Get<std::string>();
					
					mTextureAlphaUsageType = vGetTextureAlphaUsageType(str.c_str());
				}
			}
			
			if (model->materials[primitive.material].extras.Has("MaterialName")) {
				const tinygltf::Value::Object &o =
					model->materials[primitive.material].extras.Get<tinygltf::Value::Object>();
				const tinygltf::Value &name = o.find("MaterialName")->second;
				
				if (name.IsString()) 
				{
					const std::string &str = name.Get<std::string>();
					
					mMaterial = vGetMaterialFromName(str.c_str());
					
					printf("Got material: %s\n", str.c_str());
				}
			}
		}
	}
	
	DCFlushRange(mVertices, mVertexBufferSize);
	
	//printf("Done with mesh\n");
}

//---------------------------------------------------------------------------------

void vModel::BuildFromGLTFModel(tinygltf::Model* model, const char* basePath)
{
	auto& scene = model->scenes[model->defaultScene];
	
	for (auto nodeIndex : scene.nodes) {
		CreateMeshesFromNode(model, nodeIndex, basePath);
	}
}

//---------------------------------------------------------------------------------

vModel::vModel(tinygltf::Model* model, const char* basePath)
{
	BuildFromGLTFModel(model, basePath);
}

//---------------------------------------------------------------------------------

vModel::vModel(const char* path)
{
	char basePath[TFILE_MAX_PATH] = { '\0' };
	int filenameIndex;
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;
	
	tFile* glbFile = tOpenFile(path);
	
	if (!glbFile)
		printf("oops file %s can't be found\n", path);
	
	bool loaded = loader.LoadBinaryFromMemory(&model, &err, &warn, (const unsigned char*)glbFile->data, glbFile->filesize, "");
	
	if (!warn.empty())
	{
		printf("GLTF WARNING: %s\n", warn.c_str());
	}
	if (!err.empty())
	{
		printf("GLTF ERROR: %s\n", err.c_str());
	}
	
	if (!loaded)
		printf("Failed to load glTF: %s\n", glbFile->filename);
	else
		printf("Loaded glTF: %s\n", glbFile->filename);
	
	tCloseFile(glbFile);
	
	if (loaded)
	{
		// find base path
		
		// isolate filename from path
		for (filenameIndex = strlen(path) - 1; filenameIndex >= 0; filenameIndex--)	
		{
			if (path[filenameIndex] == '/' || path[filenameIndex] == '\\')
			{
				break;
			}
		}
		
		if (filenameIndex < 0)
			filenameIndex = 0;
		
		memcpy(basePath, path, filenameIndex); // copy the path up to but not including the filename
		
		printf("Base path of model: %s\n", basePath);
		
		BuildFromGLTFModel(&model, basePath);
	}
}

//---------------------------------------------------------------------------------

void vModel::CreateMeshesFromNode(tinygltf::Model* model, size_t nodeIndex, const char* basePath)
{
	mSolids.reserve(model->nodes.size());
	
	mSolids.emplace_back(model, model->nodes[nodeIndex], basePath);
	
	for (auto childNodeIndex : model->nodes[nodeIndex].children)
		CreateMeshesFromNode(model, childNodeIndex, basePath);
}

//---------------------------------------------------------------------------------

void vModel::Render(vView* view, tMatrix4 *transform)
{
	if (!view || !transform)
		return;
	tMatrix4 mv; // modelview matrix.	
	tMatrix4 WtoL; // world to local matrix.	
	
	// bad naming: this means multiply a by b and put the result into c (ab)
	tMulMatrix(&mv, &view->ViewMatrix, transform);
	
	// load the modelview matrix into matrix memory
	GX_LoadPosMtxImm(*(Mtx44*)&mv, GX_PNMTX0);
	
	tInvertMatrix(&WtoL, transform);
	
	GX_SetCullMode(GX_CULL_BACK);
	
	const float kMaxRenderDist = 1000.0f; // 1km away max
	const float kMaxRenderDistEnvmap = 150.0f; // 150m away max
	float x2x1 = ((*transform)[0][3]) - view->Position.x;
	float y2y1 = ((*transform)[1][3]) - view->Position.y;
	float z2z1 = ((*transform)[2][3]) - view->Position.z;
	
	float distance = sqrtf((x2x1*x2x1) + (y2y1*y2y1) + (z2z1*z2z1));
	
	if (distance > (view->ID == VVIEW_ENVMAP ? kMaxRenderDistEnvmap : kMaxRenderDist)) // don't render past maximum distance
		return;
	
	for (size_t solid = 0; solid < mSolids.size(); solid++)
	{
		for (size_t mesh = 0; mesh < mSolids[solid].mMeshes.size(); mesh++)
		{
			vMesh& m = mSolids[solid].mMeshes[mesh];
			if (!m.mVertices || !m.mIndices || m.mVertexCount == 0 || m.mIndexCount == 0)
				continue;
			// tells gx where our position and color data is
			// args: type of data, pointer, array stride
			GX_SetArray(GX_VA_POS, &m.mVertices[0].position, sizeof(vVertex));
			GX_SetArray(GX_VA_CLR0, &m.mVertices[0].color, sizeof(vVertex));
			GX_SetArray(GX_VA_CLR1, &m.mVertices[0].color, sizeof(vVertex));
			GX_SetArray(GX_VA_TEX0, &m.mVertices[0].texcoord, sizeof(vVertex));
			GX_SetArray(GX_VA_NRM, &m.mVertices[0].normal, sizeof(vVertex));
			//DCFlushRange(mSolids[solid].mMeshes[mesh].mVertices, mSolids[solid].mMeshes[mesh].mVertexBufferSize);
			
			// setup the vertex descriptor
			// tells the flipper to expect 16bit indexes for position
			// and color data. could also be set to direct.
			GX_ClearVtxDesc();
			GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
			GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX16);
			GX_SetVtxDesc(GX_VA_CLR1, GX_INDEX16);
			GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);
			GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16);
			
			// setup the vertex attribute table
			// describes the data
			// args: vat location 0-7, type of data, data format, size, scale
			// so for ex. in the first call we are sending position data with
			// 3 values X,Y,Z of size S16. scale sets the number of fractional
			// bits for non float data.
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR1, GX_CLR_RGBA, GX_RGBA8, 0);
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
			
			vEffectStaticState::pCurrentEffect = vEffects[m.mEffectID];
			if (!vEffectStaticState::pCurrentEffect)
				continue;
			vEffectStaticState::pViewMatrix = &view->ViewMatrix;
			vEffectStaticState::pWorldToLocalMatrix = &WtoL;
			
			vEffectStaticState::pCurrentEffect->SetTexture(vTextureCache::GetTexture(m.mTextures.DiffuseMap));
			vEffectStaticState::pCurrentEffect->Material = m.mMaterial;
			
			if (view->ID == VVIEW_ENVMAP)
			{
				vEffectStaticState::pCurrentEffect->HalfBrightness = true;
			}
			
			vEffectStaticState::pCurrentEffect->Start();
			
			if (vEffectStaticState::pCurrentEffect->ID != VEFFECT_SKY) // sky overrides these, don't touch them
			{
				switch (m.mTextureAlphaUsageType) 
				{
					case TEXUSAGE_MODULATED:
						if (m.mTextures.DiffuseMap == CTStringHash("glass"))
						{
							GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
							GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
						}
						else
						{
							GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
							GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
						}
						break;
					default:
					case TEXUSAGE_NONE:
						GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
						GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
						break;
				}
			}
			
			// have to step through index buffer manually
			GX_Begin(GX_TRIANGLES, GX_VTXFMT0, m.mIndexCount);

			for (size_t i = 0; i < m.mIndexCount; i++) 
			{
				uint16_t index = m.mIndices[i];
				GX_Position1x16(index);
				GX_Color1x16(index);
				GX_Color1x16(index);
				GX_TexCoord1x16(index);
				GX_Normal1x16(index);
			}
			
			GX_End();
			vEffectStaticState::pCurrentEffect->End();
			vEffectStaticState::pCurrentEffect = NULL;
		}
	}
}

vModel* vModel::CreateCube(vColor color)
{
	// 24 unique vertices (4 per face x 6 faces) so normals are per face
	static const float kPositions[24][3] = {
		// +Z front
		{-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1},
		// -Z back
		{ 1,-1,-1}, {-1,-1,-1}, {-1, 1,-1}, { 1, 1,-1},
		// +X right
		{ 1,-1, 1}, { 1,-1,-1}, { 1, 1,-1}, { 1, 1, 1},
		// -X left
		{-1,-1,-1}, {-1,-1, 1}, {-1, 1, 1}, {-1, 1,-1},
		// +Y top
		{-1, 1, 1}, { 1, 1, 1}, { 1, 1,-1}, {-1, 1,-1},
		// -Y bottom
		{-1,-1,-1}, { 1,-1,-1}, { 1,-1, 1}, {-1,-1, 1},
	};

	//6 normals for a cube faces -- for GPU to know which direction to face 
	static const float kNormals[6][3] = {
		{ 0, 0, 1}, { 0, 0,-1}, { 1, 0, 0},
		{-1, 0, 0}, { 0, 1, 0}, { 0,-1, 0},
	};
	// 2 triangles per face x 6 faces = 12 triangles = 36 indices
	static const uint16_t kIndices[36] = {
		 0, 1, 2,  0, 2, 3,   // +Z
		 4, 5, 6,  4, 6, 7,   // -Z
		 8, 9,10,  8,10,11,   // +X
		12,13,14, 12,14,15,   // -X
		16,17,18, 16,18,19,   // +Y
		20,21,22, 20,22,23,   // -Y
	};


	//create the object cube
	vModel* model = new vModel();
	model->mSolids.reserve(1);
	model->mSolids.emplace_back();
	vSolid& solid = model->mSolids.back();
	solid.mMeshes.reserve(1);
	solid.mMeshes.emplace_back();
	vMesh& mesh = solid.mMeshes.back();

	mesh.mVertexCount = 24;  // 6 faces × 4 vertices each
	mesh.mIndexCount = 36;  // 6 faces × 2 triangles × 3 indices
	mesh.mVertexBufferSize = sizeof(vVertex) * 24;
	mesh.mVertices = (vVertex*)tWareMalloc(mesh.mVertexBufferSize, "CubeVB", __LINE__, ALLOC_PARAMS(MODEL_POOL, 32));
	mesh.mIndices = (uint16_t*)tWareMalloc(sizeof(uint16_t) * 36, "CubeIB", __LINE__, ALLOC_PARAMS(MODEL_POOL, 32));

	for (int i = 0; i < 24; i++)
	{
		int face = i / 4;
		mesh.mVertices[i].position.x = kPositions[i][0];
		mesh.mVertices[i].position.y = kPositions[i][1];
		mesh.mVertices[i].position.z = kPositions[i][2];
		mesh.mVertices[i].color = color;
		mesh.mVertices[i].texcoord.x = 0.0f; //no real texture mapping needed all UV's = 0
		mesh.mVertices[i].texcoord.y = 0.0f;
		mesh.mVertices[i].normal.x = kNormals[face][0];
		mesh.mVertices[i].normal.y = kNormals[face][1];
		mesh.mVertices[i].normal.z = kNormals[face][2];
	}
	for (int i = 0; i < 36; i++)
		mesh.mIndices[i] = kIndices[i];

	mesh.mTextures.DiffuseMap = tStringHash("DefaultTexture");
	mesh.mEffectID = VEFFECT_WORLD;

	return model;
}
