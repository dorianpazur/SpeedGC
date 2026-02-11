#ifndef WORLD_ISIMABLE_H
#define WORLD_ISIMABLE_H

#include <tWare/Align.h>
#include <tWare/Memory.h>
#include <btBulletDynamicsCommon.h>

class ISimable
{
public:
    btRigidBody* mBody = NULL;
	btDynamicsWorld* mWorld = NULL;
	
	DEF_TWARE_NEW_OVERRIDE(ISimable)
	
	virtual void OnCollide(ISimable* other);
	
	virtual ~ISimable() = 0;
};

#endif
