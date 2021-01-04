#include "vector.h"

#include <math.h>

float vec2_length(vec2_t v) { return sqrt(pow(v.x, 2) + pow(v.y, 2)); }
float vec3_length(vec3_t v) { return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2)); }

vec2_t vec2_add(vec2_t v1, vec2_t v2) {
  return (vec2_t){
      .x = v1.x + v2.x,
      .y = v1.y + v2.y,
  };
}

vec3_t vec3_add(vec3_t v1, vec3_t v2) {
  return (vec3_t){
      .x = v1.x + v2.x,
      .y = v1.y + v2.y,
      .z = v1.z + v2.z,
  };
}

vec2_t vec2_sub(vec2_t v1, vec2_t v2) {
  return (vec2_t){
      .x = v1.x - v2.x,
      .y = v1.y - v2.y,
  };
}

vec3_t vec3_sub(vec3_t v1, vec3_t v2) {
  return (vec3_t){
      .x = v1.x - v2.x,
      .y = v1.y - v2.y,
      .z = v1.z - v2.z,
  };
}

vec2_t vec2_mul(vec2_t v, float factor) {
  return (vec2_t){
      .x = v.x * factor,
      .y = v.y * factor,
  };
}
vec3_t vec3_mul(vec3_t v, float factor) {
  return (vec3_t){
      .x = v.x * factor,
      .y = v.y * factor,
      .z = v.z * factor,
  };
}

vec2_t vec2_div(vec2_t v, float factor) {
  return (vec2_t){
      .x = v.x / factor,
      .y = v.y / factor,
  };
}
vec3_t vec3_div(vec3_t v, float factor) {
  return (vec3_t){
      .x = v.x / factor,
      .y = v.y / factor,
      .z = v.z / factor,
  };
}

vec3_t vec3_cross(vec3_t v1, vec3_t v2) {
  return (vec3_t){
      .x = v1.y * v2.z - v1.z * v2.y,
      .y = v1.z * v2.x - v1.x * v2.z,
      .z = v1.x * v2.y - v1.y * v2.x,
  };
}

float vec2_dot(vec2_t v1, vec2_t v2) { return (v1.x * v2.x) + (v1.y * v2.y); }
float vec3_dot(vec3_t v1, vec3_t v2) { return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z); }

// We're normalizing in-place for...reasons.
void vec2_normalize(vec2_t *v) {
  float length = sqrt(pow(v->x, 2) + pow(v->y, 2));
  v->x /= length;
  v->y /= length;
}

void vec3_normalize(vec3_t *v) {
  float length = sqrt(pow(v->x, 2) + pow(v->y, 2) + pow(v->z, 2));
  v->x /= length;
  v->y /= length;
  v->z /= length;
}

vec3_t vec3_rotate_x(vec3_t v, float angle) {
  return (vec3_t){
      .x = v.x,
      .y = v.y * cos(angle) - v.z * sin(angle),
      .z = v.y * sin(angle) + v.z * cos(angle),
  };
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
  return (vec3_t){
      .x = v.x * cos(angle) - v.z * sin(angle),
      .y = v.y,
      .z = v.x * sin(angle) + v.z * cos(angle),
  };
}

vec3_t vec3_rotate_z(vec3_t v, float angle) {
  return (vec3_t){
      .x = v.x * cos(angle) - v.y * sin(angle),
      .y = v.x * sin(angle) + v.y * cos(angle),
      .z = v.z,
  };
}
