#include "Battery.h"
#include "Vehicle.h"
#include <Vulpes/model.h>
#include <Vulpes/Poly.h>
#include <Vulpes/Effect.h>

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

	float s = 1.0f;
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

	{
		float cx = (float)transformFlt[12];
		float cz = (float)transformFlt[14];
		const float outerRadius = mRadius * 1.75f;
		const float innerRadius = mRadius * 0.90f;
		const float ringHeight = 0.35f;

		const unsigned char r = 0x08;
		const unsigned char g = 0x20;
		const unsigned char b = 0xC8;
		const unsigned char a = 0x80;

		GX_LoadPosMtxImm(*(Mtx44*)&view->ViewMatrix, GX_PNMTX0);
		vEffectStaticState::pCurrentEffect = vEffects[VEFFECT_WORLD];
		vEffectStaticState::pCurrentEffect->SetTexture(nullptr);
		vEffectStaticState::pCurrentEffect->Start();
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_CLEAR);
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

		vPoly poly{};
		auto drawSegment = [&](float x0, float z0, float x1, float z1)
			{
				poly.Vertices[0].x = x0; poly.Vertices[0].y = ringHeight; poly.Vertices[0].z = z0;
				poly.Vertices[1].x = x0; poly.Vertices[1].y = ringHeight; poly.Vertices[1].z = z1;
				poly.Vertices[2].x = x1; poly.Vertices[2].y = ringHeight; poly.Vertices[2].z = z1;
				poly.Vertices[3].x = x1; poly.Vertices[3].y = ringHeight; poly.Vertices[3].z = z0;

				poly.Colours[0][0] = r; poly.Colours[0][1] = g; poly.Colours[0][2] = b; poly.Colours[0][3] = a;
				*(unsigned int*)&poly.Colours[1] = *(unsigned int*)&poly.Colours[0];
				*(unsigned int*)&poly.Colours[2] = *(unsigned int*)&poly.Colours[0];
				*(unsigned int*)&poly.Colours[3] = *(unsigned int*)&poly.Colours[0];

				vPolyRender(&poly);
			};

		drawSegment(cx - outerRadius, cz - outerRadius, cx + outerRadius, cz - innerRadius); // top
		drawSegment(cx - outerRadius, cz + innerRadius, cx + outerRadius, cz + outerRadius); // bottom
		drawSegment(cx - outerRadius, cz - innerRadius, cx - innerRadius, cz + innerRadius); // left
		drawSegment(cx + innerRadius, cz - innerRadius, cx + outerRadius, cz + innerRadius); // right

		vEffectStaticState::pCurrentEffect->End();
		vEffectStaticState::pCurrentEffect = NULL;
	}

}
