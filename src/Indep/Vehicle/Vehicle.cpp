#include "Vehicle.h"
#include "ScreenPrintf.h"
#include <Vulpes/model.h>
#include "World.h"
#include <Vulpes/Particles.h>

// car model used for rendering all vehicles
extern vModel* gCarModel;

const float speedPowerDecline = 0.04f;
const float kSpeedRate = 1.5f;
const float enginePower = 24000.0f * kSpeedRate;
const float brakePower = 50.0f;
const float wheelRadius = 0.15f;
const float wheelWidth = 0.125f;
const float wheelFriction = 6.0f; 
const float suspensionStiffness = 40.0f;
const float suspensionDamping = 2.3f;
const float suspensionCompression = 9.4f;
const float rollInfluence = -0.24f;
const float drag = 0.2f;
const btScalar suspensionRestLength(0.27f);
const float connectionHeight = -0.23f;

const btVector3 wheelDirectionCS0(0, -1, 0);
const btVector3 wheelAxleCS(-1, 0, 0);

Vehicle::Vehicle(btDynamicsWorld* world, const btVector3& startPos)
{
	mWorld = world;
    // Cube collision shape (car)
    btCollisionShape* shape = new btCompoundShape(true, 3);
	
    btTransform transform;
	
	// main body
	
	float kWidth = 1.35f;
	float kLength = 3.4f;
	float kHeight = 1.12f;
	
    transform.setIdentity();
	transform.setOrigin(btVector3(0, 0, 0));
	((btCompoundShape*)shape)->addChildShape(transform, new btBoxShape(btVector3(kWidth / 2, kHeight / 4.5f, kLength / 2))); // car-sized cube
	
	// front
	//transform.setIdentity();
    //transform.setOrigin(btVector3(0.0875, 0.2f, 0.5f));
	//((btCompoundShape*)shape)->addChildShape(transform, new btSphereShape(0.0875f));
	//transform.setIdentity();
    //transform.setOrigin(btVector3(-0.0875, 0.2f, 0.5f));
	//((btCompoundShape*)shape)->addChildShape(transform, new btSphereShape(0.0875f));
	
	// roof
	transform.setIdentity();
    transform.setOrigin(btVector3(0, kHeight / 2.5f, 0.25));
	((btCompoundShape*)shape)->addChildShape(transform, new btBoxShape(btVector3(kWidth / 2.25f, kHeight / 4, kLength / 3))); // car-sized cube
	
    transform.setIdentity();
    transform.setOrigin(startPos);
	btQuaternion baseRotation;
	baseRotation.setEuler(3.14159265359f, 0.0f, 0.0f);
	transform.setRotation(baseRotation);
	
    btScalar mass = 800.0f; // car mass
    btVector3 inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, inertia);
	
    mMotionState = btDefaultMotionState(transform);
	
    btRigidBody::btRigidBodyConstructionInfo info(
        mass, &mMotionState, shape, inertia);
	
    mBody = new btRigidBody(info);
	
    // Car behavior
    mBody->setFriction(0.0f); // no friction
    mBody->setRollingFriction(0.1f);
    mBody->setActivationState(DISABLE_DEACTIVATION);
    mBody->setDamping(0.0f, 0.0f); // lower damping for better responsiveness
	mBody->setRestitution(0.0);
	mBody->setUserPointer(this);
	
	// Continuous collision detection so the car doesn't tunnel through batteries at speed
	mBody->setCcdSweptSphereRadius(0.5f);
	mBody->setCcdMotionThreshold(0.2f);

    mWorld->addRigidBody(mBody);
	
	mRaycastVehicleRaycaster = new btDefaultVehicleRaycaster(mWorld);
	mRaycastVehicle = new btRaycastVehicle(mTuning, mBody, mRaycastVehicleRaycaster);

	///never deactivate the vehicle
	mBody->setActivationState(DISABLE_DEACTIVATION);

	mWorld->addVehicle(mRaycastVehicle);

	bool isFrontWheel = true;

	//choose coordinate system
	mRaycastVehicle->setCoordinateSystem(0, 1, 2);

	btVector3 connectionPointCS0((kWidth / 2) - 0.1f, connectionHeight, (kLength / 2) * 1.0 - wheelRadius);
	mRaycastVehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, mTuning, isFrontWheel);
	
	connectionPointCS0 = btVector3(-(kWidth / 2) + 0.1f, connectionHeight, (kLength / 2) * 1.0 - wheelRadius);
	mRaycastVehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, mTuning, isFrontWheel);
	
	isFrontWheel = false;
	
	connectionPointCS0 = btVector3(-(kWidth / 2) + 0.1f, connectionHeight, -(kLength / 2) * 1.0 + wheelRadius);
	mRaycastVehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, mTuning, isFrontWheel);
	
	connectionPointCS0 = btVector3((kWidth / 2) - 0.1f, connectionHeight, -(kLength / 2) * 1.0 + wheelRadius);
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

Vehicle::~Vehicle()
{
	if (mRaycastVehicle)
	{
		mWorld->removeVehicle(mRaycastVehicle);
		delete mRaycastVehicle;
	}
	
	if (mRaycastVehicleRaycaster)
		delete mRaycastVehicleRaycaster;
	
	if (mBody)
	{
		mWorld->removeRigidBody(mBody);
		
		// get compound shape
		btCompoundShape* compoundShape = (btCompoundShape*)(mBody->getCollisionShape());
		
		if (compoundShape)
		{
			// delete its children
			for (int i = compoundShape->getNumChildShapes() - 1; i >= 0; i--) {
				btCollisionShape* childShape = compoundShape->getChildShape(i);
				compoundShape->removeChildShapeByIndex(i);
				delete childShape;
			}
			delete compoundShape; // delete the compound shape itself
		}
		
		delete mBody;
		mBody = NULL;	
	}
}

void Vehicle::Update(float throttle, float brake, float steering, float timestep)
{
	// TODO - move this and other input logic to input manager
	if (std::abs(steering) < 0.15f)
		steering = 0.0f;

	float rawBrake = brake;
	
	if (brake < 0.1f)
		brake = 0.0f;
	
	if (throttle < 0.1f)
		throttle = 0.0f;
	
	if (steering)
		steering = (std::abs(steering) / steering) * std::pow(std::abs(steering), 4);
	
	btTransform trans;
	mBody->getMotionState()->getWorldTransform(trans);
	
	btVector3 velocity = mBody->getLinearVelocity();
	
	mVelocity.x = velocity.getX();
	mVelocity.y = velocity.getY();
	mVelocity.z = velocity.getZ();
	
    float speed = velocity.length();
	if (speed < 0.2f)
		speed = 0.0f;
	
	btVector3 up = trans.getBasis().getColumn(1); // local up direction
	// consider it really flipped only when local up points below the horizon
	bool isFlipped = (up.getY() < 0.0f);

	if (rawBrake > 0.1f && isFlipped)
	{
		btVector3 pos = trans.getOrigin();
		pos.setY(pos.getY() + 0.75f); // lift slightly off the ground

		btTransform upright;
		upright.setIdentity();
		upright.setOrigin(pos);

		// Same base rotation as spawn so the car faces the correct way down the track.
		btQuaternion baseRotation;
		baseRotation.setEuler(3.14159265359f, 0.0f, 0.0f);
		upright.setRotation(baseRotation);

		mBody->setWorldTransform(upright);
		mBody->getMotionState()->setWorldTransform(upright);
		mBody->setLinearVelocity(btVector3(0, 0, 0));
		mBody->setAngularVelocity(btVector3(0, 0, 0));

		// Refresh transform for the rest of Update().
		trans = upright;
		velocity = btVector3(0, 0, 0);
		speed = 0.0f;
	}

	// hit the brakes when going too slow and not on the throttle
	if (speed <= 3.5f && throttle == 0.0f)
	{
		brake = 1.0f;
	}
	bool wantsReverse = (rawBrake > 0.1f) && (throttle == 0.0f) && (speed < 1.0f || mIsReversing);

	// When the race is finished and this vehicle hits the endline
	// we just brake to a stop and stay there
	if (World* world = World::GetInstance())
	{
		if (world->mRaceFinished)
		{
			bool isWinner = (world->mWinnerVehicleIndex >= 0 &&
				world->mWinnerVehicleIndex < (int)world->mVehicles.size() &&
				world->mVehicles[world->mWinnerVehicleIndex] == this);

			bool hasCrossedFinish = false;
			if (mBody && mBody->getMotionState())
			{
				btTransform t;
				mBody->getMotionState()->getWorldTransform(t);
				hasCrossedFinish = (t.getOrigin().getZ() <= World::kFinishLineZ);
			}

			if (isWinner || hasCrossedFinish)
				wantsReverse = false;
		}
	}
	mIsReversing = wantsReverse;

	mBrakeInput = std::lerp(mBrakeInput, brake, 60.0f * timestep);
	mThrottleInput = std::lerp(mThrottleInput, throttle, 30.0f * timestep);
	float steeringTarget = -steering / (1.0 + std::fminf(1.5f, speed * 0.001f * mThrottleInput));
	//mSteeringInput = std::lerp(mSteeringInput, steeringTarget, (1.4f + (std::abs(steeringTarget) * 1.4f)) * timestep);
	
	float turnSpeed = (3.0f - (std::abs(steeringTarget) * 1.5f)) * (2.0f / (std::fminf(1.0f, speed / 33.0f) + 1.0f));
	mSteeringInput = tMoveTowards(mSteeringInput, steeringTarget, timestep * turnSpeed);
	
	// Fuel: consume on throttle, baseline power when empty
	if (mThrottleInput > 0.01f)
		mFuel -= kFuelConsumptionPerSecond * mThrottleInput * timestep;
	if (mFuel < 0.0f)
		mFuel = 0.0f;

	const float kBasePowerScale = 0.3f; // always-available power (0..1)
	float fuelScaleRaw = (mFuel > 0.01f) ? std::min(1.0f, mFuel / kMaxFuel) : 0.0f;
	float fuelScale = kBasePowerScale + (1.0f - kBasePowerScale) * fuelScaleRaw;

	const float kBaseMaxSpeed = 20.0f * kSpeedRate;
	const float kExtraMaxSpeed = 60.0f * kSpeedRate;
	float fuelFraction = (mFuel > 0.0f) ? (mFuel / kMaxFuel) : 0.0f;
	if (fuelFraction > 1.0f) fuelFraction = 1.0f;
	float targetMaxSpeed = kBaseMaxSpeed + kExtraMaxSpeed * fuelFraction;

	float speedLimiter = 1.0f;
	if (speed > targetMaxSpeed)
	{
		float over = speed - targetMaxSpeed;
		speedLimiter = std::max(0.0f, 1.0f - over * 0.25f);
	}

	float angVelFrictionLoss = std::powf(std::abs(mBody->getAngularVelocity().getY()) * 0.5f, 2.0f);
	angVelFrictionLoss /= 1.0f + mBrakeInput;
	mThrottleInput /= 1.0f + (mBrakeInput * 2.0f);
	float speedFrictionScaleF = std::min(1.0f, 0.3f + (speed * 0.025f));
	float speedFrictionScaleR = std::min(1.0f, 0.05f + (speed * 0.035f));
	
	//ScreenShadowPrintf(70, 220, "Speed: %.2fm/s (%.0fkm/h)", speed, speed * 3.6f);
	ScreenShadowPrintf(-300, 195, "mSteeringInput: %.2f", mSteeringInput);
	ScreenShadowPrintf(-300, 180, "mThrottleInput: %.2f", mThrottleInput);
	ScreenShadowPrintf(-300, 165, "mBrakeInput: %.2f", mBrakeInput);
	ScreenShadowPrintf(-300, 150, "angVelFrictionLoss: %.2f", angVelFrictionLoss);
	ScreenShadowPrintf(-300, 135, "speedFrictionScaleF: %.2f", speedFrictionScaleF);
	ScreenShadowPrintf(-300, 120, "speedFrictionScaleR: %.2f", speedFrictionScaleR);
	ScreenShadowPrintf(-300, 105, "reverse: %s", mIsReversing ? "ON" : "off");
	ScreenShadowPrintf(-300, 220, "Vehicle pos: (%.2f, %.2f, %.2f)",
				trans.getOrigin().getX(),
				trans.getOrigin().getY(),
				trans.getOrigin().getZ());
	
	// aerodynamics
	btVector3 downforce = btVector3(0, -600.0f * std::min(50.0f, speed) * timestep, 0);
	mBody->applyCentralForce(-velocity * drag * velocity.length());
	mBody->applyCentralForce(mBody->getWorldTransform().getBasis() * downforce);
	
	float powerMod = 1.0f / (1.0f + ((speed * speedPowerDecline) * (speed * speedPowerDecline) * (speed * speedPowerDecline) * (speed * speedPowerDecline)));
	powerMod *= std::min(1.0f, 0.05f + (speed * 0.012f));
	// TODO: modulate parameters based on speed and such
	for (int i = 0; i < mRaycastVehicle->getNumWheels(); i++)
	{
		btWheelInfo& wheel = mRaycastVehicle->getWheelInfo(i);
		wheel.m_frictionSlip = (wheelFriction * (wheel.m_bIsFrontWheel ? speedFrictionScaleF : speedFrictionScaleR)) / (1.0f + angVelFrictionLoss);
		
		if (mIsReversing)
		{
			const float kReversePowerScale = 0.012f;
			mRaycastVehicle->applyEngineForce(-mBrakeInput * enginePower * kReversePowerScale, i);
			mRaycastVehicle->setBrake(0.0f, i);
		}
		else
		{
			mRaycastVehicle->applyEngineForce(mThrottleInput* enginePower* powerMod* fuelScale* speedLimiter, i);
			mRaycastVehicle->setBrake(mBrakeInput * brakePower * std::lerp(1.0f, powerMod, 0.025f), i);
		}
		if (wheel.m_bIsFrontWheel)
		{
			mRaycastVehicle->setSteeringValue(mSteeringInput * 0.4f, i);
		}
		
		mRaycastVehicle->updateWheelTransform(i, true);
	}
	
	if (speed >= 44.0f)
	{
		tMatrix4 trailTransform;
		
		btVector3 up = trans.getBasis().getColumn(1);
		
		trailTransform[0][3] = trans.getOrigin().getX() + (up.getX() * 0.15f);
		trailTransform[1][3] = trans.getOrigin().getY() + (up.getY() * 0.15f);
		trailTransform[2][3] = trans.getOrigin().getZ() + (up.getZ() * 0.15f);
		
		AddXenonEffect(true, &contrail, &trailTransform, &mVelocity); // spawn contrails
	}
	
	mCollidedThisFrame = false;
}

void Vehicle::OnCollide(ISimable* other, const tVector3 &contactPoint)
{
	//if (other)
	//	ScreenPrintf(-80, -180, 2.0f, 0xFFFF0000, "simable collided! coords are %.2f %.2f %.2f", contactPoint.x, contactPoint.y, contactPoint.z);
	//else
	//	ScreenPrintf(-80, -180, 2.0f, 0xFFFF0000, "static collided! coords are %.2f %.2f %.2f", contactPoint.x, contactPoint.y, contactPoint.z);
	
	float speed = sqrtf((mVelocity.x * mVelocity.x) + (mVelocity.y * mVelocity.y) + (mVelocity.z * mVelocity.z));
	
	if (speed > 2.0f && !mCollidedThisFrame)
	{
		tMatrix4 sparkTransform;
		
		sparkTransform[0][3] = contactPoint.x;
		sparkTransform[1][3] = contactPoint.y;
		sparkTransform[2][3] = contactPoint.z;
		
		AddXenonEffect(false, &fxsprk_line, &sparkTransform, &mVelocity);
	}
	
	mCollidedThisFrame = true; // signal that we already collided with something this frame for optimization
}

void Vehicle::AddFuel(float amount)
{
	mFuel += amount;
	if (mFuel > kMaxFuel)
		mFuel = kMaxFuel;
}


void Vehicle::Render(vView* view)
{
	if (!gCarModel || !view || !mBody)
		return;
	
	tMatrix4 transform;
	btScalar transformFlt[16];
	btTransform trans;
	
	if (mBody->getMotionState())
	{
		mBody->getMotionState()->getWorldTransform(trans);
	}
	else
	{
		trans = mBody->getWorldTransform();
	}
	
	trans.getOpenGLMatrix(transformFlt);
	
	// Bullet (OpenGL) using GX matrix
	transform[0][0] = transformFlt[0];
	transform[1][0] = transformFlt[1];
	transform[2][0] = transformFlt[2];
	
	transform[0][1] = transformFlt[4];
	transform[1][1] = transformFlt[5];
	transform[2][1] = transformFlt[6];
	
	transform[0][2] = transformFlt[8];
	transform[1][2] = transformFlt[9];
	transform[2][2] = transformFlt[10];
	
	transform[0][3] = transformFlt[12];
	transform[1][3] = transformFlt[13];
	transform[2][3] = transformFlt[14];
	
	tVector3 fl = tVector3(1, 0, 1.45);
	tVector3 fr = tVector3(-1, 0, 1.45);
	tVector3 rl = tVector3(1, 0, -1.7);
	tVector3 rr = tVector3(-1, 0, -1.7);
	tMulVector(&fl, &transform, &fl);
	tMulVector(&fr, &transform, &fr);
	tMulVector(&rl, &transform, &rl);
	tMulVector(&rr, &transform, &rr);
	
	GX_LoadPosMtxImm(*(Mtx44*)&view->ViewMatrix, GX_PNMTX0);
	vEffectStaticState::pCurrentEffect = vEffects[VEFFECT_STANDARD];
	
	vEffectStaticState::pCurrentEffect->SetTexture(vTextureCache::GetTexture(CTStringHash("CarShadow")));
	vEffectStaticState::pCurrentEffect->Start();
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_ZERO, GX_BL_SRCCLR, GX_LO_CLEAR);
	
	vPoly poly;
	
	poly.Vertices[0].x = fl.x - 0.15;
	poly.Vertices[0].y = 0.1f;
	poly.Vertices[0].z = fl.z - 0.15;
	poly.UVs[0][0] = 0.0f;
	poly.UVs[0][1] = 0.0f;
	poly.Vertices[1].x = rl.x - 0.15;
	poly.Vertices[1].y = 0.05f;
	poly.Vertices[1].z = rl.z - 0.15;
	poly.UVs[1][0] = 0.0f;
	poly.UVs[1][1] = 1.0f;
	poly.Vertices[2].x = rr.x - 0.15;
	poly.Vertices[2].y = 0.05f;
	poly.Vertices[2].z = rr.z - 0.15;
	poly.UVs[2][0] = 1.0f;
	poly.UVs[2][1] = 1.0f;
	poly.Vertices[3].x = fr.x - 0.15;
	poly.Vertices[3].y = 0.05f;
	poly.Vertices[3].z = fr.z - 0.15;
	poly.UVs[3][0] = 1.0f;
	poly.UVs[3][1] = 0.0f;
	
	poly.Colours[0][0] = 0xFF;
	poly.Colours[0][1] = 0xFF;
	poly.Colours[0][2] = 0xFF;
	poly.Colours[0][3] = 0xFF;
	
	*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
	*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
	*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE); // disable zwrite
	vPolyRender(&poly);
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE); // enable zwrite
	
	vEffectStaticState::pCurrentEffect->End();
	
	gCarModel->Render(view, &transform);
}
