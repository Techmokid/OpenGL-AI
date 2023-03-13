#ifndef functionsList
#define functionsList
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include <fstream>
#include <string.h>
#include <iostream>
#endif

std::string GetShaderCode(std::string shaderPath);
static void error_callback(int error, const char* description);
void StartWindow();
void SetBufferValue(auto arr);
GLuint CompileShader(const char* computeShaderSource);
GLuint StartShaderProgram(GLuint computeShader);
GLuint InitializeShader(std::string shaderPath);
