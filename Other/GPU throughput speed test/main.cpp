#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>

// Function to create a buffer and upload data
void uploadData(GLsizeiptr size, const void* data) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buffer);
}

int main() {
    // Initialize GLFW and create a window
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Benchmark", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        return -1;
    }

    const size_t dataSize = 1024 * 1024 * 1024; // 1GB
    char* data = new char[dataSize]; // Allocate 1GB
    
    // Run the benchmark
    auto start = std::chrono::high_resolution_clock::now();
    uploadData(dataSize, data);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "Time to upload 1GB: " << diff.count() << " s" << std::endl;

    delete[] data;
    glfwTerminate();
    return 0;
}
