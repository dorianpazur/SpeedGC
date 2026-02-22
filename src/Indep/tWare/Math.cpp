////////////////////////////
//	
//	Vectors and matrices
//	

#include <tWare/Math.h>
#include <limits.h>

void tIdentity(tMatrix4 *mat)
{
#ifdef EA_PLATFORM_GAMECUBE
	guMtx44Identity(*(Mtx44*)mat);
#endif
}

void tMulMatrix(tMatrix4 *dest, tMatrix4 *a, tMatrix4 *b)
{
#ifdef EA_PLATFORM_GAMECUBE
	guMtx44Concat(*(Mtx44*)a, *(Mtx44*)b, *(Mtx44*)dest);
#endif
}

void tMulVector(tVector4 *dest, tMatrix4 *m, tVector4 *v)
{
#ifdef EA_PLATFORM_GAMECUBE
	guVecMultiply(*(Mtx44*)m, (guVector*)v, (guVector*)dest);
#endif
}

void tMulVector(tVector3 *dest, tMatrix4 *m, tVector3 *v)
{
#ifdef EA_PLATFORM_GAMECUBE
	guVecMultiply(*(Mtx44*)m, (guVector*)v, (guVector*)dest);
#endif
}

void tInvertMatrix(tMatrix4 *dest, tMatrix4 *src)
{
#ifdef EA_PLATFORM_GAMECUBE
	guMtx44Inverse(*(Mtx44*)src, *(Mtx44*)dest);
#endif
}

void tTransposeMatrix(tMatrix4 *dest, tMatrix4 *src)
{
#ifdef EA_PLATFORM_GAMECUBE
	guMtxTranspose(*(Mtx44*)src, *(Mtx44*)dest);
#endif
}

void tCreateLookAtMatrix(tMatrix4 *dest, tVector3 &eye, tVector3 &center, tVector3 &up)
{
#ifdef EA_PLATFORM_GAMECUBE
	guLookAt(*(Mtx44*)dest, (guVector*)&eye, (guVector*)&up, (guVector*)&center);
#endif
}

// pseudo-RNG, based on bWare
// modifies seed value after every run
unsigned int tDefaultSeed = 0xF0C5F0C5; // fox fox :3

unsigned int tRandom(int range, unsigned int *seed) {
    if (range == 0) {
        return 0;
    }
    unsigned int result = *seed;
    unsigned int next = result ^ 0x1d872b41;
    unsigned int temp = next ^ (next >> 5);
    *seed = temp ^ (next ^ (temp << 0x1b));
    return result - (result / range) * range;
}

float tRandom(float range, unsigned int *seed) {
    return range * (1.0f/INT_MAX) * tRandom(INT_MAX, seed);
}

unsigned int tRandom(int range) {
    return tRandom(range, &tDefaultSeed);
}

float tRandom(float range) {
    return tRandom(range, &tDefaultSeed);
}
