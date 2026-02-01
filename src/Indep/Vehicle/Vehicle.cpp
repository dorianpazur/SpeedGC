#include "Vehicle.h"

Vehicle::Vehicle(
    btDynamicsWorld* world,
    const btVector3& startPos
) {
    // --- Chassis shape ---
    btCollisionShape* chassisShape =
        new btBoxShape(btVector3(1.0f, 0.5f, 2.0f));

    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(startPos);

    btScalar mass = 800.0f;
    btVector3 inertia(0, 0, 0);
    chassisShape->calculateLocalInertia(mass, inertia);

    btDefaultMotionState* motion =
        new btDefaultMotionState(tr);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(
        mass, motion, chassisShape, inertia
    );

    mChassis = new btRigidBody(rbInfo);
    mChassis->setActivationState(DISABLE_DEACTIVATION);

    world->addRigidBody(mChassis);

    // --- Vehicle ---
    btRaycastVehicle::btVehicleTuning tuning;
    mRaycaster = new btDefaultVehicleRaycaster(world);
    mVehicle = new btRaycastVehicle(
        tuning, mChassis, mRaycaster
    );

    mVehicle->setCoordinateSystem(0, 1, 2); // X right, Y up, Z forward
    world->addVehicle(mVehicle);

    // --- Wheels ---
    float wheelRadius = 0.5f;
    float wheelWidth = 0.4f;
    float suspensionRest = 0.6f;

    btVector3 wheelDir(0, -1, 0);
    btVector3 wheelAxle(-1, 0, 0);

    float connectionHeight = 0.6f;

    // Front left / right
    mVehicle->addWheel(
        btVector3(-1.0f, connectionHeight, 1.6f),
        wheelDir, wheelAxle,
        suspensionRest, wheelRadius,
        tuning, true
    );
    mVehicle->addWheel(
        btVector3(1.0f, connectionHeight, 1.6f),
        wheelDir, wheelAxle,
        suspensionRest, wheelRadius,
        tuning, true
    );

    // Rear left / right
    mVehicle->addWheel(
        btVector3(-1.0f, connectionHeight, -1.6f),
        wheelDir, wheelAxle,
        suspensionRest, wheelRadius,
        tuning, false
    );
    mVehicle->addWheel(
        btVector3(1.0f, connectionHeight, -1.6f),
        wheelDir, wheelAxle,
        suspensionRest, wheelRadius,
        tuning, false
    );
}

Vehicle::~Vehicle() {
    delete mVehicle;
    delete mRaycaster;
    delete mChassis;
}

void Vehicle::applyInput(
    float engineForce,
    float brakeForce,
    float steering
) {
    // Front wheels steer
    mVehicle->setSteeringValue(steering, 0);
    mVehicle->setSteeringValue(steering, 1);

    // Rear wheels drive
    mVehicle->applyEngineForce(engineForce, 2);
    mVehicle->applyEngineForce(engineForce, 3);

    // All wheels brake
    for (int i = 0; i < 4; i++)
        mVehicle->setBrake(brakeForce, i);
}

void Vehicle::update(float /*dt*/) {
    // Nothing needed here yet
}

btRigidBody* Vehicle::getBody() const {
    return mChassis;
}

btRaycastVehicle* Vehicle::getVehicle() const {
    return mVehicle;
}
