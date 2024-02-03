#include <stdio.h>
#include <string.h>

#include "log.h"
#include "serde.h"

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  void *data = NULL;
  size_t data_size = 0;
  while ((flu_deserialize(stdin, &data, &data_size))) {
    printf("DATA_SIZE: %zu | %s\n", data_size, (char *)data);
  }
  if (ferror(stdin)) {
    flu_errorf("failed to deserialized data: %s", strerror(ferror(stdin)));
    return 1;
  }

  return 0;
}
