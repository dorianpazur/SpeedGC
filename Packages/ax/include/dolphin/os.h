#ifndef DOLPHIN_OS_H
#define DOLPHIN_OS_H

#include <gccore.h>
#include <dolphin/base.h>
#include <ogc/system.h>
#include <ogc/lwp_watchdog.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_BUS_CLOCK 162000000u  
#define OS_CORE_CLOCK 486000000u   

typedef u64 OSTime;

u32 OSGetStackPointer();
BOOL OSDisableInterrupts();
BOOL OSEnableInterrupts();
BOOL OSRestoreInterrupts(BOOL level);
void OSPanic(const char* file, int line, const char* msg, ...);
void OSReport(const char* msg, ...);
void OSVReport(const char* msg, va_list list);
void OSRegisterVersion(const char* id);
OSTime OSGetTime();

#ifdef __cplusplus
}
#endif

#endif
