#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flex.h"
#include "log.h"
#include "serde.h"

#define BUF_SIZE 1024

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

    FluToken *token = (FluToken *)data;
    FluTokenType token_type = token->type;
    if (token->has_data) {
      // Deserialize token data.
      if (!deserialize_size(stdin, &data_size)) {
        break;
      }

      data = realloc(data, data_size);
      if (!deserialize_data(stdin, data, data_size)) {
        break;
      }

      if (token_type == TK_STRING) {
        printf("\"%s\" ", (char *)data);
      } else {
        printf("%s ", (char *)data);
      }
    } else {
      printf("%c ", token_type);
    }
  }
  if (ferror(stdin)) {
    flu_errorf("failed to deserialize data: %s", strerror(errno));
    return 1;
  }

  return 0;
}
