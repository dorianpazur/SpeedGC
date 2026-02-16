#include <gccore.h>
#include <Vulpes/vulpes.h>

//---------------------------------------------------------------------------------

f32 bswap_float(f32 f)
{
	uint32_t bytes = *(uint32_t*)&f;
	bytes = __builtin_bswap32(bytes);
	return *(f32*)&bytes;
}

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
		mVertices[i].position.x = -bswap_float(bufVtx[i].x);
		mVertices[i].position.y = bswap_float(bufVtx[i].y);
		mVertices[i].position.z = -bswap_float(bufVtx[i].z);
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
		mVertices[i].texcoord.x = bswap_float(bufTexcoord[i].x);
		mVertices[i].texcoord.y = bswap_float(bufTexcoord[i].y);
	}
	
	//printf("Normals\n");
	
	vGlTFVector3* bufNrm = (vGlTFVector3*)&nrmBuffer.data[nrmBufferView.byteOffset + nrmAccessor.byteOffset];
	
	// values are little endian and the space is wrong, fix that
	for (size_t i = 0; i < mVertexCount; i++)
	{
		mVertices[i].normal.x = -bswap_float(bufNrm[i].x);
		mVertices[i].normal.y = bswap_float(bufNrm[i].y);
		mVertices[i].normal.z = -bswap_float(bufNrm[i].z);
	}
	
	//printf("Indices\n");
	
	const uint16_t* indices = reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
	
	mIndexCount = indexAccessor.count;
	
	mIndices = (uint16_t*)tWareMalloc(mIndexCount * sizeof(uint16_t), "ModelIndices", __LINE__, ALLOC_PARAMS(MAIN_POOL, 0));
	
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
	}
	
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

void vModel::Render(tMatrix4 *view, tMatrix4 *transform)
{
	const float LARGE_NUMBER = 9999999999.0f;
	tMatrix4 mv; // modelview matrix.	
	tMatrix4 WtoL; // world to local matrix.	
	tMatrix4 VtoWtmp; // view to world matrix.	
	tMatrix4 VtoW; // view to world matrix.	
	
	// bad naming: this means multiply a by b and put the result into c (ab)
	tMulMatrix(&mv, view, transform);
	
	// load the modelview matrix into matrix memory
	GX_LoadPosMtxImm(*(Mtx44*)&mv, GX_PNMTX0);
	
	tInvertMatrix(&WtoL, transform);
	tInvertMatrix(&VtoWtmp, view);
    tTransposeMatrix(&VtoW, &VtoWtmp);
	
	GX_SetCullMode(GX_CULL_BACK);
	
	for (size_t solid = 0; solid < mSolids.size(); solid++)
	{	
		// light test
		
		tVector3 lpos;
		tVector3 rimPos;
		tVector3 rimPos2;
		
		GXLightObj lobj;
		GXLightObj lspecobj;
		GXLightObj rimLight;
		GXLightObj rimLight2;
		GXLightObj rimLightSpec;
		GXLightObj rimLight2Spec;
		
		const static GXColor lightColor[] = {
			{0xF0,0xEA,0xB0,0xFF}, // Light color
			{0x16,0x28,0x40,0xFF}, // Ambient color
			{0xB0,0x9A,0x60,0xFF}, // Spec color
			{0xE0,0xD0,0xC0,0xFF}, // rim 1
			{0x3F,0x4F,0x5F,0xFF}, // rim 2
			{0x00,0x00,0x00,0xFF}, // No color
		};
		
		lpos.x = 0.707f * LARGE_NUMBER;
		lpos.y = 0.707f * LARGE_NUMBER;
		lpos.z = 0.707f * LARGE_NUMBER;
		
		rimPos.x =  0.5 * LARGE_NUMBER;
		rimPos.y =  0.707f * LARGE_NUMBER;
		rimPos.z = -0.707f * LARGE_NUMBER;
		
		rimPos2.x = -0.5 * LARGE_NUMBER;
		rimPos2.y = -0.35f * LARGE_NUMBER;
		rimPos2.z = -0.35f * LARGE_NUMBER;
		
		tMulVector(&lpos,&WtoL,&lpos);
		tMulVector(&rimPos,&VtoW,&rimPos);
		tMulVector(&rimPos2,&VtoW,&rimPos2);
		tMulVector(&rimPos,&WtoL,&rimPos);
		tMulVector(&rimPos2,&WtoL,&rimPos2);
		
		GX_InitLightPos(&lobj,lpos.x,lpos.y,lpos.z);
		GX_InitLightColor(&lobj,lightColor[0]);
		
		GX_InitSpecularDir(&lspecobj,-lpos.x,-lpos.y,-lpos.z);
		GX_InitLightColor(&lspecobj,lightColor[2]);
		GX_InitLightShininess(&lspecobj, 44.0f);
		
		GX_InitLightPos(&rimLight,rimPos.x,rimPos.y,rimPos.z);
		GX_InitLightColor(&rimLight,lightColor[3]);
		GX_InitLightAttnA(&rimLight, 2.0, 2.0, 2.0);
		
		GX_InitSpecularDir(&rimLightSpec,-rimPos.x,-rimPos.y,-rimPos.z);
		GX_InitLightColor(&rimLightSpec,lightColor[3]);
		GX_InitLightAttnA(&rimLightSpec, 2.0, 2.0, 2.0);
		GX_InitLightShininess(&rimLightSpec, 22.0f);
		
		GX_InitLightPos(&rimLight2,rimPos2.x,rimPos2.y,rimPos2.z);
		GX_InitLightColor(&rimLight2,lightColor[4]);
		GX_InitLightAttnA(&rimLight2, 2.0, 2.0, 2.0);
		
		GX_InitSpecularDir(&rimLight2Spec,-rimPos2.x,-rimPos2.y,-rimPos2.z);
		GX_InitLightColor(&rimLight2Spec,lightColor[4]);
		GX_InitLightAttnA(&rimLight2Spec, 2.0, 2.0, 2.0);
		GX_InitLightShininess(&rimLight2Spec, 22.0f);
		
		GX_LoadLightObj(&lobj,GX_LIGHT0);
		GX_LoadLightObj(&rimLight,GX_LIGHT1);
		GX_LoadLightObj(&rimLight2,GX_LIGHT2);
		GX_LoadLightObj(&lspecobj,GX_LIGHT3);
		GX_LoadLightObj(&rimLightSpec,GX_LIGHT4);
		GX_LoadLightObj(&rimLight2Spec,GX_LIGHT5);
		
		// set number of rasterized color channels
		GX_SetNumChans(2);
		GX_SetChanCtrl(GX_COLOR0,	GX_ENABLE,	GX_SRC_REG,	GX_SRC_VTX,	GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2,	GX_DF_CLAMP,	GX_AF_NONE);
		GX_SetChanCtrl(GX_COLOR1,	GX_ENABLE,	GX_SRC_REG,	GX_SRC_VTX,	GX_LIGHT3 | GX_LIGHT4 | GX_LIGHT5,	GX_DF_CLAMP,	GX_AF_SPEC);
		GX_SetChanCtrl(GX_ALPHA0,	GX_DISABLE,	GX_SRC_REG,	GX_SRC_REG,	GX_LIGHTNULL,						GX_DF_NONE,		GX_AF_NONE);
		GX_SetChanCtrl(GX_ALPHA1,	GX_DISABLE,	GX_SRC_REG,	GX_SRC_REG,	GX_LIGHTNULL,						GX_DF_NONE,		GX_AF_NONE);
	
		GX_SetChanAmbColor(GX_COLOR0A0, lightColor[1]);
		GX_SetChanAmbColor(GX_COLOR1A1, lightColor[5]);

		for (size_t mesh = 0; mesh < mSolids[solid].mMeshes.size(); mesh++)
		{
			// tells gx where our position and color data is
			// args: type of data, pointer, array stride
			GX_SetArray(GX_VA_POS, &mSolids[solid].mMeshes[mesh].mVertices[0].position, sizeof(vVertex));
			GX_SetArray(GX_VA_CLR0, &mSolids[solid].mMeshes[mesh].mVertices[0].color, sizeof(vVertex));
			GX_SetArray(GX_VA_TEX0, &mSolids[solid].mMeshes[mesh].mVertices[0].texcoord, sizeof(vVertex));
			GX_SetArray(GX_VA_NRM, &mSolids[solid].mMeshes[mesh].mVertices[0].normal, sizeof(vVertex));
			DCFlushRange(mSolids[solid].mMeshes[mesh].mVertices, mSolids[solid].mMeshes[mesh].mVertexBufferSize);
			
			// setup the vertex descriptor
			// tells the flipper to expect 16bit indexes for position
			// and color data. could also be set to direct.
			GX_ClearVtxDesc();
			GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
			GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX16);
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
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
			
			GX_LoadTexObj(&vTextureCache::GetTexture(mSolids[solid].mMeshes[mesh].mTextures.DiffuseMap)->GXTextureObj, GX_TEXMAP0);
			
			// specular combining
			GX_SetNumTexGens(1);
			GX_SetNumTevStages(2);
			
			// diffuse
			//GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO );
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV );
			
			// specular
			GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR1A1);
			GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC );
			GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV );
			
			// have to step through index buffer manually
			GX_Begin(GX_TRIANGLES, GX_VTXFMT0, mSolids[solid].mMeshes[mesh].mIndexCount);
			
			for (size_t i = 0; i < mSolids[solid].mMeshes[mesh].mIndexCount; i++)
			{
				uint16_t index = mSolids[solid].mMeshes[mesh].mIndices[i];
				GX_Position1x16(index);
				GX_Color1x16(index);
				GX_TexCoord1x16(index);
				GX_Normal1x16(index);
			}
			
			GX_End();
		}
	}
}
