#ifndef VULPES_RENDER_H
#define VULPES_RENDER_H

#include <Vulpes/Platform.h>
#include <Vulpes/Model.h>
#include <Vulpes/View.h>

extern void StuffSky(vView* view);
extern void DrawVehicles(vView* view);
extern void DrawPropCubes(vView* view);
extern void RenderWorld(vView* view);
extern void DrawBatteries(vView* view);

#endif