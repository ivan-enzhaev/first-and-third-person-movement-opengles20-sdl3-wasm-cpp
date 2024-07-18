#include <cmath>
#include <iostream>

#include "math_helper.h"

MathHelper::MathHelper()
{
}

// Sets a quat from the given angle and rotation axis, then returns it
glm::quat MathHelper::setAxisAngle(const glm::vec3 &axis, float rad)
{
    rad = rad * 0.5f;
    float s = std::sin(rad);
    glm::quat out;
    out.x = s * axis.x;
    out.y = s * axis.y;
    out.z = s * axis.z;
    out.w = std::cos(rad);
    return out;
}

// Sets a quaternion to represent the shortest rotation from one vector to another
// Both vectors are assumed to be unit length
glm::quat MathHelper::rotationTo(const glm::vec3 &initialVector,
    const glm::vec3 &destinationVector)
{
    glm::vec3 tmpvec3;
    glm::vec3 xUnitVec3(1.f, 0.f, 0.f);
    glm::vec3 yUnitVec3(0.f, 1.f, 0.f);
    glm::quat out;

    float dot = glm::dot(initialVector, destinationVector);

    if (dot < -0.999999f)
    {
        tmpvec3 = glm::cross(xUnitVec3, initialVector);
        if (glm::length(tmpvec3) < 0.000001f)
        {
            tmpvec3 = glm::cross(yUnitVec3, initialVector);
        }
        tmpvec3 = glm::normalize(tmpvec3);
        out = setAxisAngle(tmpvec3, glm::pi<float>());
        return out;
    }
    else if (dot > 0.999999f)
    {
        out.x = 0.f;
        out.y = 0.f;
        out.z = 0.f;
        out.w = 1.f;
        return out;
    }
    else
    {
        tmpvec3 = glm::cross(initialVector, destinationVector);
        out.x = tmpvec3.x;
        out.y = tmpvec3.y;
        out.z = tmpvec3.z;
        out.w = 1.f + dot;
        return glm::normalize(out);
    }
}
