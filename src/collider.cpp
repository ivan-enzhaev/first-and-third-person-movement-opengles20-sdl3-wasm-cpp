#include "collider.h"

Collider::Collider(btDynamicsWorld *world, btCollisionShape *shape,
    float mass, const btVector3 &initialPosition,
    const btQuaternion &initialRotation)
    : m_world(world)
    , m_shape(shape)
{
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(initialPosition);
    transform.setRotation(initialRotation);

    m_motionState = new btDefaultMotionState(transform);
    btVector3 localInertia(0.f, 0.f, 0.f);

    if (mass > 0.001f)
    {
        m_shape->calculateLocalInertia(mass, localInertia);
    }

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_motionState, shape, localInertia);
    body = new btRigidBody(rbInfo);
    body->setActivationState(DISABLE_DEACTIVATION);
    body->setFriction(1.f);
    body->setRollingFriction(0.1f);
    m_world->addRigidBody(body);
}

Collider::~Collider()
{
    delete body;
    delete m_motionState;
    delete m_shape;
}

glm::vec3 Collider::getPosition()
{
    btMotionState *motionState = body->getMotionState();
    motionState->getWorldTransform(m_tempTransform);
    btVector3 btPosition = m_tempTransform.getOrigin();
    glm::vec3 position(btPosition.x(), btPosition.y(), btPosition.z());
    return position;
}

glm::quat Collider::getRotation()
{
    btMotionState *motionState = body->getMotionState();
    motionState->getWorldTransform(m_tempTransform);
    btQuaternion btQuat = m_tempTransform.getRotation();
    glm::quat rotation(btQuat.w(), btQuat.x(), btQuat.y(), btQuat.z());
    return rotation;
}
