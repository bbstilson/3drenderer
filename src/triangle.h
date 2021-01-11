#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "display.h"
#include "texture.h"
#include "vector.h"

#include <stdbool.h>

typedef struct {
  int a, b, c;
  tex2_t a_uv, b_uv, c_uv;
  color_t color;
} face_t;

typedef struct {
  vec4_t points[3];
  tex2_t texcoords[3];
  color_t color;
  float avg_depth;
} triangle_t;

void draw_triangle_edges(triangle_t t);
void draw_filled_triangle(triangle_t t);
void draw_textured_triangle(triangle_t t, color_t *texture);

#endif
