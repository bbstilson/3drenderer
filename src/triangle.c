#include "triangle.h"
#include "colors.h"
#include "display.h"
#include "utils.h"

#include <assert.h>

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
  vec2_t ab = vec2_sub(b, a);
  vec2_t bc = vec2_sub(c, b);
  vec2_t ac = vec2_sub(c, a);
  vec2_t ap = vec2_sub(p, a);
  vec2_t bp = vec2_sub(p, b);

  // Calculate the area of the full triange ABC using cross product (area of parallelogram).
  float area_triangle_abc = (ab.x * ac.y - ab.y * ac.x);

  // Weight alpha is the area of subtriangle BCP divided by the area ofht full triangle ABC.
  float alpha = (bc.x * bp.y - bp.x * bc.y) / area_triangle_abc;

  // Weight beta is the area of subtriangle ACP divided by the area of the full triangle ABC.
  float beta = (ap.x * ac.y - ac.x * ap.y) / area_triangle_abc;

  // Weight gamma is easily found since barycentric coordinates always add up to 1.
  float gamma = 1 - alpha - beta;

  return (vec3_t){alpha, beta, gamma};
}

// Draw a triangle using three raw line calls
void draw_triangle_edges(triangle_t t) {
  int x0 = t.points[0].x;
  int y0 = t.points[0].y;
  int x1 = t.points[1].x;
  int y1 = t.points[1].y;
  int x2 = t.points[2].x;
  int y2 = t.points[2].y;

  draw_line(x0, y0, x1, y1, BLACK);
  draw_line(x1, y1, x2, y2, BLACK);
  draw_line(x2, y2, x0, y0, BLACK);
}

void draw_texel(int x, int y, color_t *texture, vec4_t point_a, vec4_t point_b, vec4_t point_c,
                tex2_t uv0, tex2_t uv1, tex2_t uv2) {
  vec2_t p = {x, y};
  vec3_t weights = barycentric_weights(vec2_from_vec4(point_a), vec2_from_vec4(point_b),
                                       vec2_from_vec4(point_c), p);

  float alpha = weights.x;
  float beta = weights.y;
  float gamma = weights.z;

  // Variables to store the interpolated values of U, V, and 1 / W for the current pixel.
  float interpolated_u;
  float interpolated_v;
  float interpolated_reciprocal_w;

  // Perform the interpolation of all U / W and V / W valuyes using barycentric weights and a
  // factor of 1 / W.
  interpolated_u =
      (uv0.u / point_a.w) * alpha + (uv1.u / point_b.w) * beta + (uv2.u / point_c.w) * gamma;
  interpolated_v =
      (uv0.v / point_a.w) * alpha + (uv1.v / point_b.w) * beta + (uv2.v / point_c.w) * gamma;
  interpolated_reciprocal_w =
      (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

  // Now, we can divde back bot hinterpolated values by 1 / W.
  interpolated_u /= interpolated_reciprocal_w;
  interpolated_v /= interpolated_reciprocal_w;

  // Map the UV coordinate the full texture width and height.
  int tex_x = abs((int)(interpolated_u * texture_width));
  int tex_y = abs((int)(interpolated_v * texture_height));

  draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);
}

// Draw a filled a triangle with a flat bottom
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color) {
  assert((y1 - y0) > 0);
  assert((y2 - y0) > 0);

  // Find the two slopes (two triangle legs)
  float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
  float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

  // Start x_start and x_end from the top vertex (x0,y0)
  float x_start = x0;
  float x_end = x0;

  // Loop all the scanlines from top to bottom
  for (int y = y0; y <= y2; y++) {
    draw_line(x_start, y, x_end, y, color);
    x_start += inv_slope_1;
    x_end += inv_slope_2;
  }
}

// Draw a filled a triangle with a flat top
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)
//
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color) {
  assert((y2 - y0) > 0);
  assert((y2 - y1) > 0);

  // Find the two slopes (two triangle legs)
  float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
  float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

  // Start x_start and x_end from the bottom vertex (x2,y2)
  float x_start = x2;
  float x_end = x2;

  // Loop all the scanlines from bottom to top
  for (int y = y2; y >= y0; y--) {
    draw_line(x_start, y, x_end, y, color);
    x_start -= inv_slope_1;
    x_end -= inv_slope_2;
  }
}

// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
//
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)------(mx,my)
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
//
void draw_filled_triangle(triangle_t t) {
  int x0 = t.points[0].x;
  int y0 = t.points[0].y;
  int x1 = t.points[1].x;
  int y1 = t.points[1].y;
  int x2 = t.points[2].x;
  int y2 = t.points[2].y;

  // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
  if (y0 > y1) {
    int_swap(&y0, &y1);
    int_swap(&x0, &x1);
  }
  if (y1 > y2) {
    int_swap(&y1, &y2);
    int_swap(&x1, &x2);
  }
  if (y0 > y1) {
    int_swap(&y0, &y1);
    int_swap(&x0, &x1);
  }

  if (y0 == y1 && y1 == y2) {
    // do nothing... why does this happen?
  } else if (y1 == y2) {
    // Draw flat-bottom triangle
    fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, t.color);
  } else if (y0 == y1) {
    // Draw flat-top triangle
    fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, t.color);
  } else {
    // Calculate the new vertex (mx,my) using triangle similarity
    int my = y1;
    int mx = (((x2 - x0) * (y1 - y0)) / (y2 - y0)) + x0;

    // Draw flat-bottom triangle
    fill_flat_bottom_triangle(x0, y0, x1, y1, mx, my, t.color);

    // Draw flat-top triangle
    fill_flat_top_triangle(x1, y1, mx, my, x2, y2, t.color);
  }
}

void draw_textured_triangle(triangle_t t, color_t *texture) {
  vec4_t p0 = t.points[0];
  vec4_t p1 = t.points[1];
  vec4_t p2 = t.points[2];

  float u0 = t.texcoords[0].u;
  float v0 = t.texcoords[0].v;
  float u1 = t.texcoords[1].u;
  float v1 = t.texcoords[1].v;
  float u2 = t.texcoords[2].u;
  float v2 = t.texcoords[2].v;

  // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
  if (p0.y > p1.y) {
    vec4_swap(&p0, &p1);
    float_swap(&u0, &u1);
    float_swap(&v0, &v1);
  }
  if (p1.y > p2.y) {
    vec4_swap(&p1, &p2);
    float_swap(&u1, &u2);
    float_swap(&v1, &v2);
  }
  if (p0.y > p1.y) {
    vec4_swap(&p0, &p1);
    float_swap(&u0, &u1);
    float_swap(&v0, &v1);
  }

  // Create vector points after we sorth te vertices.
  tex2_t uv0 = {u0, v0};
  tex2_t uv1 = {u1, v1};
  tex2_t uv2 = {u2, v2};

  // Render the upper part of the triangle (flat-bottom).
  float inv_slope_1 = 0;
  float inv_slope_2 = 0;

  if (y1 - y0 != 0)
    inv_slope_1 = (float)(p1.x - p0.x) / fabsf(p1.y - p0.y);
  if (p2.y - p0.y != 0)
    inv_slope_2 = (float)(p2.x - p0.x) / fabsf(p2.y - p0.y);

  if (p1.y - p0.y != 0) {
    for (int y = p0.y; y <= p1.y; y++) {
      int x_start = p1.x + (y - p1.y) * inv_slope_1;
      int x_end = p0.x + (y - p0.y) * inv_slope_2;

      // Swap if x_start is to the right of x_end.
      if (x_end < x_start)
        int_swap(&x_start, &x_end);

      for (int x = x_start; x < x_end; x++) {
        draw_texel(x, y, texture, p0, p1, p2, uv0, uv1, uv2);
      }
    }
  }

  // Render the lower part of the triangle (flat-top).
  inv_slope_1 = 0;
  inv_slope_2 = 0;

  if (p2.y - p1.y != 0)
    inv_slope_1 = (float)(p2.x - p1.x) / fabsf(p2.y - p1.y);
  if (p2.y - p0.y != 0)
    inv_slope_2 = (float)(p2.x - p0.x) / fabsf(p2.y - p0.y);

  if (p2.y - p1.y != 0) {
    for (int y = p1.y; y <= p2.y; y++) {
      int x_start = p1.x + (y - p1.y) * inv_slope_1;
      int x_end = p0.x + (y - p0.y) * inv_slope_2;

      // Swap if x_start is to the right of x_end.
      if (x_end < x_start)
        int_swap(&x_start, &x_end);

      for (int x = x_start; x < x_end; x++) {
        draw_texel(x, y, texture, p0, p1, p2, uv0, uv1, uv2);
      }
    }
  }
}
