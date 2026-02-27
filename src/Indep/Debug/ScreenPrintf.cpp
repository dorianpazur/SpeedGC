#include "ScreenPrintf.h"

struct ScreenPrintItem
{
    char Text[128] = { '\0' };
	uint32_t Color = 0xFFFFFFFF;
    short PosX = 0;
    short PosY = 0;
    float Lifetime = 0.0f;
};

extern bool bWideScreen;
bool DoScreenPrintf = true;

ScreenPrintItem ScreenPrintItemTable[SCREEN_PRINT_ITEM_COUNT];

ScreenPrintItem* GetUnusedScreenPrintItem()
{
    for (int i = 0; i < SCREEN_PRINT_ITEM_COUNT; i++)
    {
        if (ScreenPrintItemTable[i].Lifetime < 0.0f)
            return &ScreenPrintItemTable[i];
    }
    return 0;
}

ScreenPrintItem* GetScreenPrintItemAtLocation(int x, int y)
{
    for (int i = 0; i < SCREEN_PRINT_ITEM_COUNT; i++)
    {
        if (ScreenPrintItemTable[i].PosX == x && ScreenPrintItemTable[i].PosY == y)
            return &ScreenPrintItemTable[i];
    }
    return 0;
}

void FixWidescreen(int* x)
{
    if (bWideScreen)
    {
        if (*x >= -10)
        {
            if (*x <= 10)
                return;
            *x = *x + 120;
        }
        else
        {
            *x = *x - 120;
        }
    }
}

void ScreenPrintf(int x, int y, float duration, unsigned int color, char const* fmt, va_list &argList)
{
    ScreenPrintItem* item = NULL;

    FixWidescreen(&x);

    if (DoScreenPrintf)
    {
        if ((duration == 0) || ((item = GetScreenPrintItemAtLocation(x, y)) == 0))
            item = GetUnusedScreenPrintItem();

        if (item)
        {
            item->Lifetime = duration;
            item->PosX = x;
            item->PosY = y;
			item->Color = color;
			
			va_list argListCopy;
			va_copy(argListCopy, argList);
			
            vsnprintf(item->Text, 128, fmt, argListCopy);
			
			va_end(argListCopy);
        }
    }
}

void ScreenPrintf(int x, int y, char const* fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);
    ScreenPrintf(x, y, 0.0f, 0xFFFFFFFF, fmt, argList);
    va_end(argList);
}

void ScreenPrintf(int x, int y, float duration, char const* fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);
    ScreenPrintf(x, y, duration, 0xFFFFFFFF, fmt, argList);
    va_end(argList);
}

void ScreenPrintf(int x, int y, unsigned int color, char const* fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);
    ScreenPrintf(x, y, 0.0f, color, fmt, argList);
    va_end(argList);
}

void ScreenPrintf(int x, int y, float duration, unsigned int color, char const* fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);
    ScreenPrintf(x, y, duration, color, fmt, argList);
    va_end(argList);
}

void ScreenShadowPrintf(int x, int y, float duration, unsigned int color, char const* fmt, ...)
{
    va_list argList;
	
    va_start(argList, fmt);
    ScreenPrintf(x + 1, y + 1, duration, 0x80000000, fmt, argList);
    ScreenPrintf(x, y, duration, color, fmt, argList);
    va_end(argList);
}

void ScreenShadowPrintf(int x, int y, unsigned int color, char const* fmt, ...)
{
    va_list argList;
    float duration = 0.0f;
	
    va_start(argList, fmt);
    ScreenPrintf(x + 1, y + 1, duration, 0x80000000, fmt, argList);
    ScreenPrintf(x, y, duration, color, fmt, argList);
    va_end(argList);
}

void ScreenShadowPrintf(int x, int y, char const* fmt, ...)
{
    va_list argList;
    float duration = 0.0f;
	
    va_start(argList, fmt);
    ScreenPrintf(x + 1, y + 1, duration, 0x80000000, fmt, argList);
    ScreenPrintf(x, y, duration, 0xFFFFFFFF, fmt, argList);
    va_end(argList);
}

// some stats from main

extern float CPUTime;
extern float GPUTime;
extern float gAvgFps;

void DisplayDebugScreenPrints()
{
    ScreenPrintf(-300, -30, "C: %.2fms", CPUTime);
	ScreenPrintf(-200, -30, "G: %.2fms", GPUTime);
	ScreenPrintf(-300, -15, "F: %.2ffps", gAvgFps);
	ScreenPrintf(-300, 45, "Main = %dK  Phys = %dK", tCountFreeMemory(MAIN_POOL) / 1024, tCountFreeMemory(PHYSICS_POOL) / 1024);
	ScreenPrintf(-300, 60, "Heap = %dK", ((uint32_t)SYS_GetArenaHi() - (uint32_t)SYS_GetArenaLo()) / 1024);
}

void DrawScreenPrintfs()
{
	static unsigned int prevTicker = tGetTicker();
	
	unsigned int now = tGetTicker();
	float frameTime = tGetTickerDifference(prevTicker, now);
	prevTicker = now;
	
    for (int i = SCREEN_PRINT_ITEM_COUNT - 1; i >= 0; i--) // display in reverse order
    {
        if (ScreenPrintItemTable[i].Lifetime >= 0.0f)
        {
            ScreenPrintItemTable[i].Lifetime -= frameTime * 0.001f;
			
			vScreenPrint(ScreenPrintItemTable[i].PosX, ScreenPrintItemTable[i].PosY, ScreenPrintItemTable[i].Text, ScreenPrintItemTable[i].Color);
        }
    }
}
