#ifndef DISPLAY_H
#define DISPLAY_H

#include "utils.h"
#include "vector.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint32_t color_t;

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

extern int window_width;
extern int window_height;

extern vec3_t camera_position;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *color_buffer_texture;
extern uint32_t *color_buffer;

bool initialize_window(void);
void destroy_window(void);

void render_color_buffer(void);
void clear_color_buffer(color_t color);

void draw_pixel(int x, int y, color_t color);
void draw_grid(int grid_size);
void draw_rect(int start_x, int start_y, int w, int h, color_t color);
void draw_circle(int center_x, int center_y, int radius, color_t color);
void draw_line(int x1, int y1, int x2, int y2, color_t color);

bool should_render(vec3_t *vertices);

#endif
