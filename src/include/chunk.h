#ifndef _CHUNK_H_
#define _CHUNK_H_

#include "types.h"

typedef enum {
    OP_RETURN
} op_code_t;

typedef struct {
    size_t len;
    size_t capacity;
    uint8_t *code;
} chunk_t;

void chunk_init(chunk_t *chunk);
void chunk_free(chunk_t *chunk);
void chunk_write(chunk_t *chunk, uint8_t byte);

#endif // _CHUNK_H_