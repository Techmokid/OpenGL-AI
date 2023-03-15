#include "functions.h"

std::string GetShaderCode(std::string shaderPath) {
	// Get the compute shader from disk
  std::ifstream myfile;
  myfile.open(shaderPath);
  std::string computeShaderSourceString;
  
  char c;
  while(myfile.get(c)) {
  	computeShaderSourceString += c;
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
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
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
  //if (!GLEW_VERSION_2_1) {
  //	std::cout << "GLEW version does not support 2.1" << std::endl;
  //	exit(EXIT_FAILURE);
  //}
}

GLuint CompileShader(const char* computeShaderSource) {
  GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(computeShader, 1, &computeShaderSource, NULL);
  glCompileShader(computeShader);
  
  GLint result = GL_FALSE;
  int logLength;
  
  // Check shader compilation status
  glGetShaderiv(computeShader, GL_COMPILE_STATUS, &result);
  glGetShaderiv(computeShader, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
  	std::vector<char> shaderErrorMessage(logLength + 1);
  	glGetShaderInfoLog(computeShader, logLength, NULL, &shaderErrorMessage[0]);
  	std::cout << &shaderErrorMessage[0] << std::endl;
  } else {
  	std::cout << "Shader compiled successfully" << std::endl;
  }
  
  return computeShader;
}

GLuint StartShaderProgram(GLuint computeShader) {
	GLuint computeProgram = glCreateProgram();
  glAttachShader(computeProgram, computeShader);
  glLinkProgram(computeProgram);
  return computeProgram;
}

GLuint InitializeShader(std::string shaderPath) {
  std::string computeShaderCode = GetShaderCode(shaderPath);
  
  StartWindow();
  
  //const char* versionStr = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	//std::cout << "GLSL version supported by the current context: " << versionStr << std::endl;
  std::cout << "OpenGL version supported by the current context: " << glGetString(GL_VERSION) << std::endl;
  
  char* cstr = new char[computeShaderCode.length() + 1];
  std::strcpy(cstr, computeShaderCode.c_str());
  
  GLuint computeHandle = CompileShader(cstr);
  StartShaderProgram(computeHandle);
  
  //std::cout << computeShaderCode;
  return computeHandle;
}

