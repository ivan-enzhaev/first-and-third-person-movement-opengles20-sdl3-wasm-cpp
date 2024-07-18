#ifndef LIGHTLESS_H
#define LIGHTLESS_H

#include <SDL3/SDL_opengles2.h>

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vertex_buffers.h"

class Lightless
{
public:
    Lightless(GLuint program, const VertexBuffersData &vertexBuffers);

    void draw(const glm::mat4 &projViewMatrix);

    glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 color = glm::vec3(0.5f, 1.f, 0.5f);

private:
    GLuint m_program;
    GLuint m_vertPosBuffer;
    GLint m_aPositionLocation;
    GLint m_uColorLocation;
    GLint m_uMvpMatrixLocation;
    glm::mat4 m_mvpMatrix;
    glm::mat4 m_modelMatrix;
    int m_amountOfVertices;
};

#endif // LIGHTLESS_H
