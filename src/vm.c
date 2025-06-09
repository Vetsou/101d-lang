#include "vm.h"

#include "compiler.h"
#include <stdio.h>

#ifdef DEBUG_TRACE_EXEC
#include "debug.h"
#endif // DEBUG_TRACE_EXEC

//
// PRIVATE
//

static inline void reset_stack(
    vm_t *vm
) {
    vm->stack_top = vm->stack;
}

static inline void vm_stack_negate_top(
    vm_t *vm
) {
    vm->stack_top[-1] = -vm->stack_top[-1];
}

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

static dl_result_t run_code(
    vm_t *vm
) {

#define BINARY_OP(op)                \
    do {                             \
        double b = vm_stack_pop(vm); \
        double a = vm_stack_pop(vm); \
        vm_stack_push(vm, a op b);   \
    } while (false)



#ifdef DEBUG_TRACE_EXEC
    line_iter_t line_iter;
    line_iter_init(&line_iter, &vm->chunk->lines);
#endif

    uint8_t instr;
    while(true) {

#ifdef DEBUG_TRACE_EXEC
    printf("          ");
    for (cvalue_t *slot = vm->stack; slot < vm->stack_top; slot++) {
        printf("[ ");
        print_value(*slot);
        printf(" ]");
    }
    printf("\n");
    disassemble_instr(vm->chunk, (size_t)(vm->b_ptr - vm->chunk->code), &line_iter);
#endif

        switch (instr = read_byte(vm)) {
            case OP_CONSTANT: {
                cvalue_t constant = read_const(vm);
                vm_stack_push(vm, constant);
                break;
            }
            case OP_NEGATE:   vm_stack_negate_top(vm); break;
            case OP_ADD:      BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE:   BINARY_OP(/); break;
            case OP_RETURN: {
                print_value(vm_stack_pop(vm));
                return DL_OK;
            }
        }
    }

#undef BINARY_OP
}

//
// PUBLIC
//

void vm_init(
    vm_t *vm
) {
    reset_stack(vm);
}

dl_result_t vm_interpret(
    vm_t *vm,
    const char *source
) {
    chunk_t chunk;
    chunk_init(&chunk);

    if (!compile(source, &chunk)) {
        chunk_free(&chunk);
        return DL_COMPILER_ERR;
    }

    vm->chunk = &chunk;
    vm->b_ptr = vm->chunk->code;

    // TODO: Uncomment after initial parser implementation
    //dl_result_t result = run_code(vm);
    chunk_free(&chunk);

    return DL_OK;
}

void vm_free(
    vm_t *vm
) {

}

inline void vm_stack_push(
    vm_t *vm,
    cvalue_t value
) {
    *vm->stack_top = value;
    vm->stack_top++;
}

inline cvalue_t vm_stack_pop(
    vm_t *vm
) {
    vm->stack_top--;
    return *vm->stack_top;
}