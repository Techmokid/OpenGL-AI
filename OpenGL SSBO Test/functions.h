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

struct ssbo_data_1 {
  int x = 0;
  float y = 0;
};

struct ssbo_data_2 {
  int z = 0;
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

template<typename T>
void Set_SSBO_Buffer(std::vector<T> &obj, GLuint ssbo, int buffer_index) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, obj.size() * sizeof(T), obj.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, buffer_index, ssbo);
}
