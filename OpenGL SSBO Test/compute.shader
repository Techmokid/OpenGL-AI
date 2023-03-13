#version 430 core

layout(binding = 1, std430) buffer exampleData {
  int x = 7;
  float y = 0.23;
};

void main() {
  exampleData[gl_GlobalInvocationID.x].x = 4;
  exampleData[gl_GlobalInvocationID.x].y = 4;
}

