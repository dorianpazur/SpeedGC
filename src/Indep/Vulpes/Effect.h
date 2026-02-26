#ifndef VULPES_EFFECT_H
#define VULPES_EFFECT_H

#include <EABase/eabase.h>
#include <Vulpes/TextureCache.h>

#include <tWare/Align.h>
#include <tWare/Memory.h>
#include <tWare/Math.h>

#if defined(EA_PLATFORM_GAMECUBE)
#include "GC/EffectPlat.h"
#endif

enum VEFFECT_ID
{
	VEFFECT_STANDARD,
	VEFFECT_CAR,
	VEFFECT_FE,
	VEFFECT_SKY,
	VEFFECT_PARTICLES,
	VEFFECT_WORLD,
	VEFFECT_WORLDROAD,
	VEFFECT_MOTIONBLUR,
	
	NUM_VEFFECTS
};

enum TextureAlphaUsageType
{
	TEXUSAGE_NONE,
	TEXUSAGE_MODULATED,
	TEXUSAGE_PUNCHTHRU
};

class vEffect : public vEffectPlat
{
protected:
	vTextureCache::CachedTexture* texture;
public:
	DEF_TWARE_NEW_OVERRIDE(vEffect, MAIN_POOL)
	
	virtual void Start();
	virtual void End();
	
	inline void SetTexture(vTextureCache::CachedTexture* texture)
	{
		this->texture = texture;
	}
	
	static VEFFECT_ID GetEffectIDFromString(const char* str);
};

class vEffectStaticState
{
public:
	static tMatrix4 *pViewMatrix;
	static tMatrix4 *pWorldToLocalMatrix;
	static vEffect *pCurrentEffect;
};

extern vEffect *vEffects[NUM_VEFFECTS];
extern void vEffectInit();

#endif
