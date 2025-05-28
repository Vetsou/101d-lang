#include "debug.h"
#include "values.h"

#include <stdio.h>

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
    while (offset < chunk->len) {
        offset = disassemble_instr(chunk, offset);
    }
}

int disassemble_instr(
    chunk_t *chunk,
    size_t offset
) {
    printf("%04d ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->lines[offset]);
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