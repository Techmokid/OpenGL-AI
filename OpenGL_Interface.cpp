#include "functions.h"

// How to use:
//  - Use GetShaderCode(std::string path) to get a shader from a file instead of directly via code
//  - Use StartWindow() to begin. This is required to allow OpenGL to boot up and get everything running
//  - Use CompileShader(const char* shader) to compile the shader and get back it's corresponding GLuint memory ID.
//    For program performance, I recommend only calling this once at the start of your program, then simply saving it's
//    resulting GLuint ID somewhere to have a precompiled version of your shader ready to go
//  - Use StartShaderProgram(GLuint compiledShader) to start the shader up ready to be used
//  - Use InitializeShader() in order to skip needing to call GetShaderCode(), CompileShader(), and StartShaderProgram()

std::vector<GLuint> shaderPrograms = std::vector<GLuint>();
std::vector<GLuint> shaderShader = std::vector<GLuint>();

std::string GetShaderCode(std::string shaderPath) {
  // Get the compute shader from disk
  std::ifstream myfile;
  myfile.open(shaderPath);
  
  if (myfile.fail()) {
	  printFormatted("OpenGL", "Error", "OpenGL file could not retreive filedata from file that does not exist: " + shaderPath);
	  quit();
  }
  
  std::string computeShaderSourceString;
  
  char c;
  while(myfile.get(c)) {
  	computeShaderSourceString += c;
  }
  myfile.close();
  return computeShaderSourceString;
}

static void error_callback(int error, const char* description) {
  std::string descriptionString(description);
  printFormatted("OpenGL","Error","Internal Error: " + descriptionString + "\n");
  
  switch (error) {
    case GLFW_NOT_INITIALIZED:
      fprintf(stderr, "GLFW has not been initialized.\n");
      break;
    case GLFW_NO_CURRENT_CONTEXT:
      fprintf(stderr, "No current OpenGL or OpenGL ES context.\n");
      break;
    case GLFW_INVALID_ENUM:
      fprintf(stderr, "Invalid enum value passed as argument.\n");
      break;
    case GLFW_INVALID_VALUE:
      fprintf(stderr, "Invalid value passed as argument.\n");
      break;
    case GLFW_OUT_OF_MEMORY:
      fprintf(stderr, "Out of memory.\n");
      break;
    case GLFW_API_UNAVAILABLE:
      fprintf(stderr, "Requested OpenGL or OpenGL ES version is unavailable.\n");
      break;
    case GLFW_VERSION_UNAVAILABLE:
      fprintf(stderr, "Requested OpenGL or OpenGL ES version is not supported by the graphics driver.\n");
      break;
    case GLFW_PLATFORM_ERROR:
      fprintf(stderr, "A platform-specific error occurred.\n");
      break;
    case GLFW_FORMAT_UNAVAILABLE:
      fprintf(stderr, "Requested pixel format or framebuffer configuration is not available.\n");
      break;
    default:
      fprintf(stderr, "Unknown error.\n");
      break;
  }
  std::cout << std::endl << std::endl;
  quit();
}

void StartWindow() {
  srand((unsigned)time(NULL));

  glfwInit();
  if (!glfwInit()) {
    printFormatted("OpenGL","Error","glfwInit() failed to start");
    quit();
  }
  
  glfwSetErrorCallback(error_callback);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Optional
  
  GLFWwindow* window = glfwCreateWindow(1, 1, "OpenGL Compute-shader Window", NULL, NULL);
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

GLuint CompileShader(std::string computeShaderCode) {
  char* cstr = new char[computeShaderCode.length() + 1];
  std::strcpy(cstr, computeShaderCode.c_str());
  return CompileShader(cstr);
}

GLuint CompileShader(const char* computeShaderSource) {
  GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(computeShader, 1, &computeShaderSource, NULL);
  glCompileShader(computeShader);
  checkShaderCompileStatus(computeShader);
  
  bool addVal = true;
  for (int i = 0; i < shaderShader.size(); i++) {
    if (shaderShader[i] == computeShader)
      addVal = false;
  }
  if (addVal)
    shaderShader.push_back(computeShader);
  
  return computeShader;
}

GLuint StartShaderProgram(GLuint computeShader) {
  GLuint computeProgram = glCreateProgram();
  glAttachShader(computeProgram, computeShader);
  //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
  glLinkProgram(computeProgram);
  glUseProgram(computeProgram);
  
  shaderPrograms.push_back(computeProgram);
  
  return computeProgram;
}

GLuint InitializeShader(std::string shaderPath) {
  std::string computeShaderCode = GetShaderCode(shaderPath);
  std::string computeShaderCodeCopy = computeShaderCode;
  computeShaderCodeCopy = RemoveSpecificCharacter(computeShaderCodeCopy,' ');
  computeShaderCodeCopy = RemoveSpecificCharacter(computeShaderCodeCopy,'\t');
  computeShaderCodeCopy = RemoveSpecificCharacter(computeShaderCodeCopy,'\n');
  computeShaderCodeCopy = RemoveSpecificCharacter(computeShaderCodeCopy,'\r');
  if (computeShaderCodeCopy.length() == 0)
	  printFormatted("OpenGL", "Warning", "Compute shader is empty: " + shaderPath);
  
  //const char* versionStr = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
  //std::cout << "GLSL version supported by the current context: " << versionStr << std::endl;
  //std::cout << "OpenGL version supported by the current context: " << glGetString(GL_VERSION) << std::endl;
  
  char* cstr = new char[computeShaderCode.length() + 1];
  std::strcpy(cstr, computeShaderCode.c_str());
  
  GLuint computeHandle = CompileShader(cstr);
  
  StartShaderProgram(computeHandle);
  
  return computeHandle;
}

void checkShaderCompileStatus(GLuint shader) {
  GLint success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
    char log[1024];
    GLsizei length;
    glGetShaderInfoLog(shader, sizeof(log), &length, log);
    printFormatted("OpenGL", "Error", "Shader compilation failed: " + std::string(log));
	quit();
  }
}

void ShutDownOpenGL() {
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  
  while (shaderPrograms.size() != 0) {
	glDeleteProgram(shaderPrograms[0]);
	shaderPrograms.erase(shaderPrograms.begin());
  }
  while (shaderShader.size() != 0) {
    glDeleteShader(shaderShader[0]);
	shaderShader.erase(shaderShader.begin());
  }
  
  //while (ssbos.size() != 0) {
  //  glDeleteBuffers(1, &ssbos[0]);
  //  ssbos.erase(ssbos.begin());
  //}
  
  glfwTerminate();
}
