#include "mesh.h"

#include "texture.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mesh_t mesh = {.vertices = NULL,
               .faces = NULL,
               .rotation = {0, 0, 0},
               .scale = {1.0, 1.0, 1.0},
               .translation = {0, 0, 0}};

const char VERTEX_PREFIX[] = "v ";
const char FACE_PREFIX[] = "f ";

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1}, // 1
    {.x = -1, .y = 1, .z = -1},  // 2
    {.x = 1, .y = 1, .z = -1},   // 3
    {.x = 1, .y = -1, .z = -1},  // 4
    {.x = 1, .y = 1, .z = 1},    // 5
    {.x = 1, .y = -1, .z = 1},   // 6
    {.x = -1, .y = 1, .z = 1},   // 7
    {.x = -1, .y = -1, .z = 1}   // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {
        .a = 1,
        .b = 2,
        .c = 3,
        .a_uv = {0, 0},
        .b_uv = {0, 1},
        .c_uv = {1, 1},
        .color = RED,
    },
    {
        .a = 1,
        .b = 3,
        .c = 4,
        .a_uv = {0, 0},
        .b_uv = {1, 1},
        .c_uv = {1, 0},
        .color = RED,
    },
    // right
    {
        .a = 4,
        .b = 3,
        .c = 5,
        .a_uv = {0, 0},
        .b_uv = {0, 1},
        .c_uv = {1, 1},
        .color = ORANGE,
    },
    {
        .a = 4,
        .b = 5,
        .c = 6,
        .a_uv = {0, 0},
        .b_uv = {1, 1},
        .c_uv = {1, 0},
        .color = ORANGE,
    },
    // back
    {
        .a = 6,
        .b = 5,
        .c = 7,
        .a_uv = {0, 0},
        .b_uv = {0, 1},
        .c_uv = {1, 1},
        .color = YELLOW,
    },
    {
        .a = 6,
        .b = 7,
        .c = 8,
        .a_uv = {0, 0},
        .b_uv = {1, 1},
        .c_uv = {1, 0},
        .color = YELLOW,
    },
    // left
    {
        .a = 8,
        .b = 7,
        .c = 2,
        .a_uv = {0, 0},
        .b_uv = {0, 1},
        .c_uv = {1, 1},
        .color = GREEN,
    },
    {
        .a = 8,
        .b = 2,
        .c = 1,
        .a_uv = {0, 0},
        .b_uv = {1, 1},
        .c_uv = {1, 0},
        .color = GREEN,
    },
    // top
    {
        .a = 2,
        .b = 7,
        .c = 5,
        .a_uv = {0, 0},
        .b_uv = {0, 1},
        .c_uv = {1, 1},
        .color = BLUE,
    },
    {
        .a = 2,
        .b = 5,
        .c = 3,
        .a_uv = {0, 0},
        .b_uv = {1, 1},
        .c_uv = {1, 0},
        .color = BLUE,
    },
    // bottom
    {
        .a = 6,
        .b = 8,
        .c = 1,
        .a_uv = {0, 0},
        .b_uv = {0, 1},
        .c_uv = {1, 1},
        .color = PURPLE,
    },
    {
        .a = 6,
        .b = 1,
        .c = 4,
        .a_uv = {0, 0},
        .b_uv = {1, 1},
        .c_uv = {1, 0},
        .color = PURPLE,
    },
};

void load_cube_mesh_data(void) {
  for (int i = 0; i < N_CUBE_VERTICES; i++) {
    vec3_t cube_vertex = cube_vertices[i];
    array_push(mesh.vertices, cube_vertex);
  }
  for (int i = 0; i < N_CUBE_FACES; i++) {
    face_t cube_face = cube_faces[i];
    array_push(mesh.faces, cube_face);
  }
}

void load_obj_file_data(char *file) {
  FILE *stream = fopen(file, "r");
  if (stream == NULL) {
    perror("Unable to open file!");
    exit(1);
  }

  char *line = NULL;
  size_t len = 0;
  while (getline(&line, &len, stream) != -1) {
    if (starts_with(line, (char *)VERTEX_PREFIX)) {
      vec3_t vertex;
      sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
      array_push(mesh.vertices, vertex);
    } else if (starts_with(line, (char *)FACE_PREFIX)) {
      int vertex_indicies[3];
      int texture_indicies[3];
      int normal_indicies[3];

      sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex_indicies[0], &texture_indicies[0],
             &normal_indicies[0], &vertex_indicies[1], &texture_indicies[1], &normal_indicies[1],
             &vertex_indicies[2], &texture_indicies[2], &normal_indicies[2]);

      face_t face = {
          .a = vertex_indicies[0],
          .b = vertex_indicies[1],
          .c = vertex_indicies[2],
          .color = WHITE,
      };

      array_push(mesh.faces, face);
    } else {
      // printf("other: %s\n", line);
    }
  }

  free(line);
  fclose(stream);
}

bool starts_with(char *str, char *prefix) { return strncmp(str, prefix, strlen(prefix)) == 0; }
