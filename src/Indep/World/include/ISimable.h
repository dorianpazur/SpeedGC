#ifndef WORLD_ISIMABLE_H
#define WORLD_ISIMABLE_H

#include <tWare/Align.h>
#include <tWare/Math.h>
#include <tWare/Memory.h>
#include <btBulletDynamicsCommon.h>

class ISimable
{
public:
    btRigidBody* mBody = NULL;
	btDynamicsWorld* mWorld = NULL;
	
	DEF_TWARE_NEW_OVERRIDE(ISimable, MAIN_POOL)
	
	virtual void OnCollide(ISimable* other);
	
	// per object rendering by each ISimable subclass
	// 'viewMtx' is the current camera view matrix, 
	// 'worldTransform' is the world space transform 
	virtual void Render(tMatrix4* viewMtx, tMatrix4* worldTransform) = 0;

	virtual ~ISimable() = 0;
};

#endif
