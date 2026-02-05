#include "Vehicle.h"
#include "ScreenPrintf.h"

const float speedPowerDecline = 0.1f;
const float enginePower = 7500.0f;
const float brakePower = 100.0f;
const float steeringClamp = 0.3f;
const float wheelRadius = 0.5f;
const float wheelWidth = 0.4f;
const float wheelFriction = 5.0f;  //BT_LARGE_FLOAT;
const float suspensionStiffness = 10.0f;
const float suspensionDamping = 2.3f;
const float suspensionCompression = 4.4f;
const float rollInfluence = 0.1f;
const btScalar suspensionRestLength(1.0);

const btVector3 wheelDirectionCS0(0, -1, 0);
const btVector3 wheelAxleCS(-1, 0, 0);

Vehicle::Vehicle(btDynamicsWorld* world, const btVector3& startPos)
{
    // Cube collision shape (car)
    btCollisionShape* shape = new btBoxShape(btVector3(1.0f, 0.5f, 2.0f)); // car-sized cube
	
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(startPos);
	
    btScalar mass = 800.0f; // car mass
    btVector3 inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, inertia);
	
    btDefaultMotionState* motion =
        new btDefaultMotionState(transform);
	
    btRigidBody::btRigidBodyConstructionInfo info(
        mass, motion, shape, inertia);
	
    body = new btRigidBody(info);
	
    // Car behavior
    body->setFriction(0.5f); // lower friction
    body->setRollingFriction(0.1f);
    body->setAngularFactor(btVector3(0, 1, 0)); // rotate only on Y
    body->setActivationState(DISABLE_DEACTIVATION);
    body->setDamping(0.05f, 0.1f); // lower damping for better responsiveness
	
    world->addRigidBody(body);
	
	mRaycastVehicleRaycaster = new btDefaultVehicleRaycaster(world);
	mRaycastVehicle = new btRaycastVehicle(mTuning, body, mRaycastVehicleRaycaster);

	///never deactivate the vehicle
	body->setActivationState(DISABLE_DEACTIVATION);

	world->addVehicle(mRaycastVehicle);

	float connectionHeight = 0.5f;

	bool isFrontWheel = true;

	//choose coordinate system
	mRaycastVehicle->setCoordinateSystem(0, 1, 2);

	btVector3 connectionPointCS0(0.5 - (0.3 * wheelWidth), connectionHeight, 2 * 1.0 - wheelRadius);

	mRaycastVehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, mTuning, isFrontWheel);
	connectionPointCS0 = btVector3(-0.5 + (0.3 * wheelWidth), connectionHeight, 2 * 1.0 - wheelRadius);

	mRaycastVehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, mTuning, isFrontWheel);
	connectionPointCS0 = btVector3(-0.5 + (0.3 * wheelWidth), connectionHeight, -2 * 1.0 + wheelRadius);
	isFrontWheel = false;
	mRaycastVehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, mTuning, isFrontWheel);
	connectionPointCS0 = btVector3(0.5 - (0.3 * wheelWidth), connectionHeight, -2 * 1.0 + wheelRadius);
	mRaycastVehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, mTuning, isFrontWheel);

	for (int i = 0; i < mRaycastVehicle->getNumWheels(); i++)
	{
		btWheelInfo& wheel = mRaycastVehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = suspensionDamping;
		wheel.m_wheelsDampingCompression = suspensionCompression;
		wheel.m_frictionSlip = wheelFriction;
		wheel.m_rollInfluence = rollInfluence;
	}
}

void Vehicle::Update(float engine, float brake, float steering, float timestep)
{
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	
	btVector3 velocity = body->getLinearVelocity();
    float speed = velocity.length();
	if (speed < 0.2f)
		speed = 0.0f;
	
	mSteeringInput = std::lerp(mSteeringInput, -steering / (1.0 + std::min(2.0f, speed * 0.05f)), 10.0f * timestep);
	mBrakeInput = std::lerp(mBrakeInput, brake, 40.0f * timestep);
	mThrottleInput = std::lerp(mThrottleInput, engine, 40.0f * timestep);
	
	float angVelFrictionLoss = std::abs((body->getWorldTransform().getBasis().transpose() * body->getAngularVelocity()).getY()) * 0.5f;
	angVelFrictionLoss /= 1.0f + mBrakeInput;
	mThrottleInput /= 1.0f + mBrakeInput * 2.0f;
	
	ScreenShadowPrintf(70, 220, "Speed: %.2fm/s (%.0fkm/h)", speed, speed * 3.6f);
	ScreenShadowPrintf(70, 195, "mSteeringInput: %.2f", mSteeringInput);
	ScreenShadowPrintf(70, 180, "mThrottleInput: %.2f", mThrottleInput);
	ScreenShadowPrintf(70, 165, "mBrakeInput: %.2f", mBrakeInput);
	ScreenShadowPrintf(70, 150, "angVelFrictionLoss: %.2f", angVelFrictionLoss);
	ScreenShadowPrintf(-300, 220, "Vehicle pos: (%.2f, %.2f, %.2f)",
				trans.getOrigin().getX(),
				trans.getOrigin().getY(),
				trans.getOrigin().getZ());
	
	float powerMod = 1.0f / (1.0f + (speed * speedPowerDecline));
	// TODO: modulate parameters based on speed and such
	for (int i = 0; i < mRaycastVehicle->getNumWheels(); i++)
	{
		btWheelInfo& wheel = mRaycastVehicle->getWheelInfo(i);
		wheel.m_frictionSlip = wheelFriction / (1.0f + angVelFrictionLoss);
		
		mRaycastVehicle->applyEngineForce(mThrottleInput * enginePower * powerMod, i);
		mRaycastVehicle->setBrake(mBrakeInput * brakePower * powerMod, i);
		if (wheel.m_bIsFrontWheel)
		{
			mRaycastVehicle->setSteeringValue(mSteeringInput, i);
		}
		
		mRaycastVehicle->updateWheelTransform(i, true);
	}
}
