#ifndef FLU_LEX_H_INCLUDE
#define FLU_LEX_H_INCLUDE

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "fposition.h"
#include "ftoken.h"
#include "xstd.h"

typedef enum {
  FLEX_NO_ERROR = 0,
  FLEX_READER_ERROR = 1,
  FLEX_TOKEN_ERROR = 2,
} FluLexerErrorType;

typedef struct {
  FluLexerErrorType type;
  FluPosition position;
  union {
    char token;
    int reader_error;
  } data;
} FluLexerError;

typedef struct {
  Reader *reader_;
  FluPosition cursor_;
  char current_;
  FluLexerError error_;
} FluLexer;

void flu_lexer_init(FluLexer *lexer, Reader *reader);

#ifdef FLU_IMPLEMENTATION
void flu_lexer_init(FluLexer *lexer, Reader *reader) {
  *lexer = (FluLexer){0};
  lexer->reader_ = reader;
  lexer->current_ = ' ';
  lexer->cursor_ = (FluPosition){0};
  lexer->cursor_.line = 1;
}
#endif

bool flu_lexer_lex(FluLexer *lexer, FluToken *token, char **token_data_vec);

#ifdef FLU_IMPLEMENTATION
static char flu_next_char(FluLexer *lexer) {
  size_t read = 0;
  int error = 0;
  reader_read(lexer->reader_, (uint8_t *)&lexer->current_, sizeof(char), &read,
              &error);

  if (error != 0) {
    lexer->error_ = (FluLexerError){
        .type = FLEX_READER_ERROR,
        .position = lexer->cursor_,
    };
    lexer->error_.data.reader_error = error;

    lexer->current_ = '\0';
    return lexer->current_;
  }

  if (read == 0) {
    lexer->current_ = '\0';
    return lexer->current_;
  }

  // Move cursor.
  flu_position_update(&lexer->cursor_, lexer->current_);

  return lexer->current_;
}

bool flu_lexer_lex(FluLexer *lexer, FluToken *token, char **token_data_vec) {
  *token = (FluToken){0};
  vec_reset(*token_data_vec);

  if (lexer->error_.type != FLEX_NO_ERROR)
    return false;

  if (lexer->current_ == '\0')
    flu_next_char(lexer);

  while (lexer->current_ != '\0') {
    switch (lexer->current_) {
    case '\t':
    case ' ':
    case '\n':
      flu_next_char(lexer);
      continue;

    case TK_LEFT_PARENTHESIS:
    case TK_RIGHT_PARENTHESIS:
      token->type = lexer->current_;
      token->position = lexer->cursor_;
      token->data.has_data = false;
      lexer->current_ = '\0';
      goto out_of_scan_loop;
    }

    // Identifier.
    if (isalpha(lexer->current_)) {
      token->type = TK_IDENT;
      token->position = lexer->cursor_;

      // Read until end of identifier.
      *vec_push(token_data_vec) = lexer->current_;
      while (flu_next_char(lexer) != '\0' &&
             (isalnum(lexer->current_) || lexer->current_ == '-')) {
        *vec_push(token_data_vec) = lexer->current_;
      }

      if (!isspace(lexer->current_))
        goto token_error;

      token->data.bytes = *token_data_vec;
      break;
    }

    // Literal number.
    if (isdigit(lexer->current_)) {
      token->type = TK_NUMBER;
      token->position = lexer->cursor_;

      // Read until end of number.
      *vec_push(token_data_vec) = lexer->current_;
      while (flu_next_char(lexer) != '\0' && isdigit(lexer->current_)) {
        *vec_push(token_data_vec) = lexer->current_;
      }
      // Decimal number.
      if (lexer->current_ == '.')
        do {
          *vec_push(token_data_vec) = lexer->current_;
        } while (flu_next_char(lexer) != '\0' && isdigit(lexer->current_));
      *vec_push(token_data_vec) = '\0';

      if (!isspace(lexer->current_))
        goto token_error;

      token->data.bytes = *token_data_vec;

      break;
    }

    // Literal string.
    if (lexer->current_ == '"') {
      token->type = TK_STRING;
      token->position = lexer->cursor_;

      // Read until end of string.
      *vec_push(token_data_vec) = lexer->current_;
      while (lexer->current_ != '\\' && flu_next_char(lexer) != '\0' &&
             lexer->current_ != '"') {
        *vec_push(token_data_vec) = lexer->current_;
      }

      // skip closing ".
      if (lexer->current_ == '"') {
        *vec_push(token_data_vec) = lexer->current_;
        flu_next_char(lexer);
      } else
        goto token_error;

      token->data.bytes = *token_data_vec;
      break;
    }

    // Unknown token.
  token_error:
    lexer->error_ = (FluLexerError){
        .type = FLEX_TOKEN_ERROR,
        .position = lexer->cursor_,
    };
    lexer->error_.data.token = lexer->current_;
    return false;
  }
out_of_scan_loop:

  // EOF token.
  if (token->type == TK_EOF && lexer->error_.type == FLEX_READER_ERROR &&
      lexer->error_.data.reader_error == EOF) {
    lexer->error_ = (FluLexerError){0};
    return false;
  }

  return lexer->error_.type == FLEX_NO_ERROR;
}
#endif

FluLexerError flu_lexer_error(FluLexer *lexer);

#ifdef FLU_IMPLEMENTATION
FluLexerError flu_lexer_error(FluLexer *lexer) { return lexer->error_; }
#endif

#endif
