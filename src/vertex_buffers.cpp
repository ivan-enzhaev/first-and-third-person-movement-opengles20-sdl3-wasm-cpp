#include <iostream>
#include <sstream>
#include <tinyxml2.h>
#include <vector>

#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "vertex_buffers.h"

template <typename T>
std::vector<T> split(const std::string &s, char delim) {
    std::vector<T> result;
    std::stringstream ss(s);
    std::string item;

    while (getline (ss, item, delim)) {
        T v = ::atof(item.c_str());
        result.push_back(v);
    }

    return result;
}

VertexBuffers::VertexBuffers() { }

VertexBuffersData VertexBuffers::init(const std::string &path)
{
    tinyxml2::XMLDocument xmlDoc;
    tinyxml2::XMLError eResult = xmlDoc.LoadFile("./assets/models/cube.dae");
    if (eResult != tinyxml2::XML_SUCCESS)
    {
        std::cout << "Failed to open a file" << std::endl;
    }

    tinyxml2::XMLNode* root = xmlDoc.FirstChildElement("COLLADA");
    if (root == nullptr)
    {
        std::cout << "Wrong root element" << std::endl;
    }

    tinyxml2::XMLElement* floatArrayElement = root->FirstChildElement("library_geometries")->
    FirstChildElement("geometry")->FirstChildElement("mesh")->
    FirstChildElement("source")->FirstChildElement("float_array");
    std::string positionsStr = floatArrayElement->GetText();
    std::vector<float> positions = split<float>(positionsStr, ' ');

    tinyxml2::XMLElement* pElement = root->FirstChildElement("library_geometries")->
    FirstChildElement("geometry")->FirstChildElement("mesh")->
    FirstChildElement("polylist")->FirstChildElement("p");
    std::vector<int> p = split<int>(pElement->GetText(), ' ');

    int numAttributes = 3;
    float vertPositions[p.size()];
    int vi = 0;
    // Rotation matrix to -90 degree around X axis
    glm::vec3 axis = glm::vec3(1.f, 0.f, 0.f);
    glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), axis);
    for (int i = 0; i < p.size(); i += numAttributes)
    {
        int vertPosIndex = p[i];
        float vx = positions[vertPosIndex * 3 + 0];
        float vy = positions[vertPosIndex * 3 + 1];
        float vz = positions[vertPosIndex * 3 + 2];
        glm::vec4 oldPos(vx, vy, vz, 0.f);
        glm::vec4 newPos = rotMatrix * oldPos;
        vertPositions[vi++] = newPos.x;
        vertPositions[vi++] = newPos.y;
        vertPositions[vi++] = newPos.z;
    }

    GLuint vertPosBuffer;
    glGenBuffers(1, &vertPosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertPosBuffer);
    int amount = sizeof(vertPositions) / sizeof(vertPositions[0]);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(GLfloat),
        vertPositions, GL_STATIC_DRAW);

    int amountOfVertices = p.size() / numAttributes;

    VertexBuffersData buffers;
    buffers.amountOfVertices = amountOfVertices;
    buffers.vertPosBuffer = vertPosBuffer;

    return buffers;
}
