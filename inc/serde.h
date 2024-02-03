#ifndef FLU_SERDE
#define FLU_SERDE

#include <stdio.h>
#include <stdlib.h>

int flu_serialize(FILE *stream, void *data, size_t data_size) {
  // Write header.
  if (fwrite(&data_size, sizeof(size_t), 1, stream) < 1) {
    return 0;
  }

  // Write data itself.
  if (fwrite(data, data_size, 1, stream) < 1) {
    return 0;
  }

  // End line.
  if (fwrite("\n", sizeof(char), 1, stream) < 1) {
    return 0;
  }

  return 1;
}

int flu_deserialize(FILE *stream, void **data, size_t *data_size) {
  // Read header.
  if (fread(data_size, sizeof(size_t), 1, stream) != 1) {
    return 0;
  }

  // Alloc body.
  *data = calloc(1, *data_size);
  if (*data == NULL) {
    return 0;
  }

  // Read body (+1 for newline).
  if (fread(*data, *data_size, 1, stream) != 1) {
    return 0;
  }

  // Read new line.
  char c;
  size_t read = fread(&c, sizeof(char), 1, stream);
  (void)read;

  return 1;
}

#endif
