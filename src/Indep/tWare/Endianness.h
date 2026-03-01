// quick utility for endian swapping

#ifndef TWARE_ENDIANNESS_H
#define TWARE_ENDIANNESS_H

#include <EABase/eabase.h>

#ifdef EA_SYSTEM_BIG_ENDIAN

inline void tEndianSwap(int16_t& i)
{
	uint16_t* data = (uint16_t*)&i;
	uint16_t swap = ((*data) >> 8) | ((*data) << 8);
	i = *(int16_t*)&swap;
}

inline void tEndianSwap(uint16_t& i)
{
	uint16_t* data = (uint16_t*)&i;
	uint16_t swap = ((*data) >> 8) | ((*data) << 8);
	i = *(uint16_t*)&swap;
}

inline void tEndianSwap(int32_t& i)
{
	uint32_t* data = (uint32_t*)&i;
	uint32_t swap =	(((*data) & 0xFF000000) >> 24) |
					(((*data) & 0x00FF0000) >> 8) |
					(((*data) & 0x0000FF00) << 8) |
					(((*data) & 0x000000FF) << 24);
	
	i = *(int32_t*)&swap;
}

inline void tEndianSwap(uint32_t& i)
{
	uint32_t* data = (uint32_t*)&i;
	uint32_t swap =	(((*data) & 0xFF000000) >> 24) |
					(((*data) & 0x00FF0000) >> 8) |
					(((*data) & 0x0000FF00) << 8) |
					(((*data) & 0x000000FF) << 24);
	
	i = *(uint32_t*)&swap;
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized" // false alert here when using floats
#endif

inline void tEndianSwap(float& i)
{
	uint32_t* data = (uint32_t*)&i;
	uint32_t swap =	(((*data) & 0xFF000000) >> 24) |
					(((*data) & 0x00FF0000) >> 8) |
					(((*data) & 0x0000FF00) << 8) |
					(((*data) & 0x000000FF) << 24);
	
	i = *(float*)&swap;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop // don't ignore them anymore
#endif

#else

inline void tEndianSwap(int16_t& i) {};
inline void tEndianSwap(uint16_t& i) {};
inline void tEndianSwap(int32_t& i) {};
inline void tEndianSwap(uint32_t& i) {};
inline void tEndianSwap(float& i) {};

#endif

#endif
