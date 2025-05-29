#include "debug.h"
#include "values.h"

#include <stdio.h>

//
// LINE ITERATOR
//

void line_iter_init(
    line_iter_t *iter,
    const line_array_t *lines
) {
    *iter = (line_iter_t) {
        .lines = lines,
        .line_idx = 0,
        .line_offset = 0,
        .curr = -1,
        .prev = -1
    };
}

int32_t line_iter_advance(
    line_iter_t *iter
) {
    iter->prev = iter->curr;
    if (iter->line_idx >= iter->lines->len) return -1;

    iter->curr = iter->lines->line_info[iter->line_idx].line;
    iter->line_offset++;

    if (iter->line_offset >= iter->lines->line_info[iter->line_idx].count) {
        iter->line_idx++;
        iter->line_offset = 0;
    }

    return iter->curr;
}

//
// PRIVATE
//

static int32_t simple_instr(
    const char *name,
    size_t offset
) {
    printf("%s\n", name);
    return offset + 1;
}

static int32_t const_instr(
    const char *name,
    chunk_t *chunk,
    size_t offset
) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    print_value(chunk->consts.values[constant]);
    printf("'\n");
    return offset + 2;
}

//
// PUBLIC
//

void disassemble_chunk(
    chunk_t *chunk,
    const char *name
) {
    printf("===== %s =====\n", name);

    size_t offset = 0;
    line_iter_t line_iter;
    line_iter_init(&line_iter, &chunk->lines);

    while (offset < chunk->len) {
        offset = disassemble_instr(chunk, offset, &line_iter);
    }
}

int disassemble_instr(
    chunk_t *chunk,
    size_t offset,
    line_iter_t *iter
) {
    printf("%04d ", offset);

    int32_t curr_line = line_iter_advance(iter);
    if (offset > 0 && curr_line == iter->prev) {
        printf("   | ");
    } else {
        printf("%4d ", curr_line);
    }

    uint8_t instr = chunk->code[offset];
    switch (instr) {
        case OP_RETURN:
            return simple_instr("OP_RETURN", offset);
        case OP_CONSTANT:
            return const_instr("OP_CONSTANT", chunk, offset);
        default:
            printf("Unknown opcode %d\n", instr);
            return offset + 1;
    }
}