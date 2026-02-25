// mutex wrapper

#ifndef TWARE_MUTEX_H
#define TWARE_MUTEX_H

#include <EABase/eabase.h>

#if defined (EA_PLATFORM_GAMECUBE)
#include <gccore.h>

struct tMutex
{
	mutex_t lwpMutex;
	
	tMutex()
	{
		LWP_MutexInit(&lwpMutex, true);
	};
	
	void Lock()
	{
		LWP_MutexLock(lwpMutex);
	};
	
	void Unlock()
	{
		LWP_MutexUnlock(lwpMutex);
	};
};

#endif

#endif
