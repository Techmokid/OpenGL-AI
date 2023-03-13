#version 430 core

struct exampleData{
	int x;
  float y;
};

layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0, std430) buffer exampleDataBuff {
  exampleData ED[];
};

void main() {
  ED[gl_GlobalInvocationID.x].x = 4;
  ED[gl_GlobalInvocationID.x].y = 4;
}

