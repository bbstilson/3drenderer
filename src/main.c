#include "array.h"
#include "colors.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "settings.h"
#include "state.h"
#include "texture.h"
#include "upng.h"
#include "user_input.h"
#include "vector.h"

#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Array of triangles that should be rendered frame by frame.
#define MAX_TRIANGLES_PER_MESH 10000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

int previous_frame_time = 0;

mat4_t proj_matrix;

void setup(void) {
  render_method = RENDER_TEXTURED;
  cull_method = CULL_BACKFACE;
  /*
  There is a possibility that malloc will fail to allocate that number of bytes
  in memory, e.g., when the machine does not have enough free memory. If that
  happens, malloc will return a NULL pointer. Since the main goal of this course
  is to learn the fundamentals of computer graphics and since this is basically
  an academic exercise, we avoid doing exhaustive, professional checks.
  */
  color_buffer = (color_t *)malloc(sizeof(color_t) * window_width * window_height);
  z_buffer = (float *)malloc(sizeof(float) * window_width * window_height);

  color_buffer_texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

  float fov = M_PI / 3.0;
  float aspect = (float)window_height / (float)window_width;
  float znear = 0.1;
  float zfar = 100.0;
  proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

  // load_obj_file_data("./assets/cube.obj");
  // load_png_texture_data("./assets/cube.png");
  // load_obj_file_data("./assets/f22.obj");
  // load_png_texture_data("./assets/f22.png");
  // load_obj_file_data("./assets/efa.obj");
  // load_png_texture_data("./assets/efa.png");
  load_obj_file_data("./assets/drone.obj");
  load_png_texture_data("./assets/drone.png");
  // load_obj_file_data("./assets/f117.obj");
  // load_png_texture_data("./assets/f117.png");
  // load_obj_file_data("./assets/crab.obj");
  // load_png_texture_data("./assets/crab.png");
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

  num_triangles_to_render = 0;

  // Change the mesh scale, rotation, and translation values per animation frame
  // mesh.rotation.x += 0.01;
  mesh.rotation.y += 0.02;
  // mesh.rotation.z += 0.01;
  mesh.translation.z = 5.0;

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

    vec4_t transformed_vertices[3];
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

      transformed_vertices[j] = transformed_vertex;
    }

    // Get individual vectors from A, B, and C vertices to compute normal
    vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
    vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
    vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

    // Get the vector subtraction of B-A and C-A
    vec3_t vector_ab = vec3_sub(vector_b, vector_a);
    vec3_t vector_ac = vec3_sub(vector_c, vector_a);
    vec3_normalize(&vector_ab);
    vec3_normalize(&vector_ac);

    vec3_t surface_normal = vec3_cross(vector_ab, vector_ac);
    vec3_normalize(&surface_normal);
    vec3_t camera_ray = vec3_sub(camera_position, vector_a);

    // Cull triangles that are not facing the camera.
    if (cull_method == CULL_BACKFACE) {
      if (vec3_dot(surface_normal, camera_ray) < 0) {
        continue;
      }
    }

    vec4_t projected_points[3];

    for (int i = 0; i < 3; i++) {
      projected_points[i] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[i]);

      // Scale into the viewport.
      projected_points[i].x *= (window_width / 2.0);
      projected_points[i].y *= (window_height / 2.0);

      // Invert the y values to account for flipped screen y coordinates.
      projected_points[i].y *= -1;

      // Translate the projected points to the middle of the screen.
      projected_points[i].x += (window_width / 2.0);
      projected_points[i].y += (window_height / 2.0);
    }

    float light_intensity_factor = -vec3_dot(surface_normal, light.direction);
    color_t adjusted_color = light_apply_intensity(mesh_face.color, light_intensity_factor);
    triangle_t projected_triangle = {
        .points =
            {
                {
                    projected_points[0].x,
                    projected_points[0].y,
                    projected_points[0].z,
                    projected_points[0].w,
                },
                {
                    projected_points[1].x,
                    projected_points[1].y,
                    projected_points[1].z,
                    projected_points[1].w,
                },
                {
                    projected_points[2].x,
                    projected_points[2].y,
                    projected_points[2].z,
                    projected_points[2].w,
                },
            },
        .texcoords =
            {
                mesh_face.a_uv,
                mesh_face.b_uv,
                mesh_face.c_uv,
            },
        .color = adjusted_color,
    };

    if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
      triangles_to_render[num_triangles_to_render] = projected_triangle;
      num_triangles_to_render += 1;
    }
  }
}

void render(void) {
  draw_grid(50);

  for (int i = 0; i < num_triangles_to_render; i++) {
    triangle_t t = triangles_to_render[i];

    // Draw filled triangle
    if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
      draw_filled_triangle(t.points[0].x, t.points[0].y, t.points[0].z, t.points[0].w, // vertex A
                           t.points[1].x, t.points[1].y, t.points[1].z, t.points[1].w, // vertex B
                           t.points[2].x, t.points[2].y, t.points[2].z, t.points[2].w, // vertex C
                           t.color);
    }

    // Draw textured triangle
    if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE) {
      draw_textured_triangle(
          t.points[0].x, t.points[0].y, t.points[0].z, t.points[0].w, t.texcoords[0].u,
          t.texcoords[0].v, // vertex A
          t.points[1].x, t.points[1].y, t.points[1].z, t.points[1].w, t.texcoords[1].u,
          t.texcoords[1].v, // vertex B
          t.points[2].x, t.points[2].y, t.points[2].z, t.points[2].w, t.texcoords[2].u,
          t.texcoords[2].v, // vertex C
          mesh_texture);
    }

    // Draw triangle wireframe
    if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX ||
        render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE) {
      draw_triangle(t.points[0].x, t.points[0].y, // vertex A
                    t.points[1].x, t.points[1].y, // vertex B
                    t.points[2].x, t.points[2].y, // vertex C
                    WHITE);
    }

    // Draw triangle vertex points
    if (render_method == RENDER_WIRE_VERTEX) {
      draw_rect(t.points[0].x - 3, t.points[0].y - 3, 6, 6, BLACK); // vertex A
      draw_rect(t.points[1].x - 3, t.points[1].y - 3, 6, 6, BLACK); // vertex B
      draw_rect(t.points[2].x - 3, t.points[2].y - 3, 6, 6, BLACK); // vertex C
    }
  }

  render_color_buffer();
  clear_color_buffer(0x00000000);
  clear_z_buffer();

  SDL_RenderPresent(renderer);
}

void free_resources(void) {
  free(color_buffer);
  array_free(mesh.vertices);
  array_free(mesh.faces);
  upng_free(png_texture);
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
