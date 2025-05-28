#ifndef _CHUNK_H_
#define _CHUNK_H_

#include "types.h"
#include "values.h"

typedef enum {
    OP_RETURN,
    OP_CONSTANT
} op_code_t;

typedef struct {
    size_t len;
    size_t capacity;
    uint8_t *code;
    int32_t *lines;
    value_array_t consts;
} chunk_t;

void chunk_init(chunk_t *chunk);
void chunk_free(chunk_t *chunk);
void chunk_write(chunk_t *chunk, uint8_t byte, int32_t line);
int32_t chunk_add_const(chunk_t *chunk, cvalue_t value);

#endif // _CHUNK_H_