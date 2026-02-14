#include <dolphin/base.h>

void PPCHalt();
asm (".global PPCHalt\n" "PPCHalt:\n\t"
	"sync\n\t"
"loop:\n\t"
	"nop\n\t"
	"li 3, 0\n\t"
	"nop\n\t"
	"b loop"
);