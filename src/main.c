#include "array.h"
#include "colors.h"
#include "display.h"
#include "matrix.h"
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

  // load_obj_file_data("./assets/cube.obj");
  load_cube_mesh_data();
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

  // mesh.rotation.x += 0.02;
  mesh.rotation.y += 0.02;
  // mesh.rotation.z += 0.02;

  // mesh.scale.x += 0.002;
  // mesh.scale.y += 0.002;

  // mesh.translation.x += 0.01;
  mesh.translation.z = 5;

  mat4_t scale_matrix = mat4_make_scale(mesh.scale);
  mat4_t translation_matrix = mat4_make_translation(mesh.translation);
  mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
  mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
  mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

  int num_faces = array_length(mesh.faces);
  for (int i = 0; i < num_faces; i++) {
    face_t mesh_face = mesh.faces[i];

    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];

    vec3_t transformed_vertices[3];
    for (int j = 0; j < 3; j++) {
      vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

      // Do some transformations.
      // 1) Scale
      // 2) Rotate
      // 3) Translate
      transformed_vertex = mat4_mul_vec4(scale_matrix, transformed_vertex);
      transformed_vertex = mat4_mul_vec4(rotation_matrix_x, transformed_vertex);
      transformed_vertex = mat4_mul_vec4(rotation_matrix_y, transformed_vertex);
      transformed_vertex = mat4_mul_vec4(rotation_matrix_z, transformed_vertex);
      transformed_vertex = mat4_mul_vec4(translation_matrix, transformed_vertex);

      transformed_vertices[j] = vec3_from_vec4(transformed_vertex);
    }

    // Cull triangles that are not facing the camera.
    if (cull_method == CULL_BACKFACE && !should_render(transformed_vertices)) {
      continue;
    }

    vec2_t projected_points[3];
    for (int i = 0; i < 3; i++) {
      projected_points[i] = project(transformed_vertices[i]);
      // Scale and translate the projected points to the middle of the screen.
      projected_points[i].x += (window_width / 2);
      projected_points[i].y += (window_height / 2);
    }

    // Calculate the average depth for each face based on the vertices after
    // transformation
    float avg_depth =
        (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

    triangle_t projected_triangle = {.points =
                                         {
                                             {projected_points[0].x, projected_points[0].y},
                                             {projected_points[1].x, projected_points[1].y},
                                             {projected_points[2].x, projected_points[2].y},
                                         },
                                     .color = mesh_face.color,
                                     .avg_depth = avg_depth};

    array_push(triangles_to_render, projected_triangle);
  }

  // Sort the triangles to render by their avg_depth
  int num_triangles = array_length(triangles_to_render);
  for (int i = 0; i < num_triangles; i++) {
    for (int j = i; j < num_triangles; j++) {
      if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth) {
        // Swap the triangles positions in the array
        triangle_t temp = triangles_to_render[i];
        triangles_to_render[i] = triangles_to_render[j];
        triangles_to_render[j] = temp;
      }
    }
  }
}

void render(void) {
  draw_grid(50);

  int num_triangles = array_length(triangles_to_render);
  for (int i = 0; i < num_triangles; i++) {
    triangle_t t = triangles_to_render[i];
    if (render_method == RENDER_WIRE_VERTEX) {
      for (int j = 0; j < 3; j++) {
        draw_rect(t.points[j].x - 3, t.points[j].y - 3, 6, 6, FIREBRICK);
      }
    }

    if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
      draw_filled_triangle(t);
    }
    if (render_method == RENDER_WIRE || render_method == RENDER_FILL_TRIANGLE_WIRE ||
        render_method == RENDER_WIRE_VERTEX) {
      draw_triangle_edges(t);
    }
  }

  array_free(triangles_to_render);

  render_color_buffer();
  clear_color_buffer(LIGHT_GRAY);

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
