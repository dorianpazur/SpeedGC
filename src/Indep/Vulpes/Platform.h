#ifndef VULPES_PLATFORM_H
#define VULPES_PLATFORM_H

#include <EABase/eabase.h>

#ifdef EA_PLATFORM_GAMECUBE
#include <Vulpes/GC/Platform.h>
#endif

extern void vDisplayFrame();
extern void InitializePlatform(int argc, char** argv);
extern void UpdatePlatform();

#endif
