#include <iostream>
#include "functions.h"

struct myStruct {
  int x = 7;
  float y = 0.23;
};

int main() {
  std::cout << "Initializing array" << std::endl;
  myStruct testArr[50000];
  std::cout << "Iterating" << std::endl;
  
  //Put this part on the GPU
  for (int i = 0; i < sizeof(testArr)/sizeof(testArr[0]); i++) {
    testArr[i].y *= testArr[i].x;
  }
  //End of GPU part
  std::cout << "Done Iterating" << std::endl;
  
  
  
  //Initialize the shader
  std::cout << "Initializing Shader" << std::endl;
  GLuint computeHandle = InitializeShader("compute.shader");
  
  //Run the shader program
  std::cout << "Running Shader Program" << std::endl;
  glUseProgram(computeHandle);
  glDispatchCompute(128, 128, 1);
  glMemoryBarrier(GL_ALL_BARRIER_BITS);
  
  std::cout << "Done" << std::endl;
  return 1;
}
