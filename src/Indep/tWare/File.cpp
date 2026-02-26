/// 
///	Manages loading files
/// 

#include <tWare/File.h>
#include <EASTL/vector.h>
#include <map>
#include <unordered_map>
#include <cstdio>
#include <cstring>
#include <malloc.h>

// TODO: make this std::unordered_map (Ross' suggestion)
eastl::vector<tFile*, tEASTLAllocator> gOpenFiles;

char gBaseDir[TFILE_MAX_PATH + 1] { '\0' };

tMutex gFileIOMutex;

// abstracted in order to allow for an archive format to potentially be used
tFile* tOpenFile(const char* path)
{
	if (gOpenFiles.capacity() == 0)
	{
		gOpenFiles.reserve(32); // pre-reserve file list to avoid memory fragmentation
	}

	char realPath[TFILE_MAX_PATH + 1] = { 0 };
	snprintf(realPath, TFILE_MAX_PATH, "%s%s", gBaseDir, path);
	//gFileIOMutex.Lock();
	FILE* cFile = fopen(realPath, "rb");
	
	if (cFile)
	{
		fseek(cFile, 0, SEEK_END);
		long size = ftell(cFile);
		fseek(cFile, 0, SEEK_SET);

		tFile* file = new tFile();

		memcpy(file->filename, path, std::min(TFILE_MAX_PATH, strlen(path)));
		file->filesize = size;
		
		// align to 16 bytes and ensure an extra padding byte always exists for text
		long alignedSize = (size + 1) + 16 - ((size + 1) % 16);
		
		file->data = tWareMalloc(alignedSize, "FileData", __LINE__, ALLOC_PARAMS(0, 32)); // create buffer

		if (file->data)
		{
			memset(file->data, 0, alignedSize);
			if (!fread(file->data, size, 1, cFile)) // read the file into the buffer
			{
				// clean up and return null
				free(file->data);
				delete file;
				fclose(cFile);
				gFileIOMutex.Unlock();
				return NULL;
			}
			
			gOpenFiles.push_back(file); // track it for safer releasing and for debugging
		}
		else
		{
			delete file;
			file = NULL;
		}
		
		fclose(cFile);
		//gFileIOMutex.Unlock();

		return file;
	}
	return NULL;
}

// closes file and releases its resources
void tCloseFile(tFile* file)
{
	gFileIOMutex.Lock();
	// find pointer in file list
	for (size_t i = 0; i < gOpenFiles.size(); i++)
	{
		if (gOpenFiles[i] == file)
		{
			gOpenFiles.erase(gOpenFiles.begin() + i); // remove from vector
			delete file;
			break;
		}
	}
	gFileIOMutex.Unlock();
}

// changes base directory to allow loading data from sd gecko
void tChangeBaseDir(const char* path)
{
	gFileIOMutex.Lock();
	strncpy(gBaseDir, path, TFILE_MAX_PATH);
	gFileIOMutex.Unlock();
}
