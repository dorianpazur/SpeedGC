////////////////////////////
//	
//	Vectors and matrices
//	

#ifndef TWARE_MATH_H
#define TWARE_MATH_H

#include <EABase/eabase.h>
#include <cstring>
#include <cstdio>

#ifdef EA_PLATFORM_GAMECUBE
#include <gccore.h>
#endif

struct tVector2
{
	float x;
	float y;
	
	// constructors
	tVector2() {};
	tVector2(float x, float y) { this->x = x; this->y = y; };
	tVector2(const tVector2 &v) { memcpy(this, &v, sizeof(tVector2)); };
	
	// arithmetic
    float &operator[](size_t index)
	{
		return ((float*)this)[index];
	};

	tVector2 &operator=(const tVector2 &v)
	{
		this->x = v.x;
        this->y = v.y;
		return *this;
	};
	
	tVector2 operator-(const tVector2 &v) const
	{
        tVector2 newVec;
		newVec.x = this->x - v.x;
		newVec.y = this->y - v.y;
        return newVec;
	};

    tVector2 &operator-=(const tVector2 &v)
	{
		this->x -= v.x;
        this->y -= v.y;
		return *this;
	};
	
	tVector2 operator+(const tVector2 &v) const
	{
		tVector2 newVec;
		newVec.x = this->x + v.x;
		newVec.y = this->y + v.y;
        return newVec;
	};

    tVector2 &operator+=(const tVector2 &v)
	{
		this->x += v.x;
        this->y += v.y;
		return *this;
	};

    tVector2 operator*(const float scale) const
	{
		tVector2 newVec;
		newVec.x = this->x * scale;
		newVec.y = this->y * scale;
        return newVec;
	};

    tVector2 &operator*=(const float scale)
	{
		this->x *= scale;
        this->y *= scale;
		return *this;
	};

    tVector2 operator/(const float scale) const
	{
		tVector2 newVec;
		newVec.x = this->x / scale;
		newVec.y = this->y / scale;
        return newVec;
	};

    tVector2 &operator/=(const float scale)
	{
		this->x /= scale;
        this->y /= scale;
		return *this;
	};
};

struct tVector3
{
	float x;
	float y;
	float z;
	float pad;
	
	// constructors
	tVector3() {};
	tVector3(float x, float y, float z) { this->x = x; this->y = y; this->z = z; };
	tVector3(const tVector3 &v) { memcpy(this, &v, sizeof(tVector3)); };
	
	// arithmetic
    float &operator[](size_t index)
	{
		return ((float*)this)[index];
	};

	tVector3 &operator=(const tVector3 &v)
	{
		this->x = v.x;
        this->y = v.y;
        this->z = v.z;
		return *this;
	};
	
	tVector3 operator-(const tVector3 &v) const
	{
        tVector3 newVec;
		newVec.x = this->x - v.x;
		newVec.y = this->y - v.y;
		newVec.z = this->z - v.z;
        return newVec;
	};

    tVector3 &operator-=(const tVector3 &v)
	{
		this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
		return *this;
	};
	
	tVector3 operator+(const tVector3 &v) const
	{
		tVector3 newVec;
		newVec.x = this->x + v.x;
		newVec.y = this->y + v.y;
		newVec.z = this->z + v.z;
        return newVec;
	};

    tVector3 &operator+=(const tVector3 &v)
	{
		this->x += v.x;
        this->y += v.y;
        this->z += v.z;
		return *this;
	};

    tVector3 operator*(const float scale) const
	{
		tVector3 newVec;
		newVec.x = this->x * scale;
		newVec.y = this->y * scale;
		newVec.z = this->z * scale;
        return newVec;
	};

    tVector3 &operator*=(const float scale)
	{
		this->x *= scale;
        this->y *= scale;
        this->z *= scale;
		return *this;
	};

    tVector3 operator/(const float scale) const
	{
		tVector3 newVec;
		newVec.x = this->x / scale;
		newVec.y = this->y / scale;
		newVec.z = this->z / scale;
        return newVec;
	};

    tVector3 &operator/=(const float scale)
	{
		this->x /= scale;
        this->y /= scale;
        this->z /= scale;
		return *this;
	};
};

struct tVector4
{
	float x;
	float y;
	float z;
	float w;
	
	// constructors
	tVector4() {};
	tVector4(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; };
	tVector4(const tVector4 &v) { memcpy(this, &v, sizeof(tVector4)); };
	
	// arithmetic
    float &operator[](size_t index)
	{
		return ((float*)this)[index];
	};

	tVector4 &operator=(const tVector4 &v)
	{
		this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        this->w = v.w;
		return *this;
	};
	
	tVector4 operator-(const tVector4 &v) const
	{
        tVector4 newVec;
		newVec.x = this->x - v.x;
		newVec.y = this->y - v.y;
		newVec.z = this->z - v.z;
		newVec.w = this->w - v.w;
        return newVec;
	};

    tVector4 &operator-=(const tVector4 &v)
	{
		this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        this->w -= v.w;
		return *this;
	};
	
	tVector4 operator+(const tVector4 &v) const
	{
		tVector4 newVec;
		newVec.x = this->x + v.x;
		newVec.y = this->y + v.y;
		newVec.z = this->z + v.z;
		newVec.w = this->w + v.w;
        return newVec;
	};

    tVector4 &operator+=(const tVector4 &v)
	{
		this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        this->w += v.w;
		return *this;
	};

    tVector4 operator*(const float scale) const
	{
		tVector4 newVec;
		newVec.x = this->x * scale;
		newVec.y = this->y * scale;
		newVec.z = this->z * scale;
		newVec.w = this->w * scale;
        return newVec;
	};

    tVector4 &operator*=(const float scale)
	{
		this->x *= scale;
        this->y *= scale;
        this->z *= scale;
        this->w *= scale;
		return *this;
	};

    tVector4 operator/(const float scale) const
	{
		tVector4 newVec;
		newVec.x = this->x / scale;
		newVec.y = this->y / scale;
		newVec.z = this->z / scale;
		newVec.w = this->w / scale;
        return newVec;
	};

    tVector4 &operator/=(const float scale)
	{
		this->x /= scale;
        this->y /= scale;
        this->z /= scale;
        this->w /= scale;
		return *this;
	};
};

struct tMatrix4
{
    tVector4 v0 {1, 0, 0, 0};
    tVector4 v1 {0, 1, 0, 0};
    tVector4 v2 {0, 0, 1, 0};
    tVector4 v3 {0, 0, 0, 1};

    tMatrix4() = default;
    tMatrix4(const tMatrix4 &mat) { v0 = mat.v0; v1 = mat.v1; v2 = mat.v2; v3 = mat.v3; };
	
	tMatrix4 &operator=(tMatrix4 &m)
	{
		this->v0 = m.v0;
        this->v1 = m.v1;
        this->v2 = m.v2;
        this->v3 = m.v3;
		return *this;
	}
	
    tVector4 &operator[](size_t index)
	{
		return ((tVector4*)this)[index];
	};

    const tVector4 &operator[](size_t index) const
	{
		return ((tVector4*)this)[index];
	};
};

extern void tIdentity(tMatrix4 *mat);
extern void tMulMatrix(tMatrix4 *dest, tMatrix4 *a, tMatrix4 *b);
extern void tMulVector(tVector4 *dest, tMatrix4 *m, tVector4 *v);
extern void tMulVector(tVector3 *dest, tMatrix4 *m, tVector3 *v);
extern void tInvertMatrix(tMatrix4 *dest, tMatrix4 *src);
extern void tTransposeMatrix(tMatrix4 *dest, tMatrix4 *src);
extern void tCreateLookAtMatrix(tMatrix4 *dest, tVector3 &eye, tVector3 &center, tVector3 &up);
extern unsigned int tRandom(int range, unsigned int *seed);
float tRandom(float range, unsigned int *seed);
unsigned int tRandom(int range);
float tRandom(float range);
#define tMoveTowards(cur, target, maxDelta) (cur + std::fmaxf(-(maxDelta), std::fminf(maxDelta, target - cur)))

#endif
