#include "vm.h"
#include "compiler.h"
#include "mem.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG_TRACE_EXEC
#include "debug.h"
#endif // DEBUG_TRACE_EXEC

//
// PRIVATE
//

static inline void _reset_stack(
    vm_t *vm
) {
    vm->stack_top = vm->stack;
}

static inline value_t _stack_peek(
    vm_t *vm,
    uint8_t distance
) {
    return vm->stack_top[-1 - distance];
}

// RUNTIME ERROR

static void _runtime_err(
    vm_t *vm,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instr_idx = vm->b_ptr - vm->chunk->code - 1;
    int32_t line = chunk_get_line(vm->chunk, instr_idx);
    fprintf(stderr, "[Line %d] in script\n", line);
    _reset_stack(vm);
}

static inline bool _vm_stack_negate_top(
    vm_t *vm
) {
    if (!IS_NUMBER(_stack_peek(vm, 0))) {
        _runtime_err(vm, "Operand must be a number.");
        return false;
    }

    vm->stack_top[-1] = NUMBER_VAL(-AS_NUMBER(vm->stack_top[-1]));
    return true;
}

static void _concatenate_str(
    vm_t *vm
) {
    obj_str_t *b = AS_STRING(vm_stack_pop(vm));
    obj_str_t *a = AS_STRING(vm_stack_pop(vm));

    int length = a->len + b->len;
    char* chars = ALLOC(char, length + 1);
    memcpy(chars, a->chars, a->len);
    memcpy(chars + a->len, b->chars, b->len);
    chars[length] = '\0';

    obj_str_t *result = take_str(vm->gc, chars, length);
    vm_stack_push(vm, OBJ_VAL(result));
}

static bool is_falsey(
    value_t value
) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static inline uint8_t _read_byte(
    vm_t *vm
) {
    return *vm->b_ptr++;
}

static inline value_t _read_const(
    vm_t *vm
) {
    return vm->chunk->consts.values[((uint16_t)_read_byte(vm) << 8) | _read_byte(vm)];
}

static dl_result_t run_code(
    vm_t *vm
) {

#define BINARY_OP(val_type, op)                                                   \
    do {                                                                          \
        if ( !IS_NUMBER(_stack_peek(vm, 0)) || !IS_NUMBER(_stack_peek(vm, 1)) ) { \
            _runtime_err(vm, "Operands must be numbers.");                        \
            return DL_RUNTIME_ERR;                                                \
        }                                                                         \
        double b = AS_NUMBER(vm_stack_pop(vm));                                   \
        double a = AS_NUMBER(vm_stack_pop(vm));                                   \
        vm_stack_push(vm, val_type(a op b));                                      \
    } while (false)



#ifdef DEBUG_TRACE_EXEC
    line_iter_t line_iter;
    line_iter_init(&line_iter, &vm->chunk->lines);
#endif

    uint8_t instr;
    while(true) {

#ifdef DEBUG_TRACE_EXEC
    printf("          ");
    for (value_t *slot = vm->stack; slot < vm->stack_top; slot++) {
        printf("[ ");
        print_value(*slot);
        printf(" ]");
    }
    printf("\n");
    disassemble_instr(vm->chunk, (size_t)(vm->b_ptr - vm->chunk->code), &line_iter);
#endif

        switch (instr = _read_byte(vm)) {
            case OP_NIL:   vm_stack_push(vm, NIL_VAL); break;
            case OP_TRUE:  vm_stack_push(vm, BOOL_VAL(true)); break;
            case OP_FALSE: vm_stack_push(vm, BOOL_VAL(false)); break;
            case OP_CONSTANT: {
                value_t constant = _read_const(vm);
                vm_stack_push(vm, constant);
                break;
            }

            case OP_NEGATE:
                if (!_vm_stack_negate_top(vm)) return DL_RUNTIME_ERR;
                break;
            case OP_ADD: {
                if (IS_STRING(_stack_peek(vm, 0)) && IS_STRING(_stack_peek(vm, 1))) {
                    _concatenate_str(vm);
                } else if (IS_NUMBER(_stack_peek(vm, 0)) && IS_NUMBER(_stack_peek(vm, 1))) {
                    double b = AS_NUMBER(vm_stack_pop(vm));
                    double a = AS_NUMBER(vm_stack_pop(vm));
                    vm_stack_push(vm, NUMBER_VAL(a + b));
                } else {
                    _runtime_err(vm, "Operands must be two numbers or two strings.");
                    return DL_RUNTIME_ERR;
                }
                break;
            }
            case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
            case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
            case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;

            case OP_GREATER:       BINARY_OP(BOOL_VAL, >);  break;
            case OP_LESS:          BINARY_OP(BOOL_VAL, <);  break;
            case OP_GREATER_EQUAL: BINARY_OP(BOOL_VAL, >=); break;
            case OP_LESS_EQUAL:    BINARY_OP(BOOL_VAL, <=); break;
            case OP_NOT:
                vm_stack_push( vm, BOOL_VAL(is_falsey(vm_stack_pop(vm))) );
                break;
            case OP_EQUAL: {
                value_t a = vm_stack_pop(vm);
                value_t b = vm_stack_pop(vm);
                vm_stack_push(vm, BOOL_VAL(values_equal(a, b)));
                break;
            }
            case OP_UNEQUAL: {
                value_t a = vm_stack_pop(vm);
                value_t b = vm_stack_pop(vm);
                vm_stack_push(vm, BOOL_VAL(!values_equal(a, b)));
                break;
            }

            case OP_RETURN: {
                print_value(vm_stack_pop(vm));
                return DL_OK;
            }
        }
    }

#undef BINARY_OP
}

//
// PUBLIC STACK
//

inline void vm_stack_push(
    vm_t *vm,
    value_t value
) {
    *vm->stack_top = value;
    vm->stack_top++;
}

inline value_t vm_stack_pop(
    vm_t *vm
) {
    vm->stack_top--;
    return *vm->stack_top;
}

//
// PUBLIC VM
//

void vm_init(
    vm_t *vm
) {
    _reset_stack(vm);
}

dl_result_t vm_interpret(
    vm_t *vm,
    const char *source
) {
    chunk_t chunk;
    chunk_init(&chunk);

    gc_t gc;
    gc_init(&gc);

    if (!compile(&gc, source, &chunk)) {
        chunk_free(&chunk);
        return DL_COMPILER_ERR;
    }

    vm->chunk = &chunk;
    vm->gc = &gc;
    vm->b_ptr = vm->chunk->code;

    dl_result_t result = run_code(vm);
    if (result != DL_OK) {
        chunk_free(&chunk);
        return result;
    }

    chunk_free(&chunk);
    gc_free(&gc);

    return DL_OK;
}

void vm_free(
    vm_t *vm
) {

}