// thread wrapper stuff

#ifndef TWARE_THREAD_H
#define TWARE_THREAD_H

#include <EABase/eabase.h>
#if defined (EA_PLATFORM_GAMECUBE)
#include <gccore.h>
#endif

extern void tInitThreads();
extern void tThreadYield();

#endif
