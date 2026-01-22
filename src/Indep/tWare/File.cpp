/// 
///	Manages loading files
/// 

#include <tWare/File.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <cstdio>
#include <cstring>

// TODO: make this std::unordered_map (Ross' suggestion)
std::vector<tFile*> gOpenFiles;

const char* pathPrefix = "dvd://%s";

// abstracted in order to allow for an archive format to potentially be used
tFile* tOpenFile(const char* path)
{
	char realPath[260] = { 0 };
	snprintf(realPath, 260, pathPrefix, path);
	FILE* cFile = fopen(realPath, "rb");
	
	if (cFile)
	{
		fseek(cFile, 0, SEEK_END);
		long size = ftell(cFile);
		fseek(cFile, 0, SEEK_SET);

		tFile* file = new tFile();

		file->filename = path;
		file->filesize = size;
		
		// align to 16 bytes and ensure an extra padding byte always exists for text
		long alignedSize = (size + 1) + 16 - ((size + 1) % 16);
		
		file->data = malloc(alignedSize); // create buffer
		memset(file->data, 0, alignedSize);

		if (file->data)
		{
			if (!fread(file->data, size, 1, cFile)) // read the file into the buffer
			{
				// clean up and return null
				free(file->data);
				delete file;
				return NULL;
			}

			gOpenFiles.push_back(file); // track it for safer releasing and for debugging
		}
		else
		{
			delete file;
			file = NULL;
		}

		return file;
	}
	return NULL;
}

// closes file and releases its resources
void tCloseFile(tFile* file)
{
	// find pointer in file list
	for (size_t i = 0; i < gOpenFiles.size(); i++)
	{
		if (gOpenFiles[i] == file)
		{
			gOpenFiles.erase(gOpenFiles.begin() + i); // remove from vector
			free(file->data);
			delete file;
			break;
		}
	}
}
