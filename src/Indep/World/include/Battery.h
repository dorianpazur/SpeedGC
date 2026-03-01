#ifndef WORLD_BATTERY_H
#define WORLD_BATTERY_H

#include <tWare/Align.h>
#include <tWare/Memory.h>
#include <btBulletDynamicsCommon.h>
#include "ISimable.h"

class ALIGN(32) Battery : public ISimable
{
public:
	DEF_TWARE_NEW_OVERRIDE(Battery, MAIN_POOL)

		Battery(btDynamicsWorld * world, const btVector3 & pos, float radius = 0.8f);
	~Battery();

	virtual void OnCollide(ISimable * other, const tVector3 & contactPoint) override;
	virtual void Render(vView * view) override;

	bool mCollected = false;

private:
	btCollisionShape* mShape = NULL;
	btDefaultMotionState* mMotionState = NULL;
	float mRadius;
};

#endif
