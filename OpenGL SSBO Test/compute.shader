#version 430 core

layout (local_size_x = 32, local_size_y = 1) in;
layout(std430, binding = 3) buffer layoutName {
  int x = 7;
  float y = 0.23;
};

void main() {
	int ID = gl_GlobalInvocationID.x;
    result[ID].y = result[ID].x + result[ID].y;
}