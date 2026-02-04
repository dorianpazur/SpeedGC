#pragma once
#include <btBulletDynamicsCommon.h>

class Vehicle
{
public:
    Vehicle(btDynamicsWorld* world, const btVector3& startPos);

    void applyInput(float engineForce, float brakeForce, float steering);
    btRigidBody* getBody() { return body; }

private:
    btRigidBody* body;
};
