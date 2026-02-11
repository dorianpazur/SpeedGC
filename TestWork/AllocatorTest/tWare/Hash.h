////////////////////////////
//	
//	DJB2 hash functions
//	

#ifndef TWARE_HASH_H
#define TWARE_HASH_H

#include <cstdint>

typedef uint32_t tHash;

static constexpr tHash _CTStringHash(const char* text)
{
    uint32_t h = ~0;
	
    while (*text) { // loop through each char until string terminator is reached
        h = (h << 5) + h;
        h += *(text++);
    }
	
    return h;
}
template <tHash V> static constexpr tHash _CTStringHashTemplate = V;

#define CTStringHash(text) _CTStringHashTemplate<_CTStringHash(text)> // compile-time hash for speed

extern tHash tStringHash(const char* text);

#endif