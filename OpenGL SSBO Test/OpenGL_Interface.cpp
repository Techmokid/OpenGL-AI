#include "functions.h"

std::string GetShaderCode(std::string shaderPath) {
	// Get the compute shader from disk
  std::ifstream myfile;
  myfile.open(shaderPath);
  std::string computeShaderSourceString;
  if (myfile.is_open()) {
	  while (myfile) {
		  computeShaderSourceString += myfile.get();
	  }
  }
  myfile.close();
  return computeShaderSourceString;
}

static void error_callback(int error, const char* description) {fprintf(stderr, "Error: %s\n", description);}
void StartWindow() {
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		std::cout << "glfwInit() failed to start" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	window = glfwCreateWindow(640,480, "Simple Example", NULL, NULL);
	if (!window) {
		glfwTerminate();
		std::cout << "glfwCreateWindow() failed to start" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	glfwMakeContextCurrent(window);
  
  //glewExperimental=true;
  GLenum err = glewInit();
  if (err != GLEW_OK) {
  	std::cout << "glewInit() failed to start" << std::endl;
  	exit(EXIT_FAILURE);
  }
  if (!GLEW_VERSION_2_1) {
  	std::cout << "GLEW version does not support 2.1" << std::endl;
  	exit(EXIT_FAILURE);
  }
}

void SetBufferValue(auto arr) {
	GLuint ssbo;
  glGenBuffers(1,&ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  //glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(arr), arr, GLenum usage);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
}

GLuint CompileShader(const char* computeShaderSource) {
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(computeShader, 1, &computeShaderSource, NULL);
  glCompileShader(computeShader);
  return computeShader;
}

GLuint StartShaderProgram(GLuint computeShader) {
	GLuint computeProgram = glCreateProgram();
  glAttachShader(computeProgram, computeShader);
  glLinkProgram(computeProgram);
  return computeProgram;
}

GLuint InitializeShader(std::string shaderPath) {
	const char* computeShaderSource = GetShaderCode(shaderPath).c_str();
  StartWindow();
  
  GLuint computeHandle = CompileShader(computeShaderSource);
  StartShaderProgram(computeHandle);
  return computeHandle;
}
