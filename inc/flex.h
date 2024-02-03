#ifndef FLU_LEX
#define FLU_LEX

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "serde.h"
#include "stream.h"
#include "vector.h"

typedef enum {
  TK_LEFT_PARENTHESIS = '(',
  TK_RIGHT_PARENTHESIS = ')',
  TK_IDENT = 'I',
  TK_STRING = 'S',
  TK_NUMBER = 'D',
} FluTokenType;

typedef struct {
  FluTokenType type;
  int has_data;
} FluToken;

typedef struct {
  size_t size;
  union {
    char *ident;
    void *raw;
  } data;
} FluTokenData;

size_t flu_lex(FILE *in, FILE *out, char **err) {
  int escaped = 0;
  char c = '\0';
  Vector vec = vector_new(32 * 1024, sizeof(char));

  while ((c = fgetc(in))) {
    if (c == '\\') {
      escaped = 1;
      c = fgetc(in);
    }

    FluToken token = {0};

    switch (c) {
    case EOF:
      return ftell(in);

      // Skip whitespaces.
    case '\t':
    case ' ':
    case '\n':
      continue;

    case TK_LEFT_PARENTHESIS:
    case TK_RIGHT_PARENTHESIS:
      token.type = c;
      token.has_data = 0;
      serialize(out, token);
      continue;

    default: {
      // Identifier.
      if (isalpha(c)) {
        token.type = TK_IDENT;
        token.has_data = 1;
        serialize(out, token);

        // Token data.
        vector_reset(&vec);
        do {
          vector_push(&vec, c);
        } while (isalnum((c = fgetc(in))));
        fseek(in, -1, SEEK_CUR);

        SerdeData data = {0};
        data.size = vec.len;
        data.data = vec.data;
        serialize_data(out, data);

        continue;
      }

      // Number.
      if (isdigit(c)) {
        token.type = TK_NUMBER;
        token.has_data = 1;
        serialize(out, token);

        vector_reset(&vec);
        do {
          vector_push(&vec, c);
        } while (isdigit((c = fgetc(in))));

        // Floating point number.
        if (c == '.') {
          // Decimals.
          do {
            vector_push(&vec, c);
          } while (isdigit((c = fgetc(in))));
        }
        fseek(in, -1, SEEK_CUR);

        SerdeData data = {0};
        data.size = vec.len;
        data.data = vec.data;
        serialize_data(out, data);

        continue;
      }

      // String literal.
      if (c == '"' && !escaped) {
        token.type = TK_STRING;
        token.has_data = 1;
        serialize(out, token);

        // Skip ".
        c = fgetc(in);

        // Collect string.
        vector_reset(&vec);
        char prev = '\0';
        while ((prev == '\\' && c == '"') || (c != '"' && c != EOF)) {
          vector_push(&vec, c);
          prev = c;
          c = fgetc(in);
        }

        SerdeData data = {0};
        data.size = vec.len;
        data.data = vec.data;
        serialize_data(out, data);

        continue;
      }

      assert(asprintf(err, "unexpected char '%c' at bytes %ld", c, ftell(in)));
    }
    }
  }

  return ftell(in);
}

#endif
