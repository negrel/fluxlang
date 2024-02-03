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

  SerdeData serde_token = {0};
  SerdeData serde_token_data = {0};
  while (deserialize_data(stdin, &serde_token)) {
    FluToken *token = (FluToken *)(serde_token.data);
    FluTokenType token_type = token->type;

    if (token->has_data) {
      // Deserialize token data.
      if (!deserialize_data(stdin, &serde_token_data)) {
        break;
      }

      FluTokenData *token_data = (FluTokenData *)&serde_token_data;
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
