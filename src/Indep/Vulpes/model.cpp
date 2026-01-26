#include <gccore.h>
#include <Vulpes/vulpes.h>

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
	Mtx44 WtoL; // world to local matrix.	
	
	// bad naming: this means multiply a by b and put the result into c (ab)
	guMtxConcat(view,transform,mv);
	
	// load the modelview matrix into matrix memory
	GX_LoadPosMtxImm(mv, GX_PNMTX0);
	
	guMtxInverse(transform, WtoL);
	
	for (size_t mesh = 0; mesh < mMeshes.size(); mesh++)
	{
		// tells gx where our position and color data is
		// args: type of data, pointer, array stride
		GX_SetArray(GX_VA_POS, &mMeshes[mesh].mVertices[0].position, sizeof(vVertex));
		GX_SetArray(GX_VA_CLR0, &mMeshes[mesh].mVertices[0].color, sizeof(vVertex));
		GX_SetArray(GX_VA_NRM, &mMeshes[mesh].mVertices[0].normal, sizeof(vVertex));
		DCFlushRange(mMeshes[mesh].mVertices, mMeshes[mesh].mVertexBufferSize);
		
		// setup the vertex descriptor
		// tells the flipper to expect 16bit indexes for position
		// and color data. could also be set to direct.
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
		GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX16);
		GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16);
		
		// setup the vertex attribute table
		// describes the data
		// args: vat location 0-7, type of data, data format, size, scale
		// so for ex. in the first call we are sending position data with
		// 3 values X,Y,Z of size S16. scale sets the number of fractional
		// bits for non float data.
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
		
		// light test
		
		guVector lpos;
		guVector center = { 0, 0, 0 };
		GXLightObj lobj;
		
		const static GXColor lightColor[] = {
			{0xB0,0x9A,0x70,0xFF}, // Light color
			{0x36,0x46,0x50,0xFF}, // Ambient color
			{0x00,0x00,0x00,0xFF}  // Mat color
		};
		
		lpos.x = -0.707f;
		lpos.y = -0.707f;
		lpos.z = -0.707f;
		
		guVecMultiply(transform,&center,&center);
		lpos.x += center.x;
		lpos.y += center.y;
		lpos.z += center.z;
		guVecMultiply(WtoL,&lpos,&lpos);
	
		GX_InitSpecularDir(&lobj,lpos.x,lpos.y,lpos.z);
		GX_InitLightColor(&lobj,lightColor[0]);
		GX_InitLightShininess(&lobj, 22.0f);
		
		GX_LoadLightObj(&lobj,GX_LIGHT0);
		
		// set number of rasterized color channels
		GX_SetNumChans(2);
		GX_SetChanCtrl(GX_COLOR0,GX_ENABLE,GX_SRC_REG,GX_SRC_VTX,GX_LIGHT0,GX_DF_CLAMP,GX_AF_NONE);
		GX_SetChanCtrl(GX_COLOR0,GX_ENABLE,GX_SRC_REG,GX_SRC_VTX,GX_LIGHT0,GX_DF_NONE,GX_AF_SPEC);
    	GX_SetChanCtrl(GX_ALPHA0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
    	GX_SetChanCtrl(GX_ALPHA1, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);

		GX_SetChanAmbColor(GX_COLOR0A0,lightColor[1]);
		GX_SetChanAmbColor(GX_COLOR1A1,lightColor[2]);
		
		// no idea...sets to no textures
		// i don't know anything about textures or lighting yet :|
		GX_SetNumChans(1);
		GX_SetNumTexGens(0);
		GX_SetNumTevStages(2);
		
		// specular combining
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    	GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV );
    	GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO );
		
		// have to step through index buffer manually
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, mMeshes[mesh].mIndexCount);
		
		for (size_t i = 0; i < mMeshes[mesh].mIndexCount; i++)
		{
			uint16_t index = mMeshes[mesh].mIndices[i];
			GX_Position1x16(index);
			GX_Color1x16(index);
			GX_Normal1x16(index);
		}
		
		GX_End();
	}
}
