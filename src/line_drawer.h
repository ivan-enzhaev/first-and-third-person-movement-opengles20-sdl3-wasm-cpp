#ifndef LINE_DRAWER
#define LINE_DRAWER

#include <SDL3/SDL_opengles2.h>

#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "lightless.h"

class LineDrawer
{
public:
    LineDrawer(Lightless *lightless);

    void draw(const glm::vec3 &from, const glm::vec3 &to,
        const glm::vec3 &color, float thickness = 0.05f);
    void updateProjViewMatrix(const glm::mat4 &projViewMatrix);

private:
    void bind();

private:
    Lightless *m_lightless;
    glm::vec3 m_tempVec;
    glm::mat4 m_projViewMatrix;
};

#endif // LINE_DRAWER
