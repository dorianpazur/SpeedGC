#ifndef VULPES_VECTORS_H
#define VULPES_VECTORS_H

#include <tWare/Align.h>
#include <cstdint>
#include <cstddef>

struct vGlTFVector3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

struct vVector2 {
	float x = 0.0f;
	float y = 0.0f;
};

struct vVector3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
private:
	float pad;
};

struct vVector4 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;
};

struct vColor {
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 0;
};

struct vColorShort {
	uint16_t r = 0;
	uint16_t g = 0;
	uint16_t b = 0;
	uint16_t a = 0;
};

#endif
