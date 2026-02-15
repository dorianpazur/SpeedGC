#ifndef DOLPHIN_BASE_H
#define DOLPHIN_BASE_H

#include <gccore.h>
#include <ogc/system.h>
#include <ogc/lwp_watchdog.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void PPCHalt();

#ifdef DEBUG

#define ASSERTLINE(line, cond) { if(!(cond)) { OSPanic(__FILE__, __LINE__, "Failed assertion "#cond); } }
#define ASSERTMSGLINE(line, cond, msg) { if(!(cond)) { OSPanic(__FILE__, __LINE__, msg); } }

#else
	
#define ASSERTLINE(line, cond)
#define ASSERTMSGLINE(line, cond, msg)

#endif

#ifdef __cplusplus
}
#endif

#endif
