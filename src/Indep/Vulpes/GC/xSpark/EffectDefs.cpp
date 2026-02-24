
#include "EffectDefs.h"

// for reference
//struct EmitterDef
//{
//	tVector3 VolumeCenter;
//    tVector3 VelocityDelta;
//    tVector3 VolumeExtent;
//    tVector3 VelocityInherit;
//    tVector3 VelocityStart;
//    tVector4 Colour1;
//    EmitterUV* emitteruv;
//    float Life;
//    float NumParticlesVariance;
//    float GravityStart;
//    float HeightStart;
//    float GravityDelta;
//    float LengthStart;
//    float LengthDelta;
//    float LifeVariance;
//    float NumParticles;
//    int16_t Spin;
//    int8_t zSprite;
//    int8_t zContrail;
//};
//
//struct EmitterUV
//{
//    float StartU;
//    float EndU;
//    float StartV;
//	  float EndV;
//};

EmitterUV emsprk_uv
{
	0.0f, // StartU
	0.5f, // EndU
	0.0f, // StartV
	1.0f  // EndV
};

EmitterUV trail_uv
{
	0.5f, // StartU
	1.0f, // EndU
	0.0f, // StartV
	1.0f  // EndV
};

EmitterDef emsprk_line1
{
	/*VolumeCenter*/
	{
      0.0f,
      0.0f,
      0.0f
	},
    /*VelocityDelta*/
	{
      0.4f,
      2.0f,
      0.4f
	},
    /*VolumeExtent*/
	{
      0.0f,
      0.0f,
      0.5f
	},
    /*VelocityInherit*/
	{
      -1.0f,
      -0.1f,
      -1.0f,
	},
    /*VelocityStart*/
	{
      0.0f,
      1.0f,
      0.0f
	},
    /*Colour1*/ {
      1.0f,
      0.86f,
      0.75f,
      1.0f
	},
    /*emitteruv*/ &emsprk_uv,
    /*Life*/ 1,
    /*NumParticlesVariance*/ 0,
    /*GravityStart*/ -12,
    /*HeightStart*/ 62,
    /*GravityDelta*/ 9,
    /*LengthStart*/ 255,
    /*LengthDelta*/ 140,
    /*LifeVariance*/ 0.05f,
    /*NumParticles*/ 5,
    /*Spin*/ 0,
    /*zSprite*/ 0,
    /*zContrail*/ 0
};

EmitterDef emsprk_line2
{
	/*VolumeCenter*/
	{
      0.0f,
      0.0f,
      0.0f
	},
    /*VelocityDelta*/
	{
      2.0f,
      2.0f,
      2.0f
	},
    /*VolumeExtent*/
	{
      0.05f,
      0.25f,
      0.8f
	},
    /*VelocityInherit*/
	{
      -0.2f,
       0.0f,
      -0.2f,
	},
    /*VelocityStart*/
	{
      0.1f,
      1.0f,
      0.1f
	},
    /*Colour1*/ {
      1.0f,
      0.56f,
      0.28f,
      1.0f
	},
    /*emitteruv*/ &emsprk_uv,
    /*Life*/ 0.4f,
    /*NumParticlesVariance*/ 0,
    /*GravityStart*/ -10,
    /*HeightStart*/ 34,
    /*GravityDelta*/ 2,
    /*LengthStart*/ 37,
    /*LengthDelta*/ 33,
    /*LifeVariance*/ 0.1f,
    /*NumParticles*/ 3,
    /*Spin*/ 0,
    /*zSprite*/ 0,
    /*zContrail*/ 0
};

EffectDef fxsprk_line
{
	2, // number of them
	{
		&emsprk_line1,
		&emsprk_line2,
	}
};

EmitterDef trail3
{
	/*VolumeCenter*/
	{
      0.0f,
      0.0f,
      0.0f
	},
    /*VelocityDelta*/
	{
      0.0f,
      0.0f,
      0.0f
	},
    /*VolumeExtent*/
	{
      1.5f,
      0.75f,
      2.0f
	},
    /*VelocityInherit*/
	{
      -0.3f,
      -0.1f,
      -0.3f,
	},
    /*VelocityStart*/
	{
       0.0f,
      -1.0f,
       0.0f
	},
    /*Colour1*/ {
      0.64f,
      1.0f,
      0.85f,
      0.12f
	},
    /*emitteruv*/ &trail_uv,
    /*Life*/ 0.25f,
    /*NumParticlesVariance*/ 0,
    /*GravityStart*/ -2,
    /*HeightStart*/ 255,
    /*GravityDelta*/ 0,
    /*LengthStart*/ 200,
    /*LengthDelta*/ 50,
    /*LifeVariance*/ 0.25f,
    /*NumParticles*/ 15,
    /*Spin*/ 0,
    /*zSprite*/ 0,
    /*zContrail*/ 1
};

EffectDef contrail
{
	1, // number of them
	{
		&trail3,
	}
};
