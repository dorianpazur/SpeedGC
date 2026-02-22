
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
    /*emitteruv*/ NULL,
    /*Life*/ 1,
    /*NumParticlesVariance*/ 0,
    /*GravityStart*/ -12,
    /*HeightStart*/ 62,
    /*GravityDelta*/ 9,
    /*LengthStart*/ 255,
    /*LengthDelta*/ 140,
    /*LifeVariance*/ 0.05f,
    /*NumParticles*/ 30,
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
    /*emitteruv*/ NULL,
    /*Life*/ 0.4f,
    /*NumParticlesVariance*/ 0,
    /*GravityStart*/ -10,
    /*HeightStart*/ 34,
    /*GravityDelta*/ 2,
    /*LengthStart*/ 37,
    /*LengthDelta*/ 33,
    /*LifeVariance*/ 0.1f,
    /*NumParticles*/ 10,
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
