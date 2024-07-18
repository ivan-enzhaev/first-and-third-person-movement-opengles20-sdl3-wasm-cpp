#ifndef VERTEX_BUFFERS_H
#define VERTEX_BUFFERS_H

#include <SDL3/SDL_opengles2.h>
#include <string>

struct VertexBuffersData
{
    int amountOfVertices;
    GLuint vertPosBuffer;
};

class VertexBuffers
{
public:
    VertexBuffers();
    static VertexBuffersData init(const std::string &path);
};

#endif // VERTEX_BUFFERS_H
