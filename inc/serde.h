#ifndef FLU_SERDE
#define FLU_SERDE

#include <stdio.h>
#include <stdlib.h>

#define serialize(out, data)                                                   \
  serialize_data(out, (SerdeData){sizeof(data), &data})

typedef struct {
  size_t size;
  void *data;
} SerdeData;

int serialize_data(FILE *stream, SerdeData data) {
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

int deserialize_data(FILE *stream, SerdeData *data) {
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
