
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
