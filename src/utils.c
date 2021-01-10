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
