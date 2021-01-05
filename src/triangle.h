#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "display.h"
#include "vector.h"

#include <stdbool.h>

typedef struct {
  int a;
  int b;
  int c;
} face_t;

typedef struct {
  vec2_t points[3];
} triangle_t;

void draw_triangle_edges(triangle_t t, color_t color);
void draw_filled_triangle(triangle_t t, color_t color);

#endif
