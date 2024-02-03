#include <stdio.h>
#include <string.h>

#include "log.h"
#include "serde.h"

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  flu_data_t data = {};
  while ((flu_deserialize(stdin, &data))) {
    printf("DATA_SIZE: %zu | %s\n", data.size, (char *)data.data);
  }
  if (ferror(stdin)) {
    flu_errorf("failed to deserialized data: %s", strerror(ferror(stdin)));
    return 1;
  }

  return 0;
}
