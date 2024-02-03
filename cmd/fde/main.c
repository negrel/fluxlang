#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "serde.h"

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  size_t data_size = 0;
  void *data = NULL;
  while (deserialize_size(stdin, &data_size)) {
    data = realloc(data, data_size);
    if (!deserialize_data(stdin, data, data_size)) {
      break;
    }

    printf("DATA_SIZE: %zu | %s\n", data_size, (char *)data);
  }
  if (ferror(stdin)) {
    flu_errorf("failed to deserialized data: %s", strerror(ferror(stdin)));
    return 1;
  }

  return 0;
}
