#include <iostream>
#include "functions.h"

int main() {
  std::cout << "Initializing array" << std::endl;
  ssbo_data testArr[500];
  //Put this part on the GPU
  //for (int i = 0; i < sizeof(testArr)/sizeof(testArr[0]); i++) {
  //  testArr[i].y *= testArr[i].x;
  //}
  
  //Initialize the shader
  std::cout << "Initializing Shader" << std::endl;
  GLuint computeHandle = InitializeShader("compute.shader");
  glUseProgram(computeHandle);
  
  vector<GLfloat> initPos;
	int num_numeros = 12;

	for (int i = 0; i < num_numeros; i++){
		initPos.push_back(0.0f);
	}

	glGenBuffers(1, &SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, num_numeros * sizeof(GLfloat), &initPos, GL_DYNAMIC_DRAW);

	glDispatchCompute(num_numeros/2, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);

	GLfloat *ptr;
	ptr = (GLfloat *) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	initPos.clear();

	for (int i = 0; i < num_numeros; i++){
		initPos.push_back(ptr[i]);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	for (int i = 0; i < num_numeros; i++){
		cout << "p" << i << ": " << initPos[i] <<  endl;
	}
	cout << endl;
  
  return 1;
}
