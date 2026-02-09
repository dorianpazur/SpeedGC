#ifndef VEHICLE_VEHICLE_H
#define VEHICLE_VEHICLE_H

#include <tWare/Align.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>

#include "ISimable.h"

class ALIGN(32) Vehicle : public ISimable
{
public:
    Vehicle(btDynamicsWorld* world, const btVector3& startPos);
	~Vehicle();
    void Update(float throttle, float brake, float steering, float timestep);
	virtual void OnCollide(ISimable* other);

private:
	btRaycastVehicle::btVehicleTuning mTuning;
	btVehicleRaycaster* mRaycastVehicleRaycaster;
	btRaycastVehicle* mRaycastVehicle;
	
	float mThrottleInput = 0.0f;
	float mSteeringInput = 0.0f;
	float mBrakeInput = 0.0f;
};

#endif
