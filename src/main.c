#include "array.h"
#include "colors.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "settings.h"
#include "state.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "user_input.h"
#include "vector.h"

#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Array of triangles that should be rendered frame by frame.
triangle_t *triangles_to_render = NULL;

int previous_frame_time = 0;

mat4_t proj_matrix;

void setup(void) {
  render_method = RENDER_FILL_TRIANGLE;
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
      renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

  float fov = M_PI / 3.0;
  float aspect = (float)window_height / (float)window_width;
  float znear = 0.1;
  float zfar = 100.0;
  proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

  // load_obj_file_data("./assets/f22.obj");
  load_cube_mesh_data();
  load_png_texture_data("./assets/cube.png");
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

  // mesh.rotation.x += 0.01;
  mesh.rotation.y += 0.005;
  // mesh.rotation.z += 0.01;

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

      // Do some transformations...order matters!
      // 1) Scale
      // 2) Rotate
      // 3) Translate
      mat4_t world_matrix = mat4_identity();
      world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
      world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

      transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

      transformed_vertices[j] = vec3_from_vec4(transformed_vertex);
    }

    vec3_t ab = vec3_sub(transformed_vertices[1], transformed_vertices[0]);
    vec3_normalize(&ab);
    vec3_t ac = vec3_sub(transformed_vertices[2], transformed_vertices[0]);
    vec3_normalize(&ac);
    vec3_t surface_normal = vec3_cross(ab, ac);
    vec3_normalize(&surface_normal);
    vec3_t camera_ray = vec3_sub(camera_position, transformed_vertices[0]);

    // Cull triangles that are not facing the camera.
    if (cull_method == CULL_BACKFACE) {
      if (vec3_dot(surface_normal, camera_ray) < 0) {
        continue;
      }
    }

    vec4_t projected_points[3];
    for (int i = 0; i < 3; i++) {
      projected_points[i] =
          mat4_mul_vec4_project(proj_matrix, vec4_from_vec3(transformed_vertices[i]));

      // Scale into the viewport.
      projected_points[i].x *= (window_width / 2.0);
      projected_points[i].y *= (window_height / 2.0);

      // Invert the y values to account for flipped screen y coordinates.
      projected_points[i].y *= -1;

      // Translate the projected points to the middle of the screen.
      projected_points[i].x += (window_width / 2.0);
      projected_points[i].y += (window_height / 2.0);
    }

    // Calculate the average depth for each face based on the vertices after
    // transformation
    float avg_depth =
        (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

    float light_intensity_factor = -vec3_dot(surface_normal, light.direction);
    color_t adjusted_color = light_apply_intensity(mesh_face.color, light_intensity_factor);
    triangle_t projected_triangle = {
        .points =
            {
                {projected_points[0].x, projected_points[0].y, projected_points[0].z,
                 projected_points[0].w},
                {projected_points[1].x, projected_points[1].y, projected_points[1].z,
                 projected_points[1].w},
                {projected_points[2].x, projected_points[2].y, projected_points[2].z,
                 projected_points[2].w},
            },
        .texcoords =
            {
                mesh_face.a_uv,
                mesh_face.b_uv,
                mesh_face.c_uv,
            },
        .color = adjusted_color,
        .avg_depth = avg_depth,
    };

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
        draw_rect(t.points[j].x - 3, t.points[j].y - 2, 4, 4, BLACK);
      }
    }

    switch (render_method) {
    case RENDER_WIRE:
      draw_triangle_edges(t);
      break;
    case RENDER_WIRE_VERTEX:
      draw_triangle_edges(t);
      break;
    case RENDER_FILL_TRIANGLE:
      draw_filled_triangle(t);
      break;
    case RENDER_FILL_TRIANGLE_WIRE:
      draw_filled_triangle(t);
      draw_triangle_edges(t);
      break;
    case RENDER_TEXTURED:
      draw_textured_triangle(t, mesh_texture);
      break;
    case RENDER_TEXTURED_WIRE:
      draw_textured_triangle(t, mesh_texture);
      draw_triangle_edges(t);
      break;
    }
  }

  array_free(triangles_to_render);

  render_color_buffer();
  clear_color_buffer(SKY_BLUE);

  SDL_RenderPresent(renderer);
}

void free_resources(void) {
  free(color_buffer);
  array_free(mesh.vertices);
  array_free(mesh.faces);
  upng_free(mesh_texture);
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
