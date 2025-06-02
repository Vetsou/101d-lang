#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "chunk.h"

typedef struct {
    size_t line_idx;
    int32_t line_offset;
    const line_array_t *lines;
    int32_t prev;
    int32_t curr;
} line_iter_t;

void line_iter_init(line_iter_t *iter, const line_array_t *lines);
int32_t line_iter_advance(line_iter_t *iter);

void disassemble_chunk(chunk_t *chunk, const char *name);
int32_t disassemble_instr(chunk_t *chunk, size_t offset, line_iter_t *iter);

#endif // _DEBUG_H_