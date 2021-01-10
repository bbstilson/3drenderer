#include "light.h"

light_t light = {.direction = {0, 0, 1}};

void clamp_float(float min, float max, float *x) {
  if (*x < min) {
    *x = min;
  }
  if (*x > max) {
    *x = max;
  }
}

color_t light_apply_intensity(color_t c, float percentage_factor) {
  clamp_float(0, 1, &percentage_factor);

  color_t a = (c & 0xFF000000);
  color_t r = (c & 0x00FF0000) * percentage_factor;
  color_t g = (c & 0x0000FF00) * percentage_factor;
  color_t b = (c & 0x000000FF) * percentage_factor;
  return a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}
