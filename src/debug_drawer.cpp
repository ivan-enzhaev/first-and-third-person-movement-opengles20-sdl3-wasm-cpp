#include <cmath>

#include "debug_drawer.h"
#include "math_helper.h"

DebugDrawer::DebugDrawer(btDynamicsWorld *world, Lightless *lightless, float thickness)
    : m_world(world)
    , m_thickness(thickness)
{
    m_lineDrawer = new LineDrawer(lightless);
}

void DebugDrawer::updateProjViewMatrix(const glm::mat4 &projViewMatrix)
{
    m_lineDrawer->updateProjViewMatrix(projViewMatrix);
}

void DebugDrawer::setDebugMode(int debugMode)
{
    m_debugMode = debugMode;
}

int DebugDrawer::getDebugMode() const
{
    return m_debugMode;
}

void DebugDrawer::drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB,
    btScalar distance, int lifeTime, const btVector3 &color)
{
}

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to,
    const btVector3 &color)
{
    m_lineDrawer->draw(glm::vec3(from.x(), from.y(), from.z()),
        glm::vec3(to.x(), to.y(), to.z()), glm::vec3(color.x(), color.y(), color.z()), m_thickness);
}

void DebugDrawer::draw3dText(const btVector3 &location, const char *textString)
{
}

void DebugDrawer::reportErrorWarning(const char *warningString)
{
}
