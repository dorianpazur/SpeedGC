
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
		GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 4, -2.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_2);
		GX_SetNumTexGens(1);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
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
	GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	
	GX_SetNumChans(1);
	GX_SetNumTevStages(1);
	
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	
	if (texture)
	{
		GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 0, 0.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_1);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetNumTexGens(1);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
			
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
	
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
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
		GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 0, 0.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_1);
		
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
			
			GX_SetTevColorIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CC_CPREV );
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
		
		GX_InitTexObjLOD(&texture->GXTextureObj, GX_LINEAR, GX_LINEAR, 0, 0, 0.0f, GX_ENABLE, GX_ENABLE, GX_ANISO_1);
		
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
		GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 4, 0.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_1);
		GX_SetNumTexGens(1);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
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
		GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 4, -4.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_2);
		
		((__gx_texobj*)&texture->GXTextureObj)->tex_filt = (((__gx_texobj*)&texture->GXTextureObj)->tex_filt & ~0xF) | (GX_REPEAT) | ((GX_REPEAT) << 2);
		
		GX_SetNumTexGens(1);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
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
		
		GX_InitTexObjLOD(&texture->GXTextureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 0, 0.0f, GX_DISABLE, GX_ENABLE, GX_ANISO_1);
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
		
		const float LARGE_NUMBER = 9999999999.0f;
		
		tVector3 lpos;
		tVector3 lpos2;
		tVector3 rimPos;
		tVector3 rimPos2;
		
		GXLightObj lobj;
		GXLightObj lobj2;
		GXLightObj lspecobj;
		GXLightObj rimLight;
		GXLightObj rimLight2;
		GXLightObj rimLightSpec;
		GXLightObj rimLight2Spec;
		
		const static GXColor lightColor[] = {
			{0xB0,0xB0,0xB0,0xFF}, // Light 1 color
			{0x58,0x58,0x58,0xFF}, // Light 2 color
			{0x50,0x50,0x60,0xFF}, // Ambient color
			{0x60,0x60,0x60,0xFF}, // Spec color
			{0xE0,0xD0,0xC0,0xFF}, // rim 1
			{0x3F,0x4F,0x5F,0xFF}, // rim 2
			{0x00,0x00,0x00,0xFF}, // No color
		};
		
		lpos.x = 0.707f * LARGE_NUMBER;
		lpos.y = 0.707f * LARGE_NUMBER;
		lpos.z = 0.707f * LARGE_NUMBER;
		
		lpos2.x = -0.707f * LARGE_NUMBER;
		lpos2.y = -0.707f * LARGE_NUMBER;
		lpos2.z = -0.707f * LARGE_NUMBER;
		
		rimPos.x =  0.5 * LARGE_NUMBER;
		rimPos.y =  0.707f * LARGE_NUMBER;
		rimPos.z = -0.707f * LARGE_NUMBER;
		
		rimPos2.x = -0.5 * LARGE_NUMBER;
		rimPos2.y = -0.35f * LARGE_NUMBER;
		rimPos2.z = -0.35f * LARGE_NUMBER;
		
		tMulVector(&lpos,vEffectStaticState::pWorldToLocalMatrix,&lpos);
		tMulVector(&rimPos,&VtoW,&rimPos);
		tMulVector(&rimPos2,&VtoW,&rimPos2);
		tMulVector(&rimPos,vEffectStaticState::pWorldToLocalMatrix,&rimPos);
		tMulVector(&rimPos2,vEffectStaticState::pWorldToLocalMatrix,&rimPos2);
		
		GX_InitLightPos(&lobj,lpos.x,lpos.y,lpos.z);
		GX_InitLightColor(&lobj,lightColor[0]);
		
		GX_InitLightPos(&lobj2,lpos2.x,lpos2.y,lpos2.z);
		GX_InitLightColor(&lobj2,lightColor[1]);
		
		GX_InitSpecularDir(&lspecobj,-lpos.x,-lpos.y,-lpos.z);
		GX_InitLightColor(&lspecobj,lightColor[3]);
		GX_InitLightShininess(&lspecobj, 60.0f);
		
		GX_InitLightPos(&rimLight,rimPos.x,rimPos.y,rimPos.z);
		GX_InitLightColor(&rimLight,lightColor[4]);
		GX_InitLightAttnA(&rimLight, 2.0, 2.0, 2.0);
		
		GX_InitSpecularDir(&rimLightSpec,-rimPos.x,-rimPos.y,-rimPos.z);
		GX_InitLightColor(&rimLightSpec,lightColor[4]);
		GX_InitLightAttnA(&rimLightSpec, 2.0, 2.0, 2.0);
		GX_InitLightShininess(&rimLightSpec, 22.0f);
		
		GX_InitLightPos(&rimLight2,rimPos2.x,rimPos2.y,rimPos2.z);
		GX_InitLightColor(&rimLight2,lightColor[5]);
		GX_InitLightAttnA(&rimLight2, 2.0, 2.0, 2.0);
		
		GX_InitSpecularDir(&rimLight2Spec,-rimPos2.x,-rimPos2.y,-rimPos2.z);
		GX_InitLightColor(&rimLight2Spec,lightColor[5]);
		GX_InitLightAttnA(&rimLight2Spec, 2.0, 2.0, 2.0);
		GX_InitLightShininess(&rimLight2Spec, 22.0f);
		
		GX_LoadLightObj(&lobj,GX_LIGHT0);
		GX_LoadLightObj(&lobj2,GX_LIGHT1);
		GX_LoadLightObj(&rimLight,GX_LIGHT2);
		GX_LoadLightObj(&rimLight2,GX_LIGHT3);
		GX_LoadLightObj(&lspecobj,GX_LIGHT4);
		GX_LoadLightObj(&rimLightSpec,GX_LIGHT5);
		GX_LoadLightObj(&rimLight2Spec,GX_LIGHT6);
		
		// set number of rasterized color channels
		GX_SetNumChans(2);
		GX_SetChanCtrl(GX_COLOR0,	GX_ENABLE,	GX_SRC_REG,	GX_SRC_VTX,	GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2 | GX_LIGHT3,	GX_DF_CLAMP,	GX_AF_NONE);
		GX_SetChanCtrl(GX_COLOR1,	GX_ENABLE,	GX_SRC_REG,	GX_SRC_VTX,	GX_LIGHT4 | GX_LIGHT5 | GX_LIGHT6,	GX_DF_CLAMP,	GX_AF_SPEC);
		GX_SetChanCtrl(GX_ALPHA0,	GX_DISABLE,	GX_SRC_REG,	GX_SRC_VTX,	GX_LIGHTNULL,						GX_DF_CLAMP,		GX_AF_NONE);
		GX_SetChanCtrl(GX_ALPHA1,	GX_DISABLE,	GX_SRC_REG,	GX_SRC_VTX,	GX_LIGHTNULL,						GX_DF_CLAMP,		GX_AF_NONE);
	
		GX_SetChanAmbColor(GX_COLOR0A0, lightColor[2]);
		GX_SetChanAmbColor(GX_COLOR1A1, lightColor[6]);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		
		if (gEnvmapTexture)
		{
			GX_SetNumTexGens(2);
			GX_SetNumTevStages(3);
			
			tMatrix4 model;
			tMatrix4 mv;
			tMatrix4 envmap;
			guMtxInverse(*(Mtx*)vEffectStaticState::pWorldToLocalMatrix, *(Mtx*)&model);
			guMtxConcat(*(Mtx*)vEffectStaticState::pViewMatrix, *(Mtx*)&model, *(Mtx*)&mv);
			guMtxInverse(*(Mtx*)&mv, *(Mtx*)&mv);
			guMtxTranspose(*(Mtx*)&mv, *(Mtx*)&mv);
			
			Mtx s;
			Mtx t;
			
			guMtxScale(s, 0.5F, -0.5F, 0.0F);
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
			GX_SetNumTevStages(2);
		}
		
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		// diffuse
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO );
		GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO );
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV );
		GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		
		// specular
		GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR1A1);
		GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC );
		GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CA_APREV );
		GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV );
		GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		
		// envmap
		if (gEnvmapTexture)
		{
			GX_SetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_1_4);
			GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
			GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV );
			GX_SetTevAlphaIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CA_APREV );
			GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV );
			GX_SetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		}
	}
}
