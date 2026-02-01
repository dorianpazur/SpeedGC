#ifndef DEBUG_DEBUGMENURENDER_H
#define DEBUG_DEBUGMENURENDER_H

#include "IMenuRender.h"

class DebugMenuRender : public IMenuRender
{
public:
    virtual void renderOverlay(int x, int y, int width, int height, float alpha, unsigned int color);
    virtual void print(int x, int y, unsigned int color, const char* str);
    virtual void getStringSize(const char* str, int* x, int* y);
    DebugMenuRender();
	~DebugMenuRender();
};

#endif