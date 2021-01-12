#include "utils.h"

#include <math.h>

float distance(int x1, int y1, int x2, int y2) { return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)); }

void int_swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void float_swap(float *a, float *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void vec2_swap(vec2_t *a, vec2_t *b) {
  vec2_t temp = *a;
  *a = *b;
  *b = temp;
}

void vec3_swap(vec3_t *a, vec3_t *b) {
  vec3_t temp = *a;
  *a = *b;
  *b = temp;
}

void vec4_swap(vec4_t *a, vec4_t *b) {
  vec4_t temp = *a;
  *a = *b;
  *b = temp;
}
