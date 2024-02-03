#ifndef FLU_SERDE
#define FLU_SERDE

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  size_t size;
  void *data;
} flu_data_t;

int flu_serialize(FILE *stream, flu_data_t data) {
  // Write header.
  if (fwrite(&data.size, sizeof(size_t), 1, stream) < 1) {
    return 0;
  }

  // Write data itself.
  if (fwrite(data.data, data.size, 1, stream) < 1) {
    return 0;
  }

  // End line.
  if (fwrite("\n", sizeof(char), 1, stream) < 1) {
    return 0;
  }

  return 1;
}

int flu_deserialize(FILE *stream, flu_data_t *data) {
  // Read header.
  if (fread(&data->size, sizeof(size_t), 1, stream) != 1) {
    return 0;
  }

  // Alloc body.
  data->data = calloc(1, data->size);
  if (data->data == NULL) {
    return 0;
  }

  // Read body (+1 for newline).
  if (fread(data->data, data->size, 1, stream) != 1) {
    return 0;
  }

  // Read new line.
  char c;
  size_t read = fread(&c, sizeof(char), 1, stream);
  (void)read;

  return 1;
}

#endif
