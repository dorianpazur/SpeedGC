#include <gccore.h>
#include <Vulpes/vulpes.h>

//---------------------------------------------------------------------------------

void vPolyFERender(vPoly* poly, struct vTextureCache::CachedTexture* texture)
{
	vPoly polyMod;
	
	memcpy(&polyMod, poly, sizeof(vPoly));
	
	polyMod.Vertices[0].x = (poly->Vertices[0].x / 320.0f) - 1.0f;
	polyMod.Vertices[0].y = (poly->Vertices[0].y / 240.0f) - 1.0f;
	
	polyMod.Vertices[1].x = (poly->Vertices[1].x / 320.0f) - 1.0f;
	polyMod.Vertices[1].y = (poly->Vertices[1].y / 240.0f) - 1.0f;
	
	polyMod.Vertices[2].x = (poly->Vertices[2].x / 320.0f) - 1.0f;
	polyMod.Vertices[2].y = (poly->Vertices[2].y / 240.0f) - 1.0f;
	
	polyMod.Vertices[3].x = (poly->Vertices[3].x / 320.0f) - 1.0f;
	polyMod.Vertices[3].y = (poly->Vertices[3].y / 240.0f) - 1.0f;
	
	vPolyRender(&polyMod, texture);
}

//---------------------------------------------------------------------------------

void vPolyRender(vPoly* poly, struct vTextureCache::CachedTexture* texture)
{
	if (texture)
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	else
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
	
	GX_SetCullMode(GX_CULL_NONE);
	
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	if (texture)
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	if (texture)
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	
	GX_SetNumChans(1);
	GX_SetNumTevStages(1);
	
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL,
	              GX_DF_NONE, GX_AF_NONE);
	
	if (texture)
	{
		GX_SetNumTexGens(1);
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
	
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
	
	for (int i = 0; i < 4; i++)
	{
		GX_Position3f32(poly->Vertices[i].x, poly->Vertices[i].y, poly->Vertices[i].z);
		GX_Color4u8(poly->Colours[i][0], poly->Colours[i][1], poly->Colours[i][2], poly->Colours[i][3]);
		if (texture)
			GX_TexCoord2f32(poly->UVs[i][0], poly->UVs[i][1]);
	}
	
	GX_End();
}
