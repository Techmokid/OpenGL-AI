#include <iostream>
#include "functions.h"

int main() {
  std::cout << "Initializing array" << std::endl;
  ssbo_data testArr[512];
  //Put this part on the GPU
  //for (int i = 0; i < sizeof(testArr)/sizeof(testArr[0]); i++) {
  //  testArr[i].y *= testArr[i].x;
  //}
  
  //Initialize the shader
  std::cout << "Initializing Shader" << std::endl;
  GLuint computeHandle = InitializeShader("compute.shader");
  glUseProgram(computeHandle);
  
  //Apply the SSBO
  std::cout << "Applying Shader SSBO" << std::endl;
  GLuint ssbo = 0;
  glGenBuffers(1, &ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(testArr), &testArr, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
  
  //Run the shader program
  std::cout << "Running Shader Program" << std::endl;
  glDispatchCompute(4,1,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
  
  //Get the data back from the GPU
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  
  ssbo_data* ptr = (ssbo_data*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
  for (int i = 0; i < 500; i++) {
  	testArr[i] = ptr[i];
  }
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  
  std::cout << testArr[2].y << std::endl;
  std::cout << "Done" << std::endl;
  return 1;
}
