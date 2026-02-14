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

#define ASSERTLINE(a, b)
#define ASSERTMSGLINE(a, b, c)

#ifdef __cplusplus
}
#endif

#endif
