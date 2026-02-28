#ifndef VEHICLE_VEHICLE_H
#define VEHICLE_VEHICLE_H

#include <tWare/Align.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>
#include <Vulpes/View.h>

#include "ISimable.h"

class ALIGN(32) Vehicle : public ISimable
{
public:
	DEF_TWARE_NEW_OVERRIDE(Vehicle, MAIN_POOL)
    Vehicle(btDynamicsWorld* world, const btVector3& startPos);
	~Vehicle();
    void Update(float throttle, float brake, float steering, float timestep);
	virtual void OnCollide(ISimable* other, const tVector3 &contactPoint);
	virtual void AddFuel(float amount) override;

	virtual void Render(vView* view) override;

private:
	btRaycastVehicle::btVehicleTuning mTuning;
	btVehicleRaycaster* mRaycastVehicleRaycaster;
	btRaycastVehicle* mRaycastVehicle;
	btDefaultMotionState mMotionState;
	tVector3 mVelocity;
	
	float mThrottleInput = 0.0f;
	float mSteeringInput = 0.0f;
	float mBrakeInput = 0.0f;
	
	bool mCollidedThisFrame = false;
	bool mIsReversing = false;

	float mFuel = 0.0f;
	static constexpr float kMaxFuel = 100.0f;
	static constexpr float kFuelConsumptionPerSecond = 10.0f;

};

#endif
