#ifndef VULPES_POLY_H
#define VULPES_POLY_H

#include <tWare/hash.h>
#include <Vulpes/Vectors.h>

struct vPoly {
    struct vVector3 Vertices[4];
    float UVs[4][2]{ { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
    float UVsMask[4][2] = { { 1, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
    unsigned char Colours[4][4] = { { 0x80, 0x80, 0x80, 0x80 }, { 0x80, 0x80, 0x80, 0x80}, { 0x80, 0x80, 0x80, 0x80 }, { 0x80, 0x80, 0x80, 0x80} };
    unsigned char flags = 0;
    unsigned char Flailer = 0;
};

void vPolyFERender(vPoly* poly, struct vTextureCache::CachedTexture* texture);
void vPolyRender(vPoly* poly, struct vTextureCache::CachedTexture* texture);

#endif