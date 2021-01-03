#include "display.h"
#include "utils.h"

int window_width = 800;
int window_height = 600;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *color_buffer_texture = NULL;
uint32_t *color_buffer = NULL;

bool initialize_window(void)
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  // Fullscreen mode...
  // SDL_DisplayMode display_mode;
  // SDL_GetCurrentDisplayMode(0, &display_mode);
  // window_width = display_mode.w;
  // window_height = display_mode.h;

  window = SDL_CreateWindow(
      NULL,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      window_width,
      window_height,
      SDL_WINDOW_BORDERLESS);

  if (!window)
  {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);

  if (!renderer)
  {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }

  /*
  There is a possibility that malloc will fail to allocate that number of bytes in memory, e.g.,
  when the machine does not have enough free memory. If that happens, malloc will return a NULL
  pointer. Since the main goal of this course is to learn the fundamentals of computer graphics
  and since this is basically an academic exercise, we avoid doing exhaustive, professional checks.
  */
  color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);

  color_buffer_texture = SDL_CreateTexture(
      renderer,
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STREAMING,
      window_width,
      window_height);

  return true;
}

void set_pixel(int x, int y, uint32_t color)
{
  color_buffer[(window_width * y) + x] = color;
}

void render_color_buffer(void)
{
  SDL_UpdateTexture(
      color_buffer_texture,
      NULL,
      color_buffer,
      (int)(window_width * sizeof(uint32_t)));
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color)
{
  for (int y = 0; y < window_height; y++)
  {
    for (int x = 0; x < window_width; x++)
    {
      set_pixel(x, y, color);
    }
  }
}

void draw_rect(int start_x, int start_y, int w, int h, uint32_t color)
{
  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      set_pixel(x + start_x, y + start_y, color);
    }
  }
}

void draw_circle(int center_x, int center_y, int radius, uint32_t color)
{
  for (int y = -radius; y < radius; y++)
  {
    for (int x = -radius; x < radius; x++)
    {
      if (abs((int)floor(distance(x + center_x, y + center_y, center_x, center_y))) < radius)
      {
        set_pixel(x + center_x, y + center_y, color);
      }
    }
  }
}
