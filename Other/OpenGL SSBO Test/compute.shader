#version 430

struct ssbo_data_1 {
  int x;
  float y;
};

struct ssbo_data_2 {
  int z;
};

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer mySsboBuffer1 {
  ssbo_data_1 ED1[];
};
layout(std430, binding = 1) buffer mySsboBuffer2 {
  ssbo_data_2 ED2[];
};

void main() {
  uint ID = gl_GlobalInvocationID.x;
  ED1[ID].y *= ED1[ID].x + ED2[ID].z;
}

