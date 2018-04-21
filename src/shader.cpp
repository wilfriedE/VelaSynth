#include "shader.hpp"


Shader::Shader(){
}

Shader::Shader(string vertexShader, string fragmentShader){
    vertexShaderID = loadShader(vertexShader, GL_VERTEX_SHADER);
    fragmentShaderID = loadShader(fragmentShader, GL_FRAGMENT_SHADER);

    //create a program and attach the shader objects
    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);

    //Link the shader program and verify
    glLinkProgram(programID);

    GLint linked;
    glGetProgramiv(programID, GL_LINK_STATUS, &linked);

    if(!linked){
        cerr << "Error linking program with shaders " << vertexShader
        << " and " << fragmentShader << endl;
    }
}

void Shader::startShader(){
    glUseProgram(programID);
}

void Shader::stopShader(){
    glUseProgram(0);
}

GLuint Shader::getProgram(){
    return programID;
}

void Shader::setUniform1i(string identifier, GLint value){
    glUniform1i(glGetUniformLocation(programID, identifier.c_str()),value);
}

void Shader::setUniform1f(string identifier, GLfloat value){
    glUniform1f(glGetUniformLocation(programID, identifier.c_str()),value);
}

void Shader::cleanUpShader(){
    stopShader();
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    glDeleteProgram(programID);
}

GLuint Shader::loadShader(string sourceFileName, GLenum type){
    ifstream infile;

    infile.open(sourceFileName);

    if(infile){
        string shader = "", line;

       // concatenate each line into a single string
        while(getline(infile, line)){
            shader += line;
            shader += "\n";
        }

        //Create a shader object to attach the source shader string to and compile
        GLuint shaderID = glCreateShader(type);

        if(shaderID != 0){
            const char* c_str = shader.c_str();
            glShaderSource(shaderID, 1, &c_str, NULL);
            glCompileShader(shaderID);

            GLint compiled;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);

            if(!compiled){
                cerr << sourceFileName << " failed to compile" << endl;

                GLint eLogSize;
                //Query for the current size of the error log
                glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &eLogSize);
                char* infoLog = new char[eLogSize];

                glGetShaderInfoLog(shaderID, eLogSize, NULL, infoLog);

                cerr << infoLog << endl;
                delete [] infoLog;
                exit(EXIT_FAILURE);
            }

        }else{
            cerr << "Error occurred creating shader of type " << type << endl;
            exit(EXIT_FAILURE);
        }

        return shaderID;

    }else{
         cerr << "Shader file " << sourceFileName << " could not be found!" << endl;
         exit(EXIT_FAILURE);
    }
}

Shader::~Shader(){
    stopShader();
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    glDeleteProgram(programID);
}
