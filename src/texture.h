#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  float u, v;
} tex2_t;

extern int texture_width;
extern int texture_height;
extern color_t *mesh_texture;
extern const uint8_t REDBRICK_TEXTURE[];

#endif
