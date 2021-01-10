#include "user_input.h"
#include "settings.h"
#include "state.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type) {
  case SDL_QUIT:
    is_running = false;
    break;
  case SDL_KEYDOWN:
    switch (event.key.keysym.sym) {
    case SDLK_ESCAPE:
      is_running = false;
      break;
    case SDLK_1:
      render_method = RENDER_WIRE_VERTEX;
      break;
    case SDLK_2:
      render_method = RENDER_WIRE;
      break;
    case SDLK_3:
      render_method = RENDER_FILL_TRIANGLE;
      break;
    case SDLK_4:
      render_method = RENDER_FILL_TRIANGLE_WIRE;
      break;
    case SDLK_c:
      cull_method = CULL_NONE;
      break;
    case SDLK_d:
      cull_method = CULL_BACKFACE;
      break;
    }
    break;
  }
}
