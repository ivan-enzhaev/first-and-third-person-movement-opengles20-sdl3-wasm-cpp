#include <cmath>

#include "line_drawer.h"
#include "math_helper.h"

LineDrawer::LineDrawer(Lightless *lightless): m_lightless(lightless)
{
}

void LineDrawer::updateProjViewMatrix(const glm::mat4 &projViewMatrix)
{
    m_projViewMatrix = projViewMatrix;
}

void LineDrawer::draw(const glm::vec3 &from, const glm::vec3 &to,
    const glm::vec3 &color, float thickness)
{
    m_lightless->color[0] = color.x;
    m_lightless->color[1] = color.y;
    m_lightless->color[2] = color.z;

    float centerX, centerY, centerZ;

    if (from.x > to.x)
    {
        centerX = to.x + std::abs(from.x - to.x) / 2.f;
    }
    else
    {
        centerX = from.x + std::abs(to.x - from.x) / 2.f;
    }

    if (from.y > to.y)
    {
        centerY = to.y + std::abs(from.y - to.y) / 2.f;
    }
    else
    {
        centerY = from.y + std::abs(to.y - from.y) / 2.f;
    }

    if (from.z > to.z)
    {
        centerZ = to.z + std::abs(from.z - to.z) / 2.f;
    }
    else
    {
        centerZ = from.z + std::abs(to.z - from.z) / 2.f;
    }

    m_tempVec.x = to.x - from.x;
    m_tempVec.y = to.y - from.y;
    m_tempVec.z = to.z - from.z;
    float length = glm::length(m_tempVec);
    bool isAxis = false;
    if (length < 0.2f)
    {
        length = 2.f;
        isAxis = true;
    }

    m_tempVec = glm::normalize(m_tempVec);
    m_lightless->rotation = MathHelper::rotationTo(glm::vec3(1.f, 0.f, 0.f), m_tempVec);

    m_lightless->scale.x = length;
    m_lightless->scale.y = thickness;
    m_lightless->scale.z = thickness;
    if (!isAxis)
    {
        m_lightless->position.x = centerX;
        m_lightless->position.y = centerY;
        m_lightless->position.z = centerZ;
    }
    else
    {
        m_tempVec.x = m_tempVec.x * length / 2.f;
        m_tempVec.y = m_tempVec.y * length / 2.f;
        m_tempVec.z = m_tempVec.z * length / 2.f;
        m_lightless->position = glm::vec3(centerX, centerY, centerZ) + m_tempVec;
    }

    m_lightless->draw(m_projViewMatrix);
}
