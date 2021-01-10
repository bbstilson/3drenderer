#ifndef LIGHT_H
#define LIGHT_H

#include "colors.h"
#include "vector.h"

typedef struct {
  vec3_t direction;
} light_t;

extern light_t light;

color_t light_apply_intensity(color_t c, float percentage_factor);

#endif
