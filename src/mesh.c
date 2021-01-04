#include "mesh.h"
#include "array.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mesh_t mesh = {.vertices = NULL, .faces = NULL, {.x = 0, .y = 0, .z = 0}};

const char VERTEX_PREFIX[] = "v ";
const char FACE_PREFIX[] = "f ";

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
