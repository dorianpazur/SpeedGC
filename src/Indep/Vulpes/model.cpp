#include <gccore.h>
#include <Vulpes/vulpes.h>

extern TPLFile DefaultTPL;

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
			
			vVector2* bufTexcoord = (vVector2*)&tex0Buffer.data[tex0BufferView.byteOffset + tex0Accessor.byteOffset];
			
			// get texcoords
			for (size_t i = 0; i < mVertexCount; i++)
			{
				mVertices[i].texcoord.x = bswap_float(bufTexcoord[i].x);
				mVertices[i].texcoord.y = bswap_float(bufTexcoord[i].y);
			}
			
			vGlTFVector3* bufNrm = (vGlTFVector3*)&nrmBuffer.data[nrmBufferView.byteOffset + nrmAccessor.byteOffset];
			
			// values are little endian and the space is wrong, fix that
			for (size_t i = 0; i < mVertexCount; i++)
			{
				mVertices[i].normal.x = bswap_float(bufNrm[i].z);
				mVertices[i].normal.y = bswap_float(bufNrm[i].y);
				mVertices[i].normal.z = bswap_float(bufNrm[i].x);
			}
			
			const uint16_t* indices = reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
			
			mIndexCount = indexAccessor.count;
			
			mIndices = (uint16_t*)malloc(mIndexCount * sizeof(uint16_t));
			
			for (size_t i = 0; i < mIndexCount; i++)
			{
				// little endian so we have to fix it
				mIndices[i] = __builtin_bswap16(indices[i]);
			}
			
			// get texture, if any
			if (model->images.size() > 0 && model->textures.size() > 0 && model->materials.size() > 0)
			{
				int textureIndex = model->materials[primitives].pbrMetallicRoughness.baseColorTexture.index;
				printf("Texture index: %d\n", textureIndex);
				int sourceIndex = model->textures[textureIndex].source;
				printf("Source index: %d\n", sourceIndex);
				printf("Texture name: %s\n", model->images[sourceIndex].name.c_str());
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
	const float LARGE_NUMBER = 9999999999.0f;
	Mtx44 mv; // modelview matrix.	
	Mtx44 WtoLtmp; // world to local matrix.	
	Mtx44 WtoL; // world to local matrix.	
	Mtx44 VtoWtmp; // view to world matrix.	
	Mtx44 VtoW; // view to world matrix.	
	
	// bad naming: this means multiply a by b and put the result into c (ab)
	guMtxConcat(view,transform,mv);
	
	// load the modelview matrix into matrix memory
	GX_LoadPosMtxImm(mv, GX_PNMTX0);
	
	guMtxInverse(transform, WtoL);
    //guMtxTranspose(WtoLtmp,WtoL);
	guMtxInverse(view, VtoWtmp);
    guMtxTranspose(VtoWtmp,VtoW);
	
	for (size_t mesh = 0; mesh < mMeshes.size(); mesh++)
	{	
		// tells gx where our position and color data is
		// args: type of data, pointer, array stride
		GX_SetArray(GX_VA_POS, &mMeshes[mesh].mVertices[0].position, sizeof(vVertex));
		GX_SetArray(GX_VA_CLR0, &mMeshes[mesh].mVertices[0].color, sizeof(vVertex));
		GX_SetArray(GX_VA_TEX0, &mMeshes[mesh].mVertices[0].texcoord, sizeof(vVertex));
		GX_SetArray(GX_VA_NRM, &mMeshes[mesh].mVertices[0].normal, sizeof(vVertex));
		DCFlushRange(mMeshes[mesh].mVertices, mMeshes[mesh].mVertexBufferSize);
		
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
		
		// light test
		
		guVector lpos;
		guVector rimPos;
		guVector rimPos2;
		guVector center = { 0, 0, 0 };
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
		
		guVecMultiply(WtoL,&lpos,&lpos);
		guVecMultiply(VtoW,&rimPos,&rimPos);
		guVecMultiply(VtoW,&rimPos2,&rimPos2);
		guVecMultiply(WtoL,&rimPos,&rimPos);
		guVecMultiply(WtoL,&rimPos2,&rimPos2);
	
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
		
		GXTexObj texObj;
		TPL_GetTexture(&DefaultTPL, 0, &texObj);
		GX_LoadTexObj(&texObj, GX_TEXMAP0);
		
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
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, mMeshes[mesh].mIndexCount);
		
		for (size_t i = 0; i < mMeshes[mesh].mIndexCount; i++)
		{
			uint16_t index = mMeshes[mesh].mIndices[i];
			GX_Position1x16(index);
			GX_Color1x16(index);
			GX_TexCoord1x16(index);
			GX_Normal1x16(index);
		}
		
		GX_End();
	}
}
