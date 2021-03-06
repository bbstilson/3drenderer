#include "display.h"

int window_width = 800;
int window_height = 600;

vec3_t camera_position = {.x = 0, .y = 0, .z = 0};

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *color_buffer_texture = NULL;
color_t *color_buffer = NULL;
float *z_buffer = NULL;

bool initialize_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  // Fullscreen mode...
  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  window_width = display_mode.w;
  window_height = display_mode.h;

  window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width,
                            window_height, SDL_WINDOW_BORDERLESS);

  if (!window) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);

  if (!renderer) {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }

  /*
  There is a possibility that malloc will fail to allocate that number of bytes in memory, e.g.,
  when the machine does not have enough free memory. If that happens, malloc will return a NULL
  pointer. Since the main goal of this course is to learn the fundamentals of computer graphics
  and since this is basically an academic exercise, we avoid doing exhaustive, professional checks.
  */
  color_buffer = (color_t *)malloc(sizeof(color_t) * window_width * window_height);

  color_buffer_texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

  return true;
}

void destroy_window(void) {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void render_color_buffer(void) {
  SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer,
                    (int)(window_width * sizeof(color_t)));
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(color_t color) {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++) {
      draw_pixel(x, y, color);
    }
  }
}

void clear_z_buffer() {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++) {
      z_buffer[(window_width * y) + x] = 1.0;
    }
  }
}
void draw_pixel(int x, int y, color_t color) {
  if (x >= 0 && y >= 0 && x < window_width && y < window_height) {
    color_buffer[(window_width * y) + x] = color;
  }
}

void draw_grid(int grid_size) {
  for (int y = 1; y < window_height; y++) {
    for (int x = 1; x < window_width; x++) {
      if (x % grid_size == 0 || y % grid_size == 0) {
        draw_pixel(x, y, 0xFFFFFFFF);
      }
    }
  }
}

void draw_rect(int start_x, int start_y, int w, int h, color_t color) {
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      draw_pixel(x + start_x, y + start_y, color);
    }
  }
}

void draw_circle(int center_x, int center_y, int radius, color_t color) {
  for (int y = -radius; y < radius; y++) {
    for (int x = -radius; x < radius; x++) {
      if (abs((int)floor(distance(x + center_x, y + center_y, center_x, center_y))) < radius) {
        draw_pixel(x + center_x, y + center_y, color);
      }
    }
  }
}

void draw_line(int x1, int y1, int x2, int y2, color_t color) {
  int delta_x = x2 - x1;
  int delta_y = y2 - y1;

  int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

  float x_inc = delta_x / (float)side_length;
  float y_inc = delta_y / (float)side_length;

  float current_x = x1;
  float current_y = y1;

  for (int i = 0; i <= side_length; i++) {
    draw_pixel(round(current_x), round(current_y), color);
    current_x += x_inc;
    current_y += y_inc;
  }
}
