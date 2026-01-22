/// <summary>
///	Manages loading and persistence of assets
/// 
/// author Dorian Pazur, Jan 2026
/// </summary>

#ifndef TWAREFILE_H
#define TWAREFILE_H

#include <cstdlib>
#include <cstdint>

struct tFile
{
	void* data = NULL;
	const char* filename = "";
	size_t filesize = 0;
	
	~tFile()
	{
		if (data)
		{
			free(data);
			data = NULL;
		}
	}
};

extern tFile* tOpenFile(const char* path);
extern void tCloseFile(tFile* file);

#endif
