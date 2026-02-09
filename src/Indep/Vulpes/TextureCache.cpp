////////////////////////////////////
//	
//	Caches textures for use
//	

#include <Vulpes/TextureCache.h>
#include <tWare/Align.h>
#include <unordered_map>

namespace vTextureCache
{
	struct ALIGN(32) CacheEntry
	{
		CachedTexture* texture = NULL;
		size_t refcount = 1;
		
		CacheEntry()
		{
			
		};
		
		CacheEntry(CachedTexture* texture)
		{
			this->texture = texture;
		};
		
		~CacheEntry()
		{
			if (texture)
			{
				delete texture;
				texture = NULL;
			}
		};
	};
	
	CachedTexture* gDefaultTexture = NULL;
	std::unordered_map<tHash, CacheEntry> gTextureCache;
	
	//---------------------------------------------------------------------------------

	bool LoadTextureFromPath(const char* path)
	{
		if (!path)
			return false;
		
		char name[64];
		int32_t filenameIndex;
		size_t length = strlen(path);
		tHash hash = 0;
		
		if (length == 0)
			return false;
		
		memset(name, 0, sizeof(name)); // fill filename with 0s
		
		// isolate filename from path
		for (filenameIndex = length - 1; filenameIndex >= 0; filenameIndex--)	
		{
			if (path[filenameIndex] == '/' || path[filenameIndex] == '\\')
			{
				filenameIndex++;
				break;
			}
		}
		
		if (filenameIndex < 0)
			filenameIndex = 0;
		
		memcpy(name, &path[filenameIndex], length - filenameIndex);
		
		// remove extension by slapping null terminator in the right place
		for (size_t i = 0; i < length - filenameIndex; i++)
		{
			if (name[i] == '.')
			{
				name[i] = '\0';
			}
		}
		
		hash = tStringHash(name);
		
		if (gTextureCache.contains(hash))
		{
			gTextureCache[hash].refcount++;
			return true;
		}
		
		printf("Loading texture %s (hash: 0x%08X)\n", name, hash);
		
		tFile* file = tOpenFile(path);
		if (!file)
			return false;
		
		CachedTexture* texture = new CachedTexture(file, hash, name);
		
		gTextureCache.emplace(hash, texture);
		
		if (hash == CTStringHash("DefaultTexture"))
		{
			//printf("Caching defaulttexture\n");
			gDefaultTexture = texture;
		}
		
		tCloseFile(file);
		
		return true;
	}
	
	//---------------------------------------------------------------------------------

	CachedTexture* GetTexture(tHash nameHash, bool returnDefaultTextureIfNotFound)
	{
		if (gTextureCache.contains(nameHash))
		{
			return gTextureCache[nameHash].texture;
		}
		else if (returnDefaultTextureIfNotFound)
		{
			return gDefaultTexture;
		}
		else
		{
			return NULL;
		}
	}
	
	//---------------------------------------------------------------------------------
	
	void ReleaseTexture(tHash nameHash)
	{
		if (gTextureCache.contains(nameHash))
		{
			gTextureCache[nameHash].refcount--;
			
			if (!gTextureCache[nameHash].refcount)
			{
				if (gDefaultTexture && gDefaultTexture->nameHash == nameHash)
				{
					gTextureCache[nameHash].refcount = 1; // we don't want to delete this!
				}
				else
				{
					gTextureCache.erase(nameHash);
				}
			}
		}
	}
	
	//---------------------------------------------------------------------------------
	
	void Init()
	{
		gTextureCache.reserve(128);
		gTextureCache.clear();
	}
	
	//---------------------------------------------------------------------------------
	
	void Uninit()
	{
		Init();
	}
}
