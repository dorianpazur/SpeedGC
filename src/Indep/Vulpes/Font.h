#ifndef VULPES_FONT_H
#define VULPES_FONT_H

#include <map>
#include <tWare/hash.h>

extern void vScreenPrint(int x, int y, const char* text, uint32_t color = 0xFFFFFFFF, tHash fontName = CTStringHash("Arial"));

#endif