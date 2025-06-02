#include "vm.h"
#include <stdio.h>

#ifdef DEBUG_TRACE_EXEC
#include "debug.h"
#endif // DEBUG_TRACE_EXEC

//
// PRIVATE
//

static inline uint8_t read_byte(
    vm_t *vm
) {
    return *vm->b_ptr++;
}

static inline cvalue_t read_const(
    vm_t *vm
) {
    return vm->chunk->consts.values[((uint16_t)read_byte(vm) << 8) | read_byte(vm)];
}

static interpret_result_t run_code(
    vm_t *vm
) {
    uint8_t instr;

#ifdef DEBUG_TRACE_EXEC
    line_iter_t line_iter;
    line_iter_init(&line_iter, &vm->chunk->lines);
#endif

    while(true) {

#ifdef DEBUG_TRACE_EXEC
    disassemble_instr(vm->chunk, (size_t)(vm->b_ptr - vm->chunk->code), &line_iter);
#endif
        switch (instr = read_byte(vm)) {
            case OP_CONSTANT: {
                cvalue_t constant = read_const(vm);
                break;
            }
            case OP_RETURN: {
                return INTERPRET_OK;
            }
        }
    }
}

//
// PUBLIC
//

void vm_init(
    vm_t *vm
) {

}

interpret_result_t vm_interpret(
    vm_t *vm, chunk_t *chunk
) {
    vm->chunk = chunk;
    vm->b_ptr = chunk->code;
    return run_code(vm);
}

void vm_free(
    vm_t *vm
) {

}