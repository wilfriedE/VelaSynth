#ifndef SHADER_H
#define SHADER_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

class Shader{
    public:
        Shader();
        Shader(string vertexShader, string fragmentShader);
        ~Shader();

        void startShader();
        void stopShader();
        GLuint getProgram();
        void setUniform1i(string identifier, GLint value);
        void setUniform1f(string identifier, GLfloat value);
        void cleanUpShader();

    protected:
        GLuint programID;
        GLuint vertexShaderID;
        GLuint fragmentShaderID;

        GLuint loadShader(string sourceFileName, GLenum type);

};

#endif // SHADER_H