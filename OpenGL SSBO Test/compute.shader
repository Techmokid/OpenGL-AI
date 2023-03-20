#version 430

struct ssbo_data {
  int x;
  float y;
};

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer mySsboBuffer {
  ssbo_data ED[];
};

void main() {
  uint ID = gl_GlobalInvocationID.x;
  //ED[ID].x = 4;
  //ED[ID].y = 4.0f;
  
  ED[ID].y *= ED[ID].x;
}

