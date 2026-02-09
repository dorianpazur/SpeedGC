#ifndef WORLD_ISIMABLE_H
#define WORLD_ISIMABLE_H

#include <tWare/Align.h>
#include <btBulletDynamicsCommon.h>

class ISimable
{
public:
    btRigidBody* mBody = NULL;
	btDynamicsWorld* mWorld = NULL;
	
	virtual void OnCollide(ISimable* other);
	
	virtual ~ISimable() = 0;
};

#endif
