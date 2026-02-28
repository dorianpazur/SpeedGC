
#include <Vulpes/Effect.h>
#include <string.h>

#if defined(EA_PLATFORM_GAMECUBE)
#include "GC/EffectPlat.cpp"
#endif

// material stuff

vMaterial DullPlastic
{
	
};

vMaterial CarPaint
{
	.EnvmapR = 0.6f,
	.EnvmapG = 0.6f,
	.EnvmapB = 0.6f,
	.EnvmapA = 0.6f,
	
	.DiffuseMin = 0.65f,
	.DiffuseMax = 1.0f
};

vMaterial Glass
{
	.EnvmapR = 0.7f,
	.EnvmapG = 0.7f,
	.EnvmapB = 0.7f,
	.EnvmapA = 0.7f,
	
	.DiffuseMin = 0.25f,
	.DiffuseMax = 0.5f
};

vMaterial GlassMask
{
	.EnvmapR = 0.7f,
	.EnvmapG = 0.7f,
	.EnvmapB = 0.7f,
	.EnvmapA = 0.7f,
	
	.DiffuseMin = 0.25f,
	.DiffuseMax = 0.5f
};

vMaterial Chrome
{
	.EnvmapR = 2.0f,
	.EnvmapG = 2.0f,
	.EnvmapB = 2.0f,
	.EnvmapA = 2.0f,
	
	.DiffuseMin = 0.1f,
	.DiffuseMax = 0.25f
};

vMaterial Taillights
{
	.EnvmapR = 0.65f,
	.EnvmapG = 0.65f,
	.EnvmapB = 0.65f,
	.EnvmapA = 0.65f,
	
	.DiffuseMin = 0.75f,
	.DiffuseMax = 1.0f
};

vMaterial Reflectors
{
	.EnvmapR = 0.85f,
	.EnvmapG = 0.85f,
	.EnvmapB = 0.85f,
	.EnvmapA = 0.85f,
	
	.DiffuseMin = 0.25f,
	.DiffuseMax = 0.5f
};

vMaterial *vGetMaterialFromName(const char* name)
{
	if (strcmp(name, "DullPlastic") == 0)
		return &DullPlastic;
	else if (strcmp(name, "CarPaint") == 0)
		return &CarPaint;
	else if (strcmp(name, "Glass") == 0)
		return &Glass;
	else if (strcmp(name, "GlassMask") == 0)
		return &GlassMask;
	else if (strcmp(name, "Chrome") == 0)
		return &Chrome;
	else if (strcmp(name, "Taillights") == 0)
		return &Taillights;
	else if (strcmp(name, "Reflectors") == 0)
		return &Reflectors;
	
	return &DullPlastic; // fallback to dullplastic
}

// effect stuff

tMatrix4 *vEffectStaticState::pViewMatrix = NULL;
tMatrix4 *vEffectStaticState::pWorldToLocalMatrix = NULL;
vEffect *vEffectStaticState::pCurrentEffect = NULL;

vEffect *vEffects[NUM_VEFFECTS];

void vEffect::Start() {};
void vEffect::End()
{
	vEffectStaticState::pViewMatrix = NULL;
	vEffectStaticState::pWorldToLocalMatrix = NULL;
	HalfBrightness = false;
	Material = &DullPlastic;
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

TextureAlphaUsageType vGetTextureAlphaUsageType(const char* str)
{
	if (strcmp(str, "MODULATED") == 0)
		return TEXUSAGE_MODULATED;
	else if (strcmp(str, "PUNCHTHRU") == 0)
		return TEXUSAGE_PUNCHTHRU;
	
	return TEXUSAGE_NONE; // fallback to none
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
	
	for (int i = 0; i < NUM_VEFFECTS; i++)
	{
		if (vEffects[i])
			vEffects[i]->ID = (VEFFECT_ID)i;
	}
}

