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

    value_array_init(&chunk->consts);
}

void chunk_free(
    chunk_t *chunk
) {
    ARRAY_FREE(uint8_t, chunk->code, chunk->capacity);
    value_array_free(&chunk->consts);
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

int32_t chunk_add_const(
    chunk_t *chunk,
    cvalue_t value
) {
    value_array_write(&chunk->consts, value);
    return chunk->consts.len - 1;
}