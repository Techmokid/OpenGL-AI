#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <fstream>

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
  
  
  
  
  
  std::cout << "Reading Compute Shader" << std::endl;
  std::ifstream myfile;
  myfile.open("compute.shader");
  std::string computeShaderSourceString;
  if (myfile.is_open()) {
	  while (myfile) {
		  computeShaderSourceString += myfile.get();
	  }
  }
  myfile.close();
  const char* computeShaderSource = computeShaderSourceString.c_str();
  
  //Initialize the shader
  std::cout << "Initializing Shader" << std::endl;
  GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(computeShader, 1, &computeShaderSource, NULL);
  glCompileShader(computeShader);
  
  //Initialize the shader program
  std::cout << "Initializing Shader Program" << std::endl;
  GLuint computeProgram = glCreateProgram();
  glAttachShader(computeProgram, computeShader);
  glLinkProgram(computeProgram);
  
  //Run the shader program
  std::cout << "Running Shader Program" << std::endl;
  glUseProgram(computeProgram);
  glDispatchCompute(128, 128, 1);
  glMemoryBarrier(GL_ALL_BARRIER_BITS);
  
  std::cout << "Done" << std::endl;
  return 1;
}
