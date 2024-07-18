#ifndef ORBIT_CONTROLS_H
#define ORBIT_CONTROLS_H

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

class OrbitControls
{
public:
    OrbitControls(float viewDistance, const glm::vec2 &cameraRotationXY,
        const glm::vec2 &cameraPanXY, int windowWidth, int windowHeigth,
        void (*updateViewMatrix)(void));

    void startCameraRotation(int x, int y);
    void finishCameraRotation();

    void startCameraPanning(int x, int y);
    void finishCameraPanning();

    void mouseMove(int x, int y);
    void zoomInZoomOut(float angleDeltaY);

    glm::mat4 getViewMatrix();
    void resize(int width, int height);

private:
    void (*updateViewMatrix)(void);

private:
    float m_viewDistance;
    glm::vec2 m_cameraRotationXY;
    glm::vec2 m_cameraPanXY;
    bool m_mouseHolding = false;
    int m_mousePrevRotX = 0;
    int m_mousePrevRotY = 0;
    float m_degreesPerPixelX;
    float m_degreesPerPixelY;

    bool m_mousePanning = false;
    int m_mousePrevPanX = 0;
    int m_mousePrevPanY = 0;
    glm::vec3 m_center;
};

#endif // ORBIT_CONTROLS_H
