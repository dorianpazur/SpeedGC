#ifndef DEBUG_SCREENPRINTF_H
#define DEBUG_SCREENPRINTF_H

#include <cstdio>
#include <Vulpes/vulpes.h>
#include <tWare/Time.h>
#include <tWare/Memory.h>

extern bool DoScreenPrintf;

#define SCREEN_PRINT_ITEM_COUNT 64
#define SCREEN_PRINT_BUFFER_SIZE 128

extern void ScreenPrintf(int x, int y, char const* fmt, ...);
extern void ScreenPrintf(int x, int y, float duration, char const* fmt, ...);
extern void ScreenPrintf(int x, int y, unsigned int color, char const* fmt, ...);
extern void ScreenPrintf(int x, int y, float duration, unsigned int color, char const* fmt, ...);
extern void ScreenShadowPrintf(int x, int y, unsigned int color, char const* fmt, ...);
extern void ScreenShadowPrintf(int x, int y, char const* fmt, ...);

extern void DisplayDebugScreenPrints();
extern void DrawScreenPrintfs();

#endif
