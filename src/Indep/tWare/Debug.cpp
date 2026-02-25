////////////////////////
//
//	Debug utilities
//

#include <tWare/Debug.h>
#undef tBreak
#if defined(EA_PLATFORM_GAMECUBE)
#include <dolphin.h>
#endif

#if defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_REVOLUTION)
void tBreak(const char* filename, int linenum)
#else
void tBreak()
#endif
{
#if defined(EA_COMPILER_MSVC) && defined(EA_PLATFORM_WINDOWS)
	__debugbreak();
#elif defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_REVOLUTION)
	OSPanic(filename, linenum, "Breakpoint hit");
#else
	*(uint8_t*)0 = 0; // trigger access violation as a last resort
#endif
}
