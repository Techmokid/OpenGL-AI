#include <iostream>
#include "functions.h"

struct ssbo_data {
  int x = 7;
  float y = 0.23;
};

void checkGLErrors(const char* file, int line) {
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error (" << file << ":" << line << "): " << err << std::endl;
  }
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

int main() {
  // Initialize GLFW and GLEW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", NULL, NULL);
  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  glewInit();

  // Create a SSBO
  GLuint ssbo;
  glGenBuffers(1, &ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, 100 * sizeof(ssbo_data), NULL, GL_STATIC_DRAW);

  // Map the SSBO to client memory
  ssbo_data* my_data = (ssbo_data*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 100 * sizeof(ssbo_data), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

  // Initialize the SSBO data
  for (int i = 0; i < 100; i++) {
    my_data[i].x = 7;
    my_data[i].y = 0.23;
  }

  // Unmap the SSBO
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

  // Create a compute shader
  GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);

  // Compile the compute shader
  const char* compute_shader_source = "#version 430\n"
									  "\nstruct ssbo_data {\nint x;\nfloat y;\n};\n"
                                      "layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;\n"
                                      "layout(std430, binding = 0) buffer my_ssbo {\n"
                                      "  ssbo_data my_data[];\n"
                                      "};\n"
                                      "void main() {\n"
                                      "  uint i = gl_GlobalInvocationID.x;\n"
                                      "  my_data[i].y *= my_data[i].x;\n"
                                      "}\n";
  glShaderSource(compute_shader, 1, &compute_shader_source, NULL);
  glCompileShader(compute_shader);
  checkShaderCompileStatus(compute_shader);

  // Create a program object and attach the compute shader to it
  GLuint program = glCreateProgram();
  glAttachShader(program, compute_shader);

  

  // Link the program object and use it
  glLinkProgram(program);
  glUseProgram(program);
  
  // Bind the SSBO to a binding point
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

  // Dispatch the compute shader
  glDispatchCompute(100 / 64, 1, 1);

  // Wait for the compute shader to finish
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // Map the SSBO to client memory again to read the result
  my_data = (ssbo_data*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 100 * sizeof(ssbo_data), GL_MAP_READ_BIT);

  // Print the result
  for (int i = 0; i < 100; i++) {
    std::cout << my_data[i].y << std::endl;
  }
  
  // Unmap the SSBO again
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

  // Delete the program object, compute shader, and SSBO
  glDeleteProgram(program);
  glDeleteShader(compute_shader);
  glDeleteBuffers(1, &ssbo);

  // Terminate GLFW
  glfwTerminate();

  return 0;
}