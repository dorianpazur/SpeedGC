////////////////////////
//
//	Debug utilities
//

#ifndef TWARE_DEBUG_H
#define TWARE_DEBUG_H

//#include <tWare/Memory.h>
#include <EABase/eabase.h>

#ifdef EA_PLATFORM_GAMECUBE
#include <gccore.h>
extern "C"
{
	extern void PPCExcptDefaultHandler(void);
}
#elifdef EA_PLATFORM_WINDOWS
#include <Windows.h>
#endif

extern void tBreak();

#endif