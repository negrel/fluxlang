#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "serde.h"

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  for (int i = 0; i < argc; i++) {
    size_t len = strlen(argv[i]) + 1; // +1 for null byte.
    if (!flu_serialize(stdout, argv[i], sizeof(char) * len)) {
      flu_errorf("failed to serialized data: %s", strerror(errno));
    }
  }

  return 0;
}
