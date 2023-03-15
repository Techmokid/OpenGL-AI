#version 450 core

struct exampleData{
  int x;
  float y;
};

layout(binding = 1, std430) buffer exampleDataBuff {
  exampleData ED[];
};

void main() {
  ED[gl_GlobalInvocationID.x].x = 4;
  ED[gl_GlobalInvocationID.x].y = 4.0f;
}

