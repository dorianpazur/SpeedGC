
#ifndef XSPARK_HPP
#define XSPARK_HPP

#include <EABase/eabase.h>

#if EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

const uint32_t MAX_NGPARTICLES = 2048;
const uint32_t MAX_NGEMITTERS = 100;

#include <EASTL/vector.h>
#include <tWare/Math.h>
#include <tWare/Memory.h>
#include <cmath>
#include <vector>

struct XenonEffectDef
{
    tVector3 vel = { 0, 0, 0 };
    tMatrix4 mat;
    //Attrib::Collection* spec = NULL;
	struct EffectDef* def = NULL;
    //struct EmitterGroup* piggyback_effect = NULL;
	bool isContrail = false;
};

typedef eastl::vector<XenonEffectDef, tEASTLAllocator> XenonFXVec;
class XenonEffectList : public XenonFXVec
{
public:
	XenonEffectList()
	{
		this->reserve(MAX_NGEMITTERS);
	}
};

class NGParticle
{
public:

    enum Flags
    {
        SPAWN = 1 << 0,
        DEBRIS = 1 << 1,
        BOUNCED = 1 << 2,
    };

    tVector3 initialPos;
    unsigned int color;
    tVector3 vel;
    float gravity;
    tVector3 impactNormal;
    ALIGN(16) uint16_t flags;
    uint16_t spin;
    uint16_t life;
    uint8_t length;
    uint8_t width;
    uint8_t uv[4];
    float age;
};

class ParticleList
{
public:
    NGParticle mParticles[MAX_NGPARTICLES];
    unsigned int mNumParticles;
    void AgeParticles(float dt);
    void GeneratePolys();
    NGParticle* GetNextParticle();
};

struct EmitterUV
{
    float StartU;
    float EndU;
    float StartV;
	float EndV;
};

struct EmitterDef
{
	tVector3 VolumeCenter;
    tVector3 VelocityDelta;
    tVector3 VolumeExtent;
    tVector3 VelocityInherit;
    tVector3 VelocityStart;
    tVector4 Colour1;
    EmitterUV* emitteruv;
    float Life;
    float NumParticlesVariance;
    float GravityStart;
    float HeightStart;
    float GravityDelta;
    float LengthStart;
    float LengthDelta;
    float LifeVariance;
    float NumParticles;
    int16_t Spin;
    int8_t zSprite;
    int8_t zContrail;
};

struct EffectDef
{
	uint32_t mNumEmitters;
	EmitterDef* mEmitters[8];
};

struct CGEmitter
{
    EmitterDef* mEmitterDef;
    EmitterUV* mTextureUVs;
    tVector3 mVel;
    tMatrix4 mLocalWorld;
    CGEmitter(EmitterDef* emitterDef, XenonEffectDef* eDef);
    void SpawnParticles(float dt, float intensity, bool isContrail);
};

class NGEffect
{
public:
    NGEffect(XenonEffectDef* eDef, float dt);
    EffectDef* mEffectDef;
};

// external interfaces to work with the particle system
extern void AddXenonEffect(
    bool isContrail,
    EffectDef* def,
    tMatrix4* mat,
    tVector3* vel);

extern void UpdateXenonEmitters(float dt);

#endif // XSPARK_HPP
