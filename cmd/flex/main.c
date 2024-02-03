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

  char *err = NULL;
  flu_lex(stdin, stdout, &err);
  if (err) {
    flu_errorf("flex error: %s\n", err);
    return 1;
  }

  return 0;
}
