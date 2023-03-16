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
  glUseProgram(computeHandle);													// Use the compute shader we just compiled
  
  //Apply the SSBO
  std::cout << "Applying Shader SSBO" << std::endl;
  GLuint ssbo = 1;
  glGenBuffers(1,&ssbo);  // Generate 1 new buffer
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);					// Bind the new buffer to our accessable memory
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ssbo_data) * (sizeof(testArr)/sizeof(testArr[0])), &testArr, GL_STATIC_DRAW);	// Insert data to buffer
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);	// Now bind the buffer to it's shader binding index
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);						// Unbind our code from the compute shader
  
  //Run the shader program
  std::cout << "Running Shader Program" << std::endl;
  glDispatchCompute(512,1,1);														// Dispatch the task to run on the GPU
  //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);				// Wait for the GPU to finish
  glMemoryBarrier(GL_ALL_BARRIER_BITS);				// Wait for the GPU to finish
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);			// Unbind our code from the compute shader
  
  //Get the data back from the GPU
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);					// Bind to the SSBO array on the GPU
  ssbo_data* ptr = (ssbo_data*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);	// Copy the data out from the GPU to our code
  for (int i = 0; i < 512; i++) { testArr[i] = ptr[i]; }// Copy the data from our pointer to our variable
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);							// Disconnect from
  
  std::cout << testArr[2].y << std::endl;
  std::cout << ptr[0].y << std::endl;
  std::cout << "Done" << std::endl;
  return 1;
}
