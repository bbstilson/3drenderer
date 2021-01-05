#include "triangle.h"

void load_triangle_points(triangle_t *t, vec2_t points[]) {
  for (int i = 0; i < 3; i++) {
    t->points[i] = points[i];
  }
}

void draw_triangle_edges(triangle_t t, color_t color) {
  draw_line(t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, color);
  draw_line(t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y, color);
  draw_line(t.points[2].x, t.points[2].y, t.points[0].x, t.points[0].y, color);
}

void draw_filled_triangle(triangle_t t, color_t color) {
  vec2_t ordered[3];
  ordered[0] = t.points[0];
  ordered[1] = t.points[1];
  ordered[2] = t.points[2];

  if (ordered[0].y > ordered[1].y)
    vec2_swap(&ordered[0], &ordered[1]);

  if (ordered[1].y > ordered[2].y)
    vec2_swap(&ordered[1], &ordered[2]);

  if (ordered[0].y > ordered[1].y)
    vec2_swap(&ordered[0], &ordered[1]);

  vec2_t v0 = ordered[0];
  vec2_t v1 = ordered[1];
  vec2_t v2 = ordered[2];

  float my = v1.y;
  float mx = ((float)((v2.x - v0.x) * (v1.y - v0.y)) / (float)(v2.y - v0.y)) + v0.x;

  vec2_t mid_point = {.x = mx, .y = my};

  vec2_t flat_bottom_points[3] = {v0, v1, mid_point};
  vec2_t flat_top_points[3] = {v1, mid_point, v2};

  triangle_t flat_bottom;
  triangle_t flat_top;

  load_triangle_points(&flat_bottom, flat_bottom_points);
  load_triangle_points(&flat_top, flat_top_points);

  if (v1.y == v2.y) {
    fill_flat_bottom_triangle(flat_bottom, color);
  } else if (v0.y == v1.y) {
    fill_flat_top_triangle(flat_top, color);
  } else {
    fill_flat_bottom_triangle(flat_bottom, color);
    fill_flat_top_triangle(flat_top, color);
  }
}

void fill_flat_bottom_triangle(triangle_t t, color_t color) {
  float inv_slope_1 = vec2_inv_slope(t.points[0], t.points[1]);
  float inv_slope_2 = vec2_inv_slope(t.points[0], t.points[2]);

  float x_start = t.points[0].x;
  float x_end = t.points[0].x;

  for (int y = t.points[0].y; y <= t.points[2].y; y++) {
    draw_line(x_start, y, x_end, y, color);
    x_start += inv_slope_1;
    x_end += inv_slope_2;
  }
}

void fill_flat_top_triangle(triangle_t t, color_t color) {
  float inv_slope_1 = vec2_inv_slope(t.points[2], t.points[1]);
  float inv_slope_2 = vec2_inv_slope(t.points[2], t.points[0]);

  float x_start = t.points[2].x;
  float x_end = t.points[2].x;

  for (int y = t.points[2].y; y > t.points[0].y; y--) {
    draw_line(x_start, y, x_end, y, color);
    x_start -= inv_slope_1;
    x_end -= inv_slope_2;
  }
}
