#ifndef VULPES_VIEWPLAT_H
#define VULPES_VIEWPLAT_H

#include <EABase/eabase.h>
#include <Vulpes/TextureCache.h>

#include <tWare/Align.h>
#include <tWare/Memory.h>

enum VRENDERTARGET_ID
{
	TARGET_DISPLAYBUFFER_FULL,
	TARGET_DISPLAYBUFFER_P1,
	TARGET_DISPLAYBUFFER_P2,
	TARGET_ENVMAP,
	
	NUM_RENDER_TARGETS,
	FIRST_RENDER_TARGET = TARGET_DISPLAYBUFFER_FULL,
	LAST_RENDER_TARGET = NUM_RENDER_TARGETS - 1,
};

struct vRenderTarget
{
	uint32_t Left;
	uint32_t Top;
	uint32_t Width;
	uint32_t Height;
};

extern vRenderTarget vRenderTargets[NUM_RENDER_TARGETS];

#endif
