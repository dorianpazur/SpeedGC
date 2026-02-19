
#include <Vulpes/Effect.h>

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
	
	GX_SetNumTexGens(1);
	
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		
	GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
	
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
}

class vEffect_FE : public vEffect
{	
public:
	DEF_TWARE_NEW_OVERRIDE(vEffect_FE, MAIN_POOL)
	virtual void Start();
};

void vEffect_FE::Start()
{
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
	
	GX_SetNumChans(1);
	GX_SetNumTevStages(1);
	
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL,
				GX_DF_NONE, GX_AF_NONE);
	
	if (texture)
	{
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
	GX_SetNumTevStages(1);
	
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL,
				GX_DF_NONE, GX_AF_NONE);
	
	if (texture)
	{
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
		
		GX_SetNumTexGens(1);
		
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		
		GX_LoadTexObj(&texture->GXTextureObj, GX_TEXMAP0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	}
	else
	{
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
		GX_SetNumTexGens(0);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	}
}
