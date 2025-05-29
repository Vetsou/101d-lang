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
    line_array_init(&chunk->lines);
}

void chunk_free(
    chunk_t *chunk
) {
    ARRAY_FREE(uint8_t, chunk->code, chunk->capacity);
    line_array_free(&chunk->lines);
    value_array_free(&chunk->consts);
    chunk_init(chunk);
}

void chunk_write(
    chunk_t *chunk,
    uint8_t byte,
    int32_t line
) {
    if (chunk->capacity < chunk->len + 1) {
        size_t prev_capacity = chunk->capacity;
        chunk->capacity = CAPACITY_GROW(prev_capacity);
        chunk->code = ARRAY_GROW(uint8_t, chunk->code,
            prev_capacity, chunk->capacity);
    }

    line_array_write(&chunk->lines, line);
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

int32_t chunk_get_line(
    chunk_t *chunk,
    size_t instr_index
) {
    size_t accumulated = 0;

    for (size_t i = 0; i < chunk->lines.len; i++) {
        accumulated += chunk->lines.line_info[i].count;
        if (instr_index < accumulated) {
            return chunk->lines.line_info[i].line;
        }
    }

    // Should not happen
    return -1;
}