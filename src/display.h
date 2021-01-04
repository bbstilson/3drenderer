#ifndef DISPLAY_N
#define DISPLAY_N

#include "triangle.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

extern int window_width;
extern int window_height;

extern vec3_t camera_position;
extern float fov_factor;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *color_buffer_texture;
extern uint32_t *color_buffer;

bool initialize_window(void);
void destroy_window(void);

void render_color_buffer(void);
void clear_color_buffer(uint32_t color);

void draw_pixel(int x, int y, uint32_t color);
void draw_grid(int grid_size);
void draw_rect(int start_x, int start_y, int w, int h, uint32_t color);
void draw_circle(int center_x, int center_y, int radius, uint32_t color);
void draw_line(int x1, int y1, int x2, int y2, uint32_t color);
void draw_triangle(triangle_t t, uint32_t color);

bool should_render(vec3_t *vertices);

#endif
