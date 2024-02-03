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

  char buf[BUF_SIZE] = {};
  while (1) {
    char *line = fgets(buf, BUF_SIZE, stdin);
    if (line == NULL) {
      if (feof(stdin)) {
        return 0;
      } else {
        flu_errorf("%s failed to read from stdin: %s\n", argv[0],
                   strerror(ferror(stdin)));
        return 1;
      }
    }

    // Lex line.
    flu_lexer_t lexer = {
        .cursor = line,
    };
    flu_token_t token = {0};
    flu_token_data_t token_data = {0};
    while (flu_lex(&lexer, &token, &token_data)) {
      flu_data_t data = {
          .size = sizeof(token),
          .data = &token,
      };
      // Serialize token.
      if (!flu_serialize(stdout, data)) {
        flu_errorf("failed to serialized token: %s", strerror(errno));
        return 1;
      }

      if (token.has_data) {
        // Serialize token data.
        if (!flu_serialize(
                stdout, (flu_data_t){token_data.size, token_data.data.raw})) {
          flu_errorf("failed to serialized token data: %s", strerror(errno));
          return 1;
        }
      }
    }
    if (!flu_lex_eol(&lexer)) {
      flu_errorf("error: %s\n", flu_lex_error(&lexer));
    }
  }

  return 0;
}
