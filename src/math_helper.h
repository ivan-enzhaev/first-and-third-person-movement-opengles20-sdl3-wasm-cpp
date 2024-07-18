#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class MathHelper
{
public:
    MathHelper();

    // Sets a quat from the given angle and rotation axis, then returns it
    static glm::quat setAxisAngle(const glm::vec3 &axis, float rad);

    // Sets a quaternion to represent the shortest rotation from one vector to another
    // Both vectors are assumed to be unit length
    static glm::quat rotationTo(const glm::vec3 &initialVector,
        const glm::vec3 &destinationVector);

    // static QVector3D rotateX(const QVector3D &vector,
    //     const QVector3D &origin, float rad);
};

#endif // MATH_HELPER_H
