#include "chunk.h"
#include "mem.h"

void chunk_init(
    chunk_t *chunk
) {
    *chunk = (chunk_t) {
        .capacity = 0,
        .len = 0,
        .code = NULL
    };
}

void chunk_free(
    chunk_t *chunk
) {
    ARRAY_FREE(uint8_t, chunk->code, chunk->capacity);
    chunk_init(chunk);
}

void chunk_write(
    chunk_t *chunk,
    uint8_t byte
) {
    if (chunk->capacity < chunk->len + 1) {
        size_t prev_capacity = chunk->capacity;
        chunk->capacity = CAPACITY_GROW(prev_capacity);
        chunk->code = ARRAY_GROW(uint8_t, chunk->code,
            prev_capacity, chunk->capacity);
    }

    chunk->code[chunk->len] = byte;
    chunk->len++;
}