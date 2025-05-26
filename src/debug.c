#include "debug.h"
#include <stdio.h>

//
// PRIVATE
//

static int simple_instr(
    const char *name,
    size_t offset
) {
    printf("%s\n", name);
    return offset + 1;
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

    uint8_t instr = chunk->code[offset];
    switch (instr) {
        case OP_RETURN:
            return simple_instr("OP_RETURN", offset);
        default:
            printf("Unknown opcode %d\n", instr);
            return offset + 1;
    }
}