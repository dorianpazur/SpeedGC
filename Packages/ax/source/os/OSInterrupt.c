#include <dolphin/os.h>

extern void __RAS_OSDisableInterrupts_begin(void);
extern void __RAS_OSDisableInterrupts_end(void);

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