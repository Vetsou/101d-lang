#ifndef _CHUNK_H_
#define _CHUNK_H_

#include "types.h"
#include "values.h"
#include "line.h"

typedef enum {
    OP_RETURN,

    OP_CONSTANT,
    OP_NIL, OP_TRUE, OP_FALSE,

    OP_NEGATE,
    OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE,

    OP_NOT,
    OP_EQUAL, OP_GREATER, OP_LESS,
} op_code_t;

typedef struct {
    size_t len;
    size_t capacity;
    uint8_t *code;
    line_array_t lines;
    value_array_t consts;
} chunk_t;

void chunk_init(chunk_t *chunk);
void chunk_free(chunk_t *chunk);
void chunk_write_instr(chunk_t *chunk, uint8_t byte, int32_t line);
int32_t chunk_write_const(chunk_t *chunk, value_t value, int32_t line);
int32_t chunk_get_line(chunk_t *chunk, size_t instr_index);

#endif // _CHUNK_H_