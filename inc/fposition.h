#ifndef FLU_POSITION_H_INCLUDE
#define FLU_POSITION_H_INCLUDE

#include <stddef.h>
#include <stdint.h>

typedef struct {
  size_t byte;
  size_t line;
  size_t column;
} FluPosition;

void flu_position_update(FluPosition *pos, uint8_t c);
void flu_position_update(FluPosition *pos, uint8_t c) {
  pos->byte++;
  pos->column++;

  if (c == '\n') {
    pos->line++;
    pos->column = 0;
  }
}

#endif
