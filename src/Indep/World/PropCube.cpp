
#include "PropCube.h"
#include <Vulpes/model.h>

//All PropCube instances share one model, it is loaded once in main
extern vModel* gCubeModel;

PropCube::PropCube(btDynamicsWorld* world, const btVector3& pos, const btVector3& halfExtents)
{
    mWorld = world;

    //the physics collision box with the given size
    mShape = new btBoxShape(halfExtents);

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(pos);

    // static prop - mass = 0
    mMotionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, mMotionState, mShape, btVector3(0, 0, 0));
    rbInfo.m_restitution = 0.3f;
    rbInfo.m_friction = 0.8f;

    //Links the Bullet physics body to this PropCube object 
    // used in World.cpp to identify which ISimable owns this body
    mBody = new btRigidBody(rbInfo);
    mBody->setUserPointer(this);

    mWorld->addRigidBody(mBody);
}

PropCube::~PropCube()
{
    if (mBody)
    {
        mWorld->removeRigidBody(mBody);
        delete mBody;
        mBody = NULL;
    }
    if (mMotionState)
    {
        delete mMotionState;
        mMotionState = NULL;
    }
    if (mShape)
    {
        delete mShape;
        mShape = NULL;
    }
}

//empty for now, could be used later to add effects(sound, score...)
void PropCube::OnCollide(ISimable* other, const tVector3 &contactPoint)
{
}

void PropCube::Render(vView* view)
{
    if (!gCubeModel || !view || !mBody)
        return;

    tMatrix4 transform;
    btScalar transformFlt[16];
    btTransform trans;

    if (mBody->getMotionState())
        mBody->getMotionState()->getWorldTransform(trans);
    else
        trans = mBody->getWorldTransform();

    trans.getOpenGLMatrix(transformFlt);

    // Scale each rotation column by the box half-extents so the unit cube
    // mesh matches the Bullet btBoxShape exactly
    //btVector3 halfExtents = ((btBoxShape*)mShape)->getHalfExtentsWithoutMargin();
    const float sx = 1.0f;//halfExtents.getX();
    const float sy = 1.0f;//halfExtents.getY();
    const float sz = 1.0f;//halfExtents.getZ();


    // the GameCube GPU(GX) doesn't understand Bullet's matrix format
    // Bullet (OpenGL column-major) to GX row-major, scaled by half-extents
    transform[0][0] = transformFlt[0] * sx;  // col0.x to row0.x
    transform[1][0] = transformFlt[1] * sx;  // col0.y to row1.x
    transform[2][0] = transformFlt[2] * sx;

    transform[0][1] = transformFlt[4] * sy;
    transform[1][1] = transformFlt[5] * sy;
    transform[2][1] = transformFlt[6] * sy;

    transform[0][2] = transformFlt[8] * sz;
    transform[1][2] = transformFlt[9] * sz;
    transform[2][2] = transformFlt[10] * sz;

    transform[0][3] = transformFlt[12];
    transform[1][3] = transformFlt[13] - 2.0f;
    transform[2][3] = transformFlt[14];

    gCubeModel->Render(view, &transform);
}
