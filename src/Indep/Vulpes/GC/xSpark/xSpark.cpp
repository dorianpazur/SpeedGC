
#include "xSpark.hpp"
#include "Render/xSprites.hpp"

XenonEffectList gNGEffectList;
ParticleList gParticleList;

float gElapsedSparkTime = 0;
float gElapsedContrailTime = 0;

unsigned int randomSeed = 0xDEADBEEF;

// xenon bounce physics

//void CalcCollisiontime(NGParticle* particle)
//{
//    UMath::Vector4 ray[2]{};
//
//    ray[1].x = -(particle->life * particle->vel.y + particle->initialPos.y);
//    ray[1].y = particle->life * particle->vel.z + particle->initialPos.z + particle->life * particle->life * particle->gravity;
//    ray[1].z = (particle->life * particle->vel.x) + particle->initialPos.x;
//    ray[1].w = 1.0f;
//
//    ray[0].x = -particle->initialPos.y;
//    ray[0].y = particle->initialPos.z;
//    ray[0].z = particle->initialPos.x;
//    ray[0].w = 1.0f;
//
//    WCollisionMgr::WorldCollisionInfo collisionInfo;
//    WCollisionMgr collisionMgr(0, 0);
//
//    if (collisionMgr.CheckHitWorld(ray, &collisionInfo, 3u))
//    {
//        float newLife = 0.0f;
//        float dist = ((particle->vel.z * particle->vel.z) - (((particle->initialPos.z - collisionInfo.fCollidePt.y) * particle->gravity) * 4.0f));
//        if (dist > 0.0f)
//        {
//            dist = sqrt(dist);
//            newLife = ((dist - particle->vel.z) / (particle->gravity * 2.0f));
//            if (newLife < 0.0f)
//                newLife = ((-particle->vel.z - dist) / (particle->gravity * 2.0f));
//        }
//        particle->life = (uint16_t)(newLife * 8191.0f);
//        particle->flags |= NGParticle::Flags::SPAWN;
//        particle->impactNormal.x = collisionInfo.fNormal.z;
//        particle->impactNormal.y = -collisionInfo.fNormal.x;
//        particle->impactNormal.z = collisionInfo.fNormal.y;
//    }
//}
//
//void BounceParticle(NGParticle* particle)
//{
//    float life;
//    float gravity;
//    float gravityOverTime;
//    float velocityMagnitude;
//    UMath::Vector3 newVelocity = particle->vel;
//
//    life = particle->life / 8191.0f;
//    gravity = particle->gravity;
//
//    gravityOverTime = particle->gravity * life;
//
//    particle->initialPos += newVelocity * life;
//    particle->initialPos.z += gravityOverTime * life;
//
//    newVelocity.z += gravityOverTime;
//
//    velocityMagnitude = UMath::Length(newVelocity);
//
//    newVelocity = UMath::Normalize(newVelocity);
//    
//    particle->life = 8191;
//    particle->age = 0.0f;
//    particle->flags = NGParticle::Flags::BOUNCED;
//
//    float bounceCos = UMath::Dot(newVelocity, particle->impactNormal);
//
//    particle->vel = (newVelocity - (particle->impactNormal * bounceCos * 2.0f)) * velocityMagnitude;
//}

// ParticleList

void ParticleList::AgeParticles(float dt)
{
    size_t aliveCount = 0;

    for (size_t i = 0; i < mNumParticles; i++)
    {
        NGParticle& particle = mParticles[i];
        if ((particle.age + dt) <= particle.life / 8191.0f)
        {
            mParticles[aliveCount] = mParticles[i];
            mParticles[aliveCount].age += dt;
            aliveCount++;
        }
        //else if (particle.flags & NGParticle::Flags::SPAWN)
        //{
        //    BounceParticle(&particle);
        //    aliveCount++;
        //}
    }
    mNumParticles = aliveCount;
}

void ParticleList::GeneratePolys()
{
    if (mNumParticles)
        NGSpriteManager.AddSpark(mParticles, mNumParticles);
}

NGParticle* ParticleList::GetNextParticle()
{
    if (mNumParticles >= MAX_NGPARTICLES)
        return NULL;
    return &mParticles[mNumParticles++];
}

// CGEmitter

CGEmitter::CGEmitter(EmitterDef* emitterDef, XenonEffectDef* eDef) :
    mEmitterDef(emitterDef),
    mTextureUVs(mEmitterDef->emitteruv)
{
    mLocalWorld = eDef->mat;
    mVel = eDef->vel;
};

void CGEmitter::SpawnParticles(float dt, float intensity, bool isContrail)
{
    if (intensity <= 0.0f)
        return;

    // Local variables
    struct tMatrix4 local_world;
    struct tMatrix4 local_orientation;
    unsigned int random_seed = randomSeed;
    float life_variance = mEmitterDef->Life * mEmitterDef->LifeVariance;
    float life = mEmitterDef->Life - life_variance;
    int r = (int)(mEmitterDef->Colour1.x * 255.0f);
    int g = (int)(mEmitterDef->Colour1.y * 255.0f);
    int b = (int)(mEmitterDef->Colour1.z * 255.0f);
    int a = (int)(mEmitterDef->Colour1.w * 255.0f);
	
	if (intensity != 1.0f)
	{
		a = (int)std::fminf(42.0f, intensity * 42.0f);
	}
	
	intensity = std::fmaxf(1.0f, intensity);
    // end next gen code

    unsigned int particleColor = (a << 24) | (r << 16) | (g << 8) | b;
    float num_particles_variance = (intensity * mEmitterDef->NumParticles) * mEmitterDef->NumParticlesVariance * 100.0f;
    float num_particles = (intensity * mEmitterDef->NumParticles) - num_particles_variance;
    float particle_age_factor = dt / num_particles;
    float current_particle_age = 0.0f;

    local_world = this->mLocalWorld;
    local_orientation = this->mLocalWorld;
	
	// we only want orientation here, no transform
	local_orientation[0][3] = 0.0f;
	local_orientation[1][3] = 0.0f;
	local_orientation[2][3] = 0.0f;

    while (num_particles > 0.0f)
    {
        NGParticle* particle;
        float sparkLength;
        struct tVector3 pvel;
        struct tVector3 rand;
        struct tVector3 rotatedVel;
        float gravity;
        struct tVector3 ppos;

        num_particles--;

        if (!(particle = gParticleList.GetNextParticle())) // get next particle in list
            break;

        sparkLength = mEmitterDef->LengthStart + tRandom(mEmitterDef->LengthDelta, &random_seed);

        if (sparkLength < 0.0f)
            break;
        else if (sparkLength >= 255.0f)
            sparkLength = 255.0f;

        rand.x = 1.0f - (mEmitterDef->VelocityDelta.x - (2 * tRandom(mEmitterDef->VelocityDelta.x, &random_seed)));
        rand.y = 1.0f - (mEmitterDef->VelocityDelta.y - (2 * tRandom(mEmitterDef->VelocityDelta.y, &random_seed)));
        rand.z = 1.0f - (mEmitterDef->VelocityDelta.z - (2 * tRandom(mEmitterDef->VelocityDelta.z, &random_seed)));

        pvel = mEmitterDef->VelocityInherit;
		
		pvel.x *= mVel.x;
        pvel.y *= mVel.y;
        pvel.z *= mVel.z;
        tMulVector(&rotatedVel, &local_orientation, &mEmitterDef->VelocityStart);
        pvel += rotatedVel;
        pvel.x *= rand.x;
        pvel.y *= rand.y;
        pvel.z *= rand.z;

        particle->vel.x = pvel.x;
        particle->vel.y = pvel.y;
        particle->vel.z = pvel.z;

        rand.x = tRandom(mEmitterDef->VolumeExtent.x, &random_seed);
        rand.y = tRandom(mEmitterDef->VolumeExtent.y, &random_seed);
        rand.z = tRandom(mEmitterDef->VolumeExtent.z, &random_seed);

        ppos.x = rand.x - mEmitterDef->VolumeExtent.x * 0.5f + mEmitterDef->VolumeCenter.x;
        ppos.y = rand.y - mEmitterDef->VolumeExtent.y * 0.5f + mEmitterDef->VolumeCenter.y;
        ppos.z = rand.z - mEmitterDef->VolumeExtent.z * 0.5f + mEmitterDef->VolumeCenter.z;
		
		tMulVector(&ppos, &local_world, &ppos);
		particle->initialPos = (pvel * current_particle_age) + ppos;
		
        particle->age = current_particle_age;

        gravity = (tRandom(mEmitterDef->GravityDelta, &random_seed) * 2) + mEmitterDef->GravityStart - mEmitterDef->GravityDelta;
        particle->gravity = gravity;

        particle->initialPos.y += gravity * current_particle_age * current_particle_age; // fall

        particle->life = (uint16_t)(life * 8191);
        particle->width = (uint8_t)mEmitterDef->HeightStart;
        particle->length = (uint8_t)sparkLength;

        particle->color = particleColor;

        //particle->uv[0] = (uint8_t)(mTextureUVs->StartU * 255.0f);
        //particle->uv[1] = (uint8_t)(mTextureUVs->StartV * 255.0f);
        //particle->uv[2] = (uint8_t)(mTextureUVs->EndU * 255.0f);
        //particle->uv[3] = (uint8_t)(mTextureUVs->EndV * 255.0f);

        current_particle_age += particle_age_factor;

        // begin next gen code
        //particle->flags = (!mEmitterDef->zSprite ? NGParticle::Flags::DEBRIS : NULL);
        //particle->spin = mEmitterDef->Spin;
        //if ((particle->flags & NGParticle::Flags::BOUNCED) == 0 && !isContrail && bBounceParticles)
        //{
        //    CalcCollisiontime(particle);
        //}
        // end next gen code
    }

    randomSeed = random_seed;
}

// NGEffect

NGEffect::NGEffect(XenonEffectDef* eDef, float dt) :
    mEffectDef(eDef->def)
{
    for (uint32_t i = 0; i < mEffectDef->mNumEmitters; i++)
    {
        float intensity = 1.0f;

        if (eDef->isContrail)
        {
			const float kMaxIntensity = 0.75f;
			const float kMinIntensity = 0.75f;
			float vellength = std::sqrtf((eDef->vel.x * eDef->vel.x) + (eDef->vel.y * eDef->vel.y) + (eDef->vel.z * eDef->vel.z));
            float carspeed = (vellength - 44.0f) / 30.0f;
            intensity = std::lerp(kMinIntensity, kMaxIntensity, carspeed);
            if (intensity > kMaxIntensity)
                intensity = kMaxIntensity;
            else if (intensity < kMinIntensity)
                intensity = kMinIntensity;
        }
		
        CGEmitter anEmitter{ mEffectDef->mEmitters[i], eDef };
        anEmitter.SpawnParticles(dt, intensity, eDef->isContrail);
    }
}

// external interface

void AddXenonEffect(
    bool isContrail,
    EffectDef* def,
    tMatrix4* mat,
    tVector3* vel)
{
    XenonEffectDef newEffect;
	
    if (gNGEffectList.size() < gNGEffectList.capacity())
    {
		newEffect.mat[0][3] = (*mat)[0][3];
		newEffect.mat[1][3] = (*mat)[1][3];
		newEffect.mat[2][3] = (*mat)[2][3];
		
        newEffect.def = def;
        newEffect.vel = *vel;
        newEffect.isContrail = isContrail;
        gNGEffectList.push_back(newEffect);
    }
}

void UpdateXenonEmitters(float dt)
{
    gParticleList.AgeParticles(dt);

    // spawn emitters from all emitterdefs
    for (size_t i = 0; i < gNGEffectList.size(); i++)
    {
        XenonEffectDef& effectDef = gNGEffectList[i];
        NGEffect effect{ &effectDef, dt }; // create NGEffect from effect def
    }

    // clear list of emitterdefs
    gNGEffectList.clear();

    // generate mesh for rendering
    gParticleList.GeneratePolys();
}
