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
	// 'viewMtx' is the current camera view matrix (TODO: replace this with tView when it's done)
	virtual void Render(tMatrix4* viewMtx);

	virtual ~ISimable() = 0;
};

#endif
