#ifndef functionsList
#define functionsList
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include <fstream>
#include <string.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <optional>
#endif

struct ssbo_data {
  int x = 7;
  float y = 0.23;
};

std::string GetShaderCode(std::string shaderPath);
static void error_callback(int error, const char* description);
void StartWindow();
GLuint CompileShader(const char* computeShaderSource);
GLuint StartShaderProgram(GLuint computeShader);
GLuint StartShaderProgram(GLuint computeShader, std::vector<GLuint*> ssbo);
GLuint InitializeShader(std::string shaderPath);
void checkShaderCompileStatus(GLuint shader);
void ShutDownOpenGL();