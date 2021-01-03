#ifndef DISPLAY_N
#define DISPLAY_N

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

extern int window_width;
extern int window_height;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *color_buffer_texture;
extern uint32_t *color_buffer;

bool initialize_window(void);
void draw_pixel(int x, int y, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void draw_rect(int start_x, int start_y, int w, int h, uint32_t color);
void draw_circle(int center_x, int center_y, int radius, uint32_t color);

#endif
