#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "display.h"
#include "vector.h"

#include <stdbool.h>

typedef struct {
  int a, b, c;
  color_t color;
} face_t;

typedef struct {
  vec2_t points[3];
  color_t color;
  float avg_depth;
} triangle_t;

void draw_triangle_edges(triangle_t t);
void draw_filled_triangle(triangle_t t);

#endif
