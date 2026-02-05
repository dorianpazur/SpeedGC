#include "DebugMenuRender.h"
#include "ScreenPrintf.h"
#include <Vulpes/vulpes.h>

extern bool bWideScreen;

void GetStringSize(const char* str, int* x, int* y)
{
	int ry = 18;
	
	size_t characterCount = strlen(str);
	
	float currentOffset = 0.0f;
	
	for (size_t c = 0; c < characterCount; c++)
	{
		currentOffset += vGetFontKern(str[c]);
	}
	
    *x = (int)currentOffset;
    *y = ry;
}

DebugMenuRender::DebugMenuRender()
{

}

DebugMenuRender::~DebugMenuRender()
{

}

void DebugMenuRender::getStringSize(const char* str, int* x, int* y)
{
    GetStringSize(str, x, y);
}

void DebugMenuRender::renderOverlay(int x, int y, int width, int height, float alpha, unsigned int color)
{
    if (bWideScreen)
    {
        x -= 120;
        width += 240;
    }
	
	vPoly poly;
	
	poly.Vertices[0].x = (float)x;
	poly.Vertices[0].y = (float)y;
	poly.Vertices[0].z = 0.0f;
	poly.Vertices[1].z = 0.0f;
	poly.Vertices[1].x = (float)(width + x);
	poly.Vertices[2].z = 0.0f;
	poly.Vertices[3].z = 0.0f;
	poly.Vertices[1].y = poly.Vertices[0].y;
	poly.Vertices[2].x = poly.Vertices[1].x;
	poly.Vertices[2].y = (float)(height + y);
	poly.Vertices[3].x = poly.Vertices[0].x;
	poly.Vertices[3].y = poly.Vertices[2].y;
	
	poly.Colours[0][0] = color >> 0;
	poly.Colours[0][1] = color >> 8;
	poly.Colours[0][2] = color >> 16;
	poly.Colours[0][3] = (uint8_t)(alpha * 255.0f);
	
	*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
	*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
	*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];
	
	vPolyFERender(&poly, NULL);
}

// print a line in the debug menu
void DebugMenuRender::print(int x, int y, unsigned int color, const char* str)
{
    DoScreenPrintf = true;

    ScreenPrintf(x - 320, y - 240, color, str);

    DoScreenPrintf = false;
}
