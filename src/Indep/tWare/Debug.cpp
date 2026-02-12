////////////////////////
//
//	Debug utilities
//

#include <tWare/Debug.h>
#undef tBreak

// reverse-engineered from Dolphin SDK by doldecomp guys
// ASM routines ported from Metrowerks to GCC syntax
#if defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_REVOLUTION)
extern "C"
{
	u32 OSGetStackPointer();
	asm (".global OSGetStackPointer\n\t" "OSGetStackPointer:\n\t"
		"mr 3, 1\n\t" // put the stack pointer in r3 (return value)
		"blr\n\t"
	);
	
	void PPCHalt();
	asm (".global PPCHalt\n\t" "PPCHalt:\n\t"
		"sync\n\t"
	"loop:\n\t"
		"nop\n\t"
		"li 3, 0\n\t"
		"nop\n\t"
		"b loop"
	);
	
	void OSDisableInterrupts()
	{
		asm (
			"mfmsr   3\n\t"
			"rlwinm  4, 3, 0, 17, 15\n\t"
			"mtmsr   4\n\t"
			"rlwinm  3, 3, 17, 31, 31"
			:
            :
			: "3", "4"
		);
	}
	
	void OSPanic(const char* file, int line, const char* msg, ...)
	{
		va_list marker;
		u32 i;
		u32* p;
		
		OSDisableInterrupts();
		va_start(marker, msg);
		vprintf(msg, marker);
		va_end(marker);
		SYS_Report(" in \"%s\" on line %d.\n", file, line);
		
		SYS_Report("\nAddress:      Back Chain    LR Save\n");
		for (i = 0, p = (u32*)OSGetStackPointer();
			p && (u32)p != 0xffffffff && i++ < 16; p = (u32*)*p) {
			SYS_Report("0x%08x:   0x%08x    0x%08x\n", (uintptr_t)p, p[0], p[1]);
		}
		
		fflush(stdout);
		
		PPCHalt();
	}
}
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