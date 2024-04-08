#ifndef FLU_TOKEN_H_INCLUDE
#define FLU_TOKEN_H_INCLUDE

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "fposition.h"
#include "xstd.h"

typedef enum {
  TK_EOF = 0,
  TK_LEFT_PARENTHESIS = '(',
  TK_RIGHT_PARENTHESIS = ')',
  TK_IDENT = 'I',
  TK_STRING = 'S',
  TK_NUMBER = 'D',
} FluTokenType;

typedef union {
  bool has_data;
  char *bytes;
} FluTokenData;

typedef struct {
  FluTokenType type;
  FluPosition position;
  FluTokenData data;
} FluToken;

void write_token(Writer *writer, FluToken *token, int *error);

#define FLU_IMPLEMENTATION
#ifdef FLU_IMPLEMENTATION
void write_token(Writer *writer, FluToken *token, int *error) {
  int error_ = 0;
  if (error == NULL)
    error = &error_;

  writer_write(writer, (uint8_t *)token, sizeof(FluToken), NULL, error);
  if (*error != 0)
    return;

  if (token->data.has_data) {
    size_t data_len = vec_len(token->data.bytes);

    // Write data size.
    writer_write(writer, (void *)&data_len, sizeof(data_len), NULL, error);
    if (*error != 0)
      return;

    // Write data.
    writer_write(writer, (void *)token->data.bytes, data_len, NULL, error);
    if (*error != 0)
      return;
  }
}
#endif

void read_token(Reader *reader, FluToken *token, uint8_t **token_data_vec,
                int *error);

#ifdef FLU_IMPLEMENTATION
void read_token(Reader *reader, FluToken *token, uint8_t **token_data_vec,
                int *error) {
  *token = (FluToken){0};

  int error_ = 0;
  if (error == NULL)
    error = &error_;

  reader_read(reader, (uint8_t *)token, sizeof(FluToken), NULL, error);
  if (*error != 0)
    return;

  if (token->data.has_data) {
    size_t data_len = 0;

    reader_read(reader, (uint8_t *)&data_len, sizeof(size_t), NULL, error);
    if (*error != 0)
      return;

    vec_resize(token_data_vec, data_len);

    reader_read(reader, *token_data_vec, data_len, NULL, error);
    if (*error != 0)
      return;

    headerof_vec(*token_data_vec)->len_ = data_len;

    token->data.bytes = (char *)*token_data_vec;
  }
}
#endif

#endif
