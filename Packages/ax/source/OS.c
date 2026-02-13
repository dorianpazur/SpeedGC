#include <dolphin/os.h>

extern void __RAS_OSDisableInterrupts_begin(void);
extern void __RAS_OSDisableInterrupts_end(void);

void OSReport(const char* msg, ...) {
    va_list marker;
    va_start(marker, msg);
    vprintf(msg, marker);
    va_end(marker);
}

void OSVReport(const char* msg, va_list list) {
    vprintf(msg, list);
}

void OSRegisterVersion(const char* id) {
    OSReport("%s\n", id);
}

u32 OSGetStackPointer();
asm (".global OSGetStackPointer\n" "OSGetStackPointer:\n\t"
	"mr 3, 1\n\t" // put the stack pointer in r3 (return value)
	"blr\n\t"
);

void PPCHalt();
asm (".global PPCHalt\n" "PPCHalt:\n\t"
	"sync\n\t"
"loop:\n\t"
	"nop\n\t"
	"li 3, 0\n\t"
	"nop\n\t"
	"b loop"
);

BOOL OSDisableInterrupts();
asm (".global OSDisableInterrupts\n" "OSDisableInterrupts:\n\t"
	"mfmsr   3\n"
".global    __RAS_OSDisableInterrupts_begin\n__RAS_OSDisableInterrupts_begin:\n\t"
	"rlwinm  4, 3, 0, 17, 15\n\t"
	"mtmsr   4\n"
".global    __RAS_OSDisableInterrupts_end\n__RAS_OSDisableInterrupts_end:\n\t"
	"rlwinm  3, 3, 17, 31, 31\n\t"
	"blr"
);

BOOL OSEnableInterrupts();
asm (".global OSEnableInterrupts\n" "OSEnableInterrupts:\n\t"
	"mfmsr   3\n\t"
	"ori     4, 3, 32768\n\t"
	"mtmsr   4\n\t"
	"rlwinm  3, 3, 17, 31, 31"
);

BOOL OSRestoreInterrupts(BOOL level);
asm (".global OSRestoreInterrupts\n" "OSRestoreInterrupts:\n\t"
	"cmpwi   3, 0\n\t"
	"mfmsr   4\n\t"
	"beq     _disable\n\t"
	"ori     5, 4, 32768\n\t"
	"b       _restore\n"
"_disable:\n\t"
	"rlwinm  5, 4, 0, 17, 15\n"
"_restore:\n\t"
	"mtmsr   5\n\t"
	"rlwinm  3, 4, 17, 31, 31\n\t"
	"blr\n\t"
);

void OSPanic(const char* file, int line, const char* msg, ...)
{
	va_list marker;
	u32 i;
	u32* p;
	
	OSDisableInterrupts();
	va_start(marker, msg);
	vprintf(msg, marker);
	va_end(marker);
	OSReport(" in \"%s\" on line %d.\n", file, line);
	
	OSReport("\nAddress:      Back Chain    LR Save\n");
	for (i = 0, p = (u32*)OSGetStackPointer();
		p && (u32)p != 0xffffffff && i++ < 16; p = (u32*)*p) {
		OSReport("0x%08x:   0x%08x    0x%08x\n", (uintptr_t)p, p[0], p[1]);
	}
	
	fflush(stdout);
	
	PPCHalt();
}

OSTime OSGetTime()
{
	return (OSTime)gettime();
}
