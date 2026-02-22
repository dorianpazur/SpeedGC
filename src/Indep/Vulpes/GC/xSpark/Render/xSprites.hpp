#ifndef XSPRITES_HPP
#define XSPRITES_HPP

#include <EABase/eabase.h>

#if EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../xSpark.hpp"
#include <tWare/Math.h>
#include <gccore.h>
#include <cmath>
#include <vector>
#include <Vulpes/vulpes.h>

struct XSparkVert
{
    tVector3 position;
    unsigned int color;
    float texcoord[2];
};

struct XSpark
{
    XSparkVert v[4];
};

template <typename T, typename U>
struct SpriteBuffer
{
    unsigned int mVertexCount = 0;
    unsigned int mMaxSprites = 0;
    unsigned int mNumPolys = 0;

    bool mbLocked = false;
    T* mLockedVB = NULL;
    U* mpVB = NULL;
    uint16_t* mpIB = NULL;

    void Draw(vEffect& effect, vTextureCache::CachedTexture* pTexture)
    {	
        GX_SetCullMode(GX_CULL_NONE);
		
		// tells gx where our position and color data is
		// args: type of data, pointer, array stride
		GX_SetArray(GX_VA_POS, &mpVB[0].position, sizeof(U));
		GX_SetArray(GX_VA_CLR0, &mpVB[0].color, sizeof(U));
		GX_SetArray(GX_VA_TEX0, &mpVB[0].texcoord, sizeof(U));
		
		// setup the vertex descriptor
		// tells the flipper to expect 16bit indexes for position
		// and color data. could also be set to direct.
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
		GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX16);
		GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);
		
		// setup the vertex attribute table
		// describes the data
		// args: vat location 0-7, type of data, data format, size, scale
		// so for ex. in the first call we are sending position data with
		// 3 values X,Y,Z of size S16. scale sets the number of fractional
		// bits for non float data.
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
		
		GX_SetNumChans(1);
		
		GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL,
					GX_DF_NONE, GX_AF_NONE);
		
		vEffectStaticState::pCurrentEffect = &effect;
		
		//vEffectStaticState::pCurrentEffect->SetTexture(vTextureCache::GetTexture(mSolids[solid].mMeshes[mesh].mTextures.DiffuseMap));
		
		vEffectStaticState::pCurrentEffect->Start();
		
		// have to step through index buffer manually
		GX_Begin(GX_QUADS, GX_VTXFMT0, mNumPolys * 4);
		
		for (size_t i = 0; i < mNumPolys * 4; i++)
		{
			uint16_t index = mpIB[i];
			GX_Position1x16(index);
			GX_Color1x16(index);
			GX_TexCoord1x16(index);
		}
		
		GX_End();
		vEffectStaticState::pCurrentEffect->End();
    }

    void Init(uint32_t spriteCount)
    {
        mNumPolys = 0;
        mVertexCount = 4 * spriteCount;
        mMaxSprites = spriteCount;
		
		mpVB = (U*)tWareMalloc(sizeof(T) * mVertexCount, "xSprites VB", __LINE__, ALLOC_PARAMS(MAIN_POOL, 32));
		mpIB = (uint16_t*)tWareMalloc(sizeof(uint16_t) * mVertexCount, "xSprites IB", __LINE__, ALLOC_PARAMS(MAIN_POOL, 32));
		
        if (mpIB)
        {
			printf("SpriteBuffer Init!\n");
            for (uint32_t i = 0; i < mVertexCount; i++) // vertices will be in the correct order, will be drawing quads
			{
				mpIB[i] = i;
			}
        }
		else
			printf("SpriteBuffer Init failed!\n");
    }

    void Reset()
    {
        if (mpVB)
		{
            tFree(mpVB);
			mpVB = NULL;
		}
        if (mpIB)
		{
            tFree(mpIB);
			mpIB = NULL;
		}
    }

    void Lock()
    {
        if (mpVB)
        {
            mNumPolys = 0;
			mLockedVB = (T*)mpVB;
            mbLocked = true;
        }
    }

    void Unlock()
    {
        if (mbLocked && mLockedVB)
        {
			DCFlushRange(mLockedVB, mVertexCount * sizeof(U));
			mLockedVB = NULL;
            mbLocked = false;
        }
    }
};

template <typename Sprite, typename SpriteVert, size_t NumViews>
class XSpriteList
{
public:
    SpriteBuffer<Sprite, SpriteVert> mSprintListView[NumViews];
    size_t mNumViews = 0;
    size_t mCurrViewBuffer = 0;
    vTextureCache::CachedTexture* mTexture = NULL;

    void Draw(int viewId, int viewBuffer, vEffect &effect, vTextureCache::CachedTexture* pOverrideTexture)
    {
        (void)viewId; // not using this atm

        if (viewBuffer >= 0 && mSprintListView[viewBuffer].mNumPolys)
        {
            mSprintListView[viewBuffer].Draw(effect, pOverrideTexture ? pOverrideTexture : this->mTexture);
        }
    }

    void Init(const uint32_t spriteCount)
    {
        this->mTexture = NULL;

        for (size_t i = 0; i < NumViews; i++)
        {
            mSprintListView[i].Init(spriteCount);
        }
    }

    void Reset()
    {
        for (size_t i = 0; i < NumViews; i++)
        {
            mSprintListView[i].Reset();
        }
    }

    void Lock()
    {
        mCurrViewBuffer = mNumViews;
        //mNumViews = (mNumViews + 1) % NumViews;
        mSprintListView[mCurrViewBuffer].Lock();
    }

    void Unlock()
    {
        mSprintListView[mCurrViewBuffer].Unlock();
    }
};

class XSpriteManager
{
public:
    XSpriteList<XSpark, XSparkVert, 1> sparkList;
    bool bBatching = false;

    void RenderAll(vView* view);
    void AddSpark(NGParticle* particleList, unsigned int numParticles);
    void Init();
    void Reset();
    void Flip();
};

extern XSpriteManager NGSpriteManager;

#endif // XSPRITES_HPP
