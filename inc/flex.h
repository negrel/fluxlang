#ifndef FLU_LEX
#define FLU_LEX

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "serde.h"

typedef enum {
  TK_LEFT_PARENTHESIS = '(',
  TK_RIGHT_PARENTHESIS = ')',
  TK_IDENT = 'I',
  TK_STRING = 'S',
  TK_NUMBER = 'D',
} flu_token_type_t;

typedef struct {
  flu_token_type_t type;
  int has_data;
} flu_token_t;

typedef struct {
  size_t size;
  union {
    char *ident;
    void *raw;
  } data;
} flu_token_data_t;

typedef struct {
  char *cursor;
  char *error;
} flu_lexer_t;

int flu_lex(flu_lexer_t *lexer, flu_token_t *token, flu_token_data_t *data) {

  while (1) {
    switch (*lexer->cursor) {
      // Skip whitespaces.
    case '\t':
    case ' ':
    case '\n':
      lexer->cursor++;
      continue;

    case TK_LEFT_PARENTHESIS:
    case TK_RIGHT_PARENTHESIS:
      token->type = *lexer->cursor;
      token->has_data = 0;
      lexer->cursor++;
      return 1;

    case '\0':
      return 0;

    default: {
      // Identifier.
      if (isalpha(*lexer->cursor)) {
        token->type = TK_IDENT;
        token->has_data = 1;

        char *start = lexer->cursor;
        while (isalnum(*lexer->cursor)) {
          lexer->cursor++;
        }
        data->size = lexer->cursor - start;

        data->data.ident = calloc(data->size + 1, sizeof(char));
        strncpy(data->data.ident, start, data->size);

        return 1;
      }

      // Number.
      if (isdigit(*lexer->cursor)) {
        token->type = TK_NUMBER;
        token->has_data = 1;

        char *start = lexer->cursor;
        while (isdigit(*lexer->cursor)) {
          lexer->cursor++;
        }

        // Floating point number.
        if (*lexer->cursor == '.') {
          lexer->cursor++;
          // Decimals.
          while (isdigit(*lexer->cursor)) {
            lexer->cursor++;
          }
        }

        data->size = lexer->cursor - start;

        data->data.ident = calloc(data->size + 1, sizeof(char));
        strncpy(data->data.ident, start, data->size);

        return 1;
      }

      // String literal.
      if (*lexer->cursor == '"' || *lexer->cursor == '\'') {
        token->type = TK_STRING;
        token->has_data = 1;

        char delim = *lexer->cursor;
        char prev = *lexer->cursor;

        char *start = ++lexer->cursor;
        while (*lexer->cursor && *lexer->cursor != delim && prev != '\\') {
          prev = *lexer->cursor++;
        }

        data->size = lexer->cursor - start;

        data->data.ident = calloc(data->size + 1, sizeof(char));
        strncpy(data->data.ident, start, data->size);

        lexer->cursor++;
        return 1;
      }

      assert(asprintf(&lexer->error,
                      "unexpected char at position 0 in sequence '%s'",
                      lexer->cursor));

      return 0;
    }
    }
  }
}

inline int flu_lex_eol(flu_lexer_t *lexer) { return *lexer->cursor == '\0'; }

inline char *flu_lex_error(flu_lexer_t *lexer) { return lexer->error; }

#endif
