#ifndef WORLD_PROPCUBE_H
#define WORLD_PROPCUBE_H

#include <tWare/Align.h>
#include <tWare/Memory.h>
#include <btBulletDynamicsCommon.h>
#include "ISimable.h"

class ALIGN(32) PropCube : public ISimable
{
public:
    DEF_TWARE_NEW_OVERRIDE(PropCube, MAIN_POOL)

        // halfExtents: half-size of the cube in each axis (e.g. btVector3(1,1,1) = 2x2x2 cube)
        PropCube(btDynamicsWorld * world, const btVector3 & pos, const btVector3 & halfExtents = btVector3(1.0f, 1.0f, 1.0f));
    ~PropCube();

    virtual void OnCollide(ISimable * other, const tVector3 &contactPoint) override;
    virtual void Render(vView * view) override;

private:
    btCollisionShape* mShape = NULL;
    btDefaultMotionState* mMotionState = NULL;
};

#endif
