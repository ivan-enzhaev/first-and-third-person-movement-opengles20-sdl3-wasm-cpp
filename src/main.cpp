#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h>

#include <cmath>
#include <map>
#include <vector>
#include <iostream>

#include <btBulletDynamicsCommon.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif // __EMSCRIPTEN__

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include <glm/gtx/transform.hpp>

#include "collider.h"
#include "debug_drawer.h"
#include "orbit_controls.h"
#include "math_helper.h"
#include "lightless.h"
#include "shader_program.h"
#include "vertex_buffers.h"

Collider *groundCollider;
Collider *slideCollider;
Collider *playerCollider;
Collider *platformCollider;
DebugDrawer *debugDrawer;
Lightless *colliderEdge;
OrbitControls *orbitControls;

bool isInstructionsShown = true;
bool isFirstResize = true;

const float angularSpeed = 3.f;
const float linearSpeed = 5.f;
glm::vec3 forward(0.f, 0.f, 1.f);
glm::vec3 direction = forward;
const float lowAngleLimit = -60.f;
const float highAngleLimit = 60.f;
float cameraAngleX = 0.f;
float cameraSpeed = 50.f;

std::map<std::string, bool> keys;

EM_JS(void, callHideInstructions, (),
      {
          hideInstructions();
      })

EM_JS(void, callShowSidePanel, (),
      {
          showSidePanel();
      })

EM_JS(void, callHideSidePanel, (),
      {
          hideSidePanel();
      })

btBroadphaseInterface *overlappingPairCache;
btCollisionDispatcher *dispatcher;
btCollisionConfiguration *collisionConfiguration;
btConstraintSolver *solver;
btDynamicsWorld *world;

bool isOrbitControls = false;
bool showColliders = true;
glm::mat4 projMatrix, viewMatrix, projViewMatrix, modelMatrix, mvpMatrix;
GLuint program;
GLint aPositionLocation;
GLint uColorLocation, uMvpMatrixLocation;
GLuint vertPosBuffer;

struct AppContext
{
    SDL_Window *window;
    SDL_GLContext glcontext;
    SDL_bool app_quit = SDL_FALSE;
};

void updateViewMatrix()
{
    viewMatrix = orbitControls->getViewMatrix();
    projViewMatrix = projMatrix * viewMatrix;
    debugDrawer->updateProjViewMatrix(projViewMatrix);
}

int SDL_Fail()
{
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return -1;
}

int SDL_AppInit(void **appstate, int argc, char *argv[])
{
    // Init the library, here we make a window so we only need the Video capabilities
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        return SDL_Fail();
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow("First and third person movement using C++, SDL3, Emscripten, OpenGL ES 2.0, and Bullet Physics", 500, 500,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        return SDL_Fail();
    }

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // Enable MULTISAMPLE
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2); // can be 2, 4, 8 or 16

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    if (!glcontext)
    {
        return SDL_Fail();
    }

    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glEnable(GL_DEPTH_TEST);

    std::string vertShaderPath = "./assets/shaders/lightless.vert";
    std::string fragShaderPath = "./assets/shaders/lightless.frag";
    program = createShaderProgram(vertShaderPath, fragShaderPath);

    // Mouse move
    emscripten_set_mousemove_callback(
        "#canvas", nullptr, 0,
        +[](int eventType, const EmscriptenMouseEvent *e, void *userData) -> EM_BOOL
        {
            if (isOrbitControls)
            {
                orbitControls->mouseMove(e->targetX, e->targetY);
            }
            else
            {
                int dx = e->movementX;
                int dy = e->movementY;

                auto impulse = btVector3(0.f, -angularSpeed * dx / 10.f, 0.f);
                playerCollider->body->setAngularVelocity(impulse);

                cameraAngleX -= cameraSpeed * dy / 100.f;
                if (cameraAngleX < lowAngleLimit)
                {
                    cameraAngleX = lowAngleLimit;
                }
                if (cameraAngleX > highAngleLimit)
                {
                    cameraAngleX = highAngleLimit;
                }
            }
            return EM_FALSE;
        });

    // Hide instructions
    emscripten_set_mousedown_callback(
        "#instructions", nullptr, 0,
        +[](int eventType, const EmscriptenMouseEvent *e, void *userData) -> EM_BOOL
        {
            if (e->button == 0)
            {
                if (isOrbitControls)
                {
                    callShowSidePanel();
                    callHideInstructions();
                    isInstructionsShown = false;
                }
                else
                {
                    callHideInstructions();
                    isInstructionsShown = false;
                    emscripten_request_pointerlock("#canvas", true);
                }
            }
            return EM_FALSE;
        });

    emscripten_set_pointerlockchange_callback(
        EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, 0,
        +[](int eventType, const EmscriptenPointerlockChangeEvent *e, void *userData) -> EM_BOOL
        {
            bool active = e->isActive;
            if (!e->isActive)
            {
                isOrbitControls = true;
                callShowSidePanel();
                viewMatrix = orbitControls->getViewMatrix();
                projViewMatrix = projMatrix * viewMatrix;
                debugDrawer->updateProjViewMatrix(projViewMatrix);

                auto impulse = btVector3(0.f, 0.f, 0.f);
                playerCollider->body->setAngularVelocity(impulse);
            }
            return EM_FALSE;
        });

    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache,
        solver, collisionConfiguration);
    world->setGravity(btVector3(0, -9.8, 0));
    btVector3 gravity = world->getGravity();

    VertexBuffersData buffers = VertexBuffers::init("./assets/models/cube.dae");

    colliderEdge = new Lightless(program, buffers);
    debugDrawer = new DebugDrawer(world, colliderEdge, 0.03f);
    debugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
    world->setDebugDrawer(debugDrawer);

    btBoxShape *groundShape = new btBoxShape(btVector3(15.f, 0.2f, 15.f));
    groundCollider = new Collider(world, groundShape, 0);

    btBoxShape *slideShape = new btBoxShape(btVector3(2.5f, 0.25f, 2.5f));
    slideCollider = new Collider(world, slideShape, 0, btVector3(3.f, 0.7f, 5.f),
        btQuaternion(-0.217, 0, 0, 0.976));

    btBoxShape *platformShape = new btBoxShape(btVector3(2.5f, 0.25f, 2.5f));
    platformCollider = new Collider(world, platformShape, 0, btVector3(3.f, 1.8f, 10.f));

    // Player collider
    btSphereShape *playerShape = new btSphereShape(1);
    playerCollider = new Collider(world, playerShape, 80.f, btVector3(0.f, 0.5f, -5.f));
    playerCollider->body->setAngularFactor(btVector3(0.f, 1.f, 0.f));
    playerCollider->body->setDamping(0.9, 0.99);

    SDL_ShowWindow(window);

    // Set up the application data
    *appstate = new AppContext {
        window,
        glcontext,
    };

    return 0;
}

int SDL_AppEvent(void *appstate, const SDL_Event *event)
{
    auto *app = (AppContext *)appstate;

    if (isFirstResize)
    {
        int w = SDL_GetWindowSurface(app->window)->w;
        int h = SDL_GetWindowSurface(app->window)->h;
        glViewport(0, 0, w, h);
        orbitControls = new OrbitControls(20.f, glm::vec2(30.f, 0.f),
            glm::vec2(0.f, 0.f), w, h, &updateViewMatrix);
        projMatrix = glm::perspective(glm::radians(50.f), w / (float) h, 0.1f, 1000.f);
        viewMatrix = orbitControls->getViewMatrix();
        projViewMatrix = projMatrix * viewMatrix;
        debugDrawer->updateProjViewMatrix(projViewMatrix);
        isFirstResize = false;
    }

    switch (event->type)
    {
        case SDL_EVENT_WINDOW_RESIZED:
        {
            int w = SDL_GetWindowSurface(app->window)->w;
            int h = SDL_GetWindowSurface(app->window)->h;
            glViewport(0, 0, w, h);
            orbitControls->resize(w, h);
            projMatrix = glm::perspective(glm::radians(50.f), w / (float) h, 0.1f, 1000.f);
            viewMatrix = orbitControls->getViewMatrix();
            projViewMatrix = projMatrix * viewMatrix;
            debugDrawer->updateProjViewMatrix(projViewMatrix);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            if (event->button.button == SDL_BUTTON_LEFT || event->button.button == SDL_BUTTON_MIDDLE)
            {
                if (isOrbitControls)
                {
                    int x = event->button.x;
                    int y = event->button.y;
                    orbitControls->startCameraRotation(x, y);
                }
            }

            if (event->button.button == SDL_BUTTON_RIGHT)
            {
                if (isOrbitControls)
                {
                    int x = event->button.x;
                    int y = event->button.y;
                    orbitControls->startCameraPanning(x, y);
                }
            }

            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            if (event->button.button == SDL_BUTTON_LEFT || event->button.button == SDL_BUTTON_MIDDLE)
            {
                orbitControls->finishCameraRotation();
            }

            if (event->button.button == SDL_BUTTON_RIGHT)
            {
                orbitControls->finishCameraPanning();
            }

            break;
        }
        case SDL_EVENT_MOUSE_WHEEL:
        {
            if (isOrbitControls)
            {
                orbitControls->zoomInZoomOut(event->wheel.y);
            }

            break;
        }
        case SDL_EVENT_KEY_DOWN:
        {
            if (!isInstructionsShown)
            {
                if (event->key.scancode == SDL_SCANCODE_F)
                {
                    if (isOrbitControls)
                    {
                        isOrbitControls = false;
                        emscripten_request_pointerlock("#canvas", true);
                        callHideSidePanel();
                    }
                }

                if (event->key.scancode == SDL_SCANCODE_C)
                {
                    showColliders = !showColliders;
                }

                if (event->key.scancode == SDL_SCANCODE_W)
                {
                    keys["up"] = true;
                }

                if (event->key.scancode == SDL_SCANCODE_S)
                {
                    keys["down"] = true;
                }

                if (event->key.scancode == SDL_SCANCODE_A)
                {
                    keys["left"] = true;
                }

                if (event->key.scancode == SDL_SCANCODE_D)
                {
                    keys["right"] = true;
                }
            }

            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            if (!isInstructionsShown)
            {
                if (event->key.scancode == SDL_SCANCODE_W)
                {
                    keys["up"] = false;
                }

                if (event->key.scancode == SDL_SCANCODE_S)
                {
                    keys["down"] = false;
                }

                if (event->key.scancode == SDL_SCANCODE_A)
                {
                    keys["left"] = false;
                }

                if (event->key.scancode == SDL_SCANCODE_D)
                {
                    keys["right"] = false;
                }
            }

            break;
        }
        case SDL_EVENT_QUIT:
        {
            app->app_quit = SDL_TRUE;
            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}

void keyboardHandler()
{

    if (keys["up"])
    {
        float vy = playerCollider->body->getLinearVelocity().y();
        btVector3 impulse = btVector3(direction.x * linearSpeed, vy,
            direction.z * linearSpeed);
        playerCollider->body->setLinearVelocity(impulse);
    }

    if (keys["down"])
    {
        float vy = playerCollider->body->getLinearVelocity().y();
        btVector3 impulse = btVector3(-direction.x * linearSpeed, vy,
            -direction.z * linearSpeed);
        playerCollider->body->setLinearVelocity(impulse);
    }

    if (keys["left"])
    {
        if (isOrbitControls)
        {
            btVector3 impulse = btVector3(0.f, angularSpeed, 0.f);
            playerCollider->body->setAngularVelocity(impulse);
        }
        else
        {
            auto vy = playerCollider->body->getLinearVelocity().y();
            auto rotationMatrix = glm::toMat4(playerCollider->getRotation());
            auto vec = rotationMatrix * glm::vec4(forward, 0.f);
            auto cross = glm::cross(glm::vec3(0.f, 1.f, 0.f), glm::vec3(vec.x, vec.y, vec.z));
            auto impulse = btVector3(cross.x * linearSpeed, vy,
                cross.z * linearSpeed);
            playerCollider->body->setLinearVelocity(impulse);
        }
    }

    if (keys["right"])
    {
        if (isOrbitControls)
        {
            btVector3 impulse = btVector3(0.f, -angularSpeed, 0.f);
            playerCollider->body->setAngularVelocity(impulse);
        }
        else
        {
            auto vy = playerCollider->body->getLinearVelocity().y();
            auto rotationMatrix = glm::toMat4(playerCollider->getRotation());
            auto vec = rotationMatrix * glm::vec4(forward, 0.f);
            auto cross = glm::cross(glm::vec3(0.f, 1.f, 0.f), glm::vec3(vec.x, vec.y, vec.z));
            auto impulse = btVector3(-cross.x * linearSpeed, vy,
                -cross.z * linearSpeed);
            playerCollider->body->setLinearVelocity(impulse);
        }
    }

    auto rotationMatrix = glm::toMat4(playerCollider->getRotation());
    direction = rotationMatrix * glm::vec4(forward, 0.f);
}

uint32_t lastTickTime = 0;

int SDL_AppIterate(void *appstate)
{
    auto *app = (AppContext *)appstate;

    keyboardHandler();

    uint32_t currentTickTime = SDL_GetTicks();
    float deltaTime = (currentTickTime - lastTickTime) / 1000.f;
    lastTickTime = currentTickTime;

    world->stepSimulation(deltaTime, 8);

    if (!isOrbitControls)
    {
        auto rotationMatrix = glm::rotate(glm::mat4(1.f), glm::radians(-cameraAngleX), glm::vec3{1.f, 0.f, 0.f});
        auto pitchForward = rotationMatrix * glm::vec4(forward, 0.f);

        rotationMatrix = glm::toMat4(playerCollider->getRotation());
        auto forwardTranform = rotationMatrix * pitchForward;

        auto dist = playerCollider->getPosition() +
            glm::vec3(forwardTranform.x, forwardTranform.y, forwardTranform.z);
        auto cameraPos = playerCollider->getPosition() + glm::vec3(0.f, 3.f, 0.f);
        dist.y = dist.y + 3.f;
        viewMatrix = glm::lookAt(cameraPos, dist, glm::vec3(0.f, 1.f, 0.f));

        projViewMatrix = projMatrix * viewMatrix;
        debugDrawer->updateProjViewMatrix(projViewMatrix);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (showColliders)
    {
        world->debugDrawWorld();
    }

    SDL_GL_SwapWindow(app->window);

    return app->app_quit;
}

void SDL_AppQuit(void *appstate)
{
    auto *app = (AppContext *)appstate;
    if (app)
    {
        SDL_GL_DeleteContext(app->glcontext);
        SDL_DestroyWindow(app->window);
        delete app;
    }

    delete world;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;

    delete groundCollider;
    delete playerCollider;
    delete slideCollider;
    delete platformCollider;
    delete debugDrawer;
    delete colliderEdge;
    delete orbitControls;

    SDL_Quit();
}
