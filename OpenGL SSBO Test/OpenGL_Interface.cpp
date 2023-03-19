#include "functions.h"

// How to use:
//  - Use GetShaderCode(std::string path) to get a shader from a file instead of directly via code
//  - Use StartWindow() to begin. This is required to allow OpenGL to boot up and get everything running
//  - Use CompileShader(const char* shader) to compile the shader and get back it's corresponding GLuint memory ID. For program performance, I recommend only calling this once at the start of your program, then simply saving it's resulting GLuint ID somewhere to have a precompiled version of your shader ready to go
//  - Use StartShaderProgram(GLuint compiledShader) to start the shader up ready to be used
//  - Use InitializeShader() in order to skip needing to call GetShaderCode(), CompileShader(), and StartShaderProgram()

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
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Optional
  
  glfwSetErrorCallback(error_callback);
  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", NULL, NULL);
  if (!window) {
    glfwTerminate();
    std::cout << "glfwCreateWindow() failed to start" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  
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

GLuint StartShaderProgram(GLuint computeShader) {
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

//void ShutDownOpenGL(GLuint program, GLuint compute_shader, std::vector<GLuint> ssbos = std::vector<GLuint>()) {
//  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
//  glDeleteProgram(program);
//  glDeleteShader(compute_shader);
//  while (ssbos.size() != 0) {
//    glDeleteBuffers(1, &ssbos[0]);
//    ssbos.erase(ssbos.begin());
//  }
//  glfwTerminate();
//}
