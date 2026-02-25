#include <dolphin/os.h>

OSTime OSGetTime()
{
	return (OSTime)gettime();
}
