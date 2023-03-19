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
  glfwInit();
  if (!glfwInit()) {
    std::cout << "glfwInit() failed to start" << std::endl;
    exit(EXIT_FAILURE);
  }
	
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
  glfwSetErrorCallback(error_callback);
  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Computation Window", NULL, NULL);
  if (!window) {
    glfwTerminate();
    std::cout << "glfwCreateWindow() failed to start" << std::endl;
    exit(EXIT_FAILURE);
  }
	
  glfwMakeContextCurrent(window);
  glewExperimental=true;
  
  GLenum err = glewInit();
  if (err != GLEW_OK) {
  	std::cout << "glewInit() failed to start" << std::endl;
  	exit(EXIT_FAILURE);
  }
}

GLuint CompileShader(const char* computeShaderSource) {
  GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(computeShader, 1, &computeShaderSource, NULL);
  glCompileShader(computeShader);
  checkShaderCompileStatus(computeShader);
  
  return computeShader;
}

GLuint StartShaderProgram(GLuint computeShader) { StartShaderProgram(computeShader,NULL); }
GLuint StartShaderProgram(GLuint computeShader, std::vector<GLuint*> ssbo) {
  GLuint computeProgram = glCreateProgram();
  glAttachShader(computeProgram, computeShader);
  //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
  glLinkProgram(computeProgram);
  glUseProgram(computeProgram);
  return computeProgram;
}

GLuint InitializeShader(std::string shaderPath) {
  std::string computeShaderCode = GetShaderCode(shaderPath);
  
  //const char* versionStr = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
  //std::cout << "GLSL version supported by the current context: " << versionStr << std::endl;
  //std::cout << "OpenGL version supported by the current context: " << glGetString(GL_VERSION) << std::endl;
  
  char* cstr = new char[computeShaderCode.length() + 1];
  std::strcpy(cstr, computeShaderCode.c_str());
  
  GLuint computeHandle = CompileShader(cstr);
  StartShaderProgram(computeHandle);
  
  //std::cout << computeShaderCode;
  return computeHandle;
}

void checkShaderCompileStatus(GLuint shader) {
  GLint success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
    char log[1024];
    GLsizei length;
    glGetShaderInfoLog(shader, sizeof(log), &length, log);
    std::cerr << "Shader compilation failed: " << log << std::endl;
  }
}

void ShutDownOpenGL(GLuint program, GLuint compute_shader) { ShutDownOpenGL(program,compute_shader,NULL); }
void ShutDownOpenGL(GLuint program, GLuint compute_shader, std::vector<GLuint> ssbos) {
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  glDeleteProgram(program);
  glDeleteShader(compute_shader);
  if (ssbos != NULL) {
    while (ssbos.size() != 0) {
	  glDeleteBuffers(1, &ssbos[0]);
	  ssbos.erase(myvector.begin());
	}
  }
  glfwTerminate();
}