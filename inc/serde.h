#ifndef FLU_SERDE
#define FLU_SERDE

#include <stdio.h>
#include <stdlib.h>

#define serialize(out, data) serialize_data(out, &data, sizeof(data))

int serialize_data(FILE *stream, void *data, size_t size) {
  // Write header.
  if (fwrite(&size, sizeof(size_t), 1, stream) < 1) {
    return 0;
  }

  // Write data itself.
  if (fwrite(data, size, 1, stream) < 1) {
    return 0;
  }

  if (fputc('\n', stream) == EOF) {
    return 0;
  }

  return 1;
}

int deserialize_size(FILE *stream, size_t *size) {
  // Read header.
  if (fread(size, sizeof(size_t), 1, stream) != 1) {
    return 0;
  }

  return 1;
}

int deserialize_data(FILE *stream, void *data, size_t size) {
  // Read body (+1 for newline).
  if (fread(data, size, 1, stream) != 1) {
    return 0;
  }

  fgetc(stream);

  return 1;
}

#endif
