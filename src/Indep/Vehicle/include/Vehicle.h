#pragma once
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>

class Vehicle
{
public:
    Vehicle(btDynamicsWorld* world, const btVector3& startPos);

    void Update(float engineForce, float brakeForce, float steering, float timestep);
    btRigidBody* getBody() { return body; }

private:
    btRigidBody* body;
	
	btRaycastVehicle::btVehicleTuning mTuning;
	btVehicleRaycaster* mRaycastVehicleRaycaster;
	btRaycastVehicle* mRaycastVehicle;
	
	float mThrottleInput = 0.0f;
	float mSteeringInput = 0.0f;
	float mBrakeInput = 0.0f;
};
