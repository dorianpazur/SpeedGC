/// <summary>
///	Manages loading and persistence of assets
/// 
/// author Dorian Pazur, Jan 2026
/// </summary>

#ifndef TWAREFILE_H
#define TWAREFILE_H

#include <tWare/Memory.h>
#include <cstdlib>
#include <cstdint>

#define TFILE_MAX_PATH ((size_t)260)

struct tFile
{
	void* data = NULL;
	char filename[TFILE_MAX_PATH] = { '\0' };
	size_t filesize = 0;
	
	DEF_TWARE_NEW_OVERRIDE(tFile, MAIN_POOL)

	~tFile()
	{
		if (data)
		{
			tFree(data);
			data = NULL;
		}
	}
};

extern tFile* tOpenFile(const char* path);
extern void tCloseFile(tFile* file);
extern void tChangeBaseDir(const char* path);

#endif
