#include "functions.h"

int main() {
  std::cout << "Initializing array" << std::endl;
  std::vector<ssbo_data_1> testVec1(8);
  std::vector<ssbo_data_2> testVec2(8);
  for (int i = 0; i < 8; i++) {
    testVec1[i].x = 14;
    testVec1[i].y = 0.23;
    testVec2[i].z = 7;
  }
  
  //Starting computation window
  StartWindow();
  
  //Initialize the shader
  std::cout << "Initializing Shader" << std::endl;
  GLuint computeHandle = InitializeShader("compute.shader");
  
  //Apply the SSBOs
  std::cout << "Applying Shader SSBO" << std::endl;
  GLuint ssbos[2];
  glGenBuffers(2, ssbos);
  Set_SSBO_Buffer(testVec1, ssbos[0], 0);
  Set_SSBO_Buffer(testVec2, ssbos[1], 1);
  
  //Run the shader program
  std::cout << "Running Shader Program" << std::endl;
  glDispatchCompute(8,1,1);								// Dispatch the task to run on the GPU
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);				// Wait for the GPU to finish
  
  // Map the SSBO to client memory again to read the result
  std::cout << "Reading Out Results" << std::endl;
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[0]);
  ssbo_data_1* myOutputData1 = (ssbo_data_1*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, testVec1.size() * sizeof(ssbo_data_1), GL_MAP_READ_BIT);
  //ssbo_data_2* myOutputData2 = (ssbo_data_2*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 1, arrLen2 * sizeof(ssbo_data_2), GL_MAP_READ_BIT);
  
  if (myOutputData1 != nullptr) {
    for (int i = 0; i < testVec1.size(); i++) {
      std::cout << myOutputData1[i].y << std::endl;
    }
  } else {
    std::cout << "Failed to map buffer for reading." << std::endl;
  }
  
  std::cout << "Shutting Down" << std::endl;
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  ShutDownOpenGL();
  return 1;
}
