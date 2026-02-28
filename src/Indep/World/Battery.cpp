#include "Battery.h"
#include "Vehicle.h"
#include <Vulpes/model.h>

extern vModel* gBatteryModel;

Battery::Battery(btDynamicsWorld* world, const btVector3& pos, float radius)
	: mRadius(radius)
{
	mWorld = world;
	mShape = new btSphereShape(radius);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(pos);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, mShape, btVector3(0, 0, 0));
	rbInfo.m_restitution = 0.2f;
	rbInfo.m_friction = 0.5f;

	mMotionState = motionState;
	mBody = new btRigidBody(rbInfo);
	mBody->setUserPointer(this);

	mWorld->addRigidBody(mBody);
}

Battery::~Battery()
{
	if (mBody && mWorld)
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

void Battery::OnCollide(ISimable* other, const tVector3& contactPoint)
{
	if (mCollected)
		return;
	Vehicle* vehicle = dynamic_cast<Vehicle*>(other);
	if (vehicle)
	{
		vehicle->AddFuel(40.0f);
		mCollected = true;
		if (mBody)
		{
			mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			mBody->setActivationState(WANTS_DEACTIVATION);
		}
	}
}

void Battery::Render(vView* view)
{
	if (mCollected || !gBatteryModel || !view || !mBody)
		return;

	tMatrix4 transform;
	btScalar transformFlt[16];
	btTransform trans;

	if (mBody->getMotionState())
		mBody->getMotionState()->getWorldTransform(trans);
	else
		trans = mBody->getWorldTransform();

	trans.getOpenGLMatrix(transformFlt);

	float s = mRadius * 2.0f * 0.75f;
	transform[0][0] = (float)transformFlt[0] * s;
	transform[1][0] = (float)transformFlt[1] * s;
	transform[2][0] = (float)transformFlt[2] * s;
	transform[0][1] = (float)transformFlt[4] * s;
	transform[1][1] = (float)transformFlt[5] * s;
	transform[2][1] = (float)transformFlt[6] * s;
	transform[0][2] = (float)transformFlt[8] * s;
	transform[1][2] = (float)transformFlt[9] * s;
	transform[2][2] = (float)transformFlt[10] * s;
	transform[0][3] = (float)transformFlt[12];
	transform[1][3] = (float)transformFlt[13];
	transform[2][3] = (float)transformFlt[14];

	if (!gBatteryModel || !view)
		return;

	gBatteryModel->Render(view, &transform);
}
