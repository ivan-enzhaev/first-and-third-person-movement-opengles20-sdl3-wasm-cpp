#ifndef COLLIDER_H
#define COLLIDER_H

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <btBulletDynamicsCommon.h>

class Collider
{
public:
    Collider(btDynamicsWorld *world, btCollisionShape *shape, float mass,
        const btVector3 &initialPosition = btVector3(0, 0, 0),
        const btQuaternion &initialRotation = btQuaternion(0, 0, 0, 1));
    ~Collider();

    glm::vec3 getPosition();
    glm::quat getRotation();

    btRigidBody *body;

private:
    btDynamicsWorld *m_world;
    btCollisionShape *m_shape;
    btDefaultMotionState *m_motionState;
    btTransform m_tempTransform;
};

#endif // COLLIDER_H
