#ifndef _VM_H_
#define _VM_H_

#include "chunk.h"
#include "err.h"

#define STACK_MAX 4096

typedef struct {
    chunk_t *chunk;
    uint8_t *b_ptr;
    value_t stack[STACK_MAX];
    value_t *stack_top;
} vm_t;

void vm_init(vm_t *vm);
dl_result_t vm_interpret(vm_t *vm, const char *source);
void vm_free(vm_t *vm);

void vm_stack_push(vm_t *vm, value_t value);
value_t vm_stack_pop(vm_t *vm);

#endif // _VM_H_