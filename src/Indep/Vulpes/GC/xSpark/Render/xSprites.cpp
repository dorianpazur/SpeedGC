
#include "xSprites.hpp"

XSpriteManager NGSpriteManager;

void XSpriteManager::Init()
{
    sparkList.Init(MAX_NGPARTICLES);
    sparkList.mTexture = vTextureCache::GetTexture(CTStringHash("particles"));
}

void XSpriteManager::Reset()
{
    sparkList.Reset();
}

void XSpriteManager::Flip()
{
    sparkList.mNumViews = 0;
}

void XSpriteManager::AddSpark(NGParticle* particleList, unsigned int numParticles)
{
    sparkList.Lock();

    bBatching = true;

    for (size_t i = 0; i < numParticles; i++)
    {
        NGParticle* particle = &particleList[i];

        tVector3 startPos;
        tVector3 endPos;
        float endAge;
        float width = particle->width / 2048.0f;

        if (i < sparkList.mSprintListView[sparkList.mCurrViewBuffer].mMaxSprites)
        {
            XSpark* spark = &sparkList.mSprintListView[sparkList.mCurrViewBuffer].mLockedVB[i];

            sparkList.mSprintListView[sparkList.mCurrViewBuffer].mNumPolys = i;

            if (spark)
            {
                uint32_t color = particle->color;

                startPos = (particle->vel * particle->age) + particle->initialPos;
				startPos.y += particle->age * particle->age * particle->gravity;

                endAge = (particle->length / 2048.0f) + particle->age;
				
				endPos = (particle->vel * endAge) + particle->initialPos;
				endPos.y += endAge * endAge * particle->gravity;

                // fade out particles on death
                uint8_t alpha = (color & 0xFF);
                alpha = (uint8_t)(alpha * (1 - std::powf(particle->age / (particle->life / 8191.0f), 3.0f)));
                color = (color & 0xFFFFFF00) | (alpha);

                spark->v[0].position = startPos;
                spark->v[0].color = color;
                spark->v[0].texcoord[0] = particle->uv[0] / 255.0f;
                spark->v[0].texcoord[1] = particle->uv[1] / 255.0f;

                spark->v[1].position = startPos;
                spark->v[1].position.y += width;
                spark->v[1].color = color;
                spark->v[1].texcoord[0] = particle->uv[2] / 255.0f;
                spark->v[1].texcoord[1] = particle->uv[1] / 255.0f;

                spark->v[2].position = endPos;
                spark->v[2].position.y += width;
                spark->v[2].color = color;
                spark->v[2].texcoord[0] = particle->uv[2] / 255.0f;
                spark->v[2].texcoord[1] = particle->uv[3] / 255.0f;

                spark->v[3].position = endPos;
                spark->v[3].color = color;
                spark->v[3].texcoord[0] = particle->uv[0] / 255.0f;
                spark->v[3].texcoord[1] = particle->uv[3] / 255.0f;
            }
        }
    }

    if (bBatching && sparkList.mSprintListView[sparkList.mCurrViewBuffer].mpVB)
        bBatching = false;

    sparkList.Unlock();
}

void XSpriteManager::RenderAll(vView* view)
{
    sparkList.Draw(view->ID, 0, *vEffects[VEFFECT_PARTICLES], NULL);
}
