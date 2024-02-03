#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "flex.h"
#include "log.h"
#include "serde.h"

#define BUF_SIZE 1024

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  flu_data_t serde_token = {0};
  flu_data_t serde_token_data = {0};
  while (flu_deserialize(stdin, &serde_token)) {
    flu_token_t *token = (flu_token_t *)(serde_token.data);
    flu_token_type_t token_type = token->type;

    if (token->has_data) {
      // Deserialize token data.
      if (!flu_deserialize(stdin, &serde_token_data)) {
        break;
      }

      flu_token_data_t *token_data = (flu_token_data_t *)&serde_token_data;
      if (token_type == TK_STRING) {
        printf("\"%s\" ", token_data->data.ident);
      } else {
        printf("%s ", token_data->data.ident);
      }

      free(serde_token_data.data);
    } else {
      printf("%c ", token->type);
    }

    free(serde_token.data);
  }
  if (ferror(stdin)) {
    flu_errorf("failed to deserialized data: %s", strerror(ferror(stdin)));
    return 1;
  }

  return 0;
}
