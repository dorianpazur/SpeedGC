
#include <Vulpes/Effect.h>
#include <string.h>

#if defined(EA_PLATFORM_GAMECUBE)
#include "GC/EffectPlat.cpp"
#endif

vEffect *vEffectStaticState::pCurrentEffect = NULL;

vEffect *vEffects[NUM_VEFFECTS];

void vEffect::Start() {};
void vEffect::End()
{
	texture = NULL;
};

VEFFECT_ID vEffect::GetEffectIDFromString(const char* str)
{
	if (strcmp(str, "STANDARD") == 0)
		return VEFFECT_STANDARD;
	else if (strcmp(str, "FE") == 0)
		return VEFFECT_FE;
	else if (strcmp(str, "SKY") == 0)
		return VEFFECT_SKY;
	
	return VEFFECT_STANDARD; // fallback to standard
}

void vEffectInit()
{
	for (int i = 0; i < NUM_VEFFECTS; i++)
	{
		switch(i)
		{
		default:
		case VEFFECT_STANDARD:
			vEffects[i] = new vEffect_STANDARD();
			break;
		case VEFFECT_FE:
			vEffects[i] = new vEffect_FE();
			break;
		case VEFFECT_SKY:
			vEffects[i] = new vEffect_SKY();
			break;
		}
	}
}

