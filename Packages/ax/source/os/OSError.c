#include <dolphin/os.h>

void OSReport(const char* msg, ...) {
    va_list marker;
    va_start(marker, msg);
    vprintf(msg, marker);
    va_end(marker);
}

void OSVReport(const char* msg, va_list list) {
    vprintf(msg, list);
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
	OSReport(" in \"%s\" on line %d.\n", file, line);
	
	OSReport("\nAddress:      Back Chain    LR Save\n");
	for (i = 0, p = (u32*)OSGetStackPointer();
		p && (u32)p != 0xffffffff && i++ < 16; p = (u32*)*p) {
		OSReport("0x%08x:   0x%08x    0x%08x\n", (uintptr_t)p, p[0], p[1]);
	}
	
	fflush(stdout);
	
	PPCHalt();
}
