#include <dolphin/os.h>

void OSRegisterVersion(const char* id) {
    OSReport("%s\n", id);
}

OSTime OSGetTime()
{
	return (OSTime)gettime();
}
