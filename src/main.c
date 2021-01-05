#include "array.h"
#include "display.h"
#include "mesh.h"
#include "settings.h"
#include "triangle.h"
#include "vector.h"

#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

triangle_t *triangles_to_render = NULL;

bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
  render_method = RENDER_WIRE;
  cull_method = CULL_BACKFACE;
  /*
  There is a possibility that malloc will fail to allocate that number of bytes
  in memory, e.g., when the machine does not have enough free memory. If that
  happens, malloc will return a NULL pointer. Since the main goal of this course
  is to learn the fundamentals of computer graphics and since this is basically
  an academic exercise, we avoid doing exhaustive, professional checks.
  */
  color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);

  color_buffer_texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

  load_obj_file_data("./assets/cube.obj");
}

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

vec2_t project(vec3_t point) {
  vec2_t projected_point = {.x = (fov_factor * point.x) / point.z,
                            .y = (fov_factor * point.y) / point.z};

  return projected_point;
}

void do_delay(void) {
  // Wait some time until the reach the target frame time in milliseconds
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

  // Only delay execution if we are running too fast
  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();
}

void update(void) {
  do_delay();

  triangles_to_render = NULL;

  mesh.rotation.x += 0.02;
  // mesh.rotation.y += 0.02;
  mesh.rotation.z += 0.02;

  int num_faces = array_length(mesh.faces);
  for (int i = 0; i < num_faces; i++) {
    face_t cube_face = mesh.faces[i];

    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[cube_face.a - 1];
    face_vertices[1] = mesh.vertices[cube_face.b - 1];
    face_vertices[2] = mesh.vertices[cube_face.c - 1];

    vec3_t transformed_vertices[3];
    for (int j = 0; j < 3; j++) {
      vec3_t transformed_vertex = face_vertices[j];

      // Do some rotations.
      transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
      transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
      transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

      // Adjust for camera.
      // transformed_vertex.x -= camera_position.x;
      // transformed_vertex.y -= camera_position.y;
      // transformed_vertex.z -= camera_position.z;
      transformed_vertex.z += 5;

      transformed_vertices[j] = transformed_vertex;
    }

    // Cull triangles that are not facing the camera.
    if (cull_method == CULL_BACKFACE && !should_render(transformed_vertices)) {
      continue;
    }

    triangle_t projected_triangle;

    for (int j = 0; j < 3; j++) {
      // Project the current vertext to 2d.
      vec2_t projected_point = project(transformed_vertices[j]);

      // Scale and translate the projected points to the middle of the screen.
      projected_point.x += (window_width / 2);
      projected_point.y += (window_height / 2);

      projected_triangle.points[j] = projected_point;
    }

    array_push(triangles_to_render, projected_triangle);
  }
}

color_t black = 0xFF000000;
color_t white = 0xFFFFFFFF;
color_t lavender = 0xFFE6E6fA;
color_t firebrick = 0xFFB22222;
color_t lime = 0xC8FF01;
color_t background_gray = 0xFFF3F3F3;

void render(void) {
  draw_grid(50);

  int num_triangles = array_length(triangles_to_render);
  for (int i = 0; i < num_triangles; i++) {
    triangle_t t = triangles_to_render[i];
    if (render_method == RENDER_WIRE_VERTEX) {
      for (int j = 0; j < 3; j++) {
        draw_rect(t.points[j].x - 3, t.points[j].y - 3, 6, 6, firebrick);
      }
    }

    if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
      draw_filled_triangle(t, firebrick);
    }
    if (render_method == RENDER_WIRE || render_method == RENDER_FILL_TRIANGLE_WIRE ||
        render_method == RENDER_WIRE_VERTEX) {
      draw_triangle_edges(t, black);
    }
  }

  array_free(triangles_to_render);

  render_color_buffer();
  clear_color_buffer(background_gray);

  SDL_RenderPresent(renderer);
}

void free_resources(void) {
  free(color_buffer);
  array_free(mesh.vertices);
  array_free(mesh.faces);
}

int main(void) {
  is_running = initialize_window();

  setup();

  while (is_running) {
    process_input();
    update();
    render();
  }

  destroy_window();
  free_resources();

  return 0;
}
