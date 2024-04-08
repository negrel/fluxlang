#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define XSTD_IMPLEMENTATION
#include "xstd.h"

#define FLU_IMPLEMENTATION
#include "flex.h"
#include "ftoken.h"

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  FileWriter fwriter = file_writer(stdout);
  int write_error = 0;

  uint8_t reader_buf[4096] = {0};
  FileReader freader = file_reader(stdin);
  BufReader breader = buf_reader(&freader.reader, reader_buf, 4096);

  char *token_data_buf = vec_new(g_libc_allocator, 4096, sizeof(char));

  FluLexer lexer = {0};
  flu_lexer_init(&lexer, &breader.reader);

  FluToken token = {0};
  while (flu_lexer_lex(&lexer, &token, &token_data_buf)) {
    write_token(&fwriter.writer, &token, &write_error);
    if (write_error != 0)
      fprintf(stderr, "failed to write token: %s (%d)\n", strerror(write_error),
              write_error);
  }

  FluLexerError error = flu_lexer_error(&lexer);
  if (error.type != FLEX_NO_ERROR) {
    fprintf(stderr, "lexer error code %d at %lu:%lu: %d %c\n", error.type,
            error.position.line, error.position.column, error.data.reader_error,
            error.data.token);
    return error.type;
  }

  return 0;
}
