#include "vector.h"

#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector 2 Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

float vec2_length(vec2_t v) { return sqrt(pow(v.x, 2) + pow(v.y, 2)); }

vec2_t vec2_add(vec2_t v1, vec2_t v2) {
  return (vec2_t){
      .x = v1.x + v2.x,
      .y = v1.y + v2.y,
  };
}

vec2_t vec2_sub(vec2_t v1, vec2_t v2) {
  return (vec2_t){
      .x = v1.x - v2.x,
      .y = v1.y - v2.y,
  };
}

vec2_t vec2_mul(vec2_t v, float factor) {
  return (vec2_t){
      .x = v.x * factor,
      .y = v.y * factor,
  };
}

vec2_t vec2_div(vec2_t v, float factor) {
  return (vec2_t){
      .x = v.x / factor,
      .y = v.y / factor,
  };
}

float vec2_dot(vec2_t v1, vec2_t v2) { return (v1.x * v2.x) + (v1.y * v2.y); }

// We're normalizing in-place for...reasons.
void vec2_normalize(vec2_t *v) {
  float length = sqrt(pow(v->x, 2) + pow(v->y, 2));
  v->x /= length;
  v->y /= length;
}

void vec2_swap(vec2_t *a, vec2_t *b) {
  vec2_t temp = *a;
  *a = *b;
  *b = temp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector 3 Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

float vec3_length(vec3_t v) { return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2)); }

vec3_t vec3_add(vec3_t v1, vec3_t v2) {
  return (vec3_t){
      .x = v1.x + v2.x,
      .y = v1.y + v2.y,
      .z = v1.z + v2.z,
  };
}

vec3_t vec3_sub(vec3_t v1, vec3_t v2) {
  return (vec3_t){
      .x = v1.x - v2.x,
      .y = v1.y - v2.y,
      .z = v1.z - v2.z,
  };
}

vec3_t vec3_mul(vec3_t v, float factor) {
  return (vec3_t){
      .x = v.x * factor,
      .y = v.y * factor,
      .z = v.z * factor,
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

float vec3_dot(vec3_t v1, vec3_t v2) { return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z); }

void vec3_normalize(vec3_t *v) {
  float length = sqrt(pow(v->x, 2) + pow(v->y, 2) + pow(v->z, 2));
  v->x /= length;
  v->y /= length;
  v->z /= length;
}

void vec3_swap(vec3_t *a, vec3_t *b) {
  vec3_t temp = *a;
  *a = *b;
  *b = temp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

vec4_t vec4_from_vec3(vec3_t v) { return (vec4_t){v.x, v.y, v.z, 1}; }

vec3_t vec3_from_vec4(vec4_t v) { return (vec3_t){v.x, v.y, v.z}; }
