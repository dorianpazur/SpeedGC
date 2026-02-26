
#include <Vulpes/Effect.h>
#include <string.h>

#if defined(EA_PLATFORM_GAMECUBE)
#include "GC/EffectPlat.cpp"
#endif

tMatrix4 *vEffectStaticState::pViewMatrix = NULL;
tMatrix4 *vEffectStaticState::pWorldToLocalMatrix = NULL;
vEffect *vEffectStaticState::pCurrentEffect = NULL;

vEffect *vEffects[NUM_VEFFECTS];

void vEffect::Start() {};
void vEffect::End()
{
	vEffectStaticState::pViewMatrix = NULL;
	vEffectStaticState::pWorldToLocalMatrix = NULL;
	texture = NULL;
};

VEFFECT_ID vEffect::GetEffectIDFromString(const char* str)
{
	if (strcmp(str, "STANDARD") == 0)
		return VEFFECT_STANDARD;
	else if (strcmp(str, "CAR") == 0)
		return VEFFECT_CAR;
	else if (strcmp(str, "FE") == 0)
		return VEFFECT_FE;
	else if (strcmp(str, "SKY") == 0)
		return VEFFECT_SKY;
	else if (strcmp(str, "PARTICLES") == 0)
		return VEFFECT_PARTICLES;
	else if (strcmp(str, "WORLD") == 0)
		return VEFFECT_WORLD;
	else if (strcmp(str, "WORLDROAD") == 0)
		return VEFFECT_WORLDROAD;
	
	return VEFFECT_STANDARD; // fallback to standard
}

void vEffectInit()
{
	for (int i = 0; i < NUM_VEFFECTS; i++)
	{
		if (vEffects[i]) // already initialized
			continue;
		
		switch(i)
		{
		default:
		case VEFFECT_STANDARD:
			vEffects[i] = new vEffect_STANDARD();
			break;
		case VEFFECT_CAR:
			vEffects[i] = new vEffect_CAR();
			break;
		case VEFFECT_FE:
			vEffects[i] = new vEffect_FE();
			break;
		case VEFFECT_SKY:
			vEffects[i] = new vEffect_SKY();
			break;
		case VEFFECT_PARTICLES:
			vEffects[i] = new vEffect_PARTICLES();
			break;
		case VEFFECT_WORLD:
			vEffects[i] = new vEffect_WORLD();
			break;
		case VEFFECT_WORLDROAD:
			vEffects[i] = new vEffect_WORLDROAD();
			break;
		case VEFFECT_MOTIONBLUR:
			vEffects[i] = new vEffect_MOTIONBLUR();
			break;
		}
	}
}

