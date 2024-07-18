#ifndef DEBUG_DRAWER_H
#define DEBUG_DRAWER_H

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <btBulletDynamicsCommon.h>

#include "lightless.h"
#include "line_drawer.h"

class DebugDrawer : public btIDebugDraw
{
public:
    DebugDrawer(btDynamicsWorld *world, Lightless *lightless, float thickness = 0.05f);

    void updateProjViewMatrix(const glm::mat4 &projViewMatrix);

    // Debug mode methods
    virtual void setDebugMode(int debugMode) override;
    virtual int getDebugMode() const override;

    // Drawing methods
    virtual void drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB,
        btScalar distance, int lifeTime, const btVector3 &color) override;
    virtual void drawLine(const btVector3 &from, const btVector3 &to,
        const btVector3 &color) override;

    // Unused
    virtual void draw3dText(const btVector3 &location, const char *textString) override;
    virtual void reportErrorWarning(const char *warningString) override;

private:
    int m_debugMode = 1;
    btDynamicsWorld *m_world;
    LineDrawer *m_lineDrawer;
    float m_thickness;
};

#endif // DEBUG_DRAWER_H
