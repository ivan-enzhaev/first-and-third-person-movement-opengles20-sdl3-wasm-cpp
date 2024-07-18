#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <SDL3/SDL_opengles2.h>
#include <string>

// Helper function for creating shaders
GLuint createShader(const char *shaderSource, int shaderType);

// Helper function for creating a shader program
GLuint createShaderProgram(const std::string &vShaderPath, const std::string &fShaderPath);

#endif // SHADER_PROGRAM_H
