#version 430 core

layout(binding = 1, std430) buffer exampleData {
  int x = 7;
  float y = 0.23;
};

void main() {
	int ID = gl_GlobalInvocationID.x;
  exampleData[ID].y = exampleData[ID].x + exampleData[ID].y;
}
