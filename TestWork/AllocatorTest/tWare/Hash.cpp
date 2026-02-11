////////////////////////////
//	
//	DJB2 hash functions
//	

#include <tWare/Hash.h>

// DJB2 hash function with ~0 (all Fs) as seed number
tHash tStringHash(const char* text)
{
	uint32_t h = ~0;
	
    while (*text) { // loop through each char until string terminator is reached
        h = (h << 5) + h;
        h += *(text++);
    }
	
    return h;
}
