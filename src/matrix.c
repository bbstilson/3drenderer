#include "matrix.h"
#include <math.h>

mat4_t mat4_identity(void) {
  mat4_t id = {{
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1},
  }};
  return id;
}

mat4_t mat4_make_scale(vec3_t v) {
  mat4_t m = mat4_identity();
  m.m[0][0] = v.x;
  m.m[1][1] = v.y;
  m.m[2][2] = v.z;
  return m;
}

mat4_t mat4_make_translation(vec3_t v) {
  mat4_t m = mat4_identity();
  m.m[0][3] = v.x;
  m.m[1][3] = v.y;
  m.m[2][3] = v.z;
  return m;
}

mat4_t mat4_make_rotation_x(float a) {
  float c = cos(a);
  float s = sin(a);
  mat4_t m = mat4_identity();
  m.m[1][1] = c;
  m.m[1][2] = -s;
  m.m[2][1] = s;
  m.m[2][2] = c;
  return m;
}
mat4_t mat4_make_rotation_y(float a) {
  float c = cos(a);
  float s = sin(a);
  mat4_t m = mat4_identity();
  m.m[0][0] = c;
  m.m[0][2] = s;
  m.m[2][0] = -s;
  m.m[2][2] = c;
  return m;
}
mat4_t mat4_make_rotation_z(float a) {
  float c = cos(a);
  float s = sin(a);
  mat4_t m = mat4_identity();
  m.m[0][0] = c;
  m.m[0][1] = -s;
  m.m[1][0] = s;
  m.m[1][1] = c;

  return m;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
  float x = (m.m[0][0] * v.x) + (m.m[0][1] * v.y) + (m.m[0][2] * v.z) + (m.m[0][3] * v.w);
  float y = (m.m[1][0] * v.x) + (m.m[1][1] * v.y) + (m.m[1][2] * v.z) + (m.m[1][3] * v.w);
  float z = (m.m[2][0] * v.x) + (m.m[2][1] * v.y) + (m.m[2][2] * v.z) + (m.m[2][3] * v.w);
  float w = (m.m[3][0] * v.x) + (m.m[3][1] * v.y) + (m.m[3][2] * v.z) + (m.m[3][3] * v.w);
  return (vec4_t){x, y, z, w};
}
