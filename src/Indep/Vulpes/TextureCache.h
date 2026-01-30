////////////////////////////////////
//	
//	Caches textures for use
//	

#ifndef VULPES_TEXTURECACHE_H
#define VULPES_TEXTURECACHE_H

#include <tWare/Hash.h>
#include <tWare/File.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#ifdef GEKKO
	#include <gccore.h>
#endif

namespace vTextureCache
{
	#ifdef GEKKO
		struct CachedTexturePlat 
		{
			GXTexObj GXTextureObj;
			
			u32 format = 0;
			
			CachedTexturePlat(tFile* file)
			{
				size_t alignedFileSize = file->filesize + 32 - (file->filesize % 32);
				UnalignedRawData = malloc(alignedFileSize);
				RawData = (void*)(((int)UnalignedRawData - 1u + 32) & -32);
				
				memcpy(RawData, file->data, file->filesize);
				
				TPL_OpenTPLFromMemory(&tpl, RawData, file->filesize);
				TPL_GetTexture(&tpl, 0, &GXTextureObj);
			}
			
			~CachedTexturePlat()
			{
				RawData = NULL;
				free(UnalignedRawData);
				UnalignedRawData = NULL;
			}
			
			void GetTexturePlatInfo(uint32_t &width, uint32_t &height)
			{
				u16 tplWidth;
				u16 tplHeight;
				
				TPL_GetTextureInfo(&tpl, 0, &format, &tplWidth, &tplHeight);
				
				width = tplWidth;
				height = tplHeight;
			}
			
		private:
			TPLFile tpl;
			void* RawData = NULL;
			void* UnalignedRawData = NULL;
		};
	#endif
	
	struct CachedTexture : CachedTexturePlat
	{
		tHash nameHash;
		char debugName[20];
		uint32_t width = 0;
		uint32_t height = 0;
		
		size_t refcount = 1;
		
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
}

#endif
