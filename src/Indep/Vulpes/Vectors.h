#ifndef VULPES_VECTORS_H
#define VULPES_VECTORS_H

#include <tWare/Align.h>
#include <cstdint>
#include <cstddef>

struct vGlTFVector3 {
	float x;
	float y;
	float z;
};

struct vVector2 {
	float x;
	float y;
};

struct vVector3 {
	float x;
	float y;
	float z;
private:
	float pad;
};

struct vVector4 {
	float x;
	float y;
	float z;
	float w;
};

struct vColor {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct vColorShort {
	uint16_t r;
	uint16_t g;
	uint16_t b;
	uint16_t a;
};

#endif
