#include "Shader.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GLuint compileShader(GLenum shaderType, const char * sourceBuffer, int bufferSize)
{
    GLuint shaderObject = glCreateShader(shaderType);
    const char * sc[1] = { sourceBuffer };
    glShaderSource(shaderObject, 1, sc, NULL);
    glCompileShader(shaderObject);
    checkCompileError(shaderObject, sc);
    return shaderObject;
}

GLuint compileShaderFromFile(GLenum shaderType, const char * path)
{
    FILE * shaderFileDesc = fopen( path, "rb" );
    if (!shaderFileDesc)
    {
        std::cerr << "enable to open file" << std::endl;
        return 0;
    }

    fseek ( shaderFileDesc , 0 , SEEK_END );
    long fileSize = ftell ( shaderFileDesc );
    rewind ( shaderFileDesc );
    char * buffer = new char[fileSize + 1];
    fread( buffer, 1, fileSize, shaderFileDesc );
    buffer[fileSize] = '\0';
    GLuint shaderObject = compileShader(shaderType, buffer, fileSize );
    delete[] buffer;
    return shaderObject;
}


int checkCompileError(GLuint shader, const char ** sourceBuffer)
{
    // Get error log size and print it eventually
    int logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        char * log = new char[logLength];
        glGetShaderInfoLog(shader, logLength, &logLength, log);
        char *token, *string;
        string = strdup(sourceBuffer[0]);
        int lc = 0;
        while ((token = strsep(&string, "\n")) != NULL) {
           printf("%3d : %s\n", lc, token);
           ++lc;
        }
        fprintf(stderr, "Compile : %s", log);
        delete[] log;
    }
    // If an error happend quit
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
        return -1;
    return 0;
}
