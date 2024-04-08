#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XSTD_IMPLEMENTATION
#include "xstd.h"

#define FLU_IMPLEMENTATION
#include "flex.h"
#include "ftoken.h"
#include "log.h"

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  FileReader freader = file_reader(stdin);
  int read_error = 0;

  uint8_t *token_data_vec = vec_new(g_libc_allocator, 16, sizeof(uint8_t));
  FluToken token = {0};

  while (1) {
    read_token(&freader.reader, &token, &token_data_vec, &read_error);
    if (read_error != 0) {
      if (read_error == EOF)
        break;
      fprintf(stderr, "failed to read token: %s (%d)", strerror(read_error),
              read_error);
    }

    if (token.data.has_data) {
      fprintf(stderr, "%c \'%.*s\' %lu:%lu\n", token.type,
              (int)vec_len(token_data_vec), token_data_vec, token.position.line,
              token.position.column);
    } else {
      fprintf(stderr, "%c %lu:%lu\n", token.type, token.position.line,
              token.position.column);
    }
  }

  return 0;
}
