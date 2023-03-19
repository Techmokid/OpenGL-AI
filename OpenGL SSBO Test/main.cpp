#include "functions.h"

//ssbo_data testArr[512];
//for (int i = 0; i < sizeof(testArr)/sizeof(testArr[0]); i++) {
//  testArr[i].y *= testArr[i].x;
//}

int main() {
  std::cout << "Initializing array" << std::endl;
  ssbo_data testArr[512];
  int arrLen = sizeof(testArr)/sizeof(testArr[0]);
  
  //Starting computation window
  StartWindow();
  
  //Apply the SSBO
  std::cout << "Applying Shader SSBO" << std::endl;
  GLuint ssbo;
  glGenBuffers(1, &ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, arrLen * sizeof(ssbo_data), NULL, GL_STATIC_DRAW);
  
  // Map the SSBO to client memory and then write to it
  ssbo_data* my_data = (ssbo_data*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, arrLen * sizeof(ssbo_data), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
  for (int i = 0; i < arrLen; i++) {
    my_data[i].x = 7;
    my_data[i].y = 0.23;
  }
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
  
  //Initialize the shader
  std::cout << "Initializing Shader" << std::endl;
  GLuint computeHandle = InitializeShader("compute.shader");
  
  //Run the shader program
  std::cout << "Running Shader Program" << std::endl;
  glDispatchCompute(512/64,1,1);								// Dispatch the task to run on the GPU
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);				// Wait for the GPU to finish
  
  // Map the SSBO to client memory again to read the result
  std::cout << "Reading Out Results" << std::endl;
  my_data = (ssbo_data*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, arrLen * sizeof(ssbo_data), GL_MAP_READ_BIT);
  
  for (int i = 0; i < arrLen; i++) {
    std::cout << my_data[i].y << std::endl;
  }
  
  std::cout << "Shutting Down" << std::endl;
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  ShutDownOpenGL();
  return 1;
}
