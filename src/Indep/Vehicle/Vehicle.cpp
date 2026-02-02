#include "Vehicle.h"

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
}

void Vehicle::applyInput(float engine, float brake, float steering)
{
    // Get the car's forward direction from its rotation
    // for or now we keep physics-forward as -Z; rendering can be rotated separately if needed.
    btMatrix3x3 basis = body->getWorldTransform().getBasis();
    btVector3 forward = basis * btVector3(0, 0, -1);
    forward.normalize();
    
    // current velocity
    btVector3 velocity = body->getLinearVelocity();
    float speed = velocity.length();

    // Debug by print velocity and forces occasionally
    static int debugCounter = 0;
    if (++debugCounter % 60 == 0) // Every ~1 second at 60fps
    {
        printf("Speed: %f, Engine: %f, Brake: %f, Steering: %f\n", speed, engine, brake, steering);
        printf("Velocity: (%f, %f, %f)\n", velocity.x(), velocity.y(), velocity.z());
        printf("Forward: (%f, %f, %f)\n", forward.x(), forward.y(), forward.z());
    }

    // Apply engine force (forward)
    if (engine > 0.0f)
    {
        btVector3 force = forward * engine;
        body->applyCentralForce(force);
    }

    // Apply reverse/brake force
    if (brake > 0.0f)
    {
        if (speed > 0.3f)
        {
            // If moving, apply brake (opposite to velocity direction)
            btVector3 brakeDir = velocity;
            if (brakeDir.length2() > SIMD_EPSILON * SIMD_EPSILON)
            {
                brakeDir.normalize();
                body->applyCentralForce(-brakeDir * brake * 1.5f); // Stronger brake
            }
        }
        else
        {
            // If not moving much- apply reverse force
            body->applyCentralForce(-forward * brake);
        }
    }

    // Steering /stability
    if (steering != 0.0f)
    {
        //direct control over yaw angular velocity so turning feels snappy
        btVector3 angVel = body->getAngularVelocity();
        float yawSpeed = 20.0f; //higher = faster turning
        angVel.setY(steering * yawSpeed);
        body->setAngularVelocity(angVel);
    }
    else
    {
        // No steering input:
        // 1) kill unwanted yaw drift so the car doesn't keep turning by itself
        // 2) kill sideways (lateral) velocity so it drives straight

        // 1) damp yaw when there's no steering
        btVector3 angVel = body->getAngularVelocity();
        angVel.setY(angVel.getY() * 0.2f);
        body->setAngularVelocity(angVel);

        // 2) remove lateral (sideways) speed so we only keep forward/back motion
        btVector3 right = basis * btVector3(1, 0, 0); // local X axis (right/left)
        right.normalize();
        btScalar lateralSpeed = velocity.dot(right);
        btVector3 lateralVel = right * lateralSpeed;
        btVector3 forwardBackVel = velocity - lateralVel; //forward  or back only
        body->setLinearVelocity(forwardBackVel);
    }
}
