// thread wrapper stuff

#include <tWare/Thread.h>

#if defined(EA_PLATFORM_GAMECUBE)

lwp_t gMainThread;

void tInitThreads()
{
	gMainThread = LWP_GetSelf();
}

bool tIsMainThread()
{
	return LWP_GetSelf() == gMainThread;
}

void tThreadYield()
{
	LWP_YieldThread();
}

#endif
