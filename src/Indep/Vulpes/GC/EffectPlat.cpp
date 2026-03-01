
#include <Vulpes/Effect.h>
#include "World.h"

struct __gx_texobj
{
	u32 tex_filt;
	u32 tex_lod;
	u32 tex_size;
	u32 tex_maddr;
	u32 usr_data;
	u32 tex_fmt;
	u32 tex_tlut;
	u16 tex_tile_cnt;
	u8 tex_tile_type;
	u8 tex_flag;
} __attribute__((packed));

class vEffect_STANDARD : public vEffect
{	
public:
	DEF_TWARE_NEW_OVERRIDE(vEffect_STANDARD, MAIN_POOL)
	virtual void Start();
};

void vEffect_STANDARD::Start()
{
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	
	GX_SetNumChans(1);
	GX_SetNumTevStages(1);
	
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL,
				GX_DF_NONE, GX_AF_NONE);
	
	if (texture)
	{
		//GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 4, -2.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_2);
		GX_SetNumTexGens(1);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
		GX_SetTevColorOp(GX_TEVSTAGE0,GX_TEV_ADD,GX_TB_ZERO,HalfBrightness ? GX_CS_DIVIDE_2 : GX_CS_SCALE_1,GX_TRUE,GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	}
	else
	{
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
		GX_SetNumTexGens(0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	}
}

class vEffect_FE : public vEffect
{	
public:
	DEF_TWARE_NEW_OVERRIDE(vEffect_FE, MAIN_POOL)
	virtual void Start();
};

void vEffect_FE::Start()
{
	u8 stage = 0;
	GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	
	GX_SetNumChans(1);
	
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	
	if (texture)
	{
		//GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 0, 0.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_1);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetNumTexGens(2);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
		
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		GX_SetTevOp(stage, GX_MODULATE);
		GX_SetTevOrder(stage, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		stage++;
		
		if (miscmap1) // use this as mask for UI
		{
			GX_LoadTexObj(&miscmap1->GXTextureObj, GX_TEXMAP1);
			
			GX_SetTevOrder(stage, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
			GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV );
			GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO );
			GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			stage++;
		}
	}
	else
	{
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
		GX_SetNumTexGens(0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		stage++;
	}
	
	GX_SetNumTevStages(stage);
}

class vEffect_SKY : public vEffect
{	
public:
	DEF_TWARE_NEW_OVERRIDE(vEffect_SKY, MAIN_POOL)
	virtual void Start();
};

void vEffect_SKY::Start()
{
	GX_SetNumChans(1);
	
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	
	if (texture)
	{
		//GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 0, 0.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_1);
		
		if (texture->nameHash == CTStringHash("SKY_BACKDROP"))
		{
			GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
			GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
		}
		else
		{
			GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
			GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
		}
		
		((__gx_texobj*)&texture->GXTextureObj)->tex_filt = (((__gx_texobj*)&texture->GXTextureObj)->tex_filt & ~0x0c) | ((GX_CLAMP) << 2);
		
		if (texture->nameHash == CTStringHash("SKY_CLOUDS"))
		{
			GX_SetNumTevStages(2);
			GX_SetNumTexGens(2);
			
			Mtx scrollMtx1;
			scrollMtx1[0][0] = 1;
			scrollMtx1[0][1] = 0;
			scrollMtx1[0][2] = 0;
			scrollMtx1[1][0] = 0;
			scrollMtx1[1][1] = 1;
			scrollMtx1[1][2] = 0;
			scrollMtx1[2][0] = 0;
			scrollMtx1[2][1] = 0;
			scrollMtx1[2][2] = 1;
			scrollMtx1[0][3] = 0;
			scrollMtx1[1][3] = 0;
			scrollMtx1[2][3] = 0;
			
			Mtx scrollMtx2;
			scrollMtx2[0][0] = 1;
			scrollMtx2[0][1] = 0;
			scrollMtx2[0][2] = 0;
			scrollMtx2[1][0] = 0;
			scrollMtx2[1][1] = 1;
			scrollMtx2[1][2] = 0;
			scrollMtx2[2][0] = 0;
			scrollMtx2[2][1] = 0;
			scrollMtx2[2][2] = 1;
			scrollMtx2[0][3] = 0;
			scrollMtx2[1][3] = 0;
			scrollMtx2[2][3] = 0;
			
			World* world;
			if ((world = World::GetInstance()))
			{
				float timeTicker = world->mTimeElapsed * 0.01f;
				scrollMtx1[0][3] += timeTicker * 0.4f; // scroll
				scrollMtx2[0][3] += (timeTicker * 0.2f) + 0.3f; // scroll
			}

			GX_LoadTexMtxImm(scrollMtx1, GX_TEXMTX0, GX_TG_MTX2x4);
			GX_LoadTexMtxImm(scrollMtx2, GX_TEXMTX1, GX_TG_MTX2x4);
			
			GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1);
			
			GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
			
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR0A0);
			
			GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
			
			GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV );
			GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO );
			GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
		}
		else
		{
			GX_SetNumTevStages(1);
			GX_SetNumTexGens(1);
			
			GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
			
			GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
			
			GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		}
	}
	else
	{
		GX_SetNumTevStages(1);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
		GX_SetNumTexGens(0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	}
}

class vEffect_PARTICLES : public vEffect
{	
public:
	DEF_TWARE_NEW_OVERRIDE(vEffect_PARTICLES, MAIN_POOL)
	virtual void Start();
};

void vEffect_PARTICLES::Start()
{
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR);
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE); // disable zwrite
	
	GX_SetNumChans(1);
	GX_SetNumTevStages(1);
	
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	
	if (texture)
	{
		((__gx_texobj*)&texture->GXTextureObj)->tex_filt = (((__gx_texobj*)&texture->GXTextureObj)->tex_filt & ~0xF) | (GX_CLAMP) | ((GX_CLAMP) << 2);
		
		//GX_InitTexObjLOD(&texture->GXTextureObj, GX_LINEAR, GX_LINEAR, 0, 0, 0.0f, GX_ENABLE, GX_ENABLE, GX_ANISO_1);
		
		GX_SetNumTexGens(1);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	}
	else
	{
		GX_SetNumTexGens(0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	}
	
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV); // make it 2x brighter
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
}


class vEffect_WORLD : public vEffect
{	
public:
	DEF_TWARE_NEW_OVERRIDE(vEffect_WORLD, MAIN_POOL)
	virtual void Start();
};

void vEffect_WORLD::Start()
{
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	
	GX_SetNumChans(1);
	GX_SetNumTevStages(1);
	
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL,
				GX_DF_NONE, GX_AF_NONE);
	
	if (texture)
	{
		//GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 4, 0.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_1);
		GX_SetNumTexGens(1);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
		GX_SetTevColorOp(GX_TEVSTAGE0,GX_TEV_ADD,GX_TB_ZERO,HalfBrightness ? GX_CS_DIVIDE_2 : GX_CS_SCALE_1,GX_TRUE,GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	}
	else
	{
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
		GX_SetNumTexGens(0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	}
}


class vEffect_WORLDROAD : public vEffect
{	
public:
	DEF_TWARE_NEW_OVERRIDE(vEffect_WORLDROAD, MAIN_POOL)
	virtual void Start();
};

void vEffect_WORLDROAD::Start()
{
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	
	GX_SetNumChans(1);
	GX_SetNumTevStages(1);
	
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL,
				GX_DF_NONE, GX_AF_NONE);
	
	if (texture)
	{
		//GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 4, -4.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_2);
		
		((__gx_texobj*)&texture->GXTextureObj)->tex_filt = (((__gx_texobj*)&texture->GXTextureObj)->tex_filt & ~0xF) | (GX_REPEAT) | ((GX_REPEAT) << 2);
		
		GX_SetNumTexGens(1);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
		GX_SetTevColorOp(GX_TEVSTAGE0,GX_TEV_ADD,GX_TB_ZERO,HalfBrightness ? GX_CS_DIVIDE_2 : GX_CS_SCALE_1,GX_TRUE,GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	}
	else
	{
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
		GX_SetNumTexGens(0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	}
}

class vEffect_MOTIONBLUR : public vEffect
{	
public:
	DEF_TWARE_NEW_OVERRIDE(vEffect_MOTIONBLUR, MAIN_POOL)
	virtual void Start();
};

void vEffect_MOTIONBLUR::Start()
{
	if (texture)
	{
		GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
		
		GX_SetNumChans(1);
		GX_SetNumTevStages(2);
		
		GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
		
		//GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 0, 0.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_1);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetNumTexGens(2);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
			
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		GX_SetTevColorIn(GX_TEVSTAGE0,GX_CC_ZERO,GX_CC_TEXC,GX_CC_HALF,GX_CC_ZERO);
		GX_SetTevAlphaIn(GX_TEVSTAGE0,GX_CA_ZERO,GX_CA_ZERO,GX_CA_ZERO,GX_CA_RASA);
		
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		
		GX_SetTevColorOp(GX_TEVSTAGE0,GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_TRUE,GX_TEVPREV);
		GX_SetTevAlphaOp(GX_TEVSTAGE0,GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_TRUE,GX_TEVPREV);
		
		GX_SetTevColorIn(GX_TEVSTAGE1,GX_CC_ZERO,GX_CC_TEXC,GX_CC_HALF,GX_CC_CPREV);
		GX_SetTevAlphaIn(GX_TEVSTAGE1,GX_CA_ZERO,GX_CA_ZERO,GX_CA_ZERO,GX_CA_APREV);
		
		GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR0A0);
		
		GX_SetTevColorOp(GX_TEVSTAGE1,GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_TRUE,GX_TEVPREV);
		GX_SetTevAlphaOp(GX_TEVSTAGE1,GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_TRUE,GX_TEVPREV);
	}
}

class vEffect_CAR : public vEffect
{	
public:
	DEF_TWARE_NEW_OVERRIDE(vEffect_CAR, MAIN_POOL)
	virtual void Start();
};

extern vTextureCache::CachedTexture* gEnvmapTexture;

void vEffect_CAR::Start()
{
	if (texture && vEffectStaticState::pViewMatrix && vEffectStaticState::pWorldToLocalMatrix)
	{
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		
		tMatrix4 VtoW;
		tInvertMatrix(&VtoW, vEffectStaticState::pViewMatrix);
		
		const float LARGE_NUMBER = 99999999.0f;
		
		tVector3 lpos;
		tVector3 lpos2;
		tVector3 lpos3;
		tVector3 fresnelPos;
		
		GXLightObj lobj;
		GXLightObj lobj2;
		GXLightObj lobj3;
		GXLightObj lspecobj;
		GXLightObj diffuseRangeLight;
		
		uint8_t DiffuseMin = (uint8_t)(((uint32_t)(Material->DiffuseMin * 255.0f)) & 0xFF);
		uint8_t DiffuseRange = (uint8_t)(((uint32_t)((Material->DiffuseMax - Material->DiffuseMin) * 255.0f)) & 0xFF);
		uint8_t SpecularBrightness = (uint8_t)(((uint32_t)(Material->SpecularBrightness * 255.0f)) & 0xFF);
		
		GXColor light1Color = {0xF0,0xF0,0xF0,0}; // Light 1 color
		GXColor light2Color = {0xB8,0xB8,0xB8,0}; // Light 2 color
		GXColor light3Color = {0xB8,0xB8,0xB8,0}; // Light 3 color
		GXColor ambientColor = {0x50,0x50,0x50,DiffuseMin}; // Ambient color
		GXColor specularColor = {SpecularBrightness,SpecularBrightness,SpecularBrightness,SpecularBrightness}; // Spec color
		GXColor diffuseRangeColor = {0,0,0,DiffuseRange}; // diffuse
		
		lpos.x = 0.707f * LARGE_NUMBER;
		lpos.y = 0.707f * LARGE_NUMBER;
		lpos.z = 0.707f * LARGE_NUMBER;
		
		lpos2.x = -0.707f * LARGE_NUMBER;
		lpos2.y = -0.707f * LARGE_NUMBER;
		lpos2.z = -0.707f * LARGE_NUMBER;
		
		lpos3.x = 0;
		lpos3.y = 1.0 * LARGE_NUMBER;
		lpos3.z = 0;
		
		fresnelPos.x =  0 * LARGE_NUMBER;
		fresnelPos.y =  0 * LARGE_NUMBER;
		fresnelPos.z =  1 * LARGE_NUMBER;
		
		tMulVector(&lpos,vEffectStaticState::pWorldToLocalMatrix,&lpos);
		tMulVector(&fresnelPos,&VtoW,&fresnelPos);
		tMulVector(&fresnelPos,vEffectStaticState::pWorldToLocalMatrix,&fresnelPos);
		
		tVector3 lposSpec;
		float lposLength = sqrt((lpos.x * lpos.x) + (lpos.z * lpos.z) + (lpos.z * lpos.z));
		
		lposSpec.x = -lpos.x / lposLength;
		lposSpec.y = -lpos.y / lposLength;
		lposSpec.z = -lpos.z / lposLength;
		
		GX_InitLightPos(&lobj,lpos.x,lpos.y,lpos.z);
		GX_InitLightColor(&lobj,light1Color);
		GX_InitLightAttnA(&lobj, 0.0f, 1.0f, 0.1f);
		
		GX_InitLightPos(&lobj2,lpos2.x,lpos2.y,lpos2.z);
		GX_InitLightColor(&lobj2,light2Color);
		GX_InitLightAttnA(&lobj2, 0.0f, 1.0f, 0.1f);
		
		GX_InitLightPos(&lobj3,lpos3.x,lpos3.y,lpos3.z);
		GX_InitLightColor(&lobj3,light3Color);
		GX_InitLightAttnA(&lobj3, 0.0f, 0.5f, 0.5f);
		
		GX_InitSpecularDir(&lspecobj,-lpos.x,-lpos.y,-lpos.z);
		GX_InitLightColor(&lspecobj,specularColor);
		GX_InitLightShininess(&lspecobj, Material->SpecularPower);
		
		GX_InitLightPos(&diffuseRangeLight,fresnelPos.x,fresnelPos.y,fresnelPos.z);
		GX_InitLightColor(&diffuseRangeLight, diffuseRangeColor);
		
		GX_LoadLightObj(&lobj,GX_LIGHT0);
		GX_LoadLightObj(&lobj2,GX_LIGHT1);
		GX_LoadLightObj(&lobj3,GX_LIGHT2);
		GX_LoadLightObj(&lspecobj,GX_LIGHT3);
		GX_LoadLightObj(&diffuseRangeLight,GX_LIGHT4);
		
		// set number of rasterized color channels
		GX_SetNumChans(2);
		GX_SetChanCtrl(GX_COLOR0,	GX_ENABLE,	GX_SRC_REG,	GX_SRC_REG,	GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2,	GX_DF_CLAMP,	GX_AF_NONE); // diffuse light
		GX_SetChanCtrl(GX_ALPHA0,	GX_ENABLE,	GX_SRC_REG,	GX_SRC_REG,	GX_LIGHT4, GX_DF_CLAMP,	GX_AF_NONE); // diffuse falloff
		GX_SetChanCtrl(GX_COLOR1,	GX_DISABLE,	GX_SRC_REG,	GX_SRC_VTX,	GX_LIGHT_NULL, GX_DF_NONE,	GX_AF_NONE); // envmap
		GX_SetChanCtrl(GX_ALPHA1,	GX_ENABLE,	GX_SRC_REG,	GX_SRC_REG,	GX_LIGHT3,	GX_DF_CLAMP,	GX_AF_SPEC); // specular light
	
		GX_SetChanAmbColor(GX_COLOR0A0, ambientColor);
		GX_SetChanMatColor(GX_COLOR0A0, (GXColor){0xFF,0xFF,0xFF,0xFF});
		
		GX_SetChanAmbColor(GX_COLOR1A1, (GXColor){0x00,0x00,0x00,0x00});
		GX_SetChanMatColor(GX_COLOR1A1, (GXColor){0xFF,0xFF,0xFF,0xFF});
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		
		if (gEnvmapTexture)
		{
			GX_SetNumTexGens(2);
			
			tMatrix4 model;
			tMatrix4 mv;
			tMatrix4 envmap;
			guMtxInverse(*(Mtx*)vEffectStaticState::pWorldToLocalMatrix, *(Mtx*)&model);
			guMtxConcat(*(Mtx*)vEffectStaticState::pViewMatrix, *(Mtx*)&model, *(Mtx*)&mv);
			guMtxInverse(*(Mtx*)&mv, *(Mtx*)&mv);
			guMtxTranspose(*(Mtx*)&mv, *(Mtx*)&mv);
			
			Mtx s;
			Mtx t;
			
			guMtxScale(s, -0.5F, -0.5F, 0.0F);
			guMtxTrans(t, 0.5F, 0.5F, 1.0F);
			guMtxConcat(s, *(Mtx*)&mv, *(Mtx*)&envmap);
			guMtxConcat(t, *(Mtx*)&envmap, *(Mtx*)&envmap);
			
			GX_LoadTexObj(&gEnvmapTexture->GXTextureObj, GX_TEXMAP1);
			GX_LoadTexMtxImm(*(Mtx44*)&envmap, GX_TEXMTX0, GX_TG_MTX3x4);
			GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
		}
		else
		{
			GX_SetNumTexGens(1);
		}
		
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		u8 stage = GX_TEVSTAGE0;
		
		// diffuse
		GX_SetTevOrder(stage, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
		GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_TEXA, GX_CA_ONE, GX_CA_ZERO);
		GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
		GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
		stage++;
		
		// diffuse falloff
		GX_SetTevOrder(stage, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASA, GX_CC_ZERO);
		GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
		GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
		GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
		stage++;
		
		// specular
		GX_SetTevOrder(stage, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR1A1);
		GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_RASA, GX_CC_ONE, GX_CC_CPREV);
		GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
		GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
		GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
		stage++;
		
		// envmap
		if (gEnvmapTexture)
		{
			GXColor envmapColor
			{
				(uint8_t)(((uint32_t)(0xFF * Material->EnvmapR)) & 0xFF),
				(uint8_t)(((uint32_t)(0xFF * Material->EnvmapG)) & 0xFF),
				(uint8_t)(((uint32_t)(0xFF * Material->EnvmapB)) & 0xFF),
				(uint8_t)(((uint32_t)(0xFF * Material->EnvmapA)) & 0xFF)
			};
			GX_SetTevKColor(GX_KCOLOR0, envmapColor);
			GX_SetTevKColorSel(stage, GX_TEV_KCSEL_K0);
			GX_SetTevKAlphaSel(stage, GX_TEV_KASEL_K0_A);
			GX_SetTevOrder(stage, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR1A1);
			GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV);
			GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
			GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
			GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
			stage++;
			
			// overbright
			if (Material->EnvmapR > 1.0f || Material->EnvmapG > 1.0f || Material->EnvmapB > 1.0f || Material->EnvmapA > 1.0f)
			{	
				GXColor envmapOverbrightColor
				{
					(uint8_t)(((uint32_t)(0xFF * std::max(0.0f, Material->EnvmapR - 1.0f))) & 0xFF),
					(uint8_t)(((uint32_t)(0xFF * std::max(0.0f, Material->EnvmapG - 1.0f))) & 0xFF),
					(uint8_t)(((uint32_t)(0xFF * std::max(0.0f, Material->EnvmapB - 1.0f))) & 0xFF),
					(uint8_t)(((uint32_t)(0xFF * std::max(0.0f, Material->EnvmapA - 1.0f))) & 0xFF)
				};
				
				GX_SetTevKColor(GX_KCOLOR1, envmapOverbrightColor);
				GX_SetTevKColorSel(stage, GX_TEV_KCSEL_K1);
				GX_SetTevKAlphaSel(stage, GX_TEV_KASEL_K1_A);
				GX_SetTevOrder(stage, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR1A1);
				GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV);
				GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
				stage++;
			}
		}
		
		// multiply lighting by vertex color
		GX_SetTevOrder(stage, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR1A1);
		GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
		GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
		GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
		GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
		stage++;
		
		GX_SetNumTevStages(stage);
	}
}
