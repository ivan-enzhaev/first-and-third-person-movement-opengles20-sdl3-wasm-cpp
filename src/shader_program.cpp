#include "shader_program.h"

#include <iostream>
#include <fstream>
#include <iostream>
#include <sstream>

// Helper function for creating shaders
GLuint createShader(const char *shaderSource, int shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
        glDeleteShader(shader); // Don't leak the shader
        std::cout << &(errorLog[0]) << std::endl;
        std::cout << shaderSource << std::endl;
    }
    return shader;
}

// Helper function for creating a shader program
GLuint createShaderProgram(const std::string &vShaderPath,
    const std::string &fShaderPath)
{
    std::ifstream vf(vShaderPath);
    if (!vf.is_open())
    {
        std::cout << "Failed to open the file: " << vShaderPath << std::endl;
        return 0;
    }
    std::stringstream vsstr;
    vf >> vsstr.rdbuf();
    std::string vContent = vsstr.str();
    vf.close();

    std::ifstream ff(fShaderPath);
    if (!ff.is_open())
    {
        std::cout << "Failed to open the file: " << fShaderPath << std::endl;
        return 0;
    }
    std::stringstream fsstr;
    ff >> fsstr.rdbuf();
    std::string fContent = fsstr.str();
    ff.close();

    GLuint program = glCreateProgram();
    GLuint vShader = createShader(vContent.c_str(), GL_VERTEX_SHADER);
    GLuint fShader = createShader(fContent.c_str(), GL_FRAGMENT_SHADER);

    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);
    glUseProgram(program);

    return program;
}
