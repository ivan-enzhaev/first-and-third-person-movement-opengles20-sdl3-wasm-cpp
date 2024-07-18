#include <iostream>

#include "lightless.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

Lightless::Lightless(GLuint program, const VertexBuffersData &vertexBuffers)
    : m_program(program)
{
    m_vertPosBuffer = vertexBuffers.vertPosBuffer;
    m_amountOfVertices = vertexBuffers.amountOfVertices;

    glUseProgram(m_program);
    m_aPositionLocation = glGetAttribLocation(m_program, "aPosition");
    m_uColorLocation = glGetUniformLocation(m_program, "uColor");
    m_uMvpMatrixLocation = glGetUniformLocation(m_program, "uMvpMatrix");
}

void Lightless::draw(const glm::mat4 &projViewMatrix)
{
    glUseProgram(m_program);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertPosBuffer);
    glVertexAttribPointer(m_aPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(m_aPositionLocation);

    // Translate
    m_modelMatrix = glm::translate(glm::mat4(1.f), position);
    // Rotate
    glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
    // glm::mat4 rotationMatrix = glm::toMat4(rotation);
    m_modelMatrix = m_modelMatrix * rotationMatrix;
    // Scale
    m_modelMatrix = glm::scale(m_modelMatrix, scale);
    // MVP-matrix
    m_mvpMatrix = projViewMatrix * m_modelMatrix;

    glUniform3f(m_uColorLocation, color[0], color[1], color[2]);
    glUniformMatrix4fv(m_uMvpMatrixLocation, 1, false, glm::value_ptr(m_mvpMatrix));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_amountOfVertices);
}
