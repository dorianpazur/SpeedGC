////////////////////////////////////
//	
//	Caches textures for use
//	

#ifndef VULPES_TEXTURECACHE_H
#define VULPES_TEXTURECACHE_H

#include <tWare/Hash.h>
#include <tWare/File.h>
#include <tWare/Align.h>
#include <tWare/Memory.h>
#include <EABase/eabase.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <malloc.h>
#ifdef EA_PLATFORM_GAMECUBE
	#include <gccore.h>
#endif

namespace vTextureCache
{
	#ifdef EA_PLATFORM_GAMECUBE
		struct CachedTexturePlat
		{
			GXTexObj GXTextureObj;
			
			u32 format = 0;
			
			char debugName[80];
			
			CachedTexturePlat(uint16_t width, uint16_t height, uint8_t format)
			{
				RawData = tWareMalloc(GX_GetTexBufferSize(width, height, format, GX_FALSE, 0), "Render Target", __LINE__, ALLOC_PARAMS(MAIN_POOL, 32));
				GX_InitTexObj(&GXTextureObj, RawData, width, height, format, GX_FALSE, GX_FALSE, GX_FALSE);
				GX_InvalidateTexAll();
			}
			
			CachedTexturePlat(tFile* file)
			{
				snprintf(debugName, 79, "Texture %s", file->filename);
				RawData = tWareMalloc(file->filesize, debugName, __LINE__, ALLOC_PARAMS(MAIN_POOL, 32));
				
				memcpy(RawData, file->data, file->filesize);
				
				TPLFile tpl;
				TPL_OpenTPLFromMemory(&tpl, RawData, file->filesize);
				TPL_GetTexture(&tpl, 0, &GXTextureObj);
				
				GX_InvalidateTexAll();
			}
			
			~CachedTexturePlat()
			{
				tFree(RawData);
				RawData = NULL;
				GX_InvalidateTexAll();
			}
			
			void GetTexturePlatInfo(uint32_t &width, uint32_t &height)
			{
				width = (uint32_t)GX_GetTexObjWidth(&GXTextureObj);
				height = (uint32_t)GX_GetTexObjHeight(&GXTextureObj);
			}
			
		private:
			void* RawData = NULL;
		};
	#endif
	
	struct ALIGN(32) CachedTexture : CachedTexturePlat
	{
		tHash nameHash;
		char debugName[20];
		uint32_t width = 0;
		uint32_t height = 0;
		
		DEF_TWARE_NEW_OVERRIDE(CachedTexture, MAIN_POOL)
		
		CachedTexture(const char* debugName, uint16_t width, uint16_t height, uint16_t format) : CachedTexturePlat(width, height, format)
		{
			nameHash = tStringHash(debugName);
			memset(this->debugName, 0, sizeof(debugName));
			memcpy(this->debugName, debugName, std::min(sizeof(this->debugName), strlen(debugName)));
			GetTexturePlatInfo(this->width, this->height);
		}
		
		CachedTexture(tFile* file, tHash hash, const char* debugName) : CachedTexturePlat(file)
		{
			nameHash = hash;
			memset(this->debugName, 0, sizeof(debugName));
			memcpy(this->debugName, debugName, std::min(sizeof(this->debugName), strlen(debugName)));
			GetTexturePlatInfo(width, height);
		}
	};
	
	extern bool LoadTextureFromPath(const char* path);
	extern CachedTexture* GetTexture(tHash nameHash, bool returnDefaultTextureIfNotFound = true);
	extern void ReleaseTexture(tHash nameHash);
	extern void Init();
	extern void Uninit();
}

#endif
