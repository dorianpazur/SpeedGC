#pragma once

#include <btBulletDynamicsCommon.h>

class Vehicle {
public:
    Vehicle(
        btDynamicsWorld* world,
        const btVector3& startPos
    );

    ~Vehicle();

    void update(float dt);
    void applyInput(
        float engineForce,
        float brakeForce,
        float steering
    );

    btRigidBody* getBody() const;
    btRaycastVehicle* getVehicle() const;

private:
    btRigidBody* mChassis;
    btRaycastVehicle* mVehicle;
    btVehicleRaycaster* mRaycaster;
};
