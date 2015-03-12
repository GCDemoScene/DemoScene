#pragma once
#include <GL/glew.h>

GLuint compileShader(GLenum shaderType, const char * sourceBuffer, int bufferSize);
GLuint compileShaderFromFile(GLenum shaderType, const char * path);
int checkCompileError(GLuint shader, const char ** sourceBuffer);
