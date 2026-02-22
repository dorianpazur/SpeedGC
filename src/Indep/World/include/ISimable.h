#ifndef WORLD_ISIMABLE_H
#define WORLD_ISIMABLE_H

#include <tWare/Align.h>
#include <tWare/Math.h>
#include <tWare/Memory.h>
#include <Vulpes/View.h>
#include <btBulletDynamicsCommon.h>

class ISimable
{
public:
    btRigidBody* mBody = NULL;
	btDynamicsWorld* mWorld = NULL;
	
	DEF_TWARE_NEW_OVERRIDE(ISimable, MAIN_POOL)
	
	virtual void OnCollide(ISimable* other, const tVector3 &contactPoint);
	
	// per object rendering by each ISimable subclass
	virtual void Render(vView* view);

	virtual ~ISimable() = 0;
};

#endif
