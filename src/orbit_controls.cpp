#include <algorithm>
#include <cmath>
#include <iostream>

#include "orbit_controls.h"

OrbitControls::OrbitControls(float viewDistance, const glm::vec2 &cameraRotationXY,
    const glm::vec2 &cameraPanXY, int windowWidth, int windowHeigth,
    void (*updateViewMatrix)(void))
    : m_viewDistance(viewDistance)
    , m_cameraRotationXY(cameraRotationXY)
    , m_cameraPanXY(cameraPanXY)
{
    this->updateViewMatrix = updateViewMatrix;
    resize(windowWidth, windowHeigth);
}

void OrbitControls::startCameraRotation(int x, int y)
{
    m_mouseHolding = true;
    m_mousePrevRotX = x;
    m_mousePrevRotY = y;
}

void OrbitControls::finishCameraRotation()
{
    m_mouseHolding = false;
}

void OrbitControls::startCameraPanning(int x, int y)
{
    m_mousePanning = true;
    m_mousePrevPanX = x;
    m_mousePrevPanY = y;
}

void OrbitControls::finishCameraPanning()
{
    m_mousePanning = false;
}

void OrbitControls::mouseMove(int x, int y)
{
    if (m_mouseHolding || m_mousePanning)
    {
        if (m_mouseHolding)
        {
            float newCameraRotX = m_cameraRotationXY.x + m_degreesPerPixelX * (float) (y - m_mousePrevRotY);
            // newCameraRotX = std::min(-85.f, std::min(85.f, newCameraRotX));
            float newCameraRotY = m_cameraRotationXY.y + m_degreesPerPixelY * (float) (x - m_mousePrevRotX);
            m_cameraRotationXY.x = newCameraRotX;
            m_cameraRotationXY.y = newCameraRotY;
            m_mousePrevRotX = x;
            m_mousePrevRotY = y;
        }

        if (m_mousePanning)
        {
            glm::mat4 viewMatrix = getViewMatrix();
            // row(0) - X axis in camera space
            glm::vec3 vecX = glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
            m_center += vecX * (float) (m_mousePrevPanX - x) / 50.f;
            // row(1) - Y axis in camera space
            glm::vec3 vecY = glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
            m_center += vecY * (float) (y - m_mousePrevPanY) / 50.f;
            m_mousePrevPanX = x;
            m_mousePrevPanY = y;
        }

        updateViewMatrix();
    }
}

void OrbitControls::zoomInZoomOut(float angleDeltaY)
{
    m_viewDistance += -angleDeltaY;

    if (m_viewDistance < 1.f)
    {
        m_viewDistance = 1.f;
    }

    updateViewMatrix();
}

glm::mat4 OrbitControls::getViewMatrix()
{
    glm::mat4 mat;

    const float cosX = std::cos(glm::radians(m_cameraRotationXY.x));
    const float sinX = std::sin(glm::radians(m_cameraRotationXY.x));
    const float cosY = std::cos(glm::radians(m_cameraRotationXY.y));
    const float sinY = std::sin(glm::radians(m_cameraRotationXY.y));

    glm::vec3 ay(sinX * sinY, cosX, -sinX * cosY); // Y axis in camera space
    glm::vec3 az(-cosX * sinY, sinX, cosX * cosY); // Z axis in camera space
    mat = glm::lookAt(az * m_viewDistance + m_center, m_center, ay);

    return mat;
}

void OrbitControls::resize(int width, int height)
{
    m_degreesPerPixelX = 180.f / (float) width;
    m_degreesPerPixelY = 180.f / (float) height;
}
