#include <dolphin/os.h>

u32 OSGetStackPointer();
asm (".global OSGetStackPointer\n" "OSGetStackPointer:\n\t"
	"mr 3, 1\n\t" // put the stack pointer in r3 (return value)
	"blr\n\t"
);
