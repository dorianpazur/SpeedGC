#include <gccore.h>
#include <Vulpes/Font.h>
#include <Vulpes/TextureCache.h>

void vScreenPrint(int x, int y, const char* text, uint32_t color, tHash fontName)
{
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	
	GX_SetCullMode(GX_CULL_NONE);
	
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	
	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetNumTevStages(1);
	
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL,
	              GX_DF_NONE, GX_AF_NONE);
	
	vTextureCache::CachedTexture* fontTexture = vTextureCache::GetTexture(fontName);
	
	GX_LoadTexObj(&fontTexture->GXTextureObj, GX_TEXMAP0);
	
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	
	size_t characterCount = strlen(text);
	
	GX_Begin(GX_QUADS, GX_VTXFMT0, characterCount * 4);
	
	float currentOffset = 0.0f;
	
	for (size_t c = 0; c < characterCount; c++)
	{
		float leftX = ((x + currentOffset) / 320.0f) - (10.0f / 320.0f);
		float topY = (y / 240.0f) - (6.0f / 320.0f);
		float rightX = leftX + (30.0f / 320.0f);
		float bottomY = topY + (30.0f / 240.0f);
		
		float texLeftX = (text[c] % 16) / 16.0f;
		float texTopY = (text[c] / 16) / 16.0f;
		float texRightX = texLeftX + (1 / 16.0f);
		float texBottomY = texTopY + (1 / 16.0f);
		
		// offset by half a pixel so it looks a bit nicer
		texLeftX += 0.5f/fontTexture->width;
		texRightX -= 0.5f/fontTexture->width;
		
		texTopY += 0.5f/fontTexture->height;
		texBottomY -= 0.5f/fontTexture->height;
		
		GX_Position3f32(rightX, topY, 0);
		GX_Color4u8( (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF );
		GX_TexCoord2f32(texRightX, texTopY);
		
		GX_Position3f32(rightX, bottomY, 0);
		GX_Color4u8( (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF );
		GX_TexCoord2f32(texRightX, texBottomY);
		
		GX_Position3f32(leftX, bottomY, 0);
		GX_Color4u8( (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF );
		GX_TexCoord2f32(texLeftX, texBottomY);
		
		GX_Position3f32(leftX, topY, 0);
		GX_Color4u8( (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF );
		GX_TexCoord2f32(texLeftX, texTopY);
		
		float kern = 11.5f;
		
		switch (text[c])
		{
			case ' ':
			case ';':
			case ':':
			case '.':
			case ',':
			case '[':
			case ']':
			case '(':
			case ')':
				kern = 6.0f;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'y':
			case 'a':
			case 'o':
			case '+':
			case '-':
			case '=':
				kern = 11.0f;
				break;
			case 'e':
				kern = 10.0f;
				break;
			case 'r':
			case 'i':
			case 'j':
			case 'f':
			case 'l':
			case 't':
			case 'I':
			case 'J':
				kern = 8.0f;
				break;
			case 'M':
			case 'm':
			case 'N':
			case 'W':
			case 'w':
				kern = 15.0f;
				break;
			
		};
		
		currentOffset += kern;
	}
	
	GX_End();
}
